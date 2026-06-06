std::atomic<bool> g_scheduled_low_priority_update = false;
std::atomic<bool> g_delayed_apply_worker_running = false;
std::atomic<int64_t> g_delayed_apply_due_ms = 0;
std::atomic<unsigned long long> g_delayed_apply_generation = 0;
std::atomic<bool> g_initial_style_apply_completed = false;
std::atomic<bool> g_initial_taskbar_size_apply_done = false;
std::atomic<int64_t> g_initial_style_apply_not_before_ms = 0;
std::atomic<int> g_force_style_apply_passes = 0;
std::atomic<int> g_high_priority_dispatch_passes = 0;
std::atomic<int> g_reset_animation_target_passes = 0;
std::atomic<int64_t> g_last_geometry_critical_apply_ms = 0;
std::atomic<bool> g_animation_followup_worker_running = false;
std::atomic<int64_t> g_suppress_low_priority_apply_until_ms = 0;
constexpr int kInitialExplorerStyleDelayMs = 2500;
constexpr int kLowPriorityStyleDelayMs = 900;
constexpr int kDefaultStyleDebounceDelayMs = 150;
constexpr int kTaskbarIslandAnimationDurationMs = 250;
constexpr int kGeometryCriticalApplyMinIntervalMs = 80;
template <typename TAnimation>
void ConfigureTaskbarIslandAnimation(TAnimation const& animation) {
  animation.Duration(winrt::Windows::Foundation::TimeSpan(std::chrono::milliseconds(kTaskbarIslandAnimationDurationMs)));
  animation.DelayTime(winrt::Windows::Foundation::TimeSpan(std::chrono::milliseconds(0)));
}
float LerpFloat(float from, float to, float progress) {
  return from + ((to - from) * progress);
}
float EstimateAnimationValue(float from, float to, int64_t startedMs, int64_t nowMs) {
  if (startedMs <= 0 || nowMs <= startedMs) {
    return from;
  }
  float progress = static_cast<float>(nowMs - startedMs) / static_cast<float>(kTaskbarIslandAnimationDurationMs);
  if (progress <= 0.0f) {
    return from;
  }
  if (progress >= 1.0f) {
    return to;
  }
  return LerpFloat(from, to, progress);
}
void ResetAnimationTargetCache(TaskbarState& state) {
  state.hasLastTargetTaskFrameOffsetX = false;
  state.hasLastTargetTrayOffsetX = false;
  state.hasLastTargetWidgetOffset = false;
  state.lastBackgroundShapeTargetWidth = 0.0f;
  state.lastBackgroundShapeTargetOffsetX = 0.0f;
  state.lastBackgroundShapeTargetOffsetY = 0.0f;
  state.backgroundAnimationFromWidth = 0.0f;
  state.backgroundAnimationToWidth = 0.0f;
  state.backgroundAnimationFromOffsetX = 0.0f;
  state.backgroundAnimationToOffsetX = 0.0f;
  state.backgroundAnimationFromOffsetY = 0.0f;
  state.backgroundAnimationToOffsetY = 0.0f;
  state.backgroundAnimationStartMs = 0;
}
void ApplySettings(HWND hTaskbarWnd);
int64_t DelayedApplyNowMs() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
             std::chrono::steady_clock::now().time_since_epoch())
      .count();
}
void QueueTaskbarAnimationFollowup(HWND hTaskbarWnd) {
  if (g_unloading || !hTaskbarWnd || !IsWindow(hTaskbarWnd)) {
    return;
  }
  bool expected = false;
  if (!g_animation_followup_worker_running.compare_exchange_strong(expected, true)) {
    return;
  }
  std::thread([hTaskbarWnd]() {
    const DWORD followupDelaysMs[] = {16, 16, 18, 25, 40, 55, 80, 100};
    for (DWORD delayMs : followupDelaysMs) {
      Sleep(delayMs);
      if (g_unloading || !hTaskbarWnd || !IsWindow(hTaskbarWnd)) {
        break;
      }
      g_force_style_apply_passes.store(8);
      g_high_priority_dispatch_passes.store(8);
      ApplySettings(hTaskbarWnd);
    }
    g_animation_followup_worker_running = false;
  }).detach();
}
void ArmInitialExplorerStyleApplyDelay() {
  g_initial_style_apply_completed = false;
  g_initial_taskbar_size_apply_done = false;
  g_initial_style_apply_not_before_ms =
      DelayedApplyNowMs() + kInitialExplorerStyleDelayMs;
  Wh_Log(L"Initial Explorer style apply armed");
}
void DelayedApplyWorker();
void EnsureDelayedApplyWorker() {
  bool expected = false;
  if (!g_delayed_apply_worker_running.compare_exchange_strong(expected, true)) {
    return;
  }
  std::thread(DelayedApplyWorker).detach();
}
bool InitializeDebounce() {
  // Kept as a compatibility shim for older call sites. The old DispatcherTimer
  // debounce was removed because timer creation/stop could race Explorer/XAML
  // initialization and crash. Scheduling is now handled by DelayedApplyWorker.
  g_already_requested_debounce_initializing = false;
  return true;
}

void CleanupDebounce() {
  g_already_requested_debounce_initializing = false;
  g_scheduled_low_priority_update = false;
  g_delayed_apply_due_ms = 0;
  g_delayed_apply_generation.fetch_add(1);

  // The debounce worker is detached, so the only safe unload strategy is to
  // force it to observe cancellation and wait until it has left mod code.
  // Otherwise a recompilation can unload this DLL while the sleeping worker
  // later resumes inside DelayedApplyWorker from the old image.
  for (int i = 0; g_delayed_apply_worker_running.load() && i < 500; ++i) {
    Sleep(10);
  }
  if (g_delayed_apply_worker_running.load()) {
    Wh_Log(L"Delayed apply worker did not exit before unload");
  }
  for (int i = 0; g_animation_followup_worker_running.load() && i < 200; ++i) {
    Sleep(10);
  }
  if (g_animation_followup_worker_running.load()) {
    Wh_Log(L"Animation follow-up worker did not exit before unload");
  }
}
void DelayedApplyWorker() {


  try {
  for (;;) {


    if (g_unloading) {


      break;
    }
    int64_t dueMs = g_delayed_apply_due_ms.load();
    if (dueMs <= 0) {


      break;
    }
    int64_t nowMs = DelayedApplyNowMs();
    if (nowMs < dueMs) {


      DWORD sleepMs = static_cast<DWORD>(std::min<int64_t>(250, std::max<int64_t>(1, dueMs - nowMs)));
      Sleep(sleepMs);
      continue;
    }
    unsigned long long generation = g_delayed_apply_generation.load();
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd || !IsWindow(hTaskbarWnd)) {


      Wh_Log(L"Delayed apply postponed: taskbar window is not ready");
      g_delayed_apply_generation.fetch_add(1);
      g_delayed_apply_due_ms = DelayedApplyNowMs() + 500;
      continue;
    }
    g_scheduled_low_priority_update = false;
    Wh_Log(L"Delayed apply triggered");
       const bool initialStyleApply = !g_initial_style_apply_completed.load();
    if (initialStyleApply) {
      // The taskbar XAML tree can reset Composition visual properties during
      // Explorer startup after an earlier sizing pass. Forget cached animation
      // targets so the first real styling pass always starts the offset/shape
      // animations instead of assuming they were already applied.
      g_reset_animation_target_passes.store(8);
      g_force_style_apply_passes.store(8);
      g_high_priority_dispatch_passes.store(8);
    }
    if (!g_initial_style_apply_completed.load() &&
        !g_initial_taskbar_size_apply_done.exchange(true)) {


      ApplySettingsTBIconSize(g_settings_tbiconsize.taskbarHeight);
    }
    ApplySettings(hTaskbarWnd);
    if (!g_initial_style_apply_completed.load() && !g_unloading) {


      Wh_Log(L"Initial ApplyStyle did not complete; retrying delayed apply");
      g_delayed_apply_generation.fetch_add(1);
      g_delayed_apply_due_ms = DelayedApplyNowMs() + 500;
      continue;
    }
    int64_t expectedDueMs = dueMs;
    if (g_delayed_apply_due_ms.compare_exchange_strong(expectedDueMs, 0)) {


      if (g_delayed_apply_generation.load() == generation) {


        break;
      }
    }
  }
  } catch (winrt::hresult_error const& ex) {
    Wh_Log(L"Delayed apply worker failed %08X: %s", ex.code(), ex.message().c_str());
  } catch (...) {
    Wh_Log(L"Delayed apply worker failed: %08X", winrt::to_hresult());
  }
  g_delayed_apply_worker_running = false;
  if (!g_unloading && g_delayed_apply_due_ms.load() > 0) {


    EnsureDelayedApplyWorker();
  }
}
void ApplySettingsDebounced(int delayMs) {
  if (g_unloading) {
    return;
  }
  if (delayMs <= 0) {
    delayMs = kLowPriorityStyleDelayMs;
  }
  if (delayMs < 1) {
    delayMs = kDefaultStyleDebounceDelayMs;
  }
  int64_t nowMs = DelayedApplyNowMs();
  int64_t dueMs = nowMs + delayMs;
  int64_t initialNotBeforeMs = g_initial_style_apply_not_before_ms.load();
  if (!g_initial_style_apply_completed.load() && initialNotBeforeMs > dueMs) {
    dueMs = initialNotBeforeMs;
  }
  g_delayed_apply_generation.fetch_add(1);
  g_delayed_apply_due_ms = dueMs;
  Wh_Log(L"Scheduled delayed apply in %lld ms", static_cast<long long>(std::max<int64_t>(0, dueMs - nowMs)));
  EnsureDelayedApplyWorker();
}
void ApplySettingsDebounced() {
  ApplySettingsDebounced(kDefaultStyleDebounceDelayMs);
}
void ApplySettingsFromTaskbarThreadImmediately() {
  if (g_unloading) {
    return;
  }
  int64_t initialNotBeforeMs = g_initial_style_apply_not_before_ms.load();
  if (!g_initial_style_apply_completed.load() && initialNotBeforeMs > DelayedApplyNowMs()) {
    ApplySettingsDebounced(1);
    return;
  }
  g_scheduled_low_priority_update = false;
  g_delayed_apply_due_ms = 0;
  g_delayed_apply_generation.fetch_add(1);
  g_force_style_apply_passes.store(8);
  g_high_priority_dispatch_passes.store(8);
  g_suppress_low_priority_apply_until_ms = DelayedApplyNowMs() + 650;
  HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
  if (!hTaskbarWnd || !IsWindow(hTaskbarWnd)) {
    Wh_Log(L"Immediate apply skipped: taskbar window is not ready");
    return;
  }
  Wh_Log(L"Immediate taskbar animation apply");
  ApplySettings(hTaskbarWnd);
  QueueTaskbarAnimationFollowup(hTaskbarWnd);
}
void ApplySettingsFromTaskbarThreadGeometryChanged() {
  if (g_unloading) {
    return;
  }
  int64_t nowMs = DelayedApplyNowMs();
  int64_t initialNotBeforeMs = g_initial_style_apply_not_before_ms.load();
  if (!g_initial_style_apply_completed.load() && initialNotBeforeMs > nowMs) {
    ApplySettingsDebounced(1);
    return;
  }

  // Layout-only taskbar changes, such as grouping-mode changes and tray width
  // changes, used to fall through the old 900 ms low-priority debounce. Treat
  // them as animation-critical, but rate-limit the direct kick so noisy WndProc
  // traffic does not constantly restart the follow-up worker.
  int64_t lastMs = g_last_geometry_critical_apply_ms.load();
  while (nowMs - lastMs >= kGeometryCriticalApplyMinIntervalMs) {
    if (g_last_geometry_critical_apply_ms.compare_exchange_weak(lastMs, nowMs)) {
      g_scheduled_low_priority_update = false;
      g_force_style_apply_passes.store(8);
      g_high_priority_dispatch_passes.store(8);
      g_suppress_low_priority_apply_until_ms = nowMs + 650;
      ApplySettingsDebounced(1);
      HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
      if (hTaskbarWnd && IsWindow(hTaskbarWnd)) {
        QueueTaskbarAnimationFollowup(hTaskbarWnd);
      }
      return;
    }
  }

  // A recent geometry-critical apply is already pending/running. Keep the next
  // few dispatcher passes forced and high-priority so the follow-up burst catches
  // the changed measured widths as soon as XAML reports them.
  g_force_style_apply_passes.store(8);
  g_high_priority_dispatch_passes.store(8);
}
void ScheduleInitialExplorerStyleApply() {
  ApplySettingsDebounced(kInitialExplorerStyleDelayMs);
}