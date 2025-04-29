DispatcherTimer debounceTimer{nullptr};
int debounceDelayMs = 300;
winrt::event_token debounceToken{};
void ApplySettings(HWND hTaskbarWnd);
bool InitializeDebounce() {
  if (debounceTimer) return true;
  debounceTimer = DispatcherTimer();
  debounceTimer.Interval(winrt::Windows::Foundation::TimeSpan(std::chrono::milliseconds(debounceDelayMs)));
  debounceToken = debounceTimer.Tick([](winrt::Windows::Foundation::IInspectable const&, winrt::Windows::Foundation::IInspectable const&) {
    debounceTimer.Stop();
    if (auto debounceHwnd = GetTaskbarWnd()) {
      Wh_Log(L"Debounce triggered");
      ApplySettings(debounceHwnd);
    }
  });
  return false;
}
void CleanupDebounce() {
  if (debounceTimer) {
    debounceTimer.Stop();
    debounceTimer.Tick(debounceToken);  // remove handler
    debounceTimer = nullptr;
  }
}
void ApplySettingsDebounced(int delayMs = 500) {
  if (!debounceTimer) return;
  HWND hTaskbarWnd = GetTaskbarWnd();
  if (!hTaskbarWnd) return;

  debounceDelayMs = delayMs;
  RunFromWindowThread(hTaskbarWnd, [](void* pParam) {
          debounceTimer.Stop();
          debounceTimer.Interval(winrt::Windows::Foundation::TimeSpan{std::chrono::milliseconds(debounceDelayMs)});
          debounceTimer.Start();
  }, 0);
}