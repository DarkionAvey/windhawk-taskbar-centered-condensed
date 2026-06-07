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
constexpr int kDefaultStyleDebounceDelayMs = 150;
constexpr int kTaskbarIslandAnimationDurationMs = 250;
constexpr int kStartButtonAnchorStablePassesRequired = 2;
constexpr double kTaskbarRepeaterVirtualWidthMultiplier = 2.0;
constexpr int kLowPriorityStyleDelayMs =
    kDefaultStyleDebounceDelayMs + (kTaskbarIslandAnimationDurationMs * 3);
constexpr int kExplorerStartupSettleAnimationWindows = 6;
constexpr int kInitialExplorerStyleDelayMs =
    kLowPriorityStyleDelayMs + kDefaultStyleDebounceDelayMs +
    (kTaskbarIslandAnimationDurationMs * kExplorerStartupSettleAnimationWindows);
constexpr int kGeometryCriticalApplyMinIntervalMs = kDefaultStyleDebounceDelayMs / 2;
constexpr int kDelayedWorkerMaxSleepMs = kTaskbarIslandAnimationDurationMs;
constexpr int kInitialExplorerStyleRetryDelayMs = kTaskbarIslandAnimationDurationMs * 2;
constexpr int kButtonSizeLowPrioritySuppressionMs = kTaskbarIslandAnimationDurationMs;
constexpr int kGeometryCriticalLowPrioritySuppressionMs =
    (kTaskbarIslandAnimationDurationMs * 2) + kDefaultStyleDebounceDelayMs;
constexpr int kScheduledLowPriorityFlagTtlMs =
    kLowPriorityStyleDelayMs + kDefaultStyleDebounceDelayMs + kTaskbarIslandAnimationDurationMs;
constexpr double kMinimumTrustedRefreshHz = 24.0;
constexpr double kMaximumTrustedRefreshHz = 1000.0;
constexpr int kDefaultFrameIntervalMs = 16;
constexpr int kMinimumFrameIntervalMs = 1;
constexpr int kMaximumFrameIntervalMs = 42;
constexpr int kAnimationFollowupGraceFrames = 2;
constexpr float kDisplayGeometryChangeToleranceDip = 0.5f;
constexpr float kDisplayRasterizationChangeTolerance = 0.001f;
template <typename TAnimation>
void ConfigureTaskbarIslandAnimation(TAnimation const& animation) {
  animation.Duration(winrt::Windows::Foundation::TimeSpan(std::chrono::milliseconds(kTaskbarIslandAnimationDurationMs)));
  animation.DelayTime(winrt::Windows::Foundation::TimeSpan(std::chrono::milliseconds(0)));
}

winrt::Windows::UI::Composition::CompositionEasingFunction CreateTaskbarIslandEasingFunction(
    winrt::Windows::UI::Composition::Compositor const& compositor) {
  if (!compositor) {
    return nullptr;
  }

  // Keep every moving piece on the same curve. The explicit ease-out avoids
  // one visual catching up linearly while another decelerates, which reads as
  // rubber-banding when the taskbar island is resized, translated, and scaled
  // at the same time.
  return compositor.CreateCubicBezierEasingFunction(
      winrt::Windows::Foundation::Numerics::float2{0.16f, 1.0f},
      winrt::Windows::Foundation::Numerics::float2{0.30f, 1.0f});
}

template <typename TAnimation, typename TValue>
void InsertTaskbarIslandKeyFrame(TAnimation const& animation,
                                 float progress,
                                 TValue const& value) {
  if (auto compositor = animation.Compositor()) {
    if (auto easing = CreateTaskbarIslandEasingFunction(compositor)) {
      animation.InsertKeyFrame(progress, value, easing);
      return;
    }
  }

  animation.InsertKeyFrame(progress, value);
}

float ApplyTaskbarIslandEasingForEstimate(float progress) {
  progress = std::clamp(progress, 0.0f, 1.0f);
  // Approximation of the compositor ease-out used above. This is only for
  // calculating a safe start value when an in-flight background shape animation
  // is retargeted; all real animations use the compositor easing function.
  const float inv = 1.0f - progress;
  return 1.0f - (inv * inv * inv);
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
   return LerpFloat(from, to, ApplyTaskbarIslandEasingForEstimate(progress));
}

static float SnapToPhysicalPixel(float value, float rasterizationScale);

float SnapScaleForPhysicalPixels(float scale,
                                 float unscaledWidth,
                                 float rasterizationScale) {
  if (unscaledWidth <= 0.0f || !std::isfinite(unscaledWidth) ||
      !std::isfinite(scale)) {
    return 1.0f;
  }

  const float snappedWidth =
      SnapToPhysicalPixel(unscaledWidth * scale, rasterizationScale);
  if (snappedWidth <= 0.0f || !std::isfinite(snappedWidth)) {
    return scale;
  }

  return snappedWidth / unscaledWidth;
}

float CalculateTaskbarIslandScale(float screenLeft,
                                  float screenRight,
                                  float screenWidth,
                                  float scaleCenterX,
                                  float maxScaleDownFactor,
                                  float rasterizationScale) {
  if (screenWidth <= 0.0f || screenRight <= screenLeft ||
      maxScaleDownFactor <= 1.0f || !std::isfinite(screenLeft) ||
      !std::isfinite(screenRight) || !std::isfinite(screenWidth) ||
      !std::isfinite(scaleCenterX)) {
    return 1.0f;
  }

  float targetScale = 1.0f;
  if (screenLeft < 0.0f && scaleCenterX > screenLeft) {
    targetScale = std::min(targetScale,
                           scaleCenterX / (scaleCenterX - screenLeft));
  }
  if (screenRight > screenWidth && screenRight > scaleCenterX) {
    targetScale = std::min(targetScale,
                           (screenWidth - scaleCenterX) /
                               (screenRight - scaleCenterX));
  }

  const float minScale = 1.0f / maxScaleDownFactor;
  targetScale = std::clamp(targetScale, minScale, 1.0f);
  targetScale = SnapScaleForPhysicalPixels(targetScale,
                                           screenRight - screenLeft,
                                           rasterizationScale);
  return std::clamp(targetScale, minScale, 1.0f);
}

float ApplyScaleToScreenX(float screenX, float scaleCenterX, float scale) {
  return scaleCenterX + ((screenX - scaleCenterX) * scale);
}

void SetVisualScaleCenterAndAnimate(
    winrt::Windows::UI::Composition::Visual const& visual,
    float targetScale,
    float localCenterX,
    float localCenterY,
    float visualOffsetTolerance,
    bool animate) {
  if (!visual) {
    return;
  }

  if (!std::isfinite(targetScale) || targetScale <= 0.0f) {
    targetScale = 1.0f;
  }
  if (!std::isfinite(localCenterX)) {
    localCenterX = 0.0f;
  }
  if (!std::isfinite(localCenterY)) {
    localCenterY = 0.0f;
  }

  visual.CenterPoint({localCenterX, localCenterY, visual.CenterPoint().z});

  const auto currentScale = visual.Scale();
  if (std::abs(currentScale.x - targetScale) <= visualOffsetTolerance &&
      std::abs(currentScale.y - targetScale) <= visualOffsetTolerance) {
    return;
  }

  if (animate) {
    if (auto compositor = visual.Compositor()) {
      auto scaleAnimation = compositor.CreateVector3KeyFrameAnimation();
      ConfigureTaskbarIslandAnimation(scaleAnimation);
      InsertTaskbarIslandKeyFrame(
          scaleAnimation,
          1.0f,
          winrt::Windows::Foundation::Numerics::float3{
              targetScale, targetScale, currentScale.z});
      visual.StartAnimation(L"Scale", scaleAnimation);
      return;
    }
  }

  visual.StopAnimation(L"Scale");
  visual.Scale({targetScale, targetScale, currentScale.z});
}
void ResetAnimationTargetCache(TaskbarState& state) {
  state.hasLastTargetTaskFrameOffsetX = false;
  state.hasLastTargetTaskbarIslandScale = false;
  state.lastTargetTaskbarIslandScale = 1.0f;
  state.lastTaskbarIslandScaleCenterX = 0.0f;
  state.hasLastTargetTrayOffsetX = false;
  state.hasLastTargetWidgetOffset = false;
  state.hasLastStartButtonAnchorRect = false;
  state.hasStableStartButtonAnchorRect = false;
  state.startButtonAnchorStablePasses = 0;
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
bool CheckAndUpdateDisplayGeometrySignature(TaskbarState& state,
                                            FrameworkElement const& xamlRootContent,
                                            FrameworkElement const& taskFrame,
                                            float rasterizationScale) {
  const float rootWidth = static_cast<float>(xamlRootContent.ActualWidth());
  const float rootHeight = static_cast<float>(xamlRootContent.ActualHeight());
  const float taskFrameWidth = taskFrame ? static_cast<float>(taskFrame.ActualWidth()) : 0.0f;
  const float taskFrameHeight = taskFrame ? static_cast<float>(taskFrame.ActualHeight()) : 0.0f;

  const bool validSignature =
      std::isfinite(rootWidth) && rootWidth > 0.0f &&
      std::isfinite(rootHeight) && rootHeight > 0.0f &&
      std::isfinite(rasterizationScale) && rasterizationScale > 0.0f;
  if (!validSignature) {
    return false;
  }

  const bool changed =
      !state.hasLastDisplayGeometrySignature ||
      std::abs(state.lastObservedRootWidth - rootWidth) > kDisplayGeometryChangeToleranceDip ||
      std::abs(state.lastObservedRootHeight - rootHeight) > kDisplayGeometryChangeToleranceDip ||
      std::abs(state.lastObservedRasterizationScale - rasterizationScale) > kDisplayRasterizationChangeTolerance ||
      std::abs(state.lastObservedTaskFrameWidth - taskFrameWidth) > kDisplayGeometryChangeToleranceDip ||
      std::abs(state.lastObservedTaskFrameHeight - taskFrameHeight) > kDisplayGeometryChangeToleranceDip;

  state.lastObservedRootWidth = rootWidth;
  state.lastObservedRootHeight = rootHeight;
  state.lastObservedRasterizationScale = rasterizationScale;
  state.lastObservedTaskFrameWidth = taskFrameWidth;
  state.lastObservedTaskFrameHeight = taskFrameHeight;
  state.hasLastDisplayGeometrySignature = true;

  return changed;
}
void ApplySettings(HWND hTaskbarWnd);
int64_t DelayedApplyNowMs() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
             std::chrono::steady_clock::now().time_since_epoch())
      .count();
}
int GetCompositionFrameIntervalMs(HWND hwnd) {
  DWM_TIMING_INFO timing{};
  timing.cbSize = sizeof(timing);
  if (SUCCEEDED(DwmGetCompositionTimingInfo(hwnd, &timing)) &&
      timing.rateRefresh.uiNumerator > 0 &&
      timing.rateRefresh.uiDenominator > 0) {
    const double detectedRefreshHz =
        static_cast<double>(timing.rateRefresh.uiNumerator) /
        static_cast<double>(timing.rateRefresh.uiDenominator);
    const double clampedRefreshHz = std::clamp(detectedRefreshHz,
                                               kMinimumTrustedRefreshHz,
                                               kMaximumTrustedRefreshHz);
    return ClampInt(static_cast<int>((1000.0 / clampedRefreshHz) + 0.5),
                    kMinimumFrameIntervalMs,
                    kMaximumFrameIntervalMs);
  }
  return kDefaultFrameIntervalMs;
}
int GetTaskbarIslandFollowupPassCount(HWND hwnd) {
  const int frameIntervalMs = GetCompositionFrameIntervalMs(hwnd);
  const int followupWindowMs =
      kTaskbarIslandAnimationDurationMs +
      (frameIntervalMs * kAnimationFollowupGraceFrames);
  return std::max(1, (followupWindowMs + frameIntervalMs - 1) / frameIntervalMs);
}
void ArmStyleApplyPasses(int passCount, bool resetAnimationTargets = false) {
  passCount = std::max(1, passCount);
  g_force_style_apply_passes.store(passCount);
  g_high_priority_dispatch_passes.store(passCount);
  if (resetAnimationTargets) {
    g_reset_animation_target_passes.store(passCount);
  }
}
void ArmStyleFollowupPasses(HWND hwnd, bool resetAnimationTargets = false) {
  ArmStyleApplyPasses(GetTaskbarIslandFollowupPassCount(hwnd), resetAnimationTargets);
}
void ArmSingleStyleApplyPass(bool resetAnimationTargets = false) {
  ArmStyleApplyPasses(1, resetAnimationTargets);
}
bool WaitForConditionWithTimeout(std::function<bool()> condition,
                                 int timeoutMs,
                                 int pollIntervalMs) {
  const int64_t startMs = DelayedApplyNowMs();
  while (!condition()) {
    if (g_unloading && timeoutMs > 0) {
      // During unload we still wait briefly for workers/hooks to exit, but never
      // spin forever inside code that may be unloaded by recompilation.
    }
    const int64_t elapsedMs = DelayedApplyNowMs() - startMs;
    if (elapsedMs >= timeoutMs) {
      return false;
    }
    Sleep(static_cast<DWORD>(std::max(1, pollIntervalMs)));
  }
  return true;
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
    const int frameIntervalMs = GetCompositionFrameIntervalMs(hTaskbarWnd);
    const int followupWindowMs =
        kTaskbarIslandAnimationDurationMs +
        (frameIntervalMs * kAnimationFollowupGraceFrames);
    int elapsedMs = 0;
    while (elapsedMs < followupWindowMs) {
      Sleep(static_cast<DWORD>(frameIntervalMs));
      elapsedMs += frameIntervalMs;
      if (g_unloading || !hTaskbarWnd || !IsWindow(hTaskbarWnd)) {
        break;
      }
      ArmSingleStyleApplyPass();
      ApplySettings(hTaskbarWnd);
    }
    g_animation_followup_worker_running = false;
  }).detach();
}

void RequestTaskbarButtonSizeRelayout() {
  if (g_unloading) {
    return;
  }

  // Taskbar buttons are virtualized/recycled. Instead of forcing an arbitrary
  // number of relayout passes, mark button widths as customized and kick one
  // immediate style pass. Every realized/recycled button is validated in the
  // normal taskbar child loop and fixed only if its width is wrong.
  g_taskbarButtonWidthCustomized = true;
  ArmSingleStyleApplyPass(true);
  g_scheduled_low_priority_update = false;
  g_suppress_low_priority_apply_until_ms =
      DelayedApplyNowMs() + kButtonSizeLowPrioritySuppressionMs;

  HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
  if (hTaskbarWnd && IsWindow(hTaskbarWnd)) {
    ApplySettings(hTaskbarWnd);
  }
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
    WaitForConditionWithTimeout(
      [] { return !g_delayed_apply_worker_running.load(); },
      kDelayedApplyWorkerShutdownTimeoutMs,
      kWorkerShutdownPollMs);
  if (g_delayed_apply_worker_running.load()) {
    Wh_Log(L"Delayed apply worker did not exit before unload");
  }
  WaitForConditionWithTimeout(
      [] { return !g_animation_followup_worker_running.load(); },
      kAnimationFollowupWorkerShutdownTimeoutMs,
      kWorkerShutdownPollMs);
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
      DWORD sleepMs = static_cast<DWORD>(std::min<int64_t>(kDelayedWorkerMaxSleepMs, std::max<int64_t>(1, dueMs - nowMs)));
      Sleep(sleepMs);
      continue;
    }
    unsigned long long generation = g_delayed_apply_generation.load();
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd || !IsWindow(hTaskbarWnd)) {
      Wh_Log(L"Delayed apply postponed: taskbar window is not ready");
      g_delayed_apply_generation.fetch_add(1);
      g_delayed_apply_due_ms = DelayedApplyNowMs() + kInitialExplorerStyleRetryDelayMs;
      continue;
    }
    g_scheduled_low_priority_update = false;
    Wh_Log(L"Delayed apply triggered");
       const bool initialStyleApply = !g_initial_style_apply_completed.load();
    if (initialStyleApply) {
           ArmStyleFollowupPasses(hTaskbarWnd, true);
    }
    if (!g_initial_style_apply_completed.load() &&
        !g_initial_taskbar_size_apply_done.exchange(true)) {
      ApplySettingsTBIconSize(g_settings_tbiconsize.taskbarHeight);
    }
    ApplySettings(hTaskbarWnd);
    if (!g_initial_style_apply_completed.load() && !g_unloading) {
      Wh_Log(L"Initial ApplyStyle did not complete; retrying delayed apply");
      g_delayed_apply_generation.fetch_add(1);
      g_delayed_apply_due_ms = DelayedApplyNowMs() + kInitialExplorerStyleRetryDelayMs;
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
  HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
  ArmStyleFollowupPasses(hTaskbarWnd);
  g_suppress_low_priority_apply_until_ms =
      DelayedApplyNowMs() + kGeometryCriticalLowPrioritySuppressionMs;
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

  HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
  int64_t lastMs = g_last_geometry_critical_apply_ms.load();
  while (nowMs - lastMs >= kGeometryCriticalApplyMinIntervalMs) {
    if (g_last_geometry_critical_apply_ms.compare_exchange_weak(lastMs, nowMs)) {
      g_scheduled_low_priority_update = false;
      ArmStyleFollowupPasses(hTaskbarWnd);
      g_suppress_low_priority_apply_until_ms =
          nowMs + kGeometryCriticalLowPrioritySuppressionMs;
      ApplySettingsDebounced(1);
      if (hTaskbarWnd && IsWindow(hTaskbarWnd)) {
        QueueTaskbarAnimationFollowup(hTaskbarWnd);
      }
      return;
    }
  }

  ArmStyleFollowupPasses(hTaskbarWnd);
}
void ScheduleInitialExplorerStyleApply() {
  ApplySettingsDebounced(kInitialExplorerStyleDelayMs);
}