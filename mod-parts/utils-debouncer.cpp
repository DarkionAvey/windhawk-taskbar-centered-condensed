std::atomic<bool> g_scheduled_low_priority_update = false;
int debounceDelayMs = 300;
winrt::event_token debounceToken{};
void ApplySettings(HWND hTaskbarWnd);
bool InitializeDebounce() {
  if (debounceTimer) return true;
  g_already_requested_debounce_initializing=true;
  debounceTimer = DispatcherTimer();
  debounceTimer.Interval(winrt::Windows::Foundation::TimeSpan(std::chrono::milliseconds(debounceDelayMs)));
  debounceToken = debounceTimer.Tick([](winrt::Windows::Foundation::IInspectable const&, winrt::Windows::Foundation::IInspectable const&) {
    g_scheduled_low_priority_update = false;
    debounceTimer.Stop();
    if (auto debounceHwnd = GetTaskbarWnd()) {
      Wh_Log(L"Debounce triggered");
      ApplySettings(debounceHwnd);
    }
  });
  return false;
}

void CleanupDebounce() {
  g_already_requested_debounce_initializing=false;
  if (debounceTimer) {
    if (auto debounceHwnd = GetTaskbarWnd()) {
      RunFromWindowThread(
          debounceHwnd,
          [](void* pParam) {
            g_scheduled_low_priority_update = false;
            debounceTimer.Stop();
            debounceTimer.Tick(debounceToken);  // remove handler
            debounceTimer = nullptr;
          },
          0);
    }
  }
}

void ApplySettingsDebounced(int delayMs) {
  HWND hTaskbarWnd = GetTaskbarWnd();
  if (!hTaskbarWnd) {
    Wh_Log(L"ApplySettingsDebounced aborted: could not find hTaskbarWnd");
    return;
  }

if (!debounceTimer) {
    if(!g_already_requested_debounce_initializing){
        g_already_requested_debounce_initializing = true;
        ApplySettingsFromTaskbarThread();
        Wh_Log(L"ApplySettingsDebounced aborted: debounceTimer is null; initializing");
    }
    return;
  }

  bool lowPriority = false;
  if (delayMs <= 0) {
    lowPriority = true;
    delayMs = 700;
  }
  debounceDelayMs = delayMs;
  if (!lowPriority) RunFromWindowThread(hTaskbarWnd, [](void* pParam) { debounceTimer.Stop(); }, 0);
  RunFromWindowThread(
      hTaskbarWnd,
      [](void* pParam) {
        debounceTimer.Interval(winrt::Windows::Foundation::TimeSpan{std::chrono::milliseconds(debounceDelayMs)});
        debounceTimer.Start();
      },
      0);
}

void ApplySettingsDebounced() { ApplySettingsDebounced(100); }