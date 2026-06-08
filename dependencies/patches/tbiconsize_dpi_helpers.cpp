std::wstring GetMonitorName(HMONITOR monitor) {
    MONITORINFOEX monitorInfo = {};
    monitorInfo.cbSize = sizeof(MONITORINFOEX);
    if (monitor && GetMonitorInfo(monitor, &monitorInfo)) {
        return std::wstring(monitorInfo.szDevice);
    }
    return L"default";
}

std::wstring GetMonitorName(HWND hwnd) {
    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    return GetMonitorName(monitor);
}

STDAPI GetDpiForMonitor
