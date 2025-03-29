// ==WindhawkMod==
// @id              taskbar-centered-condensed
// @name            Dock-like taskbar for Windows 11
// @description     Centers and floats the taskbar, moves the system tray next to the task area, and serves as an all-in-one, one-click mod to transform the taskbar into a MacOS-style dock. Based on m417z's code. For Windows 11.
// @version         1.0.0
// @author          DarkionAvey
// @github          https://github.com/DarkionAvey/windhawk-taskbar-centered-condensed
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -ldwmapi -lole32 -loleaut32 -lruntimeobject -lshcore
// ==/WindhawkMod==

// This mod is based on code released under the GNU General Public License v3.0.
// For bug reports and feedback:
// https://github.com/DarkionAvey/windhawk-taskbar-centered-condensed

// ==WindhawkModReadme==
/*
<p align="center">
  <img alt="Logo" src="/logo-raster.png" height="200"/>
</p>

![Screenshot](/screenshot.png)

# Docklike Windhawk Mod ⚒️ Under development
This mod turns Windows 11 taskbar into MacOS-like dock with proper smooth animations without sacrificing the funcitonality of the default taskbar as when using custom apps. It is currently under development. Pull requests are welcome.

# How to install
Copy [`assembled-mod.cpp`](https://raw.githubusercontent.com/DarkionAvey/windhawk-taskbar-centered-condensed/refs/heads/main/assembled-mod.cpp) contents to clipboard, then go to WindHawk > Explore > Create a new mod > Compile Mod

# Source code
You will find the actual code for the mod under [`mod-parts/docklike-mod.cpp`](https://github.com/DarkionAvey/windhawk-taskbar-centered-condensed/blob/main/mod-parts/docklike-mod.cpp). The rest of the lines are auto generated using the python code. Please don't edit the `assembled-mod.cpp`, the edits will be overwritten during the next build cycle. Instead, edit the source files found in `mod-parts`

# Credits
This mod wouldn't have been possible without `m417z`'s mods, so many thanks to him!

# Options

| Property | Name | Description | Accepted values |
| --- | --- | --- | --- |
| `TrayTaskGap` | Tray task gap | Adjusts the space between task area and tray area. | Integer (whole number) |
| `TaskbarBackgroundHorizontalPadding` | Taskbar background horizontal padding | Adjusts the horizontal padding on both sides of the taskbar background. | Integer (whole number) |
| `TaskbarOffsetY` | Taskbar vertical offset | Adjusts the vertical offset of the entire taskbar; Padding with the same value will be applied to the top. | Non-negative integer |
| `TaskbarHeight` | Taskbar height | Adjusts the height of the entire taskbar. | Non-negative integer |
| `TaskbarIconSize` | Taskbar icon size | Adjusts the width and height of the taskbar icons. | Non-negative integer |
| `TaskbarButtonSize` | Taskbar button size | Adjusts the width and height of taskbar button (surrounding the icons). | Non-negative integer |
| `TaskbarCornerRadius` | Corner radius of task bar | Defines the corner rounding of the taskbar. | Non-negative integer |
| `TaskButtonCornerRadius` | Corner radius of the task buttons | Defines the corner rounding of individual task buttons. | Non-negative integer |
| `FlatTaskbarBottomCorners` | Flat bottom corners | If enabled, the bottom corners of the taskbar will be squared instead of rounded and the taskbar will dock to the edge of the screen. Overrides the taskbar offset setting. | Boolean (true/false) |
| `TaskbarBackgroundOpacity` | Background opacity percentage |  | Non-negative integer |
| `TaskbarBackgroundTint` | Background tint percentage (high = greyscale) |  | Non-negative integer |
| `TaskbarBackgroundLuminosity` | Background luminosity percentage  (high = opaque, low = glassy) |  | Non-negative integer |
| `TaskbarBorderOpacity` | The opacity of the border surrounding the taskbar |  | Non-negative integer |
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- TrayTaskGap: 4
  $name: Tray task gap
  $description: Adjusts the space between task area and tray area.


- TaskbarBackgroundHorizontalPadding: 4
  $name: Taskbar background horizontal padding
  $description: Adjusts the horizontal padding on both sides of the taskbar background.


- TaskbarOffsetY: 6
  $name: Taskbar vertical offset
  $description: Adjusts the vertical offset of the entire taskbar; Padding with the same value will be applied to the top.


- TaskbarHeight: 78
  $name: Taskbar height
  $description: Adjusts the height of the entire taskbar.


- TaskbarIconSize: 40
  $name: Taskbar icon size
  $description: Adjusts the width and height of the taskbar icons.


- TaskbarButtonSize: 70
  $name: Taskbar button size
  $description: Adjusts the width and height of taskbar button (surrounding the icons).


- TaskbarCornerRadius: 24
  $name: Corner radius of task bar
  $description: Defines the corner rounding of the taskbar.


- TaskButtonCornerRadius: 16
  $name: Corner radius of the task buttons
  $description: Defines the corner rounding of individual task buttons.


- FlatTaskbarBottomCorners: false
  $name: Flat bottom corners
  $description: If enabled, the bottom corners of the taskbar will be squared instead of rounded and the taskbar will dock to the edge of the screen. Overrides the taskbar offset setting.


- TaskbarBackgroundOpacity: 100
  $name: Background opacity percentage


- TaskbarBackgroundTint: 0
  $name: Background tint percentage (high = greyscale)


- TaskbarBackgroundLuminosity: 30
  $name: Background luminosity percentage  (high = opaque, low = glassy)


- TaskbarBorderOpacity: 30
  $name: The opacity of the border surrounding the taskbar
*/
// ==/WindhawkModSettings==




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////  .___________.     ___           _______. __  ___ .______        ___      .______       __    __   _______  __    _______  __    __  .___________.  ////
////  |           |    /   \         /       ||  |/  / |   _  \      /   \     |   _  \     |  |  |  | |   ____||  |  /  _____||  |  |  | |           |  ////
////  `---|  |----`   /  ^  \       |   (----`|  '  /  |  |_)  |    /  ^  \    |  |_)  |    |  |__|  | |  |__   |  | |  |  __  |  |__|  | `---|  |----`  ////
////      |  |       /  /_\  \       \   \    |    <   |   _  <    /  /_\  \   |      /     |   __   | |   __|  |  | |  | |_ | |   __   |     |  |       ////
////      |  |      /  _____  \  .----)   |   |  .  \  |  |_)  |  /  _____  \  |  |\  \----.|  |  |  | |  |____ |  | |  |__| | |  |  |  |     |  |       ////
////      |__|     /__/     \__\ |_______/    |__|\__\ |______/  /__/     \__\ | _| `._____||__|  |__| |_______||__|  \______| |__|  |__|     |__|       ////
////                                                                                                                                                     ////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <windhawk_utils.h>
#undef GetCurrentTime
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <atomic>
#include <functional>
#include <limits>
#include <optional>
#ifdef _M_ARM64
#include <regex>
#endif
using namespace winrt::Windows::UI::Xaml;
#ifndef SPI_SETLOGICALDPIOVERRIDE
#define SPI_SETLOGICALDPIOVERRIDE 0x009F
#endif
struct {
    int iconSize;
    int taskbarHeight;
    int taskbarButtonWidth;
} g_settings_taskbarheight;
std::atomic<bool> g_taskbarViewDllLoaded;
std::atomic<bool> g_applyingSettings;
std::atomic<bool> g_pendingMeasureOverride;
std::atomic<bool> g_unloading;
std::atomic<int> g_hookCallCounter;
int g_originalTaskbarHeight;
int g_taskbarHeight;
bool g_inSystemTrayController_UpdateFrameSize;
bool g_taskbarButtonWidthCustomized;
bool g_inAugmentedEntryPointButton_UpdateButtonPadding;
double* double_48_value_Original;
WINUSERAPI UINT WINAPI GetDpiForWindow(HWND hwnd);
typedef enum MONITOR_DPI_TYPE {
    MDT_EFFECTIVE_DPI = 0,
    MDT_ANGULAR_DPI = 1,
    MDT_RAW_DPI = 2,
    MDT_DEFAULT = MDT_EFFECTIVE_DPI
} MONITOR_DPI_TYPE;
STDAPI GetDpiForMonitor(HMONITOR hmonitor,
                        MONITOR_DPI_TYPE dpiType,
                        UINT* dpiX,
                        UINT* dpiY);
FrameworkElement EnumChildElements(
    FrameworkElement element,
    std::function<bool(FrameworkElement)> enumCallback) {
    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < childrenCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (!child) {
            Wh_Log(L"Failed to get child %d of %d", i + 1, childrenCount);
            continue;
        }
        if (enumCallback(child)) {
            return child;
        }
    }
    return nullptr;
}
FrameworkElement FindChildByName(FrameworkElement element, PCWSTR name) {
    return EnumChildElements(element, [name](FrameworkElement child) {
        return child.Name() == name;
    });
}
FrameworkElement FindChildByClassName(FrameworkElement element,
                                      PCWSTR className) {
    return EnumChildElements(element, [className](FrameworkElement child) {
        return winrt::get_class_name(child) == className;
    });
}
using ResourceDictionary_Lookup_t = winrt::Windows::Foundation::IInspectable*(
    WINAPI*)(void* pThis,
             void** result,
             winrt::Windows::Foundation::IInspectable* key);
ResourceDictionary_Lookup_t ResourceDictionary_Lookup_Original;
winrt::Windows::Foundation::IInspectable* WINAPI
ResourceDictionary_Lookup_Hook(void* pThis,
                               void** result,
                               winrt::Windows::Foundation::IInspectable* key) {
    auto ret = ResourceDictionary_Lookup_Original(pThis, result, key);
    if (!*ret) {
        return ret;
    }
    auto keyString = key->try_as<winrt::hstring>();
    if (!keyString || keyString != L"MediumTaskbarButtonExtent") {
        return ret;
    }
    auto valueDouble = ret->try_as<double>();
    if (!valueDouble) {
        return ret;
    }
    double newValueDouble = g_settings_taskbarheight.taskbarButtonWidth;
    if (newValueDouble != *valueDouble) {
        Wh_Log(L"Overriding value %s: %f->%f", keyString->c_str(), *valueDouble,
               newValueDouble);
        *ret = winrt::box_value(newValueDouble);
    }
    return ret;
}
using IconUtils_GetIconSize_t = void(WINAPI*)(bool isSmall,
                                              int type,
                                              SIZE* size);
IconUtils_GetIconSize_t IconUtils_GetIconSize_Original;
void WINAPI IconUtils_GetIconSize_Hook(bool isSmall, int type, SIZE* size) {
    IconUtils_GetIconSize_Original(isSmall, type, size);
    if (!g_unloading && !isSmall) {
        size->cx = MulDiv(size->cx, g_settings_taskbarheight.iconSize, 24);
        size->cy = MulDiv(size->cy, g_settings_taskbarheight.iconSize, 24);
    }
}
using IconContainer_IsStorageRecreationRequired_t = bool(WINAPI*)(void* pThis,
                                                                  void* param1,
                                                                  int flags);
IconContainer_IsStorageRecreationRequired_t
    IconContainer_IsStorageRecreationRequired_Original;
bool WINAPI IconContainer_IsStorageRecreationRequired_Hook(void* pThis,
                                                           void* param1,
                                                           int flags) {
    if (g_applyingSettings) {
        return true;
    }
    return IconContainer_IsStorageRecreationRequired_Original(pThis, param1,
                                                              flags);
}
using TrayUI_GetMinSize_t = void(WINAPI*)(void* pThis,
                                          HMONITOR monitor,
                                          SIZE* size);
TrayUI_GetMinSize_t TrayUI_GetMinSize_Original;
void WINAPI TrayUI_GetMinSize_Hook(void* pThis, HMONITOR monitor, SIZE* size) {
    
    TrayUI_GetMinSize_Original(pThis, monitor, size);
    if (g_taskbarHeight) {
        UINT dpiX = 0;
        UINT dpiY = 0;
        GetDpiForMonitor(monitor, MDT_DEFAULT, &dpiX, &dpiY);
        size->cy = MulDiv(g_taskbarHeight, dpiY, 96);
    }
}
using CIconLoadingFunctions_GetClassLongPtrW_t = ULONG_PTR(WINAPI*)(void* pThis,
                                                                    HWND hWnd,
                                                                    int nIndex);
CIconLoadingFunctions_GetClassLongPtrW_t
    CIconLoadingFunctions_GetClassLongPtrW_Original;
ULONG_PTR WINAPI CIconLoadingFunctions_GetClassLongPtrW_Hook(void* pThis,
                                                             HWND hWnd,
                                                             int nIndex) {
    
    if (!g_unloading && nIndex == GCLP_HICON && g_settings_taskbarheight.iconSize <= 16) {
        nIndex = GCLP_HICONSM;
    }
    ULONG_PTR ret =
        CIconLoadingFunctions_GetClassLongPtrW_Original(pThis, hWnd, nIndex);
    return ret;
}
using CIconLoadingFunctions_SendMessageCallbackW_t =
    BOOL(WINAPI*)(void* pThis,
                  HWND hWnd,
                  UINT Msg,
                  WPARAM wParam,
                  LPARAM lParam,
                  SENDASYNCPROC lpResultCallBack,
                  ULONG_PTR dwData);
CIconLoadingFunctions_SendMessageCallbackW_t
    CIconLoadingFunctions_SendMessageCallbackW_Original;
BOOL WINAPI
CIconLoadingFunctions_SendMessageCallbackW_Hook(void* pThis,
                                                HWND hWnd,
                                                UINT Msg,
                                                WPARAM wParam,
                                                LPARAM lParam,
                                                SENDASYNCPROC lpResultCallBack,
                                                ULONG_PTR dwData) {
    
    if (!g_unloading && Msg == WM_GETICON && wParam == ICON_BIG &&
        g_settings_taskbarheight.iconSize <= 16) {
        wParam = ICON_SMALL2;
    }
    BOOL ret = CIconLoadingFunctions_SendMessageCallbackW_Original(
        pThis, hWnd, Msg, wParam, lParam, lpResultCallBack, dwData);
    return ret;
}
using TrayUI__StuckTrayChange_t = void(WINAPI*)(void* pThis);
TrayUI__StuckTrayChange_t TrayUI__StuckTrayChange_Original;
using TrayUI__HandleSettingChange_t = void(WINAPI*)(void* pThis,
                                                    void* param1,
                                                    void* param2,
                                                    void* param3,
                                                    void* param4);
TrayUI__HandleSettingChange_t TrayUI__HandleSettingChange_Original;
void WINAPI TrayUI__HandleSettingChange_Hook(void* pThis,
                                             void* param1,
                                             void* param2,
                                             void* param3,
                                             void* param4) {
    
    TrayUI__HandleSettingChange_Original(pThis, param1, param2, param3, param4);
    if (g_applyingSettings) {
        TrayUI__StuckTrayChange_Original(pThis);
    }
}
using TaskListItemViewModel_GetIconHeight_t = int(WINAPI*)(void* pThis,
                                                           void* param1,
                                                           double* iconHeight);
TaskListItemViewModel_GetIconHeight_t
    TaskListItemViewModel_GetIconHeight_Original;
int WINAPI TaskListItemViewModel_GetIconHeight_Hook(void* pThis,
                                                    void* param1,
                                                    double* iconHeight) {
    int ret =
        TaskListItemViewModel_GetIconHeight_Original(pThis, param1, iconHeight);
    if (!g_unloading) {
        *iconHeight = g_settings_taskbarheight.iconSize;
    }
    return ret;
}
using TaskListGroupViewModel_GetIconHeight_t = int(WINAPI*)(void* pThis,
                                                            void* param1,
                                                            double* iconHeight);
TaskListGroupViewModel_GetIconHeight_t
    TaskListGroupViewModel_GetIconHeight_Original;
int WINAPI TaskListGroupViewModel_GetIconHeight_Hook(void* pThis,
                                                     void* param1,
                                                     double* iconHeight) {
    int ret = TaskListGroupViewModel_GetIconHeight_Original(pThis, param1,
                                                            iconHeight);
    if (!g_unloading) {
        *iconHeight = g_settings_taskbarheight.iconSize;
    }
    return ret;
}
using TaskbarConfiguration_GetIconHeightInViewPixels_taskbarSizeEnum_t =
    double(WINAPI*)(int enumTaskbarSize);
TaskbarConfiguration_GetIconHeightInViewPixels_taskbarSizeEnum_t
    TaskbarConfiguration_GetIconHeightInViewPixels_taskbarSizeEnum_Original;
double WINAPI
TaskbarConfiguration_GetIconHeightInViewPixels_taskbarSizeEnum_Hook(
    int enumTaskbarSize) {
    Wh_Log(L"> %d", enumTaskbarSize);
    if (!g_unloading && (enumTaskbarSize == 1 || enumTaskbarSize == 2)) {
        return g_settings_taskbarheight.iconSize;
    }
    return TaskbarConfiguration_GetIconHeightInViewPixels_taskbarSizeEnum_Original(
        enumTaskbarSize);
}
using TaskbarConfiguration_GetIconHeightInViewPixels_double_t =
    double(WINAPI*)(double baseHeight);
TaskbarConfiguration_GetIconHeightInViewPixels_double_t
    TaskbarConfiguration_GetIconHeightInViewPixels_double_Original;
double WINAPI
TaskbarConfiguration_GetIconHeightInViewPixels_double_Hook(double baseHeight) {
    if (!g_unloading) {
        return g_settings_taskbarheight.iconSize;
    }
    return TaskbarConfiguration_GetIconHeightInViewPixels_double_Original(
        baseHeight);
}
using SystemTrayController_GetFrameSize_t =
    double(WINAPI*)(void* pThis, int enumTaskbarSize);
SystemTrayController_GetFrameSize_t SystemTrayController_GetFrameSize_Original;
double WINAPI SystemTrayController_GetFrameSize_Hook(void* pThis,
                                                     int enumTaskbarSize) {
    Wh_Log(L"> %d", enumTaskbarSize);
    if (g_taskbarHeight && (enumTaskbarSize == 1 || enumTaskbarSize == 2)) {
        return g_taskbarHeight;
    }
    return SystemTrayController_GetFrameSize_Original(pThis, enumTaskbarSize);
}
using SystemTraySecondaryController_GetFrameSize_t =
    double(WINAPI*)(void* pThis, int enumTaskbarSize);
SystemTraySecondaryController_GetFrameSize_t
    SystemTraySecondaryController_GetFrameSize_Original;
double WINAPI
SystemTraySecondaryController_GetFrameSize_Hook(void* pThis,
                                                int enumTaskbarSize) {
    Wh_Log(L"> %d", enumTaskbarSize);
    if (g_taskbarHeight && (enumTaskbarSize == 1 || enumTaskbarSize == 2)) {
        return g_taskbarHeight;
    }
    return SystemTraySecondaryController_GetFrameSize_Original(pThis,
                                                               enumTaskbarSize);
}
using TaskbarConfiguration_GetFrameSize_t =
    double(WINAPI*)(int enumTaskbarSize);
TaskbarConfiguration_GetFrameSize_t TaskbarConfiguration_GetFrameSize_Original;
double WINAPI TaskbarConfiguration_GetFrameSize_Hook(int enumTaskbarSize) {
    Wh_Log(L"> %d", enumTaskbarSize);
    if (!g_originalTaskbarHeight &&
        (enumTaskbarSize == 1 || enumTaskbarSize == 2)) {
        g_originalTaskbarHeight =
            TaskbarConfiguration_GetFrameSize_Original(enumTaskbarSize);
    }
    if (g_taskbarHeight && (enumTaskbarSize == 1 || enumTaskbarSize == 2)) {
        return g_taskbarHeight;
    }
    return TaskbarConfiguration_GetFrameSize_Original(enumTaskbarSize);
}
#ifdef _M_ARM64
thread_local double* g_TaskbarConfiguration_UpdateFrameSize_frameSize;
using TaskbarConfiguration_UpdateFrameSize_t = void(WINAPI*)(void* pThis);
TaskbarConfiguration_UpdateFrameSize_t
    TaskbarConfiguration_UpdateFrameSize_SymbolAddress;
TaskbarConfiguration_UpdateFrameSize_t
    TaskbarConfiguration_UpdateFrameSize_Original;
void WINAPI TaskbarConfiguration_UpdateFrameSize_Hook(void* pThis) {
    
    static LONG frameSizeOffset = []() -> LONG {
        const DWORD* start =
            (const DWORD*)TaskbarConfiguration_UpdateFrameSize_SymbolAddress;
        const DWORD* end = start + 0x80;
        std::regex regex1(R"(str\s+d\d+, \[x\d+, #0x([0-9a-f]+)\])");
        for (const DWORD* p = start; p != end; p++) {
            WH_DISASM_RESULT result1;
            if (!Wh_Disasm((void*)p, &result1)) {
                break;
            }
            std::string_view s1 = result1.text;
            if (s1 == "ret") {
                break;
            }
            std::match_results<std::string_view::const_iterator> match1;
            if (!std::regex_match(s1.begin(), s1.end(), match1, regex1)) {
                continue;
            }
            LONG offset = std::stoull(match1[1], nullptr, 16);
            Wh_Log(L"frameSizeOffset=0x%X", offset);
            return offset;
        }
        Wh_Log(L"frameSizeOffset not found");
        return 0;
    }();
    if (frameSizeOffset <= 0) {
        Wh_Log(L"frameSizeOffset <= 0");
        TaskbarConfiguration_UpdateFrameSize_Original(pThis);
        return;
    }
    g_TaskbarConfiguration_UpdateFrameSize_frameSize =
        (double*)((BYTE*)pThis + frameSizeOffset);
    TaskbarConfiguration_UpdateFrameSize_Original(pThis);
    g_TaskbarConfiguration_UpdateFrameSize_frameSize = nullptr;
}
using Event_operator_call_t = void(WINAPI*)(void* pThis);
Event_operator_call_t Event_operator_call_Original;
void WINAPI Event_operator_call_Hook(void* pThis) {
    
    if (g_TaskbarConfiguration_UpdateFrameSize_frameSize) {
        if (!g_originalTaskbarHeight) {
            g_originalTaskbarHeight =
                *g_TaskbarConfiguration_UpdateFrameSize_frameSize;
        }
        if (g_taskbarHeight) {
            *g_TaskbarConfiguration_UpdateFrameSize_frameSize = g_taskbarHeight;
        }
    }
    Event_operator_call_Original(pThis);
}
#endif  // _M_ARM64
using SystemTrayController_UpdateFrameSize_t = void(WINAPI*)(void* pThis);
SystemTrayController_UpdateFrameSize_t
    SystemTrayController_UpdateFrameSize_SymbolAddress;
SystemTrayController_UpdateFrameSize_t
    SystemTrayController_UpdateFrameSize_Original;
void WINAPI SystemTrayController_UpdateFrameSize_Hook(void* pThis) {
    
    static LONG lastHeightOffset = []() -> LONG {
#if defined(_M_X64)
        const BYTE* start =
            (const BYTE*)SystemTrayController_UpdateFrameSize_SymbolAddress;
        const BYTE* end = start + 0x200;
        for (const BYTE* p = start; p != end; p++) {
            if (p[0] == 0x66 && p[1] == 0x0F && p[2] == 0x2E && p[3] == 0xB3 &&
                p[8] == 0x7A && p[10] == 0x75) {
                LONG offset = *(LONG*)(p + 4);
                Wh_Log(L"lastHeightOffset=0x%X", offset);
                return offset;
            }
        }
#elif defined(_M_ARM64)
        const DWORD* start =
            (const DWORD*)SystemTrayController_UpdateFrameSize_SymbolAddress;
        const DWORD* end = start + 0x80;
        std::regex regex1(R"(ldr\s+d\d+, \[x\d+, #0x([0-9a-f]+)\])");
        std::regex regex2(R"(fcmp\s+d\d+, d\d+)");
        std::regex regex3(R"(b\.eq\s+0x[0-9a-f]+)");
        for (const DWORD* p = start; p != end; p++) {
            WH_DISASM_RESULT result1;
            if (!Wh_Disasm((void*)p, &result1)) {
                break;
            }
            std::string_view s1 = result1.text;
            if (s1 == "ret") {
                break;
            }
            std::match_results<std::string_view::const_iterator> match1;
            if (!std::regex_match(s1.begin(), s1.end(), match1, regex1)) {
                continue;
            }
            WH_DISASM_RESULT result2;
            if (!Wh_Disasm((void*)(p + 1), &result2)) {
                break;
            }
            std::string_view s2 = result2.text;
            if (!std::regex_match(s2.begin(), s2.end(), regex2)) {
                continue;
            }
            WH_DISASM_RESULT result3;
            if (!Wh_Disasm((void*)(p + 2), &result3)) {
                break;
            }
            std::string_view s3 = result3.text;
            if (!std::regex_match(s3.begin(), s3.end(), regex3)) {
                continue;
            }
            LONG offset = std::stoull(match1[1], nullptr, 16);
            Wh_Log(L"lastHeightOffset=0x%X", offset);
            return offset;
        }
#else
#error "Unsupported architecture"
#endif
        Wh_Log(L"lastHeightOffset not found");
        return 0;
    }();
    if (lastHeightOffset > 0) {
        *(double*)((BYTE*)pThis + lastHeightOffset) = 0;
    }
    g_inSystemTrayController_UpdateFrameSize = true;
    SystemTrayController_UpdateFrameSize_Original(pThis);
    g_inSystemTrayController_UpdateFrameSize = false;
}
using TaskbarFrame_MaxHeight_double_t = void(WINAPI*)(void* pThis,
                                                      double value);
TaskbarFrame_MaxHeight_double_t TaskbarFrame_MaxHeight_double_Original;
using TaskbarFrame_Height_double_t = void(WINAPI*)(void* pThis, double value);
TaskbarFrame_Height_double_t TaskbarFrame_Height_double_Original;
void WINAPI TaskbarFrame_Height_double_Hook(void* pThis, double value) {
    
    if (TaskbarFrame_MaxHeight_double_Original) {
        TaskbarFrame_MaxHeight_double_Original(
            pThis, std::numeric_limits<double>::infinity());
    }
    return TaskbarFrame_Height_double_Original(pThis, value);
}
void* TaskbarController_OnGroupingModeChanged;
using TaskbarController_UpdateFrameHeight_t = void(WINAPI*)(void* pThis);
TaskbarController_UpdateFrameHeight_t
    TaskbarController_UpdateFrameHeight_Original;
void WINAPI TaskbarController_UpdateFrameHeight_Hook(void* pThis) {
    
    static LONG taskbarFrameOffset = []() -> LONG {
#if defined(_M_X64)
        const BYTE* p = (const BYTE*)TaskbarController_OnGroupingModeChanged;
        if (p && p[0] == 0x48 && p[1] == 0x83 && p[2] == 0xEC &&
            (p[4] == 0x48 || p[4] == 0x4C) && p[5] == 0x8B &&
            (p[6] & 0xC0) == 0x80) {
            LONG offset = *(LONG*)(p + 7);
            Wh_Log(L"taskbarFrameOffset=0x%X", offset);
            return offset;
        }
#elif defined(_M_ARM64)
        const DWORD* start =
            (const DWORD*)TaskbarController_OnGroupingModeChanged;
        const DWORD* end = start + 10;
        std::regex regex1(R"(ldr\s+x\d+, \[x\d+, #0x([0-9a-f]+)\])");
        for (const DWORD* p = start; p != end; p++) {
            WH_DISASM_RESULT result1;
            if (!Wh_Disasm((void*)p, &result1)) {
                break;
            }
            std::string_view s1 = result1.text;
            if (s1 == "ret") {
                break;
            }
            std::match_results<std::string_view::const_iterator> match1;
            if (!std::regex_match(s1.begin(), s1.end(), match1, regex1)) {
                continue;
            }
            LONG offset = std::stoull(match1[1], nullptr, 16);
            Wh_Log(L"taskbarFrameOffset=0x%X", offset);
            return offset;
        }
#else
#error "Unsupported architecture"
#endif
        Wh_Log(L"taskbarFrameOffset not found");
        return 0;
    }();
    if (taskbarFrameOffset <= 0) {
        Wh_Log(L"taskbarFrameOffset <= 0");
        TaskbarController_UpdateFrameHeight_Original(pThis);
        return;
    }
    void* taskbarFrame = *(void**)((BYTE*)pThis + taskbarFrameOffset);
    if (!taskbarFrame) {
        Wh_Log(L"!taskbarFrame");
        TaskbarController_UpdateFrameHeight_Original(pThis);
        return;
    }
    FrameworkElement taskbarFrameElement = nullptr;
    ((IUnknown**)taskbarFrame)[1]->QueryInterface(
        winrt::guid_of<FrameworkElement>(),
        winrt::put_abi(taskbarFrameElement));
    if (!taskbarFrameElement) {
        Wh_Log(L"!taskbarFrameElement");
        TaskbarController_UpdateFrameHeight_Original(pThis);
        return;
    }
    taskbarFrameElement.MaxHeight(std::numeric_limits<double>::infinity());
    TaskbarController_UpdateFrameHeight_Original(pThis);
    auto contentGrid = Media::VisualTreeHelper::GetParent(taskbarFrameElement)
                           .try_as<FrameworkElement>();
    if (contentGrid) {
        double height = taskbarFrameElement.Height();
        double contentGridHeight = contentGrid.Height();
        if (contentGridHeight > 0 && contentGridHeight != height) {
            Wh_Log(L"Adjusting contentGrid.Height: %f->%f", contentGridHeight,
                   height);
            contentGrid.Height(height);
        }
    }
}
using SystemTraySecondaryController_UpdateFrameSize_t =
    void(WINAPI*)(void* pThis);
SystemTraySecondaryController_UpdateFrameSize_t
    SystemTraySecondaryController_UpdateFrameSize_Original;
void WINAPI SystemTraySecondaryController_UpdateFrameSize_Hook(void* pThis) {
    
    g_inSystemTrayController_UpdateFrameSize = true;
    SystemTraySecondaryController_UpdateFrameSize_Original(pThis);
    g_inSystemTrayController_UpdateFrameSize = false;
}
using SystemTrayFrame_Height_t = void(WINAPI*)(void* pThis, double value);
SystemTrayFrame_Height_t SystemTrayFrame_Height_Original;
void WINAPI SystemTrayFrame_Height_Hook(void* pThis, double value) {
    if (g_inSystemTrayController_UpdateFrameSize) {
        value = std::numeric_limits<double>::quiet_NaN();
    }
    SystemTrayFrame_Height_Original(pThis, value);
}
using TaskbarFrame_MeasureOverride_t =
    int(WINAPI*)(void* pThis,
                 winrt::Windows::Foundation::Size size,
                 winrt::Windows::Foundation::Size* resultSize);
TaskbarFrame_MeasureOverride_t TaskbarFrame_MeasureOverride_Original;
int WINAPI TaskbarFrame_MeasureOverride_Hook(
    void* pThis,
    winrt::Windows::Foundation::Size size,
    winrt::Windows::Foundation::Size* resultSize) {
    g_hookCallCounter++;
    
    int ret = TaskbarFrame_MeasureOverride_Original(pThis, size, resultSize);
    g_pendingMeasureOverride = false;
    g_hookCallCounter--;
    return ret;
}
void* TaskListButton_UpdateIconColumnDefinition_Original;
using TaskListButton_UpdateButtonPadding_t = void(WINAPI*)(void* pThis);
TaskListButton_UpdateButtonPadding_t
    TaskListButton_UpdateButtonPadding_Original;
using TaskListButton_UpdateVisualStates_t = void(WINAPI*)(void* pThis);
TaskListButton_UpdateVisualStates_t TaskListButton_UpdateVisualStates_Original;
void WINAPI TaskListButton_UpdateVisualStates_Hook(void* pThis) {
    
    if (TaskListButton_UpdateIconColumnDefinition_Original &&
        (g_applyingSettings || g_taskbarButtonWidthCustomized)) {
        static LONG mediumTaskbarButtonExtentOffset = []() -> LONG {
#if defined(_M_X64)
            const BYTE* start =
                (const BYTE*)TaskListButton_UpdateIconColumnDefinition_Original;
            const BYTE* end = start + 0x200;
            for (const BYTE* p = start; p != end; p++) {
                if (p[0] == 0xF2 && p[1] == 0x0F && p[2] == 0x10 &&
                    (p[3] & 0x81) == 0x81) {
                    LONG offset = *(LONG*)(p + 4);
                    Wh_Log(L"mediumTaskbarButtonExtentOffset=0x%X", offset);
                    return offset;
                }
                if (p[0] == 0xF2 && p[1] == 0x44 && p[2] == 0x0F &&
                    p[3] == 0x10 && (p[4] & 0x81) == 0x81) {
                    LONG offset = *(LONG*)(p + 5);
                    Wh_Log(L"mediumTaskbarButtonExtentOffset=0x%X", offset);
                    return offset;
                }
            }
#elif defined(_M_ARM64)
            const DWORD* start = (const DWORD*)
                TaskListButton_UpdateIconColumnDefinition_Original;
            const DWORD* end = start + 0x80;
            std::regex regex1(R"(fsub\s+d\d+, (d\d+), d\d+)");
            const DWORD* cmdWithReg1 = nullptr;
            std::string reg1;
            for (const DWORD* p = start; p != end; p++) {
                WH_DISASM_RESULT result1;
                if (!Wh_Disasm((void*)p, &result1)) {
                    break;
                }
                std::string_view s1 = result1.text;
                if (s1 == "ret") {
                    break;
                }
                std::match_results<std::string_view::const_iterator> match1;
                if (std::regex_match(s1.begin(), s1.end(), match1, regex1)) {
                    cmdWithReg1 = p;
                    reg1 = match1[1];
                    break;
                }
            }
            if (cmdWithReg1) {
                std::regex regex2(R"(ldr\s+(d\d+), \[x\d+, #0x([0-9a-f]+)\])");
                for (const DWORD* p = start; p != cmdWithReg1; p++) {
                    WH_DISASM_RESULT result1;
                    if (!Wh_Disasm((void*)p, &result1)) {
                        break;
                    }
                    std::string_view s1 = result1.text;
                    if (s1 == "ret") {
                        break;
                    }
                    std::match_results<std::string_view::const_iterator> match1;
                    if (!std::regex_match(s1.begin(), s1.end(), match1,
                                          regex2) ||
                        match1[1] != reg1) {
                        continue;
                    }
                    LONG offset = std::stoull(match1[2], nullptr, 16);
                    Wh_Log(L"mediumTaskbarButtonExtentOffset=0x%X", offset);
                    return offset;
                }
            }
#else
#error "Unsupported architecture"
#endif
            Wh_Log(L"mediumTaskbarButtonExtentOffset not found");
            return 0;
        }();
        if (mediumTaskbarButtonExtentOffset > 0) {
            double* mediumTaskbarButtonExtent =
                (double*)((BYTE*)pThis + mediumTaskbarButtonExtentOffset);
            if (*mediumTaskbarButtonExtent >= 1 &&
                *mediumTaskbarButtonExtent < 10000) {
                double newValue =
                    g_unloading ? 44 : g_settings_taskbarheight.taskbarButtonWidth;
                if (newValue != *mediumTaskbarButtonExtent) {
                    Wh_Log(
                        L"Updating MediumTaskbarButtonExtent for "
                        L"TaskListButton: %f->%f",
                        *mediumTaskbarButtonExtent, newValue);
                    *mediumTaskbarButtonExtent = newValue;
                    g_taskbarButtonWidthCustomized = true;
                    TaskListButton_UpdateButtonPadding_Original(pThis);
                }
            }
        }
    }
    TaskListButton_UpdateVisualStates_Original(pThis);
    if (g_applyingSettings) {
        FrameworkElement taskListButtonElement = nullptr;
        ((IUnknown*)pThis + 3)
            ->QueryInterface(winrt::guid_of<FrameworkElement>(),
                             winrt::put_abi(taskListButtonElement));
        if (taskListButtonElement) {
            if (auto iconPanelElement =
                    FindChildByName(taskListButtonElement, L"IconPanel")) {
                if (auto iconElement =
                        FindChildByName(iconPanelElement, L"Icon")) {
                    double iconSize = g_unloading ? 24 : g_settings_taskbarheight.iconSize;
                    iconElement.Width(iconSize);
                    iconElement.Height(iconSize);
                }
            }
        }
    }
}
using ExperienceToggleButton_UpdateButtonPadding_t = void(WINAPI*)(void* pThis);
ExperienceToggleButton_UpdateButtonPadding_t
    ExperienceToggleButton_UpdateButtonPadding_Original;
void WINAPI ExperienceToggleButton_UpdateButtonPadding_Hook(void* pThis) {
    
    ExperienceToggleButton_UpdateButtonPadding_Original(pThis);
    if (!g_applyingSettings) {
        return;
    }
    FrameworkElement toggleButtonElement = nullptr;
    ((IUnknown**)pThis)[1]->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(toggleButtonElement));
    if (!toggleButtonElement) {
        return;
    }
    auto panelElement =
        FindChildByName(toggleButtonElement, L"ExperienceToggleButtonRootPanel")
            .try_as<Controls::Grid>();
    if (!panelElement) {
        return;
    }
    auto className = winrt::get_class_name(toggleButtonElement);
    if (className == L"Taskbar.ExperienceToggleButton") {
    } else if (className == L"Taskbar.SearchBoxButton") {
        auto searchBoxTextBlock =
            FindChildByName(panelElement, L"SearchBoxTextBlock");
        if (searchBoxTextBlock &&
            searchBoxTextBlock.Visibility() != Visibility::Collapsed) {
            return;
        }
    } else {
        return;
    }
    double buttonWidth = panelElement.Width();
    if (!(buttonWidth > 0)) {
        return;
    }
    auto buttonPadding = panelElement.Padding();
    double newWidth = (g_unloading ? 44 : g_settings_taskbarheight.taskbarButtonWidth) - 4 +
                      (buttonPadding.Left + buttonPadding.Right);
    if (newWidth != buttonWidth) {
        Wh_Log(L"Updating MediumTaskbarButtonExtent for %s: %f->%f",
               className.c_str(), buttonWidth, newWidth);
        panelElement.Width(newWidth);
    }
}
using AugmentedEntryPointButton_UpdateButtonPadding_t =
    void(WINAPI*)(void* pThis);
AugmentedEntryPointButton_UpdateButtonPadding_t
    AugmentedEntryPointButton_UpdateButtonPadding_Original;
void WINAPI AugmentedEntryPointButton_UpdateButtonPadding_Hook(void* pThis) {
    
    g_inAugmentedEntryPointButton_UpdateButtonPadding = true;
    AugmentedEntryPointButton_UpdateButtonPadding_Original(pThis);
    g_inAugmentedEntryPointButton_UpdateButtonPadding = false;
}
using RepeatButton_Width_t = void(WINAPI*)(void* pThis, double width);
RepeatButton_Width_t RepeatButton_Width_Original;
void WINAPI RepeatButton_Width_Hook(void* pThis, double width) {
    
    RepeatButton_Width_Original(pThis, width);
    if (!g_inAugmentedEntryPointButton_UpdateButtonPadding) {
        return;
    }
    FrameworkElement button = nullptr;
    (*(IUnknown**)pThis)
        ->QueryInterface(winrt::guid_of<FrameworkElement>(),
                         winrt::put_abi(button));
    if (!button) {
        return;
    }
    FrameworkElement augmentedEntryPointContentGrid =
        FindChildByName(button, L"AugmentedEntryPointContentGrid");
    if (!augmentedEntryPointContentGrid) {
        return;
    }
    double marginValue = static_cast<double>(40 - g_settings_taskbarheight.iconSize) / 2;
    if (marginValue < 0) {
        marginValue = 0;
    }
    EnumChildElements(augmentedEntryPointContentGrid, [marginValue](
                                                          FrameworkElement
                                                              child) {
        if (winrt::get_class_name(child) != L"Windows.UI.Xaml.Controls.Grid") {
            return false;
        }
        FrameworkElement panelGrid =
            FindChildByClassName(child, L"Windows.UI.Xaml.Controls.Grid");
        if (!panelGrid) {
            return false;
        }
        FrameworkElement panel = FindChildByClassName(
            panelGrid, L"AdaptiveCards.Rendering.Uwp.WholeItemsPanel");
        if (!panel) {
            return false;
        }
        Wh_Log(L"Processing %f x %f widget", panelGrid.Width(),
               panelGrid.Height());
        double labelsTopBorderExtraMargin = 0;
        bool widePanel = panelGrid.Width() > panelGrid.Height();
        if (widePanel) {
            auto margin = Thickness{3, 3, 3, 3};
            if (!g_unloading && marginValue < 3) {
                labelsTopBorderExtraMargin = 3 - marginValue;
                margin.Left = marginValue;
                margin.Top = marginValue;
                margin.Right = marginValue;
                margin.Bottom = marginValue;
            }
            Wh_Log(L"Setting Margin=%f,%f,%f,%f for panel", margin.Left,
                   margin.Top, margin.Right, margin.Bottom);
            panel.Margin(margin);
            panelGrid.VerticalAlignment(g_unloading
                                            ? VerticalAlignment::Stretch
                                            : VerticalAlignment::Center);
        } else {
            auto margin = Thickness{8, 8, 8, 8};
            if (!g_unloading) {
                margin.Left = marginValue;
                margin.Top = marginValue;
                margin.Right = marginValue;
                margin.Bottom = marginValue;
                if (g_taskbarHeight < 48) {
                    margin.Top -= static_cast<double>(48 - g_taskbarHeight) / 2;
                    if (margin.Top < 0) {
                        margin.Top = 0;
                    }
                    margin.Bottom = marginValue * 2 - margin.Top;
                }
            }
            Wh_Log(L"Setting Margin=%f,%f,%f,%f for panel", margin.Left,
                   margin.Top, margin.Right, margin.Bottom);
            panel.Margin(margin);
        }
        FrameworkElement tickerGrid = panel;
        if ((tickerGrid = FindChildByClassName(
                 tickerGrid, L"Windows.UI.Xaml.Controls.Border")) &&
            (tickerGrid = FindChildByClassName(
                 tickerGrid, L"AdaptiveCards.Rendering.Uwp.WholeItemsPanel")) &&
            (tickerGrid = FindChildByClassName(
                 tickerGrid, L"Windows.UI.Xaml.Controls.Grid"))) {
        } else {
            return false;
        }
        double badgeMaxValue = g_unloading ? 24 : 40 - marginValue * 2;
        FrameworkElement badgeSmall = tickerGrid;
        if ((badgeSmall = FindChildByName(badgeSmall, L"SmallTicker1")) &&
            (badgeSmall = FindChildByClassName(
                 badgeSmall, L"AdaptiveCards.Rendering.Uwp.WholeItemsPanel")) &&
            (badgeSmall =
                 FindChildByName(badgeSmall, L"BadgeAnchorSmallTicker"))) {
            Wh_Log(L"Setting MaxWidth=%f, MaxHeight=%f for small badge",
                   badgeMaxValue, badgeMaxValue);
            badgeSmall.MaxWidth(badgeMaxValue);
            badgeSmall.MaxHeight(badgeMaxValue);
        }
        FrameworkElement badgeLarge = tickerGrid;
        if ((badgeLarge = FindChildByName(badgeLarge, L"LargeTicker1")) &&
            (badgeLarge = FindChildByClassName(
                 badgeLarge, L"AdaptiveCards.Rendering.Uwp.WholeItemsPanel")) &&
            (badgeLarge =
                 FindChildByName(badgeLarge, L"BadgeAnchorLargeTicker"))) {
            Wh_Log(L"Setting MaxWidth=%f, MaxHeight=%f for large badge",
                   badgeMaxValue, badgeMaxValue);
            badgeLarge.MaxWidth(badgeMaxValue);
            badgeLarge.MaxHeight(badgeMaxValue);
        }
        FrameworkElement labelsBorder = tickerGrid;
        if ((labelsBorder = FindChildByName(labelsBorder, L"LargeTicker2"))) {
            auto margin = Thickness{0, labelsTopBorderExtraMargin, 0, 0};
            Wh_Log(L"Setting Margin=%f,%f,%f,%f for labels border", margin.Left,
                   margin.Top, margin.Right, margin.Bottom);
            labelsBorder.Margin(margin);
        }
        return false;
    });
}
using SHAppBarMessage_t = decltype(&SHAppBarMessage);
SHAppBarMessage_t SHAppBarMessage_Original;
auto WINAPI SHAppBarMessage_Hook(DWORD dwMessage, PAPPBARDATA pData) {
    auto ret = SHAppBarMessage_Original(dwMessage, pData);
    if (dwMessage == ABM_QUERYPOS && ret && g_taskbarHeight) {
        pData->rc.top =
            pData->rc.bottom -
            MulDiv(g_taskbarHeight, GetDpiForWindow(pData->hWnd), 96);
    }
    return ret;
}
void LoadSettingsTaskbarHeight() {
    g_settings_taskbarheight.iconSize = Wh_GetIntSetting(L"TaskbarIconSize");
    g_settings_taskbarheight.taskbarHeight = Wh_GetIntSetting(L"TaskbarHeight") + (abs(Wh_GetIntSetting(L"TaskbarOffsetY"))*2);
    g_settings_taskbarheight.taskbarButtonWidth = Wh_GetIntSetting(L"TaskbarButtonSize");
}
HWND GetTaskbarWnd() {
    HWND hTaskbarWnd = FindWindow(L"Shell_TrayWnd", nullptr);
    DWORD processId = 0;
    if (!hTaskbarWnd || !GetWindowThreadProcessId(hTaskbarWnd, &processId) ||
        processId != GetCurrentProcessId()) {
        return nullptr;
    }
    return hTaskbarWnd;
}
bool ProtectAndMemcpy(DWORD protect, void* dst, const void* src, size_t size) {
    DWORD oldProtect;
    if (!VirtualProtect(dst, size, protect, &oldProtect)) {
        return false;
    }
    memcpy(dst, src, size);
    VirtualProtect(dst, size, oldProtect, &oldProtect);
    return true;
}
void ApplySettingsTaskbarHeight(int taskbarHeight) {
    if (taskbarHeight < 2) {
        taskbarHeight = 2;
    }
    HWND hTaskbarWnd = GetTaskbarWnd();
    if (!hTaskbarWnd) {
        Wh_Log(L"No taskbar found");
        g_taskbarHeight = taskbarHeight;
        return;
    }
    Wh_Log(L"Applying settings for taskbar %08X",
           (DWORD)(DWORD_PTR)hTaskbarWnd);
    if (!g_taskbarHeight) {
        RECT taskbarRect{};
        GetWindowRect(hTaskbarWnd, &taskbarRect);
        g_taskbarHeight = MulDiv(taskbarRect.bottom - taskbarRect.top, 96,
                                 GetDpiForWindow(hTaskbarWnd));
    }
    g_applyingSettings = true;
    if (taskbarHeight == g_taskbarHeight) {
        g_pendingMeasureOverride = true;
        g_taskbarHeight = taskbarHeight - 1;
        if (!TaskbarConfiguration_GetFrameSize_Original &&
            double_48_value_Original) {
            double tempTaskbarHeight = g_taskbarHeight;
            ProtectAndMemcpy(PAGE_READWRITE, double_48_value_Original,
                             &tempTaskbarHeight, sizeof(double));
        }
        SendMessage(hTaskbarWnd, WM_SETTINGCHANGE, SPI_SETLOGICALDPIOVERRIDE,
                    0);
        for (int i = 0; i < 100; i++) {
            if (!g_pendingMeasureOverride) {
                break;
            }
            Sleep(100);
        }
    }
    g_pendingMeasureOverride = true;
    g_taskbarHeight = taskbarHeight;
    if (!TaskbarConfiguration_GetFrameSize_Original &&
        double_48_value_Original) {
        double tempTaskbarHeight = g_taskbarHeight;
        ProtectAndMemcpy(PAGE_READWRITE, double_48_value_Original,
                         &tempTaskbarHeight, sizeof(double));
    }
    SendMessage(hTaskbarWnd, WM_SETTINGCHANGE, SPI_SETLOGICALDPIOVERRIDE, 0);
    for (int i = 0; i < 100; i++) {
        if (!g_pendingMeasureOverride) {
            break;
        }
        Sleep(100);
    }
    HWND hReBarWindow32 =
        FindWindowEx(hTaskbarWnd, nullptr, L"ReBarWindow32", nullptr);
    if (hReBarWindow32) {
        HWND hMSTaskSwWClass =
            FindWindowEx(hReBarWindow32, nullptr, L"MSTaskSwWClass", nullptr);
        if (hMSTaskSwWClass) {
            SendMessage(hMSTaskSwWClass, 0x452, 3, 0);
        }
    }
    g_applyingSettings = false;
}
bool HookTaskbarViewDllSymbols(HMODULE module) {
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] =  //
        {
            {
                {LR"(__real@4048000000000000)"},
                &double_48_value_Original,
                nullptr,
                true,
            },
            {
                {
                    LR"(public: __cdecl winrt::impl::consume_Windows_Foundation_Collections_IMap<struct winrt::Windows::UI::Xaml::ResourceDictionary,struct winrt::Windows::Foundation::IInspectable,struct winrt::Windows::Foundation::IInspectable>::Lookup(struct winrt::Windows::Foundation::IInspectable const &)const )",
                    LR"(public: struct winrt::Windows::Foundation::IInspectable __cdecl winrt::impl::consume_Windows_Foundation_Collections_IMap<struct winrt::Windows::UI::Xaml::ResourceDictionary,struct winrt::Windows::Foundation::IInspectable,struct winrt::Windows::Foundation::IInspectable>::Lookup(struct winrt::Windows::Foundation::IInspectable const &)const )",
                },
                &ResourceDictionary_Lookup_Original,
                ResourceDictionary_Lookup_Hook,
            },
            {
                {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListItemViewModel,struct winrt::Taskbar::ITaskListItemViewModel>::GetIconHeight(void *,double *))"},
                &TaskListItemViewModel_GetIconHeight_Original,
                TaskListItemViewModel_GetIconHeight_Hook,
                true,  // Gone in KB5040527 (Taskbar.View.dll 2124.16310.10.0).
            },
            {
                {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListGroupViewModel,struct winrt::Taskbar::ITaskbarAppItemViewModel>::GetIconHeight(void *,double *))"},
                &TaskListGroupViewModel_GetIconHeight_Original,
                TaskListGroupViewModel_GetIconHeight_Hook,
                true,  // Missing in older Windows 11 versions.
            },
            {
                {LR"(public: static double __cdecl winrt::Taskbar::implementation::TaskbarConfiguration::GetIconHeightInViewPixels(enum winrt::WindowsUdk::UI::Shell::TaskbarSize))"},
                &TaskbarConfiguration_GetIconHeightInViewPixels_taskbarSizeEnum_Original,
                TaskbarConfiguration_GetIconHeightInViewPixels_taskbarSizeEnum_Hook,
            },
            {
                {LR"(public: static double __cdecl winrt::Taskbar::implementation::TaskbarConfiguration::GetIconHeightInViewPixels(double))"},
                &TaskbarConfiguration_GetIconHeightInViewPixels_double_Original,
                TaskbarConfiguration_GetIconHeightInViewPixels_double_Hook,
                true,  // From Windows 11 version 22H2.
            },
            {
                {LR"(private: double __cdecl winrt::SystemTray::implementation::SystemTrayController::GetFrameSize(enum winrt::WindowsUdk::UI::Shell::TaskbarSize))"},
                &SystemTrayController_GetFrameSize_Original,
                SystemTrayController_GetFrameSize_Hook,
                true,  // From Windows 11 version 22H2, inlined sometimes.
            },
            {
                {LR"(private: double __cdecl winrt::SystemTray::implementation::SystemTraySecondaryController::GetFrameSize(enum winrt::WindowsUdk::UI::Shell::TaskbarSize))"},
                &SystemTraySecondaryController_GetFrameSize_Original,
                SystemTraySecondaryController_GetFrameSize_Hook,
                true,  // From Windows 11 version 22H2.
            },
            {
                {LR"(public: static double __cdecl winrt::Taskbar::implementation::TaskbarConfiguration::GetFrameSize(enum winrt::WindowsUdk::UI::Shell::TaskbarSize))"},
                &TaskbarConfiguration_GetFrameSize_Original,
                TaskbarConfiguration_GetFrameSize_Hook,
                true,  // From Windows 11 version 22H2.
            },
#ifdef _M_ARM64
            {
                {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskbarConfiguration::UpdateFrameSize(void))"},
                &TaskbarConfiguration_UpdateFrameSize_SymbolAddress,
                nullptr,  // Hooked manually, we need the symbol address.
            },
            {
                {LR"(public: void __cdecl winrt::event<struct winrt::delegate<> >::operator()<>(void))"},
                &Event_operator_call_Original,
                Event_operator_call_Hook,
            },
#endif
            {
                {LR"(private: void __cdecl winrt::SystemTray::implementation::SystemTrayController::UpdateFrameSize(void))"},
                &SystemTrayController_UpdateFrameSize_SymbolAddress,
                nullptr,  // Hooked manually, we need the symbol address.
                true,     // Missing in older Windows 11 versions.
            },
            {
                {LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::Taskbar::implementation::TaskbarFrame>::MaxHeight(double)const )"},
                &TaskbarFrame_MaxHeight_double_Original,
                nullptr,
                true,  // From Windows 11 version 22H2.
            },
            {
                {
                    LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::Taskbar::implementation::TaskbarFrame>::Height(double)const )",
                    LR"(public: void __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::Taskbar::implementation::TaskbarFrame>::Height(double)const )",
                },
                &TaskbarFrame_Height_double_Original,
                TaskbarFrame_Height_double_Hook,
                true,  // Gone in Windows 11 version 24H2.
            },
            {
                {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskbarController::OnGroupingModeChanged(void))"},
                &TaskbarController_OnGroupingModeChanged,
                nullptr,
                true,  // Missing in older Windows 11 versions.
            },
            {
                {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskbarController::UpdateFrameHeight(void))"},
                &TaskbarController_UpdateFrameHeight_Original,
                TaskbarController_UpdateFrameHeight_Hook,
                true,  // Missing in older Windows 11 versions.
            },
            {
                {LR"(private: void __cdecl winrt::SystemTray::implementation::SystemTraySecondaryController::UpdateFrameSize(void))"},
                &SystemTraySecondaryController_UpdateFrameSize_Original,
                SystemTraySecondaryController_UpdateFrameSize_Hook,
                true,  // Missing in older Windows 11 versions.
            },
            {
                {LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::SystemTray::SystemTrayFrame>::Height(double)const )"},
                &SystemTrayFrame_Height_Original,
                SystemTrayFrame_Height_Hook,
                true,  // From Windows 11 version 22H2.
            },
            {
                {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::IFrameworkElementOverrides>::MeasureOverride(struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size *))"},
                &TaskbarFrame_MeasureOverride_Original,
                TaskbarFrame_MeasureOverride_Hook,
            },
            {
                {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateIconColumnDefinition(void))"},
                &TaskListButton_UpdateIconColumnDefinition_Original,
                nullptr,
                true,  // Missing in older Windows 11 versions.
            },
            {
                {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateButtonPadding(void))"},
                &TaskListButton_UpdateButtonPadding_Original,
            },
            {
                {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateVisualStates(void))"},
                &TaskListButton_UpdateVisualStates_Original,
                TaskListButton_UpdateVisualStates_Hook,
            },
            {
                {LR"(protected: virtual void __cdecl winrt::Taskbar::implementation::ExperienceToggleButton::UpdateButtonPadding(void))"},
                &ExperienceToggleButton_UpdateButtonPadding_Original,
                ExperienceToggleButton_UpdateButtonPadding_Hook,
            },
            {
                {LR"(protected: virtual void __cdecl winrt::Taskbar::implementation::AugmentedEntryPointButton::UpdateButtonPadding(void))"},
                &AugmentedEntryPointButton_UpdateButtonPadding_Original,
                AugmentedEntryPointButton_UpdateButtonPadding_Hook,
            },
            {
                {LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::Windows::UI::Xaml::Controls::Primitives::RepeatButton>::Width(double)const )"},
                &RepeatButton_Width_Original,
                RepeatButton_Width_Hook,
                true,  // From Windows 11 version 22H2.
            },
        };
    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }
#ifdef _M_ARM64
    if (TaskbarConfiguration_UpdateFrameSize_SymbolAddress) {
        WindhawkUtils::Wh_SetFunctionHookT(
            TaskbarConfiguration_UpdateFrameSize_SymbolAddress,
            TaskbarConfiguration_UpdateFrameSize_Hook,
            &TaskbarConfiguration_UpdateFrameSize_Original);
    }
#endif
    if (SystemTrayController_UpdateFrameSize_SymbolAddress) {
        WindhawkUtils::Wh_SetFunctionHookT(
            SystemTrayController_UpdateFrameSize_SymbolAddress,
            SystemTrayController_UpdateFrameSize_Hook,
            &SystemTrayController_UpdateFrameSize_Original);
    }
    return true;
}
bool HookTaskbarDllSymbolsTaskbarHeight() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }
    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(void __cdecl IconUtils::GetIconSize(bool,enum IconUtils::IconType,struct tagSIZE *))"},
            &IconUtils_GetIconSize_Original,
            IconUtils_GetIconSize_Hook,
        },
        {
            {LR"(public: virtual bool __cdecl IconContainer::IsStorageRecreationRequired(class CCoSimpleArray<unsigned int,4294967294,class CSimpleArrayStandardCompareHelper<unsigned int> > const &,enum IconContainerFlags))"},
            &IconContainer_IsStorageRecreationRequired_Original,
            IconContainer_IsStorageRecreationRequired_Hook,
        },
        {
            {LR"(public: virtual void __cdecl TrayUI::GetMinSize(struct HMONITOR__ *,struct tagSIZE *))"},
            &TrayUI_GetMinSize_Original,
            TrayUI_GetMinSize_Hook,
            true,
        },
        {
            {LR"(public: virtual unsigned __int64 __cdecl CIconLoadingFunctions::GetClassLongPtrW(struct HWND__ *,int))"},
            &CIconLoadingFunctions_GetClassLongPtrW_Original,
            CIconLoadingFunctions_GetClassLongPtrW_Hook,
        },
        {
            {LR"(public: virtual int __cdecl CIconLoadingFunctions::SendMessageCallbackW(struct HWND__ *,unsigned int,unsigned __int64,__int64,void (__cdecl*)(struct HWND__ *,unsigned int,unsigned __int64,__int64),unsigned __int64))"},
            &CIconLoadingFunctions_SendMessageCallbackW_Original,
            CIconLoadingFunctions_SendMessageCallbackW_Hook,
        },
        {
            {LR"(public: void __cdecl TrayUI::_StuckTrayChange(void))"},
            &TrayUI__StuckTrayChange_Original,
        },
        {
            {LR"(public: void __cdecl TrayUI::_HandleSettingChange(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
            &TrayUI__HandleSettingChange_Original,
            TrayUI__HandleSettingChange_Hook,
        },
    };
    if (!HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }
    return true;
}
HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }
    return module;
}
using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (!module) {
        return module;
    }
    if (!g_taskbarViewDllLoaded && GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoaded.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);
        if (HookTaskbarViewDllSymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }
    return module;
}
BOOL Wh_ModInitTaskbarHeight() {
    
    LoadSettingsTaskbarHeight();
    if (!HookTaskbarDllSymbolsTaskbarHeight()) {
        return FALSE;
    }
    WindhawkUtils::Wh_SetFunctionHookT(SHAppBarMessage, SHAppBarMessage_Hook,
                                       &SHAppBarMessage_Original);
    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoaded = true;
        if (!HookTaskbarViewDllSymbols(taskbarViewModule)) {
            return FALSE;
        }
    } else {
        Wh_Log(L"Taskbar view module not loaded yet");
        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW =
            (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule,
                                                      "LoadLibraryExW");
        WindhawkUtils::Wh_SetFunctionHookT(pKernelBaseLoadLibraryExW,
                                           LoadLibraryExW_Hook,
                                           &LoadLibraryExW_Original);
    }
    return TRUE;
}
void Wh_ModAfterInitTaskbarHeight() {
    
    if (!g_taskbarViewDllLoaded) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewDllLoaded.exchange(true)) {
                Wh_Log(L"Got Taskbar.View.dll");
                if (HookTaskbarViewDllSymbols(taskbarViewModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }
    ApplySettingsTaskbarHeight(g_settings_taskbarheight.taskbarHeight);
}
void Wh_ModBeforeUninitTaskbarHeight() {
    
    g_unloading = true;
    ApplySettingsTaskbarHeight(g_originalTaskbarHeight ? g_originalTaskbarHeight : 48);
}
void Wh_ModUninitTaskbarHeight() {
    
    while (g_hookCallCounter > 0) {
        Sleep(100);
    }
}
void Wh_ModSettingsChangedTaskbarHeight() {
    
    LoadSettingsTaskbarHeight();
    ApplySettingsTaskbarHeight(g_settings_taskbarheight.taskbarHeight);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////       _______..___________.     ___      .______      .___________..______    __    __  .___________..___________.  ______   .__   __. .______     ______        _______. __  .___________. __    ______   .__   __.   ////
////      /       ||           |    /   \     |   _  \     |           ||   _  \  |  |  |  | |           ||           | /  __  \  |  \ |  | |   _  \   /  __  \      /       ||  | |           ||  |  /  __  \  |  \ |  |   ////
////     |   (----``---|  |----`   /  ^  \    |  |_)  |    `---|  |----`|  |_)  | |  |  |  | `---|  |----``---|  |----`|  |  |  | |   \|  | |  |_)  | |  |  |  |    |   (----`|  | `---|  |----`|  | |  |  |  | |   \|  |   ////
////      \   \        |  |       /  /_\  \   |      /         |  |     |   _  <  |  |  |  |     |  |         |  |     |  |  |  | |  . `  | |   ___/  |  |  |  |     \   \    |  |     |  |     |  | |  |  |  | |  . `  |   ////
////  .----)   |       |  |      /  _____  \  |  |\  \----.    |  |     |  |_)  | |  `--'  |     |  |         |  |     |  `--'  | |  |\   | |  |      |  `--'  | .----)   |   |  |     |  |     |  | |  `--'  | |  |\   |   ////
////  |_______/        |__|     /__/     \__\ | _| `._____|    |__|     |______/   \______/      |__|         |__|      \______/  |__| \__| | _|       \______/  |_______/    |__|     |__|     |__|  \______/  |__| \__|   ////
////                                                                                                                                                                                                                        ////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ApplyStyle(XamlRoot xamlRoot);
#include <windhawk_utils.h>
#include <atomic>
#include <functional>
#include <dwmapi.h>
#undef GetCurrentTime
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>
using namespace winrt::Windows::UI::Xaml;
struct {
    bool startMenuOnTheLeft;
    int startMenuWidth;
} g_settings_startbuttonposition;
HWND g_startMenuWnd;
int g_startMenuOriginalWidth;
HWND g_searchMenuWnd;
int g_searchMenuOriginalX;
STDAPI GetDpiForMonitor(HMONITOR hmonitor,
                        MONITOR_DPI_TYPE dpiType,
                        UINT* dpiX,
                        UINT* dpiY);
void* CTaskBand_ITaskListWndSite_vftable;
void* CSecondaryTaskBand_ITaskListWndSite_vftable;
using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;
using CSecondaryTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis,
                                                           void** result);
CSecondaryTaskBand_GetTaskbarHost_t CSecondaryTaskBand_GetTaskbarHost_Original;
using std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;
XamlRoot XamlRootFromTaskbarHostSharedPtr(void* taskbarHostSharedPtr[2]) {
    if (!taskbarHostSharedPtr[0] && !taskbarHostSharedPtr[1]) {
        return nullptr;
    }
    constexpr size_t kTaskbarElementIUnknownOffset = 0x40;
    auto* taskbarElementIUnknown =
        *(IUnknown**)((BYTE*)taskbarHostSharedPtr[0] +
                      kTaskbarElementIUnknownOffset);
    FrameworkElement taskbarElement = nullptr;
    taskbarElementIUnknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(taskbarElement));
    auto result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;
    std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);
    return result;
}
XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    HWND hTaskSwWnd = (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
    if (!hTaskSwWnd) {
        return nullptr;
    }
    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *(void**)taskBandForTaskListWndSite !=
                    CTaskBand_ITaskListWndSite_vftable;
         i++) {
        if (i == 20) {
            return nullptr;
        }
        taskBandForTaskListWndSite = (void**)taskBandForTaskListWndSite + 1;
    }
    void* taskbarHostSharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                      taskbarHostSharedPtr);
    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}
XamlRoot GetSecondaryTaskbarXamlRoot(HWND hSecondaryTaskbarWnd) {
    HWND hTaskSwWnd =
        (HWND)FindWindowEx(hSecondaryTaskbarWnd, nullptr, L"WorkerW", nullptr);
    if (!hTaskSwWnd) {
        return nullptr;
    }
    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *(void**)taskBandForTaskListWndSite !=
                    CSecondaryTaskBand_ITaskListWndSite_vftable;
         i++) {
        if (i == 20) {
            return nullptr;
        }
        taskBandForTaskListWndSite = (void**)taskBandForTaskListWndSite + 1;
    }
    void* taskbarHostSharedPtr[2]{};
    CSecondaryTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                               taskbarHostSharedPtr);
    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}
using RunFromWindowThreadProc_t = void(WINAPI*)(void* parameter);
bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         void* procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        void* procParam;
    };
    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return false;
    }
    if (dwThreadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }
    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
                    RUN_FROM_WINDOW_THREAD_PARAM* param =
                        (RUN_FROM_WINDOW_THREAD_PARAM*)cwp->lParam;
                    param->proc(param->procParam);
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }
    RUN_FROM_WINDOW_THREAD_PARAM param;
    param.proc = proc;
    param.procParam = procParam;
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);
    UnhookWindowsHookEx(hook);
    return true;
}
void ApplySettingsFromTaskbarThread() {
    Wh_Log(L"Applying settings");
    EnumThreadWindows(
        GetCurrentThreadId(),
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            WCHAR szClassName[32];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
                return TRUE;
            }
            XamlRoot xamlRoot = nullptr;
            if (_wcsicmp(szClassName, L"Shell_TrayWnd") == 0) {
                xamlRoot = GetTaskbarXamlRoot(hWnd);
            } else if (_wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0) {
                xamlRoot = GetSecondaryTaskbarXamlRoot(hWnd);
            } else {
                return TRUE;
            }
            if (!xamlRoot) {
                Wh_Log(L"Getting XamlRoot failed");
                return TRUE;
            }
            if (!ApplyStyle(xamlRoot)) {
                Wh_Log(L"ApplyStyles failed");
                return TRUE;
            }
            return TRUE;
        },
        0);
}
void ApplySettingsStartButtonPosition(HWND hTaskbarWnd) {
    RunFromWindowThread(
        hTaskbarWnd, [](void* pParam) { ApplySettingsFromTaskbarThread(); }, 0);
}
using IUIElement_Arrange_t =
    HRESULT(WINAPI*)(void* pThis, const winrt::Windows::Foundation::Rect* rect);
IUIElement_Arrange_t IUIElement_Arrange_Original;
HRESULT WINAPI
IUIElement_Arrange_Hook(void* pThis,
                        const winrt::Windows::Foundation::Rect* rect) {
    
    auto original = [=] { return IUIElement_Arrange_Original(pThis, rect); };
    if (g_unloading) {
        return original();
    }
    FrameworkElement element = nullptr;
    (*(IUnknown**)pThis)
        ->QueryInterface(winrt::guid_of<FrameworkElement>(),
                         winrt::put_abi(element));
    if (!element) {
        return original();
    }
    auto className = winrt::get_class_name(element);
    if (className != L"Taskbar.ExperienceToggleButton") {
        return original();
    }
    auto automationId =
        Automation::AutomationProperties::GetAutomationId(element);
    if (automationId != L"StartButton") {
        return original();
    }
    auto taskbarFrameRepeater =
        Media::VisualTreeHelper::GetParent(element).as<FrameworkElement>();
    auto widgetElement =
        EnumChildElements(taskbarFrameRepeater, [](FrameworkElement child) {
            auto childClassName = winrt::get_class_name(child);
            if (childClassName != L"Taskbar.AugmentedEntryPointButton") {
                return false;
            }
            if (child.Name() != L"AugmentedEntryPointButton") {
                return false;
            }
            auto margin = child.Margin();
            auto offset = child.ActualOffset();
            if (offset.x != margin.Left || offset.y != 0) {
                return false;
            }
            return true;
        });
    if (!widgetElement) {
        element.Dispatcher().TryRunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::High,[element]() {
 ApplySettingsFromTaskbarThread(); 
                double width = element.ActualWidth();
                double minX = std::numeric_limits<double>::infinity();
                auto taskbarFrameRepeater =
                    Media::VisualTreeHelper::GetParent(element)
                        .as<FrameworkElement>();
                EnumChildElements(taskbarFrameRepeater,
                                  [&element, &minX](FrameworkElement child) {
                                      if (child == element) {
                                          return false;
                                      }
                                      auto offset = child.ActualOffset();
                                      if (offset.x >= 0 && offset.x < minX) {
                                          minX = offset.x;
                                      }
                                      return false;
                                  });
                if (minX < width) {
                    Thickness margin = element.Margin();
                    
                    element.Margin(margin);
                } else if (minX > width * 2) {
                    Thickness margin = element.Margin();
                    
                    element.Margin(margin);
                }
            });
    }
    winrt::Windows::Foundation::Rect newRect = *rect;
    newRect.X = 0;
    return original();
}
using AugmentedEntryPointButton_UpdateButtonPadding_t =
    void(WINAPI*)(void* pThis);
void WINAPI AugmentedEntryPointButton_UpdateButtonPadding_Hook_StartButtonPosition(void* pThis) {
    
    AugmentedEntryPointButton_UpdateButtonPadding_Original(pThis);
    if (g_unloading) {
        return;
    }
    FrameworkElement button = nullptr;
    ((IUnknown**)pThis)[1]->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(button));
    if (!button) {
        return;
    }
    button.Dispatcher().TryRunAsync(
        winrt::Windows::UI::Core::CoreDispatcherPriority::High, [button]() {
            auto offset = button.ActualOffset();
            if (offset.x != 0 || offset.y != 0) {
                return;
            }
            auto margin = button.Margin();
            margin.Left = 34;
            button.Margin(margin);
        });
}
bool HookTaskbarDllSymbolsStartButtonPosition() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }
    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &CTaskBand_ITaskListWndSite_vftable,
        },
        {
            {LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &CSecondaryTaskBand_ITaskListWndSite_vftable,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
            &CTaskBand_GetTaskbarHost_Original,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"},
            &CSecondaryTaskBand_GetTaskbarHost_Original,
        },
        {
            {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
            &std__Ref_count_base__Decref_Original,
        },
    };
    return HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
}
bool HookTaskbarViewDllSymbolsStartButtonPosition(HMODULE module) {
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IUIElement<struct winrt::Windows::UI::Xaml::IUIElement>::Arrange(struct winrt::Windows::Foundation::Rect const &)const )"},
            &IUIElement_Arrange_Original,
            IUIElement_Arrange_Hook,
        },
        {
            {LR"(protected: virtual void __cdecl winrt::Taskbar::implementation::AugmentedEntryPointButton::UpdateButtonPadding(void))"},
            &AugmentedEntryPointButton_UpdateButtonPadding_Original,
            AugmentedEntryPointButton_UpdateButtonPadding_Hook_StartButtonPosition,
        },
    };
    return HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks));
}
void HandleLoadedModuleIfTaskbarView(HMODULE module, LPCWSTR lpLibFileName) {
    if (!g_taskbarViewDllLoaded && GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoaded.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);
        if (HookTaskbarViewDllSymbolsStartButtonPosition(module)) {
            Wh_ApplyHookOperations();
        }
    }
}
using LoadLibraryExW_t = decltype(&LoadLibraryExW);
HMODULE WINAPI LoadLibraryExW_Hook_StartButtonPosition(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module) {
        HandleLoadedModuleIfTaskbarView(module, lpLibFileName);
    }
    return module;
}
std::wstring GetProcessFileName(DWORD dwProcessId) {
    HANDLE hProcess =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcessId);
    if (!hProcess) {
        return std::wstring{};
    }
    WCHAR processPath[MAX_PATH];
    DWORD dwSize = ARRAYSIZE(processPath);
    if (!QueryFullProcessImageName(hProcess, 0, processPath, &dwSize)) {
        CloseHandle(hProcess);
        return std::wstring{};
    }
    CloseHandle(hProcess);
    PCWSTR processFileNameUpper = wcsrchr(processPath, L'\\');
    if (!processFileNameUpper) {
        return std::wstring{};
    }
    processFileNameUpper++;
    return processFileNameUpper;
}
using DwmSetWindowAttribute_t = decltype(&DwmSetWindowAttribute);
DwmSetWindowAttribute_t DwmSetWindowAttribute_Original;
HRESULT WINAPI DwmSetWindowAttribute_Hook(HWND hwnd,
                                          DWORD dwAttribute,
                                          LPCVOID pvAttribute,
                                          DWORD cbAttribute) {
    auto original = [=]() {
        return DwmSetWindowAttribute_Original(hwnd, dwAttribute, pvAttribute,
                                              cbAttribute);
    };
    if (dwAttribute != DWMWA_CLOAK || cbAttribute != sizeof(BOOL)) {
        return original();
    }
    BOOL cloak = *(BOOL*)pvAttribute;
    if (cloak) {
        return original();
    }
    Wh_Log(L"> %08X", (DWORD)(DWORD_PTR)hwnd);
    DWORD processId = 0;
    if (!hwnd || !GetWindowThreadProcessId(hwnd, &processId)) {
        return original();
    }
    std::wstring processFileName = GetProcessFileName(processId);
    enum class Target {
        StartMenu,
        SearchHost,
    };
    Target target;
    if (_wcsicmp(processFileName.c_str(), L"StartMenuExperienceHost.exe") ==
        0) {
        target = Target::StartMenu;
    } else if (_wcsicmp(processFileName.c_str(), L"SearchHost.exe") == 0) {
        target = Target::SearchHost;
    } else {
        return original();
    }
    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    UINT monitorDpiX = 96;
    UINT monitorDpiY = 96;
    GetDpiForMonitor(monitor, MDT_DEFAULT, &monitorDpiX, &monitorDpiY);
    MONITORINFO monitorInfo{
        .cbSize = sizeof(MONITORINFO),
    };
    GetMonitorInfo(monitor, &monitorInfo);
    RECT targetRect;
    if (!GetWindowRect(hwnd, &targetRect)) {
        return original();
    }
    int x = targetRect.left;
    int y = targetRect.top;
    int cx = targetRect.right - targetRect.left;
    int cy = targetRect.bottom - targetRect.top;
    if (target == Target::StartMenu) {
        int cxNew;
        if (g_settings_startbuttonposition.startMenuOnTheLeft) {
            cxNew = MulDiv(
                g_settings_startbuttonposition.startMenuWidth ? g_settings_startbuttonposition.startMenuWidth : 660,
                monitorDpiX, 96);
            g_startMenuWnd = hwnd;
            g_startMenuOriginalWidth = cx;
        } else {
            if (!g_startMenuOriginalWidth) {
                return original();
            }
            cxNew = g_startMenuOriginalWidth;
            g_startMenuWnd = nullptr;
            g_startMenuOriginalWidth = 0;
        }
        if (cxNew == cx) {
            return original();
        }
        cx = cxNew;
    } else if (target == Target::SearchHost) {
        int xNew;
        if (g_settings_startbuttonposition.startMenuOnTheLeft) {
            if (x == monitorInfo.rcWork.left) {
                return original();
            }
            xNew = monitorInfo.rcWork.left;
            g_searchMenuWnd = hwnd;
            g_searchMenuOriginalX = x;
        } else {
            if (!g_searchMenuOriginalX) {
                return original();
            }
            xNew = g_searchMenuOriginalX;
            g_searchMenuWnd = nullptr;
            g_searchMenuOriginalX = 0;
        }
        if (xNew == x) {
            return original();
        }
        x = xNew;
    }
    SetWindowPos(hwnd, nullptr, x, y, cx, cy, SWP_NOZORDER | SWP_NOACTIVATE);
    return original();
}
void LoadSettingsStartButtonPosition() {
    g_settings_startbuttonposition.startMenuOnTheLeft = 0;
    g_settings_startbuttonposition.startMenuWidth = 0;
}
BOOL Wh_ModInitStartButtonPosition() {
    
    LoadSettingsStartButtonPosition();
    if (!HookTaskbarDllSymbolsStartButtonPosition()) {
        return FALSE;
    }
    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoaded = true;
        if (!HookTaskbarViewDllSymbolsStartButtonPosition(taskbarViewModule)) {
            return FALSE;
        }
    } else {
        Wh_Log(L"Taskbar view module not loaded yet");
        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW =
            (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule,
                                                      "LoadLibraryExW");
        WindhawkUtils::Wh_SetFunctionHookT(pKernelBaseLoadLibraryExW,
                                           LoadLibraryExW_Hook_StartButtonPosition,
                                           &LoadLibraryExW_Original);
    }
    HMODULE dwmapiModule = LoadLibrary(L"dwmapi.dll");
    if (dwmapiModule) {
        auto pDwmSetWindowAttribute =
            (decltype(&DwmSetWindowAttribute))GetProcAddress(
                dwmapiModule, "DwmSetWindowAttribute");
        if (pDwmSetWindowAttribute) {
            WindhawkUtils::Wh_SetFunctionHookT(pDwmSetWindowAttribute,
                                               DwmSetWindowAttribute_Hook,
                                               &DwmSetWindowAttribute_Original);
        }
    }
    return TRUE;
}
void Wh_ModAfterInitStartButtonPosition() {
    
    if (!g_taskbarViewDllLoaded) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewDllLoaded.exchange(true)) {
                Wh_Log(L"Got Taskbar.View.dll");
                if (HookTaskbarViewDllSymbolsStartButtonPosition(taskbarViewModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }
    HWND hTaskbarWnd = GetTaskbarWnd();
    if (hTaskbarWnd) {
        ApplySettingsStartButtonPosition(hTaskbarWnd);
    }
}
void Wh_ModBeforeUninitStartButtonPosition() {
    
    g_unloading = true;
    HWND hTaskbarWnd = GetTaskbarWnd();
    if (hTaskbarWnd) {
        ApplySettingsStartButtonPosition(hTaskbarWnd);
    }
}
void Wh_ModUninitStartButtonPosition() {
    
    if (g_startMenuWnd && g_startMenuOriginalWidth) {
        RECT rect;
        if (GetWindowRect(g_startMenuWnd, &rect)) {
            int x = rect.left;
            int y = rect.top;
            int cx = rect.right - rect.left;
            int cy = rect.bottom - rect.top;
            if (g_startMenuOriginalWidth != cx) {
                cx = g_startMenuOriginalWidth;
                SetWindowPos(g_startMenuWnd, nullptr, x, y, cx, cy,
                             SWP_NOZORDER | SWP_NOACTIVATE);
            }
        }
    }
    if (g_searchMenuWnd && g_searchMenuOriginalX) {
        RECT rect;
        if (GetWindowRect(g_searchMenuWnd, &rect)) {
            int x = rect.left;
            int y = rect.top;
            int cx = rect.right - rect.left;
            int cy = rect.bottom - rect.top;
            if (g_searchMenuOriginalX != x) {
                x = g_searchMenuOriginalX;
                SetWindowPos(g_searchMenuWnd, nullptr, x, y, cx, cy,
                             SWP_NOZORDER | SWP_NOACTIVATE);
            }
        }
    }
}
void Wh_ModSettingsChangedStartButtonPosition() {
    
    LoadSettingsStartButtonPosition();
}


///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
////   _______    ______     ______  __  ___  __       __   __  ___  _______   ////
////  |       \  /  __  \   /      ||  |/  / |  |     |  | |  |/  / |   ____|  ////
////  |  .--.  ||  |  |  | |  ,----'|  '  /  |  |     |  | |  '  /  |  |__     ////
////  |  |  |  ||  |  |  | |  |     |    <   |  |     |  | |    <   |   __|    ////
////  |  '--'  ||  `--'  | |  `----.|  .  \  |  `----.|  | |  .  \  |  |____   ////
////  |_______/  \______/   \______||__|\__\ |_______||__| |__|\__\ |_______|  ////
////                                                                           ////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

#include <dwmapi.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>

#include <functional>
#undef GetCurrentTime
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>
using namespace winrt::Windows::UI::Xaml;
STDAPI GetDpiForMonitor(HMONITOR hmonitor, MONITOR_DPI_TYPE dpiType, UINT* dpiX, UINT* dpiY);
#include <commctrl.h>
#include <roapi.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.Animation.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winstring.h>

#include <chrono>
#include <sstream>
#include <string>
#include <string_view>
#include <winrt/Windows.UI.Xaml.Automation.h.>

struct {
  int userDefinedTrayTaskGap;
  int userDefinedTaskbarBackgroundHorizontalPadding;
  unsigned int userDefinedTaskbarOffsetY;
  unsigned int userDefinedTaskbarHeight;
  unsigned int userDefinedTaskbarIconSize;
  unsigned int userDefinedTaskbarButtonSize;
  float userDefinedTaskbarCornerRadius;
  unsigned int userDefinedTaskButtonCornerRadius;
  bool userDefinedFlatTaskbarBottomCorners;
  unsigned int userDefinedTaskbarBackgroundOpacity;
  unsigned int userDefinedTaskbarBackgroundTint;
  unsigned int userDefinedTaskbarBackgroundLuminosity;
  uint8_t userDefinedTaskbarBorderOpacity;
} g_settings;

std::wstring EscapeXmlAttribute(std::wstring_view data) {
  std::wstring buffer;
  buffer.reserve(data.size());
  for (wchar_t c : data) buffer.append((c == L'&') ? L"&amp;" : (c == L'\"') ? L"&quot;" : (c == L'<') ? L"&lt;" : (c == L'>') ? L"&gt;" : std::wstring(1, c));
  return buffer;
}

Style GetStyleFromXamlSetters(const std::wstring_view type, const std::wstring_view xamlStyleSetters, std::wstring& outXaml) {
  std::wstring xaml =
      LR"(<ResourceDictionary
    xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
    xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
    xmlns:d="http://schemas.microsoft.com/expression/blend/2008"
    xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
    xmlns:muxc="using:Microsoft.UI.Xaml.Controls")";
  if (auto pos = type.rfind('.'); pos != type.npos) {
    auto typeNamespace = std::wstring_view(type).substr(0, pos);
    auto typeName = std::wstring_view(type).substr(pos + 1);
    xaml += L"\n    xmlns:windhawkstyler=\"using:";
    xaml += EscapeXmlAttribute(typeNamespace);
    xaml += L"\">\n    <Style TargetType=\"windhawkstyler:";
    xaml += EscapeXmlAttribute(typeName);
    xaml += L"\">\n";
  } else {
    xaml += L">\n    <Style TargetType=\"";
    xaml += EscapeXmlAttribute(type);
    xaml += L"\">\n";
  }
  xaml += xamlStyleSetters;
  xaml +=
      L"    </Style>\n"
      L"</ResourceDictionary>";
  outXaml = xaml;
  auto resourceDictionary = Markup::XamlReader::Load(xaml).as<ResourceDictionary>();
  auto [styleKey, styleInspectable] = resourceDictionary.First().Current();
  return styleInspectable.as<Style>();
}

void SetElementPropertyFromString(FrameworkElement obj, const std::wstring& type, const std::wstring& propertyName, const std::wstring& propertyValue, bool isXamlValue) {
  std::wstring outXamlResult;
  try {
    std::wstring xamlSetter = L"<Setter Property=\"";
    xamlSetter += EscapeXmlAttribute(propertyName);
    xamlSetter += L"\"";
    if (isXamlValue) {
      xamlSetter +=
          L">\n"
          L"    <Setter.Value>\n";
      xamlSetter += propertyValue;
      xamlSetter += L"\n    </Setter.Value>\n";
      xamlSetter += L"</Setter>";
    } else {
      xamlSetter += L" Value=\"";
      xamlSetter += EscapeXmlAttribute(propertyValue);
      xamlSetter += L"\"/>";
    }
    auto style = GetStyleFromXamlSetters(type, xamlSetter, outXamlResult);
    for (uint32_t i = 0; i < style.Setters().Size(); ++i) {
      auto setter = style.Setters().GetAt(i).as<Setter>();
      obj.SetValue(setter.Property(), setter.Value());
    }
  } catch (const std::exception& ex) {
    if (!outXamlResult.empty()) {
      Wh_Log(L"Error: %S. Xaml Result: %s", ex.what(), outXamlResult.c_str());
    } else {
      Wh_Log(L"Error: %S", ex.what());
    }
  } catch (const winrt::hresult_error& ex) {
    if (!outXamlResult.empty()) {
      Wh_Log(L"Error %08X: %s. Xaml Result: %s", ex.code(), ex.message().c_str(), outXamlResult.c_str());
    } else {
      Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
    }
  } catch (...) {
    if (!outXamlResult.empty()) {
      Wh_Log(L"Unknown error occurred while setting property. Xaml Result: %s", outXamlResult.c_str());
    } else {
      Wh_Log(L"Unknown error occurred while setting property.");
    }
  }
}

void SetElementPropertyFromString(FrameworkElement obj, const std::wstring& type, const std::wstring& propertyName, const std::wstring& propertyValue) { return SetElementPropertyFromString(obj, type, propertyName, propertyValue, false); }

bool IsWeirdFrameworkElement(winrt::Windows::UI::Xaml::FrameworkElement const& element) {
  if (!element) return false;
  auto transform = element.TransformToVisual(nullptr);
  winrt::Windows::Foundation::Rect rect = transform.TransformBounds(winrt::Windows::Foundation::Rect(0, 0, element.ActualWidth(), element.ActualHeight()));
  return rect.X < 0 || rect.Y < 0;
}

double CalculateValidChildrenWidth(FrameworkElement element, int& childrenCount, double& leftMostEdge, double& rightMostEdge) {
  auto userDefinedTaskButtonCornerRadius = std::to_wstring(g_settings.userDefinedTaskButtonCornerRadius);
  auto userDefinedTaskbarIconSize = std::to_wstring(g_settings.userDefinedTaskbarIconSize);
  double totalWidth = 0.0;
  int childrenCountTentitive = Media::VisualTreeHelper::GetChildrenCount(element);
  leftMostEdge = std::numeric_limits<double>::max();
  rightMostEdge = std::numeric_limits<double>::lowest();
  for (int i = 0; i < childrenCountTentitive; i++) {
    auto child = Media::VisualTreeHelper::GetChild(element, i).try_as<FrameworkElement>();
    if (!child) {
      Wh_Log(L"Failed to get child %d of %d", i + 1, childrenCountTentitive);
      continue;
    }
    auto transform = child.TransformToVisual(element);
    auto rect = transform.TransformBounds(winrt::Windows::Foundation::Rect(0, 0, child.ActualWidth(), child.ActualHeight()));
    // exclude "weird" rectangles (aka recycled views)
    if (rect.X < 0 | rect.Y < 0) {
      continue;
    }
    auto className = winrt::get_class_name(child);
    if (className == L"Taskbar.TaskListButton" || className == L"Taskbar.ExperienceToggleButton" || className == L"Taskbar.OverflowToggleButton") {
      SetElementPropertyFromString(child, className.c_str(), L"MinWidth", userDefinedTaskbarIconSize);
    }
    SetElementPropertyFromString(child, className.c_str(), L"CornerRadius", userDefinedTaskButtonCornerRadius);

    // auto firstChild = EnumChildElements(child, [](auto) { return true; });
    // if(firstChild){
    // auto bgElementChild = FindChildByClassName(firstChild, L"Windows.UI.Xaml.Controls.Border");
    // if(bgElementChild){
    //     SetElementPropertyFromString(bgElementChild, L"Windows.UI.Xaml.Controls.Border",L"Background",  L"<AcrylicBrush TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\"0\" TintLuminosityOpacity=\"0\" Opacity=\"0.5\"/>",
    //                                  true);

    //                                     SetElementPropertyFromString(bgElementChild, L"Windows.UI.Xaml.Controls.Border",L"BorderThickness",  L"2,2,2,2");

    // }
    // }

    totalWidth += rect.Width;
    childrenCount++;
    if (rect.X < leftMostEdge) {
      leftMostEdge = rect.X;
    }
    double rightEdge = rect.X + rect.Width;
    if (rightEdge > rightMostEdge) {
      rightMostEdge = rightEdge;
    }
  }
  return totalWidth;
}

float g_lastTargetOffsetX = 0.0f, g_lastTargetWidth = 0.0f, g_lastTargetOffsetY = 0.0f;
bool g_isAnimating = false;
signed int g_initOffsetX = -1;
bool g_invalidateDimensions = true;
struct {
  int childrenCount = 0;
  double rightMostEdge = 0.0;
  double childrenWidth = 0.0;
} g_lastTaskbarData;
float g_lastModifierForTaskbarOffset = 0.0f;
bool g_wasOverflowing = false;

bool ApplyStyle(XamlRoot xamlRoot) {
  g_isAnimating = true;
  auto xamlRootContent = xamlRoot.Content().try_as<FrameworkElement>();
  if (!xamlRootContent) return false;

  auto taskFrame = FindChildByClassName(xamlRootContent, L"Taskbar.TaskbarFrame");
  if (!taskFrame) return false;

  auto borderView = Media::VisualTreeHelper::GetParent(xamlRootContent).try_as<FrameworkElement>();
  if (!borderView) return false;

  auto parent1 = Media::VisualTreeHelper::GetParent(borderView);
  if (!parent1) return false;

  auto frameworkElement1 = parent1.try_as<FrameworkElement>();
  if (!frameworkElement1) return false;

  auto parent2 = Media::VisualTreeHelper::GetParent(frameworkElement1);
  if (!parent2) return false;

  auto ScrollViewer = parent2.try_as<FrameworkElement>();
  if (!ScrollViewer) return false;

  auto rootGrid = FindChildByName(taskFrame, L"RootGrid");
  if (!rootGrid) return false;

  auto taskbarFrameRepeater = FindChildByName(rootGrid, L"TaskbarFrameRepeater");
  if (!taskbarFrameRepeater) return false;

  auto trayFrame = FindChildByClassName(xamlRootContent, L"SystemTray.SystemTrayFrame");
  if (!trayFrame) return false;

  auto systemTrayFrameGrid = FindChildByName(trayFrame, L"SystemTrayFrameGrid");
  if (!systemTrayFrameGrid) return false;

  auto showDesktopButton = FindChildByName(systemTrayFrameGrid, L"ShowDesktopStack");
  if (!showDesktopButton) return false;

  auto taskbarBackground = FindChildByClassName(rootGrid, L"Taskbar.TaskbarBackground");
  if (!taskbarBackground) return false;

  auto backgroundFillParent = FindChildByClassName(taskbarBackground, L"Windows.UI.Xaml.Controls.Grid");
  if (!backgroundFillParent) return false;

  auto backgroundFillChild = FindChildByName(backgroundFillParent, L"BackgroundFill");
  if (!backgroundFillChild) return false;

  auto overflowButton = FindChildByClassName(taskbarFrameRepeater, L"Taskbar.OverflowToggleButton");
  bool isOverflowing = overflowButton != nullptr && !IsWeirdFrameworkElement(overflowButton);

  double rootWidth = xamlRoot.Size().Width;

  int childrenCountTaskbar = 0;
  double leftMostEdgeTaskbar = 0.0, rightMostEdgeTaskbar = rootWidth;
  const double childrenWidthTaskbar = CalculateValidChildrenWidth(taskbarFrameRepeater, childrenCountTaskbar, leftMostEdgeTaskbar, rightMostEdgeTaskbar);

  if (childrenCountTaskbar == 0 || childrenWidthTaskbar <= 0) {
    return false;
  }

  bool rightMostEdgeChangedTaskbar = (g_lastTaskbarData.rightMostEdge != rightMostEdgeTaskbar);

  if (!isOverflowing && (rightMostEdgeChangedTaskbar || g_lastTaskbarData.rightMostEdge == 0.0)) {
    g_lastTaskbarData.childrenCount = childrenCountTaskbar;
    g_lastTaskbarData.rightMostEdge = rightMostEdgeTaskbar;
    g_lastTaskbarData.childrenWidth = childrenWidthTaskbar;
  }

  int childrenCountTray = 0;
  double leftMostEdgeTray = 0.0, rightMostEdgeTray = rootWidth;
  double trayFrameWidth = CalculateValidChildrenWidth(systemTrayFrameGrid, childrenCountTray, leftMostEdgeTray, rightMostEdgeTray);

  if (childrenCountTray == 0 || trayFrameWidth <= 0) {
    return false;
  }

  Wh_Log(L"leftMostEdgeTaskbar: %f, rightMostEdgeTaskbar: %f, g_lastTaskbarData.rightMostEdge %f, g_lastTaskbarData.count %d", leftMostEdgeTaskbar, rightMostEdgeTaskbar, g_lastTaskbarData.rightMostEdge, g_lastTaskbarData.childrenCount);
  Wh_Log(L"leftMostEdgeTray: %f, rightMostEdgeTray: %f", leftMostEdgeTray, rightMostEdgeTray);

  signed int userDefinedTaskbarOffsetY = g_settings.userDefinedFlatTaskbarBottomCorners ? 0 : g_settings.userDefinedTaskbarOffsetY;  // we invert it to make the setting more user intuitive

  trayFrameWidth += g_settings.userDefinedTrayTaskGap;
  float showDesktopButtonWidth = static_cast<float>(showDesktopButton.ActualWidth());
  float targetWidth = g_unloading ? rootWidth : (childrenWidthTaskbar + trayFrameWidth + (g_settings.userDefinedTaskbarBackgroundHorizontalPadding * 2));
  float targetOffsetX = leftMostEdgeTaskbar >= 0 ? (fmax(leftMostEdgeTaskbar - g_settings.userDefinedTaskbarBackgroundHorizontalPadding / 2.0f, 0.0f)) : ((rootWidth - targetWidth) / 2.0f);

  auto heightValue = std::to_wstring(g_settings.userDefinedTaskbarHeight + abs(userDefinedTaskbarOffsetY < 0 ? (userDefinedTaskbarOffsetY * 2) : 0));
  auto userDefinedTaskbarHeightStr = std::to_wstring(g_settings.userDefinedTaskbarHeight);

  if (g_invalidateDimensions) {
    g_invalidateDimensions = false;
    taskFrame.SetValue(FrameworkElement::WidthProperty(), winrt::box_value(std::numeric_limits<double>::quiet_NaN()));
    trayFrame.SetValue(FrameworkElement::HorizontalAlignmentProperty(), winrt::box_value(HorizontalAlignment::Center));
    SetElementPropertyFromString(ScrollViewer, L"Windows.UI.Xaml.Controls.ScrollViewer", L"Height", heightValue);
    SetElementPropertyFromString(ScrollViewer, L"Windows.UI.Xaml.Controls.ScrollViewer", L"MaxHeight", heightValue);
    SetElementPropertyFromString(ScrollViewer, L"Windows.UI.Xaml.Controls.ScrollViewer", L"MaxWidth", std::to_wstring(rootWidth));
    SetElementPropertyFromString(ScrollViewer, L"Windows.UI.Xaml.Controls.ScrollViewer", L"Width", std::to_wstring(rootWidth));

    SetElementPropertyFromString(trayFrame, L"SystemTray.SystemTrayFrame", L"MaxHeight", userDefinedTaskbarHeightStr);
    SetElementPropertyFromString(trayFrame, L"SystemTray.SystemTrayFrame", L"Height", userDefinedTaskbarHeightStr);

    SetElementPropertyFromString(borderView, L"Windows.UI.Xaml.Controls.Border", L"Height", heightValue);
    SetElementPropertyFromString(borderView, L"Windows.UI.Xaml.Controls.Border", L"MaxHeight", heightValue);

    SetElementPropertyFromString(taskFrame, L"Taskbar.TaskbarFrame", L"Height", heightValue);
    SetElementPropertyFromString(taskFrame, L"Taskbar.TaskbarFrame", L"MaxHeight", heightValue);

    SetElementPropertyFromString(taskbarFrameRepeater, L"Microsoft.UI.Xaml.Controls.ItemsRepeater", L"Height", userDefinedTaskbarHeightStr);
    SetElementPropertyFromString(taskbarFrameRepeater, L"Microsoft.UI.Xaml.Controls.ItemsRepeater", L"MaxHeight", userDefinedTaskbarHeightStr);

    SetElementPropertyFromString(taskbarFrameRepeater, L"Microsoft.UI.Xaml.Controls.ItemsRepeater", L"Width", std::to_wstring(rootWidth));
    SetElementPropertyFromString(taskbarFrameRepeater, L"Microsoft.UI.Xaml.Controls.ItemsRepeater", L"MaxWidth", std::to_wstring(rootWidth));
  }

  float centeredTray = (rootWidth - trayFrameWidth) / 2.0f;

  leftMostEdgeTaskbar = fmax(0.0f, (rootWidth - childrenWidthTaskbar) / 2.0f);

  auto taskbarFrameRepeaterVisual = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(taskbarFrameRepeater);
  auto taskbarFrameRepeaterVisualAnimation = taskbarFrameRepeaterVisual.Compositor().CreateVector3KeyFrameAnimation();

  float modifierForTaskbarOffset = isOverflowing ? ((targetWidth - childrenWidthTaskbar) / 2.0f) : (childrenWidthTaskbar - g_lastTaskbarData.childrenWidth) / 2.0f;

  Wh_Log(L"modifierForTaskbarOffset: %f, isOverflowing: %d, wasOverflowing: %d", modifierForTaskbarOffset, isOverflowing, g_wasOverflowing);

  taskbarFrameRepeaterVisualAnimation.InsertKeyFrame(1.0f, winrt::Windows::Foundation::Numerics::float3{static_cast<float>(-trayFrameWidth / 2.0f + (modifierForTaskbarOffset < 0 ? 0 : modifierForTaskbarOffset)), taskbarFrameRepeaterVisual.Offset().y, taskbarFrameRepeaterVisual.Offset().z});
  taskbarFrameRepeaterVisual.StartAnimation(L"Offset", taskbarFrameRepeaterVisualAnimation);

  // tray animations
  auto trayVisual = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(trayFrame);
  auto originalOffset = trayVisual.Offset();
  if (g_initOffsetX == -1) {
    g_initOffsetX = originalOffset.x;
  }

  float newXOffset = centeredTray + (childrenWidthTaskbar / 2.0f) + g_settings.userDefinedTrayTaskGap + showDesktopButtonWidth;

  boolean movingInwards = originalOffset.x > newXOffset;

  auto taskbarVisual = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(taskbarFrameRepeater);
  auto compositorTaskbar = taskbarVisual.Compositor();
  float targetOffsetXTray = (g_unloading ? (float)g_initOffsetX : static_cast<float>(newXOffset));
  auto trayAnimation = compositorTaskbar.CreateVector3KeyFrameAnimation();

  trayAnimation.InsertKeyFrame(1.0f, winrt::Windows::Foundation::Numerics::float3{targetOffsetXTray, taskbarVisual.Offset().y, taskbarVisual.Offset().z});
  if (movingInwards) {
    trayAnimation.DelayTime(winrt::Windows::Foundation::TimeSpan(std::chrono::milliseconds(childrenCountTaskbar * 4)));
  }

  trayVisual.StartAnimation(L"Offset", trayAnimation);

  if (!taskbarBackground) return false;

  auto taskbarGrid = FindChildByClassName(taskbarBackground, L"Windows.UI.Xaml.Controls.Grid");
  if (taskbarGrid) {
    auto taskbarStroke = FindChildByName(taskbarGrid, L"BackgroundStroke");
    if (taskbarStroke) {
      taskbarStroke.Opacity(0);
    }
  }

  auto userDefinedTaskbarBackgroundLuminosity = std::to_wstring(g_settings.userDefinedTaskbarBackgroundLuminosity / 100.0f);
  auto userDefinedTaskbarBackgroundOpacity = std::to_wstring(g_settings.userDefinedTaskbarBackgroundOpacity / 100.0f);
  auto userDefinedTaskbarBackgroundTint = std::to_wstring(g_settings.userDefinedTaskbarBackgroundTint / 100.0f);
  SetElementPropertyFromString(backgroundFillChild, L"Windows.UI.Xaml.Shapes.Rectangle", L"Fill",
                               L"<AcrylicBrush TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\"" + userDefinedTaskbarBackgroundTint + L"\" TintLuminosityOpacity=\"" + userDefinedTaskbarBackgroundLuminosity + L"\" Opacity=\"" + userDefinedTaskbarBackgroundOpacity + L"\"/>",
                               true);

  auto backgroundFillVisual = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(backgroundFillChild);
  auto compositorTaskBackground = backgroundFillVisual.Compositor();

  Wh_Log(L"rootWidth: %f, targetWidth: %f, targetOffsetX %f", rootWidth, targetWidth, targetOffsetX);

  ////////////////////////////////////////

  float targetWidthRect = targetWidth;
  float offsetXRect = (rootWidth - targetWidth) / 2;

  auto roundedRect = compositorTaskBackground.CreateRoundedRectangleGeometry();

  roundedRect.CornerRadius({g_settings.userDefinedTaskbarCornerRadius, g_settings.userDefinedTaskbarCornerRadius});

  auto clipHeight = static_cast<float>(g_settings.userDefinedTaskbarHeight + ((g_settings.userDefinedFlatTaskbarBottomCorners) ? g_settings.userDefinedTaskbarHeight - g_settings.userDefinedTaskbarCornerRadius : 0.0f));

  roundedRect.Size({g_lastTargetWidth, clipHeight});

  auto geometricClip = compositorTaskBackground.CreateGeometricClip(roundedRect);
  backgroundFillVisual.Clip(geometricClip);

  // size animation
  auto sizeAnimationRect = compositorTaskBackground.CreateVector2KeyFrameAnimation();
  sizeAnimationRect.InsertKeyFrame(0.0f, {g_lastTargetWidth, clipHeight});
  sizeAnimationRect.InsertKeyFrame(1.0f, {targetWidthRect, clipHeight});
  roundedRect.StartAnimation(L"Size", sizeAnimationRect);
  g_lastTargetWidth = targetWidthRect;
  // centering the clip animation
  auto offsetAnimationRect = compositorTaskBackground.CreateVector2KeyFrameAnimation();
  offsetAnimationRect.InsertKeyFrame(0.0f, {g_lastTargetOffsetX, g_lastTargetOffsetY});
  float newOffsetYRect = userDefinedTaskbarOffsetY < 0 ? static_cast<float>(abs(userDefinedTaskbarOffsetY)) : 0.0f;
  offsetAnimationRect.InsertKeyFrame(1.0f, {offsetXRect, newOffsetYRect});
  roundedRect.StartAnimation(L"Offset", offsetAnimationRect);
  g_lastTargetOffsetX = offsetXRect;
  g_lastTargetOffsetY = newOffsetYRect;

  // border control
  double userDefinedBorderThickness = 1.0;
  auto compositor = compositorTaskBackground;
  auto shapeVisual = compositor.CreateShapeVisual();
  shapeVisual.Size({targetWidthRect, static_cast<float>(clipHeight)});

  auto borderGeometry = compositor.CreateRoundedRectangleGeometry();
  borderGeometry.CornerRadius({g_settings.userDefinedTaskbarCornerRadius, g_settings.userDefinedTaskbarCornerRadius});
  borderGeometry.Size({static_cast<float>(targetWidthRect - userDefinedBorderThickness), static_cast<float>(clipHeight - userDefinedBorderThickness / 2.0f)});

  Wh_Log(L"userDefinedTaskbarBorderOpacity: %d", g_settings.userDefinedTaskbarBorderOpacity);

  auto borderShape = compositor.CreateSpriteShape(borderGeometry);
  winrt::Windows::UI::Color borderColor = {g_settings.userDefinedTaskbarBorderOpacity, 255, 255, 255};  // { A, R, G, B }
  borderShape.StrokeBrush(compositor.CreateColorBrush(borderColor));
  borderShape.StrokeThickness(userDefinedBorderThickness);
  borderShape.FillBrush(nullptr);
  shapeVisual.Shapes().Append(borderShape);
  shapeVisual.Offset({g_lastTargetOffsetX, g_lastTargetOffsetY, 0.0f});
  winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::SetElementChildVisual(backgroundFillChild, shapeVisual);

  g_wasOverflowing = isOverflowing;
  return true;
}

void ApplySettings(HWND hTaskbarWnd) {
  RunFromWindowThread(hTaskbarWnd, [](void* pParam) { ApplySettingsFromTaskbarThread(); }, 0);
}

using TrayUI__Hide_t = void(WINAPI*)(void* pThis);
TrayUI__Hide_t TrayUI__Hide_Original;
void WINAPI TrayUI__Hide_Hook(void* pThis) {
  ApplySettingsFromTaskbarThread();
  TrayUI__Hide_Original(pThis);
}
using CSecondaryTray__AutoHide_t = void(WINAPI*)(void* pThis, bool param1);
CSecondaryTray__AutoHide_t CSecondaryTray__AutoHide_Original;
void WINAPI CSecondaryTray__AutoHide_Hook(void* pThis, bool param1) {
  ApplySettingsFromTaskbarThread();
  CSecondaryTray__AutoHide_Original(pThis, param1);
}
using TrayUI_WndProc_t = LRESULT(WINAPI*)(void* pThis, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam, bool* flag);
TrayUI_WndProc_t TrayUI_WndProc_Original;
LRESULT WINAPI TrayUI_WndProc_Hook(void* pThis, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam, bool* flag) {
  ApplySettingsFromTaskbarThread();
  return TrayUI_WndProc_Original(pThis, hWnd, Msg, wParam, lParam, flag);
}
using CSecondaryTray_v_WndProc_t = LRESULT(WINAPI*)(void* pThis, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
CSecondaryTray_v_WndProc_t CSecondaryTray_v_WndProc_Original;
LRESULT WINAPI CSecondaryTray_v_WndProc_Hook(void* pThis, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
  ApplySettingsFromTaskbarThread();
  return CSecondaryTray_v_WndProc_Original(pThis, hWnd, Msg, wParam, lParam);
}

bool HookTaskbarDllSymbols() {
  HookTaskbarDllSymbolsTaskbarHeight();
  HMODULE module = LoadLibrary(L"taskbar.dll");
  if (!module) {
    Wh_Log(L"Failed to load taskbar.dll");
    return false;
  }
  WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {

      {
          {LR"(public: void __cdecl TrayUI::_Hide(void))"},
          &TrayUI__Hide_Original,
          TrayUI__Hide_Hook,
      },
      {
          {LR"(private: void __cdecl CSecondaryTray::_AutoHide(bool))"},
          &CSecondaryTray__AutoHide_Original,
          CSecondaryTray__AutoHide_Hook,
      },

      {
          {LR"(public: virtual __int64 __cdecl TrayUI::WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64,bool *))"},
          &TrayUI_WndProc_Original,
          TrayUI_WndProc_Hook,
      },
      {
          {LR"(private: virtual __int64 __cdecl CSecondaryTray::v_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
          &CSecondaryTray_v_WndProc_Original,
          CSecondaryTray_v_WndProc_Hook,
      },

      {
          {LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
          &CTaskBand_ITaskListWndSite_vftable,
      },
      {
          {LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})"},
          &CSecondaryTaskBand_ITaskListWndSite_vftable,
      },
      {
          {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
          &CTaskBand_GetTaskbarHost_Original,
      },
      {
          {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"},
          &CSecondaryTaskBand_GetTaskbarHost_Original,
      },
      {
          {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
          &std__Ref_count_base__Decref_Original,
      },
  };

  return HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
}

bool HookTaskbarViewDllSymbols() {
  WCHAR dllPath[MAX_PATH];
  if (!GetWindowsDirectory(dllPath, ARRAYSIZE(dllPath))) {
    Wh_Log(L"GetWindowsDirectory failed");
    return false;
  }
  wcscat_s(dllPath, MAX_PATH, LR"(\SystemApps\MicrosoftWindows.Client.Core_cw5n1h2txyewy\Taskbar.View.dll)");
  HMODULE module = LoadLibraryEx(dllPath, nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
  if (!module) {
    Wh_Log(L"Taskbar view module couldn't be loaded");
    return false;
  }
  // Taskbar.View.dll
  WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {{
      {LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IUIElement<struct winrt::Windows::UI::Xaml::IUIElement>::Arrange(struct winrt::Windows::Foundation::Rect const &)const )"},
      &IUIElement_Arrange_Original,
      IUIElement_Arrange_Hook,
  }};
  return HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks));
}

void RefreshSettings() {
  Wh_ModSettingsChangedTaskbarHeight();

  g_invalidateDimensions = true;
  g_settings.userDefinedTrayTaskGap = Wh_GetIntSetting(L"TrayTaskGap");
  g_settings.userDefinedTaskbarBackgroundHorizontalPadding = Wh_GetIntSetting(L"TaskbarBackgroundHorizontalPadding");
  g_settings.userDefinedTaskbarOffsetY = -1 * abs(Wh_GetIntSetting(L"TaskbarOffsetY"));
  g_settings.userDefinedTaskbarHeight = abs(Wh_GetIntSetting(L"TaskbarHeight"));

  g_settings.userDefinedTaskbarHeight = g_settings.userDefinedTaskbarHeight < 200 ? g_settings.userDefinedTaskbarHeight : 200;
  g_settings.userDefinedTaskbarHeight = g_settings.userDefinedTaskbarHeight < 44 ? 44 : g_settings.userDefinedTaskbarHeight;

  g_settings.userDefinedTaskbarIconSize = Wh_GetIntSetting(L"TaskbarIconSize");
  g_settings.userDefinedTaskbarButtonSize = Wh_GetIntSetting(L"TaskbarButtonSize");
  g_settings.userDefinedTaskbarCornerRadius = static_cast<float>(abs(Wh_GetIntSetting(L"TaskbarCornerRadius")));
  g_settings.userDefinedTaskbarCornerRadius = g_settings.userDefinedTaskbarCornerRadius < (g_settings.userDefinedTaskbarHeight / 2.0f) ? g_settings.userDefinedTaskbarCornerRadius : (g_settings.userDefinedTaskbarHeight / 2.0f);

  g_settings.userDefinedTaskButtonCornerRadius = abs(Wh_GetIntSetting(L"TaskButtonCornerRadius"));
  g_settings.userDefinedTaskButtonCornerRadius = g_settings.userDefinedTaskButtonCornerRadius < (g_settings.userDefinedTaskbarHeight / 2.0f) ? g_settings.userDefinedTaskButtonCornerRadius : (g_settings.userDefinedTaskbarHeight / 2.0f);

  g_settings.userDefinedFlatTaskbarBottomCorners = Wh_GetIntSetting(L"FlatTaskbarBottomCorners");

  g_settings.userDefinedTaskbarBackgroundOpacity = std::min(100, std::max(0, abs(Wh_GetIntSetting(L"TaskbarBackgroundOpacity"))));
  g_settings.userDefinedTaskbarBackgroundTint = std::min(100, std::max(0, abs(Wh_GetIntSetting(L"TaskbarBackgroundTint"))));
  g_settings.userDefinedTaskbarBackgroundLuminosity = std::min(100, std::max(0, abs(Wh_GetIntSetting(L"TaskbarBackgroundLuminosity"))));
  g_settings.userDefinedTaskbarBorderOpacity = abs(Wh_GetIntSetting(L"TaskbarBorderOpacity")) * 2.55;
  g_settings.userDefinedTaskbarBorderOpacity = static_cast<uint8_t>(g_settings.userDefinedTaskbarBorderOpacity > 255 ? 255 : g_settings.userDefinedTaskbarBorderOpacity);
}

BOOL Wh_ModInit() {
#ifdef _WIN64
  const size_t OFFSET_SAME_TEB_FLAGS = 0x17EE;
#else
  const size_t OFFSET_SAME_TEB_FLAGS = 0x0FCA;
#endif
  bool isInitialThread = *(USHORT*)((BYTE*)NtCurrentTeb() + OFFSET_SAME_TEB_FLAGS) & 0x0400;
  if (isInitialThread) {
    return FALSE;
  }
  Wh_ModInitTaskbarHeight();
  RefreshSettings();
  // needed for the xml tree
  if (!HookTaskbarDllSymbols()) {
    Wh_Log(L"HookTaskbarDllSymbols failed");
    return FALSE;
  }
  // needed for dynamic updates
  if (!HookTaskbarViewDllSymbols()) {
    Wh_Log(L"HookTaskbarViewDllSymbols failed");
    return FALSE;
  }

  // not needed?
  // HMODULE dwmapiModule = LoadLibrary(L"dwmapi.dll");
  // if (dwmapiModule) {
  //     FARPROC pDwmSetWindowAttribute =
  //             GetProcAddress(dwmapiModule, "DwmSetWindowAttribute");
  //     if (pDwmSetWindowAttribute) {
  //         Wh_SetFunctionHook((void *) pDwmSetWindowAttribute,
  //                            (void *) DwmSetWindowAttribute_Hook,
  //                            (void **) &DwmSetWindowAttribute_Original);
  //     }
  // }

  return TRUE;
}

void Wh_ModAfterInit() {
  Wh_ModAfterInitTaskbarHeight();

  HWND hTaskbarWnd = GetTaskbarWnd();
  if (hTaskbarWnd) {
    ApplySettings(hTaskbarWnd);
  }
}

void Wh_ModBeforeUninit() {
  Wh_ModBeforeUninitTaskbarHeight();

  g_unloading = true;
  HWND hTaskbarWnd = GetTaskbarWnd();
  if (hTaskbarWnd) {
    ApplySettings(hTaskbarWnd);
  }
}

void Wh_ModUninit() { Wh_ModUninitTaskbarHeight(); }

void Wh_ModSettingsChanged() {
  RefreshSettings();
  ApplySettingsFromTaskbarThread();
}
