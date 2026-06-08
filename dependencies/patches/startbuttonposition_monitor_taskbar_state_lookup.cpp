GetMonitorInfo(monitor, &monitorInfo);
auto monitorName = GetMonitorName(monitor);
auto iterationTbStates = g_taskbarStates.find(monitorName);
if (iterationTbStates == g_taskbarStates.end()) {
    return original();
}
TaskbarState& taskbarState = iterationTbStates->second;
