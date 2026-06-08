using TaskbarController_OnGroupingModeChanged_t = void(WINAPI*)(void* pThis);
TaskbarController_OnGroupingModeChanged_t
    TaskbarController_OnGroupingModeChanged_Hook_Original;
void WINAPI TaskbarController_OnGroupingModeChanged_Hook(void* pThis) {
    Wh_Log(L"TaskbarController::OnGroupingModeChanged Hook");
    TaskbarController_OnGroupingModeChanged_Hook_Original(pThis);
    ApplySettingsFromTaskbarThreadGeometryChanged();
}
