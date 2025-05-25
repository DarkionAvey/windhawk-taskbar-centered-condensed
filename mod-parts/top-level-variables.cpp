void ApplySettingsDebounced(int delayMs);
void ApplySettingsDebounced();
void ApplySettingsFromTaskbarThreadIfRequired();
int g_lastRecordedStartMenuWidth=670;
std::atomic<bool> g_already_requested_debounce_initializing = false;
STDAPI GetDpiForMonitor(HMONITOR hmonitor, MONITOR_DPI_TYPE dpiType, UINT* dpiX, UINT* dpiY);


