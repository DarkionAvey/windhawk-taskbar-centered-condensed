std::atomic<bool> g_scheduled_low_priority_update = false;
std::atomic<bool> g_delayed_apply_worker_running = false;
std::atomic<int64_t> g_delayed_apply_due_ms = 0;
std::atomic<unsigned long long> g_delayed_apply_generation = 0;
std::atomic<bool> g_initial_style_apply_completed = false;
std::atomic<bool> g_initial_taskbar_size_apply_done = false;
std::atomic<int64_t> g_initial_style_apply_not_before_ms = 0;
constexpr int kInitialExplorerStyleDelayMs = 2500;
constexpr int kLowPriorityStyleDelayMs = 900;
constexpr int kDefaultStyleDebounceDelayMs = 150;
void ApplySettings(HWND hTaskbarWnd);
int64_t DelayedApplyNowMs() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
             std::chrono::steady_clock::now().time_since_epoch())
      .count();
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
}
void DelayedApplyWorker() {
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
void ScheduleInitialExplorerStyleApply() {
  ApplySettingsDebounced(kInitialExplorerStyleDelayMs);
}