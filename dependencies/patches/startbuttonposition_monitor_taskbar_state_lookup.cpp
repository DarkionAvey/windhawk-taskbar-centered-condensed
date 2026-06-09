GetMonitorInfo(monitor, &monitorInfo);
auto monitorName = GetMonitorName(monitor);
TaskbarFlyoutStateSnapshot taskbarState;
if (!TryGetTaskbarFlyoutStateSnapshot(monitorName, &taskbarState)) {
    return original();
}
