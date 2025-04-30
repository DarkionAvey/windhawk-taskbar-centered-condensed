using TrayUI__Hide_t = void(WINAPI*)(void* pThis);
TrayUI__Hide_t TrayUI__Hide_Original;
void WINAPI TrayUI__Hide_Hook(void* pThis) {
  TrayUI__Hide_Original(pThis);
  ApplySettingsDebounced();
}
using CSecondaryTray__AutoHide_t = void(WINAPI*)(void* pThis, bool param1);
CSecondaryTray__AutoHide_t CSecondaryTray__AutoHide_Original;
void WINAPI CSecondaryTray__AutoHide_Hook(void* pThis, bool param1) {
  CSecondaryTray__AutoHide_Original(pThis, param1);
  ApplySettingsDebounced();
}
using TrayUI_WndProc_t = LRESULT(WINAPI*)(void* pThis, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam, bool* flag);
TrayUI_WndProc_t TrayUI_WndProc_Original;
LRESULT WINAPI TrayUI_WndProc_Hook(void* pThis, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam, bool* flag) {
  ApplySettingsFromTaskbarThreadIfRequired();
  return TrayUI_WndProc_Original(pThis, hWnd, Msg, wParam, lParam, flag);
}
using CSecondaryTray_v_WndProc_t = LRESULT(WINAPI*)(void* pThis, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
CSecondaryTray_v_WndProc_t CSecondaryTray_v_WndProc_Original;
LRESULT WINAPI CSecondaryTray_v_WndProc_Hook(void* pThis, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
  ApplySettingsFromTaskbarThreadIfRequired();
  return CSecondaryTray_v_WndProc_Original(pThis, hWnd, Msg, wParam, lParam);
}
using CTaskBand__ProcessWindowDestroyed_t = void(WINAPI*)(void* pThis, void* pHwnd);
CTaskBand__ProcessWindowDestroyed_t CTaskBand__ProcessWindowDestroyed_Original;
void WINAPI CTaskBand__ProcessWindowDestroyed_Hook(void* pThis, void* pHwnd) {
  Wh_Log(L"CTaskBand::CTaskBand__ProcessWindowDestroyed_Hook Hook");
  CTaskBand__ProcessWindowDestroyed_Original(pThis, pHwnd);
  ApplySettingsDebounced();
}
using CTaskBand__InsertItem_t = long(WINAPI*)(void* pThis, void* pHwnd, void** ppTaskItem, void* pHwnd1, void* pHwnd2);
CTaskBand__InsertItem_t CTaskBand__InsertItem_Original;
long WINAPI CTaskBand__InsertItem_Hook(void* pThis, void* pHwnd, void** ppTaskItem, void* pHwnd1, void* pHwnd2) {
  Wh_Log(L"CTaskBand::_InsertItem Hook");
  auto original_call = CTaskBand__InsertItem_Original(pThis, pHwnd, ppTaskItem, pHwnd1, pHwnd2);
  ApplySettingsDebounced(100);
  return original_call;
}
using CTaskBand__UpdateAllIcons_t = void(WINAPI*)(void* pThis);
CTaskBand__UpdateAllIcons_t CTaskBand__UpdateAllIcons_Original;
void WINAPI CTaskBand__UpdateAllIcons_Hook(void* pThis) {
  Wh_Log(L"CTaskBand::_UpdateAllIcons Hook");
  CTaskBand__UpdateAllIcons_Original(pThis);
  ApplySettingsDebounced();
}
using CTaskBand__TaskOrderChanged_t = void(WINAPI*)(void* pThis, void* pTaskGroup, int param);
CTaskBand__TaskOrderChanged_t CTaskBand__TaskOrderChanged_Original;
void WINAPI CTaskBand__TaskOrderChanged_Hook(void* pThis, void* pTaskGroup, int param) {
  Wh_Log(L"CTaskBand::TaskOrderChanged Hook");
  CTaskBand__TaskOrderChanged_Original(pThis, pTaskGroup, param);
  ApplySettingsDebounced();
}
using CImpWndProc__WndProc_t = __int64(WINAPI*)(void* pThis, void* pHwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam);
CImpWndProc__WndProc_t CImpWndProc__WndProc_Original;
__int64 WINAPI CImpWndProc__WndProc_Hook(void* pThis, void* pHwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam) {
  ApplySettingsFromTaskbarThreadIfRequired();
  return CImpWndProc__WndProc_Original(pThis, pHwnd, msg, wParam, lParam);
}
using CTaskBand__WndProc_t = __int64(WINAPI*)(void* pThis, void* pHwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam);
CTaskBand__WndProc_t CTaskBand__WndProc_Original;
__int64 WINAPI CTaskBand__WndProc_Hook(void* pThis, void* pHwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam) {
  ApplySettingsFromTaskbarThreadIfRequired();
  return CTaskBand__WndProc_Original(pThis, pHwnd, msg, wParam, lParam);
}
using CTaskListWnd__WndProc_t = __int64(WINAPI*)(void* pThis, void* pHwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam);
CTaskListWnd__WndProc_t CTaskListWnd__WndProc_Original;
__int64 WINAPI CTaskListWnd__WndProc_Hook(void* pThis, void* pHwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam) {
  ApplySettingsFromTaskbarThreadIfRequired();
  return CTaskListWnd__WndProc_Original(pThis, pHwnd, msg, wParam, lParam);
}
using CSecondaryTaskBand__WndProc_t = __int64(WINAPI*)(void* pThis, void* pHwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam);
CSecondaryTaskBand__WndProc_t CSecondaryTaskBand__WndProc_Original;
__int64 WINAPI CSecondaryTaskBand__WndProc_Hook(void* pThis, void* pHwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam) {
  ApplySettingsFromTaskbarThreadIfRequired();
  return CSecondaryTaskBand__WndProc_Original(pThis, pHwnd, msg, wParam, lParam);
}
using CTraySearchControl__WndProc_t = __int64(WINAPI*)(void* pThis, void* pHwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam);
CTraySearchControl__WndProc_t CTraySearchControl__WndProc_Original;
__int64 WINAPI CTraySearchControl__WndProc_Hook(void* pThis, void* pHwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam) {
  ApplySettingsFromTaskbarThreadIfRequired();
  return CTraySearchControl__WndProc_Original(pThis, pHwnd, msg, wParam, lParam);
}
interface ITaskGroup;
interface ITaskItem;
using CTaskBand__UpdateItemIcon_WithArgs_t = void(WINAPI*)(void* pThis, ITaskGroup* param1, ITaskItem* param2);
CTaskBand__UpdateItemIcon_WithArgs_t CTaskBand__UpdateItemIcon_WithArgs_Original;
void WINAPI CTaskBand__UpdateItemIcon_WithArgs_Hook(void* pThis, ITaskGroup* param1, ITaskItem* param2) {
  Wh_Log(L"Method called: CTaskBand__UpdateItemIcon");
  CTaskBand__UpdateItemIcon_WithArgs_Original(pThis, param1, param2);
  ApplySettingsDebounced(100);
}
using CTaskBand_RemoveIcon_WithArgs_t = void(WINAPI*)(void* pThis, ITaskItem* param1);
CTaskBand_RemoveIcon_WithArgs_t CTaskBand_RemoveIcon_WithArgs_Original;
void WINAPI CTaskBand_RemoveIcon_WithArgs_Hook(void* pThis, ITaskItem* param1) {
  Wh_Log(L"Method called: CTaskBand_RemoveIcon");
  CTaskBand_RemoveIcon_WithArgs_Original(pThis, param1);
  ApplySettingsDebounced();
}
