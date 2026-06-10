bool IsTaskbarGeometryMessage(UINT msg) {
  switch (msg) {
    case WM_SIZE:
    case WM_WINDOWPOSCHANGED:
    case WM_SETTINGCHANGE:
    case WM_DISPLAYCHANGE:
    case WM_DPICHANGED:
      return true;
    default:
      return false;
  }
}
using TrayUI__Hide_t = void(WINAPI*)(void* pThis);
TrayUI__Hide_t TrayUI__Hide_Original;
void WINAPI TrayUI__Hide_Hook(void* pThis) {
  if (TrayUI__Hide_Original) {
    TrayUI__Hide_Original(pThis);
  }
  ApplySettingsFromTaskbarThreadIfRequired();
}
using CSecondaryTray__AutoHide_t = void(WINAPI*)(void* pThis, bool param1);
CSecondaryTray__AutoHide_t CSecondaryTray__AutoHide_Original;
void WINAPI CSecondaryTray__AutoHide_Hook(void* pThis, bool param1) {
  if (CSecondaryTray__AutoHide_Original) {
    CSecondaryTray__AutoHide_Original(pThis, param1);
  }
  ApplySettingsFromTaskbarThreadIfRequired();
}
#ifndef HSHELL_GETMINRECT
#define HSHELL_GETMINRECT 5
#endif
static UINT g_shellHookMessageTai = 0;
static bool TryCorrectShellHookMinRectMessageTai(UINT Msg, WPARAM wParam, LPARAM lParam);

using TrayUI_WndProc_t = LRESULT(WINAPI*)(void* pThis, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam, bool* flag);
TrayUI_WndProc_t TrayUI_WndProc_Original;
LRESULT WINAPI TrayUI_WndProc_Hook(void* pThis, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam, bool* flag) {
  RecordTaskbarInvocationMonitorTai(hWnd, Msg);
  LRESULT ret = TrayUI_WndProc_Original
      ? TrayUI_WndProc_Original(pThis, hWnd, Msg, wParam, lParam, flag)
      : 0;
    if (TryCorrectShellHookMinRectMessageTai(Msg, wParam, lParam)) {
        return ret;
      }
  if (IsTaskbarGeometryMessage(Msg)) {
    ApplySettingsFromTaskbarThreadGeometryChanged();
  } else {
    ApplySettingsFromTaskbarThreadIfRequired();
  }
  return ret;
}
using CSecondaryTray_v_WndProc_t = LRESULT(WINAPI*)(void* pThis, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
CSecondaryTray_v_WndProc_t CSecondaryTray_v_WndProc_Original;
LRESULT WINAPI CSecondaryTray_v_WndProc_Hook(void* pThis, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
  RecordTaskbarInvocationMonitorTai(hWnd, Msg);
  LRESULT ret = CSecondaryTray_v_WndProc_Original
      ? CSecondaryTray_v_WndProc_Original(pThis, hWnd, Msg, wParam, lParam)
      : 0;
       if (TryCorrectShellHookMinRectMessageTai(Msg, wParam, lParam)) {
    return ret;
  }
  if (IsTaskbarGeometryMessage(Msg)) {
    ApplySettingsFromTaskbarThreadGeometryChanged();
  } else {
    ApplySettingsFromTaskbarThreadIfRequired();
  }
  return ret;
}
using CTaskBand__ProcessWindowDestroyed_t = void(WINAPI*)(void* pThis, void* pHwnd);
CTaskBand__ProcessWindowDestroyed_t CTaskBand__ProcessWindowDestroyed_Original;
void WINAPI CTaskBand__ProcessWindowDestroyed_Hook(void* pThis, void* pHwnd) {
  Wh_Log(L"CTaskBand::CTaskBand__ProcessWindowDestroyed_Hook Hook");
  if (CTaskBand__ProcessWindowDestroyed_Original) {
    CTaskBand__ProcessWindowDestroyed_Original(pThis, pHwnd);
  }
  ApplySettingsFromTaskbarThreadImmediately();
}
using CTaskBand__InsertItem_t = long(WINAPI*)(void* pThis, void* pHwnd, void** ppTaskItem, void* pHwnd1, void* pHwnd2);
CTaskBand__InsertItem_t CTaskBand__InsertItem_Original;
long WINAPI CTaskBand__InsertItem_Hook(void* pThis, void* pHwnd, void** ppTaskItem, void* pHwnd1, void* pHwnd2) {
  Wh_Log(L"CTaskBand::_InsertItem Hook");
  auto original_call = CTaskBand__InsertItem_Original
      ? CTaskBand__InsertItem_Original(pThis, pHwnd, ppTaskItem, pHwnd1, pHwnd2)
      : E_FAIL;
  ApplySettingsFromTaskbarThreadImmediately();
  return original_call;
}
using CTaskBand__UpdateAllIcons_t = void(WINAPI*)(void* pThis);
CTaskBand__UpdateAllIcons_t CTaskBand__UpdateAllIcons_Original;
void WINAPI CTaskBand__UpdateAllIcons_Hook(void* pThis) {
  Wh_Log(L"CTaskBand::_UpdateAllIcons Hook");
  if (CTaskBand__UpdateAllIcons_Original) {
    CTaskBand__UpdateAllIcons_Original(pThis);
  }
  ApplySettingsFromTaskbarThreadIfRequired();
}
using CTaskBand__TaskOrderChanged_t = void(WINAPI*)(void* pThis, void* pTaskGroup, int param);
CTaskBand__TaskOrderChanged_t CTaskBand__TaskOrderChanged_Original;
void WINAPI CTaskBand__TaskOrderChanged_Hook(void* pThis, void* pTaskGroup, int param) {
  Wh_Log(L"CTaskBand::TaskOrderChanged Hook");
  if (CTaskBand__TaskOrderChanged_Original) {
    CTaskBand__TaskOrderChanged_Original(pThis, pTaskGroup, param);
  }
  ApplySettingsFromTaskbarThreadImmediately();
}

using CImpWndProc__WndProc_t = __int64(WINAPI*)(void* pThis, void* pHwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam);
CImpWndProc__WndProc_t CImpWndProc__WndProc_Original;
__int64 WINAPI CImpWndProc__WndProc_Hook(void* pThis, void* pHwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam) {
  RecordTaskbarInvocationMonitorTai(reinterpret_cast<HWND>(pHwnd), msg);
  __int64 ret = CImpWndProc__WndProc_Original
      ? CImpWndProc__WndProc_Original(pThis, pHwnd, msg, wParam, lParam)
      : 0;
  if (TryCorrectShellHookMinRectMessageTai(msg, static_cast<WPARAM>(wParam), static_cast<LPARAM>(lParam))) {
    return ret;
  }
  ApplySettingsFromTaskbarThreadIfRequired();
  return ret;
}
using CTaskBand__WndProc_t = __int64(WINAPI*)(void* pThis, void* pHwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam);
CTaskBand__WndProc_t CTaskBand__WndProc_Original;
__int64 WINAPI CTaskBand__WndProc_Hook(void* pThis, void* pHwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam) {
  RecordTaskbarInvocationMonitorTai(reinterpret_cast<HWND>(pHwnd), msg);
  __int64 ret = CTaskBand__WndProc_Original
      ? CTaskBand__WndProc_Original(pThis, pHwnd, msg, wParam, lParam)
      : 0;
  if (TryCorrectShellHookMinRectMessageTai(msg, static_cast<WPARAM>(wParam), static_cast<LPARAM>(lParam))) {
    return ret;
  }
  if (IsTaskbarGeometryMessage(msg)) {
    ApplySettingsFromTaskbarThreadGeometryChanged();
  } else {
    ApplySettingsFromTaskbarThreadIfRequired();
  }
  return ret;
}
using CTaskListWnd__WndProc_t = __int64(WINAPI*)(void* pThis, void* pHwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam);
CTaskListWnd__WndProc_t CTaskListWnd__WndProc_Original;
__int64 WINAPI CTaskListWnd__WndProc_Hook(void* pThis, void* pHwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam) {
  RecordTaskbarInvocationMonitorTai(reinterpret_cast<HWND>(pHwnd), msg);
  __int64 ret = CTaskListWnd__WndProc_Original
      ? CTaskListWnd__WndProc_Original(pThis, pHwnd, msg, wParam, lParam)
      : 0;
  if (TryCorrectShellHookMinRectMessageTai(msg, static_cast<WPARAM>(wParam), static_cast<LPARAM>(lParam))) {
    return ret;
  }
  if (IsTaskbarGeometryMessage(msg)) {
    ApplySettingsFromTaskbarThreadGeometryChanged();
  } else {
    ApplySettingsFromTaskbarThreadIfRequired();
  }
  return ret;
}
using CSecondaryTaskBand__WndProc_t = __int64(WINAPI*)(void* pThis, void* pHwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam);
CSecondaryTaskBand__WndProc_t CSecondaryTaskBand__WndProc_Original;
__int64 WINAPI CSecondaryTaskBand__WndProc_Hook(void* pThis, void* pHwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam) {
  RecordTaskbarInvocationMonitorTai(reinterpret_cast<HWND>(pHwnd), msg);
  __int64 ret = CSecondaryTaskBand__WndProc_Original
      ? CSecondaryTaskBand__WndProc_Original(pThis, pHwnd, msg, wParam, lParam)
      : 0;
  if (TryCorrectShellHookMinRectMessageTai(msg, static_cast<WPARAM>(wParam), static_cast<LPARAM>(lParam))) {
    return ret;
  }
  if (IsTaskbarGeometryMessage(msg)) {
    ApplySettingsFromTaskbarThreadGeometryChanged();
  } else {
    ApplySettingsFromTaskbarThreadIfRequired();
  }
  return ret;
}
using CTraySearchControl__WndProc_t = __int64(WINAPI*)(void* pThis, void* pHwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam);
CTraySearchControl__WndProc_t CTraySearchControl__WndProc_Original;
__int64 WINAPI CTraySearchControl__WndProc_Hook(void* pThis, void* pHwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam) {
  RecordTaskbarInvocationMonitorTai(reinterpret_cast<HWND>(pHwnd), msg);
  ApplySettingsFromTaskbarThreadIfRequired();
  return CTraySearchControl__WndProc_Original
      ? CTraySearchControl__WndProc_Original(pThis, pHwnd, msg, wParam, lParam)
      : 0;
}
interface ITaskGroup;
interface ITaskItem;
using CTaskBand__UpdateItemIcon_WithArgs_t = void(WINAPI*)(void* pThis, ITaskGroup* param1, ITaskItem* param2);
CTaskBand__UpdateItemIcon_WithArgs_t CTaskBand__UpdateItemIcon_WithArgs_Original;
void WINAPI CTaskBand__UpdateItemIcon_WithArgs_Hook(void* pThis, ITaskGroup* param1, ITaskItem* param2) {
  Wh_Log(L"Method called: CTaskBand__UpdateItemIcon");
  if (CTaskBand__UpdateItemIcon_WithArgs_Original) {
    CTaskBand__UpdateItemIcon_WithArgs_Original(pThis, param1, param2);
  }
  ApplySettingsFromTaskbarThreadIfRequired();
}
using CTaskBand_RemoveIcon_WithArgs_t = void(WINAPI*)(void* pThis, ITaskItem* param1);
CTaskBand_RemoveIcon_WithArgs_t CTaskBand_RemoveIcon_WithArgs_Original;
void WINAPI CTaskBand_RemoveIcon_WithArgs_Hook(void* pThis, ITaskItem* param1) {
  Wh_Log(L"Method called: CTaskBand_RemoveIcon");
  if (CTaskBand_RemoveIcon_WithArgs_Original) {
    CTaskBand_RemoveIcon_WithArgs_Original(pThis, param1);
  }
  ApplySettingsFromTaskbarThreadImmediately();
}
using ITaskbarSettings_get_Alignment_t = HRESULT(WINAPI*)(void* pThis, int* alignment);
ITaskbarSettings_get_Alignment_t ITaskbarSettings_get_Alignment_Original;
HRESULT WINAPI ITaskbarSettings_get_Alignment_Hook(void* pThis, int* alignment) {
  HRESULT ret = ITaskbarSettings_get_Alignment_Original
      ? ITaskbarSettings_get_Alignment_Original(pThis, alignment)
      : E_FAIL;
  Wh_Log(L"Method called: ITaskbarSettings_get_Alignment_Hook alignment: %d", alignment ? *alignment : -1);
  if (alignment && SUCCEEDED(ret)) {
    *alignment = 1;
  }
  return ret;
}
#include <windowsx.h>
using CTaskListWnd_ComputeJumpViewPosition_t = HRESULT(WINAPI*)(void* pThis, void* taskBtnGroup, int param2, winrt::Windows::Foundation::Point* point, HorizontalAlignment* horizontalAlignment, VerticalAlignment* verticalAlignment);
CTaskListWnd_ComputeJumpViewPosition_t CTaskListWnd_ComputeJumpViewPosition_Original;
HRESULT WINAPI CTaskListWnd_ComputeJumpViewPosition_Hook(void* pThis, void* taskBtnGroup, int param2, winrt::Windows::Foundation::Point* point, HorizontalAlignment* horizontalAlignment, VerticalAlignment* verticalAlignment) {
  HRESULT ret = CTaskListWnd_ComputeJumpViewPosition_Original
      ? CTaskListWnd_ComputeJumpViewPosition_Original(pThis, taskBtnGroup, param2, point, horizontalAlignment, verticalAlignment)
      : E_FAIL;
  if (point) {
    DWORD messagePos = GetMessagePos();
    POINT pt{
        GET_X_LPARAM(messagePos),
        GET_Y_LPARAM(messagePos),
    };
    point->X = pt.x;
  }
  return ret;
}
using TrayUI__OnDPIChanged_WithoutArgs_t = void(WINAPI*)(void* pThis);
TrayUI__OnDPIChanged_WithoutArgs_t TrayUI__OnDPIChanged_WithoutArgs_Original;
void WINAPI TrayUI__OnDPIChanged_WithoutArgs_Hook(void* pThis) {
               if (TrayUI__OnDPIChanged_WithoutArgs_Original) {
                   TrayUI__OnDPIChanged_WithoutArgs_Original(pThis);
               }
               RequestTaskbarDimensionInvalidation();
            }
