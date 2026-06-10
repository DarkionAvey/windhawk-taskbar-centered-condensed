if (!GetMonitorInfo(monitor, &monitorInfo)) {
    return original();
}
HMONITOR windowMonitor =
    MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
UINT windowDpiX = monitorDpiX;
UINT windowDpiY = monitorDpiY;
if (windowMonitor &&
    (FAILED(GetDpiForMonitor(windowMonitor, MDT_DEFAULT, &windowDpiX,
                             &windowDpiY)) ||
     windowDpiX == 0 || windowDpiY == 0)) {
    windowDpiX = monitorDpiX;
    windowDpiY = monitorDpiY;
}
auto monitorName = GetMonitorName(monitor);
TaskbarFlyoutStateSnapshot taskbarState;
if (!TryGetTaskbarFlyoutStateSnapshot(monitorName, &taskbarState)) {
    return original();
}
