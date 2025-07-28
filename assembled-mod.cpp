// ==WindhawkMod==
// @id              taskbar-dock-like
// @name            WinDock (taskbar as a dock) for Windows 11
// @description     Centers and floats the taskbar, moves the system tray next to the task area, and serves as an all-in-one, one-click mod to transform the taskbar into a macOS-style dock. Based on m417z's code. For Windows 11.
// @version         1.4.223
// @author          DarkionAvey
// @github          https://github.com/DarkionAvey/windhawk-taskbar-centered-condensed
// @include         explorer.exe
// @include         StartMenuExperienceHost.exe
// @include         ShellHost.exe
// @include         ShellExperienceHost.exe
// @architecture    x86-64
// @compilerOptions -ldwmapi -lole32 -loleaut32 -lruntimeobject -lshcore -lcomctl32 -Wl,--export-all-symbols
// ==/WindhawkMod==
// This mod is based on code released under the GNU General Public License v3.0.
// For bug reports and feedback:
// https://github.com/DarkionAvey/windhawk-taskbar-centered-condensed
// ==WindhawkModReadme==
/*
![Screenshot](https://github.com/DarkionAvey/windhawk-taskbar-centered-condensed/raw/main/screenshot.png)
# WinDock Windhawk Mod
Transform your Windows 11 taskbar into a smooth, MacOS-style dock — without losing any of the original functionality!
---
## 🚀 How to Install (Development Build)
⚠️ **Note:** Please disable any mods that affect taskbar height or taskbar icons—this mod already includes those
features.
1. [Install Windhawk](https://windhawk.net/) if you haven't already.
2. Copy the contents of [
   `assembled-mod.cpp`](https://raw.githubusercontent.com/DarkionAvey/windhawk-taskbar-centered-condensed/main/assembled-mod.cpp)
   to your clipboard.
3. Open **WindHawk** and navigate to: `Explore` → `Create a new mod`.
4. Press `Ctrl+A` to select all, then `Ctrl+V` to paste.
5. Click **Compile Mod** button on the top left corner.
6. Change the mod's settings to fit your preference.
7. If your explorer.exe is not responding, disable the mod from Windhawk, open task manager, restart explorer.exe, wait
   for the default taskbar to appear, then apply WinDock.
---
## 🛠 Source Code
The actual mod code is split into files under [
`mod-parts/`](https://github.com/DarkionAvey/windhawk-taskbar-centered-condensed/blob/main/mod-parts/), which are later merged together using a Python script.
❗ **Do not edit `assembled-mod.cpp` manually**, as any changes will be overwritten in the next build cycle. Instead,
modify the source files in the `mod-parts` directory.
---
## 🛣️ To-do
1. Proper unloading
2. Fix overflowing
---
## 🙌 Credits
Huge thanks to these awesome developers who made this mod possible -- your contributions to modding Windows are truly appreciated!:
- [`Michael Maltsev (m417z)`](https://github.com/m417z)
- [`Valentin Radu (valinet)`](https://github.com/valinet)
---
## 🔥 Recommended Mods
- [Smart Auto Hide for Taskbar](https://windhawk.net/mods/taskbar-auto-hide-when-maximized)
- [Taskbar Auto-Hide Speed/Frame Rate](https://windhawk.net/mods/taskbar-auto-hide-speed)
- [Show All Tray Icons](https://windhawk.net/mods/taskbar-notification-icons-show-all)
---
# Options
| Property | Name | Description | Accepted values |
| --- | --- | --- | --- |
| `TaskbarHeight` | Taskbar height | Set the height of the taskbar (Default is 74) | Non-negative integer |
| `TaskbarIconSize` | Taskbar icon size | Set the width and height of taskbar icons (Default is 42) | Non-negative integer |
| `TaskbarButtonSize` | Taskbar button size | Set the size (width and height) of taskbar buttons (Default is 74) | Non-negative integer |
| `TaskbarOffsetY` | Taskbar vertical offset | Move the taskbar up or down. Padding of the same value is applied to the top (Default is 6) | Non-negative integer |
| `TrayTaskGap` | Tray task gap | Adjust the space between the task area and the tray area (Default is 10) | Non-negative integer |
| `TaskbarBackgroundHorizontalPadding` | Taskbar background horizontal padding | Set the horizontal padding on both sides of the taskbar background (Default is 2) | Non-negative integer |
| `FullWidthTaskbarBackground` | Full-width taskbar background | When enabled, the taskbar background fills the entire width of the screen, similar to the default Windows behavior (Default is off) | Boolean (true/false) |
| `IgnoreShowDesktopButton` | Ignore "Show Desktop" button | When enabled, the "Show Desktop" button is ignored in width calculations (Default is off) | Boolean (true/false) |
| `TaskbarCornerRadius` | Taskbar corner radius | Controls how rounded the taskbar corners appear (Default is 22) | Non-negative integer |
| `TaskButtonCornerRadius` | Task button corner radius | Controls how rounded the corners of individual task buttons are (Default is 16) | Non-negative integer |
| `FlatTaskbarBottomCorners` | Flat bottom corners | When enabled, the bottom corners of the taskbar will be squared and the taskbar will dock to the screen edge. This overrides the taskbar offset; this is always on with the full-width taskbar background option (Default is off) | Boolean (true/false) |
| `CustomizeTaskbarBackground` | Stylize the taskbar background | When enabled, the taskbar background will be changed to acrylic blur. Disable this option if you are using other mods that change the taskbar background. You may need to restart explorer.exe to restore the default taskbar background (Default is on) | Boolean (true/false) |
| `TaskbarBackgroundOpacity` | Background opacity | Adjust the opacity of the taskbar background. 0 = fully transparent, 100 = fully opaque (Default is 100) | Non-negative integer |
| `TaskbarBackgroundTint` | Background tint | Modify the taskbar tint level. Higher values = more tint. Range 0-100 (Default is 0) | Non-negative integer |
| `TaskbarBackgroundLuminosity` | Background luminosity | Adjust luminosity of the taskbar background. Higher values = more opaque, lower values = more glass-like. Range 0-100 (Default is 30) | Non-negative integer |
| `TaskbarBorderOpacity` | Border opacity | Set the opacity of the taskbar border, as well as the app dividers. Range 0-100 (Default is 20) | Non-negative integer |
| `TaskbarBorderColorHex` | Border color (HEX) | Set the color of the taskbar border and app dividers, Hex color as `#RRGGBB` (Default is `#ffffff`) | string hex color |
| `TaskbarBorderThickness` | Taskbar border thickness scale (%) | Set the scale of the taskbar border. Range 0-100 (Default is 8) | unsigned int percentage |
| `AppsDividerThickness` | Apps divider thickness scale (%) | Set the thickness scale of the taskbar dividers. Range 0-100 (Default is 8) | unsigned int percentage |
| `AppsDividerVerticalScale` | Apps divider vertical scale (%) | Set the vertical scale of the taskbar dividers. Range 0-100 (Default is 40) | unsigned int percentage |
| `AppsDividerAlignment` | Choose the side on which the app dividers should appear |  |  |
| `DividedAppNames` | App names for divider placement | Type partial app names where you'd like a divider to appear. Use ; to separate multiple entries (e.g., Steam; Notepad\+\+; Settings). Case-insensitive and supports regex. | string regex |
| `TrayAreaDivider` | Tray area divider | When enabled, the tray area will be separated by a divider (Default is on) | Boolean (true/false) |
| `StyleTrayArea` | Modify the tray area appearance | When enabled, the options for tray icon size will take effect (Default is off) | Boolean (true/false) |
| `TrayIconSize` | Tray icon size | Set the width and height of tray icons. Minimum is 15. (Default is 15) | Non-negative integer |
| `TrayButtonSize` | Tray button size | Set the size (width and height) of tray buttons. Minimum is 20. (Default is 30) | Non-negative integer |
| `MoveFlyoutStartMenu` | Move Start Menu with Taskbar | When enabled, the Start and Search menus are moved to align with taskbar size and location (Default is on). | Boolean (true/false) |
| `MoveFlyoutControlCenter` | Move Control Center with Taskbar | When enabled, the Control Center is moved to align with taskbar size and location (Default is on). | Boolean (true/false) |
| `MoveFlyoutNotificationCenter` | Move Notification Center with Taskbar | When enabled, the Notification Center is moved to align with taskbar size and location (Default is on). | Boolean (true/false) |
| `AlignFlyoutInner` | Align flyout windows to the inside of the taskbar | When enabled, the flyout windows will be aligned within the bounds of the taskbar. When off, they will be 50% inside the taskbar bounds (Default is on). | Boolean (true/false) |
*/
// ==/WindhawkModReadme==
// ==WindhawkModSettings==
/*
- TaskbarHeight: 74
  $name: Taskbar height
  $description: Set the height of the taskbar (Default is 74)
- TaskbarIconSize: 42
  $name: Taskbar icon size
  $description: Set the width and height of taskbar icons (Default is 42)
- TaskbarButtonSize: 74
  $name: Taskbar button size
  $description: Set the size (width and height) of taskbar buttons (Default is 74)
- TaskbarOffsetY: 6
  $name: Taskbar vertical offset
  $description: Move the taskbar up or down. Padding of the same value is applied to the top (Default is 6)
- TrayTaskGap: 10
  $name: Tray task gap
  $description: Adjust the space between the task area and the tray area (Default is 10)
- TaskbarBackgroundHorizontalPadding: 2
  $name: Taskbar background horizontal padding
  $description: Set the horizontal padding on both sides of the taskbar background (Default is 2)
- FullWidthTaskbarBackground: false
  $name: Full-width taskbar background
  $description: When enabled, the taskbar background fills the entire width of the screen, similar to the default Windows behavior (Default is off)
- IgnoreShowDesktopButton: false
  $name: Ignore "Show Desktop" button
  $description: When enabled, the "Show Desktop" button is ignored in width calculations (Default is off)
- TaskbarCornerRadius: 22
  $name: Taskbar corner radius
  $description: Controls how rounded the taskbar corners appear (Default is 22)
- TaskButtonCornerRadius: 16
  $name: Task button corner radius
  $description: Controls how rounded the corners of individual task buttons are (Default is 16)
- FlatTaskbarBottomCorners: false
  $name: Flat bottom corners
  $description: When enabled, the bottom corners of the taskbar will be squared and the taskbar will dock to the screen edge. This overrides the taskbar offset; this is always on with the full-width taskbar background option (Default is off)
- CustomizeTaskbarBackground: true
  $name: Stylize the taskbar background
  $description: When enabled, the taskbar background will be changed to acrylic blur. Disable this option if you are using other mods that change the taskbar background. You may need to restart explorer.exe to restore the default taskbar background (Default is on)
- TaskbarBackgroundOpacity: 100
  $name: Background opacity
  $description: Adjust the opacity of the taskbar background. 0 = fully transparent, 100 = fully opaque (Default is 100)
- TaskbarBackgroundTint: 0
  $name: Background tint
  $description: Modify the taskbar tint level. Higher values = more tint. Range 0-100 (Default is 0)
- TaskbarBackgroundLuminosity: 30
  $name: Background luminosity
  $description: Adjust luminosity of the taskbar background. Higher values = more opaque, lower values = more glass-like. Range 0-100 (Default is 30)
- TaskbarBorderOpacity: 20
  $name: Border opacity
  $description: Set the opacity of the taskbar border, as well as the app dividers. Range 0-100 (Default is 20)
- TaskbarBorderColorHex: "#ffffff"
  $name: Border color (HEX)
  $description: Set the color of the taskbar border and app dividers, Hex color as `#RRGGBB` (Default is `#ffffff`)
- TaskbarBorderThickness: 8
  $name: Taskbar border thickness scale (%)
  $description: Set the scale of the taskbar border. Range 0-100 (Default is 8)
- AppsDividerThickness: 8
  $name: Apps divider thickness scale (%)
  $description: Set the thickness scale of the taskbar dividers. Range 0-100 (Default is 8)
- AppsDividerVerticalScale: 40
  $name: Apps divider vertical scale (%)
  $description: Set the vertical scale of the taskbar dividers. Range 0-100 (Default is 40)
- AppsDividerAlignment: ""
  $name: Choose the side on which the app dividers should appear
  $options:
  - left: Left side
  - right: Right side
- DividedAppNames: ""
  $name: App names for divider placement
  $description: Type partial app names where you'd like a divider to appear. Use ; to separate multiple entries (e.g., Steam; Notepad\+\+; Settings). Case-insensitive and supports regex.
- TrayAreaDivider: true
  $name: Tray area divider
  $description: When enabled, the tray area will be separated by a divider (Default is on)
- StyleTrayArea: false
  $name: Modify the tray area appearance
  $description: When enabled, the options for tray icon size will take effect (Default is off)
- TrayIconSize: 15
  $name: Tray icon size
  $description: Set the width and height of tray icons. Minimum is 15. (Default is 15)
- TrayButtonSize: 30
  $name: Tray button size
  $description: Set the size (width and height) of tray buttons. Minimum is 20. (Default is 30)
- MoveFlyoutStartMenu: true
  $name: Move Start Menu with Taskbar
  $description: When enabled, the Start and Search menus are moved to align with taskbar size and location (Default is on).
- MoveFlyoutControlCenter: true
  $name: Move Control Center with Taskbar
  $description: When enabled, the Control Center is moved to align with taskbar size and location (Default is on).
- MoveFlyoutNotificationCenter: true
  $name: Move Notification Center with Taskbar
  $description: When enabled, the Notification Center is moved to align with taskbar size and location (Default is on).
- AlignFlyoutInner: true
  $name: Align flyout windows to the inside of the taskbar
  $description: When enabled, the flyout windows will be aligned within the bounds of the taskbar. When off, they will be 50% inside the taskbar bounds (Default is on).
*/
// ==/WindhawkModSettings==
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////  .___________..______    __    ______   ______   .__   __.      _______. __   ________   _______   ////
////  |           ||   _  \  |  |  /      | /  __  \  |  \ |  |     /       ||  | |       /  |   ____|  ////
////  `---|  |----`|  |_)  | |  | |  ,----'|  |  |  | |   \|  |    |   (----`|  | `---/  /   |  |__     ////
////      |  |     |   _  <  |  | |  |     |  |  |  | |  . `  |     \   \    |  |    /  /    |   __|    ////
////      |  |     |  |_)  | |  | |  `----.|  `--'  | |  |\   | .----)   |   |  |   /  /----.|  |____   ////
////      |__|     |______/  |__|  \______| \______/  |__| \__| |_______/    |__|  /________||_______|  ////
////                                                                                                    ////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <windhawk_utils.h>
#undef GetCurrentTime
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <atomic>
#include <functional>
#include <limits>
#include <optional>
#include <regex>
using namespace winrt::Windows::UI::Xaml;
struct {
    int taskbarHeight;
    int iconSize;
    int taskbarButtonWidth;
    int iconSizeSmall;
    int taskbarButtonWidthSmall;
} g_settings_tbiconsize;
std::atomic<bool> g_taskbarViewDllLoadedTBIconSize;
std::atomic<bool> g_searchUxUiDllLoaded;
std::atomic<bool> g_applyingSettings;
std::atomic<bool> g_pendingMeasureOverride;
std::atomic<bool> g_unloading;
std::atomic<int> g_hookCallCounter;
bool g_hasDynamicIconScaling;
bool g_smallIconSize;
int g_originalTaskbarHeight;
int g_taskbarHeight;
std::atomic<DWORD> g_shellIconLoaderV2_LoadAsyncIcon__ResumeCoro_ThreadId;
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
        struct TaskbarState {
  std::chrono::steady_clock::time_point lastApplyStyleTime{};
  struct Data {
    int childrenCount;
    int rightMostEdge;
    unsigned int childrenWidth;
  } lastTaskbarData{};
  unsigned int lastChildrenWidthTaskbar{0};
  unsigned int lastTrayFrameWidth{0};
  float lastTargetWidth{0};
  float lastTargetOffsetX{0};
  float lastTargetOffsetY{0};
  float initOffsetX{-1};
  bool wasOverflowing{false};
  float lastStartButtonXCalculated=0.0f;
  float lastStartButtonXActual=0.0f;
  float lastRootWidth=0.0f;
  float lastTargetTaskFrameOffsetX=0.0f;
  float lastLeftMostEdgeTray{0};
  int lastRightMostEdgeTray{0};
};
static std::unordered_map<std::wstring, TaskbarState> g_taskbarStates;
        struct {
  int userDefinedTrayTaskGap;
  int userDefinedTaskbarBackgroundHorizontalPadding;
  unsigned int userDefinedTaskbarOffsetY;
  unsigned int userDefinedTaskbarHeight;
  unsigned int userDefinedTaskbarIconSize;
  unsigned int userDefinedTrayIconSize;
  unsigned int userDefinedTaskbarButtonSize;
  unsigned int userDefinedTrayButtonSize;
  float userDefinedTaskbarCornerRadius;
  unsigned int userDefinedTaskButtonCornerRadius;
  bool userDefinedFlatTaskbarBottomCorners;
  unsigned int userDefinedTaskbarBackgroundOpacity;
  unsigned int userDefinedTaskbarBackgroundTint;
  unsigned int userDefinedTaskbarBackgroundLuminosity;
  uint8_t userDefinedTaskbarBorderOpacity;
  double userDefinedTaskbarBorderThickness;
  bool userDefinedFullWidthTaskbarBackground;
  bool userDefinedIgnoreShowDesktopButton;
  bool userDefinedStyleTrayArea;
  bool userDefinedTrayAreaDivider;
  unsigned int borderColorR, borderColorG, borderColorB;
  std::vector<std::wstring> userDefinedDividedAppNames;
  bool userDefinedAlignFlyoutInner;
  bool userDefinedCustomizeTaskbarBackground;
  double userDefinedAppsDividerThickness;
  float userDefinedAppsDividerVerticalScale{0.7};
  bool userDefinedDividerLeftAligned=false;
} g_settings;
        void ApplySettingsDebounced(int delayMs);
void ApplySettingsDebounced();
void ApplySettingsFromTaskbarThreadIfRequired();
bool g_invalidateDimensions =true;
int g_lastRecordedStartMenuWidth=670;
std::atomic<bool> g_already_requested_debounce_initializing = false;
STDAPI GetDpiForMonitor(HMONITOR hmonitor, MONITOR_DPI_TYPE dpiType, UINT* dpiX, UINT* dpiY);
#include <Windows.h>
bool IsStartMenuOrbLeftAligned() {
  Wh_Log(L".");
    DWORD value = 0;
    DWORD size = sizeof(value);
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
                     LR"(Software\Microsoft\Windows\CurrentVersion\Explorer\Advanced)",
                      0, KEY_READ, &hKey) == ERROR_SUCCESS) {
  Wh_Log(L".");
        if (RegQueryValueExW(hKey, L"TaskbarAl", nullptr, nullptr,
                             reinterpret_cast<LPBYTE>(&value), &size) == ERROR_SUCCESS) {
  Wh_Log(L".");
            RegCloseKey(hKey);
            return value == 0;
        }
        RegCloseKey(hKey);
    }
    return false;
}
        std::wstring GetMonitorName(HMONITOR monitor) {
  Wh_Log(L".");
            MONITORINFOEX monitorInfo = {};
            monitorInfo.cbSize = sizeof(MONITORINFOEX);
            if (monitor && GetMonitorInfo(monitor, &monitorInfo)) {
  Wh_Log(L".");
                return std::wstring(monitorInfo.szDevice);
            }
            return L"default";
        }
        std::wstring GetMonitorName(HWND hwnd) {
  Wh_Log(L".");
            HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
            return GetMonitorName(monitor);
        }
        STDAPI GetDpiForMonitor(HMONITOR hmonitor,
                        MONITOR_DPI_TYPE dpiType,
                        UINT* dpiX,
                        UINT* dpiY);
size_t OffsetFromAssemblyRegex(void* func,
                               size_t defValue,
                               std::regex regex,
                               int limit = 30) {
  Wh_Log(L".");
    BYTE* p = (BYTE*)func;
    for (int i = 0; i < limit; i++) {
  Wh_Log(L".");
        WH_DISASM_RESULT result;
        if (!Wh_Disasm(p, &result)) {
  Wh_Log(L".");
            break;
        }
        p += result.length;
        std::string_view s = result.text;
        if (s == "ret") {
  Wh_Log(L".");
            break;
        }
        std::match_results<std::string_view::const_iterator> match;
        if (std::regex_match(s.begin(), s.end(), match, regex)) {
  Wh_Log(L".");
            return std::stoull(match[1], nullptr, 16);
        }
    }
    Wh_Log(L"Failed for %p", func);
    return defValue;
}
std::optional<bool> IsOsFeatureEnabled(UINT32 featureId) {
  Wh_Log(L".");
    enum FEATURE_ENABLED_STATE {
        FEATURE_ENABLED_STATE_DEFAULT = 0,
        FEATURE_ENABLED_STATE_DISABLED = 1,
        FEATURE_ENABLED_STATE_ENABLED = 2,
    };
#pragma pack(push, 1)
    struct RTL_FEATURE_CONFIGURATION {
        unsigned int featureId;
        unsigned __int32 group : 4;
        FEATURE_ENABLED_STATE enabledState : 2;
        unsigned __int32 enabledStateOptions : 1;
        unsigned __int32 unused1 : 1;
        unsigned __int32 variant : 6;
        unsigned __int32 variantPayloadKind : 2;
        unsigned __int32 unused2 : 16;
        unsigned int payload;
    };
#pragma pack(pop)
    using RtlQueryFeatureConfiguration_t =
        int(NTAPI*)(UINT32, int, INT64*, RTL_FEATURE_CONFIGURATION*);
    static RtlQueryFeatureConfiguration_t pRtlQueryFeatureConfiguration = []() {
  Wh_Log(L".");
        HMODULE hNtDll = GetModuleHandle(L"ntdll.dll");
        return hNtDll ? (RtlQueryFeatureConfiguration_t)GetProcAddress(
                            hNtDll, "RtlQueryFeatureConfiguration")
                      : nullptr;
    }();
    if (!pRtlQueryFeatureConfiguration) {
  Wh_Log(L".");
        Wh_Log(L"RtlQueryFeatureConfiguration not found");
        return std::nullopt;
    }
    RTL_FEATURE_CONFIGURATION feature = {0};
    INT64 changeStamp = 0;
    HRESULT hr =
        pRtlQueryFeatureConfiguration(featureId, 1, &changeStamp, &feature);
    if (SUCCEEDED(hr)) {
  Wh_Log(L".");
        Wh_Log(L"RtlQueryFeatureConfiguration result for %u: %d", featureId,
               feature.enabledState);
        switch (feature.enabledState) {
  Wh_Log(L".");
            case FEATURE_ENABLED_STATE_DISABLED:
                return false;
            case FEATURE_ENABLED_STATE_ENABLED:
                return true;
            case FEATURE_ENABLED_STATE_DEFAULT:
                return std::nullopt;
        }
    } else {
        Wh_Log(L"RtlQueryFeatureConfiguration error for %u: %08X", featureId,
               hr);
    }
    return std::nullopt;
}
FrameworkElement EnumChildElements(
    FrameworkElement element,
    std::function<bool(FrameworkElement)> enumCallback) {
  Wh_Log(L".");
    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < childrenCount; i++) {
  Wh_Log(L".");
        auto child = Media::VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (!child) {
  Wh_Log(L".");
            Wh_Log(L"Failed to get child %d of %d", i + 1, childrenCount);
            continue;
        }
        if (enumCallback(child)) {
  Wh_Log(L".");
            return child;
        }
    }
    return nullptr;
}
FrameworkElement FindChildByName(FrameworkElement element, PCWSTR name) {
  Wh_Log(L".");
    return EnumChildElements(element, [name](FrameworkElement child) {
  Wh_Log(L".");
        return child.Name() == name;
    });
}
FrameworkElement FindChildByClassName(FrameworkElement element,
                                      PCWSTR className) {
  Wh_Log(L".");
    return EnumChildElements(element, [className](FrameworkElement child) {
  Wh_Log(L".");
        return winrt::get_class_name(child) == className;
    });
}
void OverrideResourceDirectoryLookup(
    PCSTR sourceFunctionName,
    const winrt::Windows::Foundation::IInspectable* key,
    winrt::Windows::Foundation::IInspectable* value) {
  Wh_Log(L".");
    if (g_unloading) {
  Wh_Log(L".");
        return;
    }
    const auto keyString = key->try_as<winrt::hstring>();
    if (!keyString) {
  Wh_Log(L".");
        return;
    }
    double newValueDouble;
    if (*keyString == L"MediumTaskbarButtonExtent") {
  Wh_Log(L".");
        newValueDouble = g_settings_tbiconsize.taskbarButtonWidth;
    } else if (*keyString == L"SmallTaskbarButtonExtent") {
  Wh_Log(L".");
        newValueDouble = g_settings_tbiconsize.taskbarButtonWidthSmall;
    } else {
        return;
    }
    const auto valueDouble = value->try_as<double>();
    if (!valueDouble) {
  Wh_Log(L".");
        return;
    }
    if (newValueDouble != *valueDouble) {
  Wh_Log(L".");
        Wh_Log(L"[%S] Overriding value %s: %f->%f", sourceFunctionName,
               keyString->c_str(), *valueDouble, newValueDouble);
        *value = winrt::box_value(newValueDouble);
    }
}
using ResourceDictionary_Lookup_TaskbarView_t =
    winrt::Windows::Foundation::IInspectable*(
        WINAPI*)(void* pThis,
                 void** result,
                 winrt::Windows::Foundation::IInspectable* key);
ResourceDictionary_Lookup_TaskbarView_t
    ResourceDictionary_Lookup_TaskbarView_Original;
winrt::Windows::Foundation::IInspectable* WINAPI
ResourceDictionary_Lookup_TaskbarView_Hook(
    void* pThis,
    void** result,
    winrt::Windows::Foundation::IInspectable* key) {
  Wh_Log(L".");
    auto ret =
        ResourceDictionary_Lookup_TaskbarView_Original(pThis, result, key);
    if (!*ret) {
  Wh_Log(L".");
        return ret;
    }
    OverrideResourceDirectoryLookup(__FUNCTION__, key, ret);
    return ret;
}
using ResourceDictionary_Lookup_SearchUxUi_t =
    winrt::Windows::Foundation::IInspectable*(
        WINAPI*)(void* pThis,
                 void** result,
                 winrt::Windows::Foundation::IInspectable* key);
ResourceDictionary_Lookup_SearchUxUi_t
    ResourceDictionary_Lookup_SearchUxUi_Original;
winrt::Windows::Foundation::IInspectable* WINAPI
ResourceDictionary_Lookup_SearchUxUi_Hook(
    void* pThis,
    void** result,
    winrt::Windows::Foundation::IInspectable* key) {
  Wh_Log(L".");
    auto ret =
        ResourceDictionary_Lookup_SearchUxUi_Original(pThis, result, key);
    if (!*ret) {
  Wh_Log(L".");
        return ret;
    }
    OverrideResourceDirectoryLookup(__FUNCTION__, key, ret);
    return ret;
}
using IconUtils_GetIconSize_t = void(WINAPI*)(bool isSmall,
                                              int type,
                                              SIZE* size);
IconUtils_GetIconSize_t IconUtils_GetIconSize_Original;
void WINAPI IconUtils_GetIconSize_Hook(bool isSmall, int type, SIZE* size) {
  Wh_Log(L".");
    [[maybe_unused]] static bool logged = [] {
        Wh_Log(L"> [%S] First call, hasDynamicIconScaling=%d",
               __PRETTY_FUNCTION__, g_hasDynamicIconScaling);
        return true;
    }();
    if (g_hasDynamicIconScaling) {
  Wh_Log(L".");
        IconUtils_GetIconSize_Original(isSmall, type, size);
        return;
    }
    IconUtils_GetIconSize_Original(isSmall, type, size);
    if (!g_unloading && !isSmall) {
  Wh_Log(L".");
        size->cx = MulDiv(size->cx, g_settings_tbiconsize.iconSize, 24);
        size->cy = MulDiv(size->cy, g_settings_tbiconsize.iconSize, 24);
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
  Wh_Log(L".");
    [[maybe_unused]] static bool logged = [] {
        Wh_Log(L"> [%S] First call, hasDynamicIconScaling=%d",
               __PRETTY_FUNCTION__, g_hasDynamicIconScaling);
        return true;
    }();
    if (g_hasDynamicIconScaling) {
  Wh_Log(L".");
        return IconContainer_IsStorageRecreationRequired_Original(pThis, param1,
                                                                  flags);
    }
    if (g_applyingSettings) {
  Wh_Log(L".");
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
  Wh_Log(L".");
    TrayUI_GetMinSize_Original(pThis, monitor, size);
    if (g_taskbarHeight) {
  Wh_Log(L".");
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
  Wh_Log(L".");
    Wh_Log(L"> hasDynamicIconScaling=%d, nIndex=%d", g_hasDynamicIconScaling,
           nIndex);
    if (g_hasDynamicIconScaling) {
  Wh_Log(L".");
        return CIconLoadingFunctions_GetClassLongPtrW_Original(pThis, hWnd,
                                                               nIndex);
    }
    if (!g_unloading && nIndex == GCLP_HICON && g_settings_tbiconsize.iconSize <= 16) {
  Wh_Log(L".");
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
  Wh_Log(L".");
    Wh_Log(L"> hasDynamicIconScaling=%d, Msg=%u, wParam=%zu, lParam=%zu",
           g_hasDynamicIconScaling, Msg, wParam, lParam);
    if (g_hasDynamicIconScaling) {
  Wh_Log(L".");
        return CIconLoadingFunctions_SendMessageCallbackW_Original(
            pThis, hWnd, Msg, wParam, lParam, lpResultCallBack, dwData);
    }
    if (!g_unloading && Msg == WM_GETICON && wParam == ICON_BIG &&
        g_settings_tbiconsize.iconSize <= 16) {
  Wh_Log(L".");
        wParam = ICON_SMALL2;
    }
    BOOL ret = CIconLoadingFunctions_SendMessageCallbackW_Original(
        pThis, hWnd, Msg, wParam, lParam, lpResultCallBack, dwData);
    return ret;
}
using ShellIconLoaderV2_LoadAsyncIcon__ResumeCoro_t =
    void(WINAPI*)(void* pThis);
ShellIconLoaderV2_LoadAsyncIcon__ResumeCoro_t
    ShellIconLoaderV2_LoadAsyncIcon__ResumeCoro_Original;
void WINAPI ShellIconLoaderV2_LoadAsyncIcon__ResumeCoro_Hook(void* pThis) {
  Wh_Log(L".");
    Wh_Log(L"> hasDynamicIconScaling=%d", g_hasDynamicIconScaling);
    if (g_hasDynamicIconScaling) {
  Wh_Log(L".");
        ShellIconLoaderV2_LoadAsyncIcon__ResumeCoro_Original(pThis);
        return;
    }
    g_shellIconLoaderV2_LoadAsyncIcon__ResumeCoro_ThreadId =
        GetCurrentThreadId();
    ShellIconLoaderV2_LoadAsyncIcon__ResumeCoro_Original(pThis);
    g_shellIconLoaderV2_LoadAsyncIcon__ResumeCoro_ThreadId = 0;
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
  Wh_Log(L".");
    TrayUI__HandleSettingChange_Original(pThis, param1, param2, param3, param4);
    if (g_applyingSettings) {
  Wh_Log(L".");
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
  Wh_Log(L".");
    [[maybe_unused]] static bool logged = [] {
        Wh_Log(L"> [%S] First call, hasDynamicIconScaling=%d",
               __PRETTY_FUNCTION__, g_hasDynamicIconScaling);
        return true;
    }();
    if (g_hasDynamicIconScaling) {
  Wh_Log(L".");
        return TaskListItemViewModel_GetIconHeight_Original(pThis, param1,
                                                            iconHeight);
    }
    int ret =
        TaskListItemViewModel_GetIconHeight_Original(pThis, param1, iconHeight);
    if (!g_unloading) {
  Wh_Log(L".");
        *iconHeight = g_settings_tbiconsize.iconSize;
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
  Wh_Log(L".");
    Wh_Log(L"> hasDynamicIconScaling=%d", g_hasDynamicIconScaling);
    if (g_hasDynamicIconScaling) {
  Wh_Log(L".");
        return TaskListGroupViewModel_GetIconHeight_Original(pThis, param1,
                                                             iconHeight);
    }
    int ret = TaskListGroupViewModel_GetIconHeight_Original(pThis, param1,
                                                            iconHeight);
    if (!g_unloading) {
  Wh_Log(L".");
        *iconHeight = g_settings_tbiconsize.iconSize;
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
  Wh_Log(L".");
    Wh_Log(L"> hasDynamicIconScaling=%d, enumTaskbarSize=%d",
           g_hasDynamicIconScaling, enumTaskbarSize);
    if (g_hasDynamicIconScaling) {
  Wh_Log(L".");
        Wh_Log(L"Setting hasDynamicIconScaling to false");
        g_hasDynamicIconScaling = false;
    }
    if (!g_unloading && (enumTaskbarSize == 1 || enumTaskbarSize == 2)) {
  Wh_Log(L".");
        return g_settings_tbiconsize.iconSize ;
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
  Wh_Log(L".");
    Wh_Log(L"> hasDynamicIconScaling=%d, baseHeight=%f",
           g_hasDynamicIconScaling, baseHeight);
    if (g_hasDynamicIconScaling) {
  Wh_Log(L".");
        Wh_Log(L"Setting hasDynamicIconScaling to false");
        g_hasDynamicIconScaling = false;
    }
    if (!g_unloading) {
  Wh_Log(L".");
        return g_settings_tbiconsize.iconSize ;
    }
    return TaskbarConfiguration_GetIconHeightInViewPixels_double_Original(
        baseHeight);
}
using TaskbarConfiguration_GetIconHeightInViewPixels_method_t =
    double(WINAPI*)(void* pThis);
TaskbarConfiguration_GetIconHeightInViewPixels_method_t
    TaskbarConfiguration_GetIconHeightInViewPixels_method_Original;
double WINAPI
TaskbarConfiguration_GetIconHeightInViewPixels_method_Hook(void* pThis) {
  Wh_Log(L".");
    [[maybe_unused]] static bool logged = [] {
        Wh_Log(L"> [%S] First call, hasDynamicIconScaling=%d",
               __PRETTY_FUNCTION__, g_hasDynamicIconScaling);
        return true;
    }();
    double iconSize =
        TaskbarConfiguration_GetIconHeightInViewPixels_method_Original(pThis);
    if (!g_unloading) {
  Wh_Log(L".");
        return iconSize <= 16 ? g_settings_tbiconsize.iconSizeSmall : g_settings_tbiconsize.iconSize;
    }
    return iconSize;
}
using TaskListButton_IconHeight_t = void(WINAPI*)(void* pThis, double height);
TaskListButton_IconHeight_t TaskListButton_IconHeight_Original;
size_t GetIconHeightOffset() {
  Wh_Log(L".");
    static size_t iconHeightOffset = []() {
  Wh_Log(L".");
        size_t offset =
#if defined(_M_X64)
            OffsetFromAssemblyRegex(
                (void*)TaskListButton_IconHeight_Original, 0,
                std::regex(R"(movsd xmm\d+, qword ptr \[rcx\+0x([0-9a-f]+)\])",
                           std::regex_constants::icase),
                30);
#elif defined(_M_ARM64)
            OffsetFromAssemblyRegex(
                (void*)TaskListButton_IconHeight_Original, 0,
                std::regex(R"(ldr\s+d\d+, \[x\d+, #0x([0-9a-f]+)\])",
                           std::regex_constants::icase),
                30);
#else
#error "Unsupported architecture"
#endif
        Wh_Log(L"iconHeightOffset=0x%X", offset);
        return offset > 0xFFFF ? 0 : offset;
    }();
    return iconHeightOffset;
}
void TaskListButton_IconHeight_InitOffsets() {
  Wh_Log(L".");
    GetIconHeightOffset();
}
using SystemTrayController_GetFrameSize_t =
    double(WINAPI*)(void* pThis, int enumTaskbarSize);
SystemTrayController_GetFrameSize_t SystemTrayController_GetFrameSize_Original;
double WINAPI SystemTrayController_GetFrameSize_Hook(void* pThis,
                                                     int enumTaskbarSize) {
  Wh_Log(L".");
    Wh_Log(L"> %d", enumTaskbarSize);
    if (g_taskbarHeight && (enumTaskbarSize == 1 || enumTaskbarSize == 2)) {
  Wh_Log(L".");
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
  Wh_Log(L".");
    Wh_Log(L"> %d", enumTaskbarSize);
    if (g_taskbarHeight && (enumTaskbarSize == 1 || enumTaskbarSize == 2)) {
  Wh_Log(L".");
        return g_taskbarHeight;
    }
    return SystemTraySecondaryController_GetFrameSize_Original(pThis,
                                                               enumTaskbarSize);
}
using TaskbarConfiguration_GetFrameSize_t =
    double(WINAPI*)(int enumTaskbarSize);
TaskbarConfiguration_GetFrameSize_t TaskbarConfiguration_GetFrameSize_Original;
double WINAPI TaskbarConfiguration_GetFrameSize_Hook(int enumTaskbarSize) {
  Wh_Log(L".");
    Wh_Log(L"> %d", enumTaskbarSize);
    if (!g_originalTaskbarHeight &&
        (enumTaskbarSize == 1 || enumTaskbarSize == 2)) {
  Wh_Log(L".");
        g_originalTaskbarHeight =
            TaskbarConfiguration_GetFrameSize_Original(enumTaskbarSize);
    }
    if (g_taskbarHeight && (enumTaskbarSize == 1 || enumTaskbarSize == 2)) {
  Wh_Log(L".");
        return g_taskbarHeight;
    }
    return TaskbarConfiguration_GetFrameSize_Original(enumTaskbarSize);
}
#ifdef _M_ARM64
thread_local double* g_TaskbarConfiguration_UpdateFrameSize_frameSize;
using TaskbarConfiguration_UpdateFrameSize_t = void(WINAPI*)(void* pThis);
TaskbarConfiguration_UpdateFrameSize_t
    TaskbarConfiguration_UpdateFrameSize_SymbolAddress;
LONG GetFrameSizeOffset() {
  Wh_Log(L".");
    static LONG frameSizeOffset = []() -> LONG {
        const DWORD* start =
            (const DWORD*)TaskbarConfiguration_UpdateFrameSize_SymbolAddress;
        const DWORD* end = start + 0x80;
        std::regex regex1(R"(str\s+d\d+, \[x\d+, #0x([0-9a-f]+)\])");
        for (const DWORD* p = start; p != end; p++) {
  Wh_Log(L".");
            WH_DISASM_RESULT result1;
            if (!Wh_Disasm((void*)p, &result1)) {
  Wh_Log(L".");
                break;
            }
            std::string_view s1 = result1.text;
            if (s1 == "ret") {
  Wh_Log(L".");
                break;
            }
            std::match_results<std::string_view::const_iterator> match1;
            if (!std::regex_match(s1.begin(), s1.end(), match1, regex1)) {
  Wh_Log(L".");
                continue;
            }
            LONG offset = std::stoull(match1[1], nullptr, 16);
            Wh_Log(L"frameSizeOffset=0x%X", offset);
            return (offset < 0 || offset > 0xFFFF) ? 0 : offset;
        }
        Wh_Log(L"frameSizeOffset not found");
        return 0;
    }();
    return frameSizeOffset;
}
void TaskbarConfiguration_UpdateFrameSize_InitOffsets() {
  Wh_Log(L".");
    GetFrameSizeOffset();
}
TaskbarConfiguration_UpdateFrameSize_t
    TaskbarConfiguration_UpdateFrameSize_Original;
void WINAPI TaskbarConfiguration_UpdateFrameSize_Hook(void* pThis) {
  Wh_Log(L".");
    LONG frameSizeOffset = GetFrameSizeOffset();
    if (!frameSizeOffset) {
  Wh_Log(L".");
        Wh_Log(L"Error: frameSizeOffset is invalid");
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
  Wh_Log(L".");
    if (g_TaskbarConfiguration_UpdateFrameSize_frameSize) {
  Wh_Log(L".");
        if (!g_originalTaskbarHeight) {
  Wh_Log(L".");
            g_originalTaskbarHeight =
                *g_TaskbarConfiguration_UpdateFrameSize_frameSize;
        }
        if (g_taskbarHeight) {
  Wh_Log(L".");
            *g_TaskbarConfiguration_UpdateFrameSize_frameSize = g_taskbarHeight;
        }
    }
    Event_operator_call_Original(pThis);
}
#endif  // _M_ARM64
using SystemTrayController_UpdateFrameSize_t = void(WINAPI*)(void* pThis);
SystemTrayController_UpdateFrameSize_t
    SystemTrayController_UpdateFrameSize_SymbolAddress;
LONG GetLastHeightOffset() {
  Wh_Log(L".");
    static LONG lastHeightOffset = []() -> LONG {
#if defined(_M_X64)
        const BYTE* start =
            (const BYTE*)SystemTrayController_UpdateFrameSize_SymbolAddress;
        const BYTE* end = start + 0x200;
        for (const BYTE* p = start; p != end; p++) {
  Wh_Log(L".");
            if (p[0] == 0x66 && p[1] == 0x0F && p[2] == 0x2E && p[3] == 0xB3 &&
                p[8] == 0x7A && p[10] == 0x75) {
  Wh_Log(L".");
                LONG offset = *(LONG*)(p + 4);
                Wh_Log(L"lastHeightOffset=0x%X", offset);
                return (offset < 0 || offset > 0xFFFF) ? 0 : offset;
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
  Wh_Log(L".");
            WH_DISASM_RESULT result1;
            if (!Wh_Disasm((void*)p, &result1)) {
  Wh_Log(L".");
                break;
            }
            std::string_view s1 = result1.text;
            if (s1 == "ret") {
  Wh_Log(L".");
                break;
            }
            std::match_results<std::string_view::const_iterator> match1;
            if (!std::regex_match(s1.begin(), s1.end(), match1, regex1)) {
  Wh_Log(L".");
                continue;
            }
            WH_DISASM_RESULT result2;
            if (!Wh_Disasm((void*)(p + 1), &result2)) {
  Wh_Log(L".");
                break;
            }
            std::string_view s2 = result2.text;
            if (!std::regex_match(s2.begin(), s2.end(), regex2)) {
  Wh_Log(L".");
                continue;
            }
            WH_DISASM_RESULT result3;
            if (!Wh_Disasm((void*)(p + 2), &result3)) {
  Wh_Log(L".");
                break;
            }
            std::string_view s3 = result3.text;
            if (!std::regex_match(s3.begin(), s3.end(), regex3)) {
  Wh_Log(L".");
                continue;
            }
            LONG offset = std::stoull(match1[1], nullptr, 16);
            Wh_Log(L"lastHeightOffset=0x%X", offset);
            return (offset < 0 || offset > 0xFFFF) ? 0 : offset;
        }
#else
#error "Unsupported architecture"
#endif
        Wh_Log(L"lastHeightOffset not found");
        return 0;
    }();
    return lastHeightOffset;
}
void SystemTrayController_UpdateFrameSize_InitOffsets() {
  Wh_Log(L".");
    GetLastHeightOffset();
}
SystemTrayController_UpdateFrameSize_t
    SystemTrayController_UpdateFrameSize_Original;
void WINAPI SystemTrayController_UpdateFrameSize_Hook(void* pThis) {
  Wh_Log(L".");
    LONG lastHeightOffset = GetLastHeightOffset();
    if (lastHeightOffset) {
  Wh_Log(L".");
        *(double*)((BYTE*)pThis + lastHeightOffset) = 0;
    } else {
        Wh_Log(L"Error: lastHeightOffset is invalid");
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
  Wh_Log(L".");
    if (TaskbarFrame_MaxHeight_double_Original) {
  Wh_Log(L".");
        TaskbarFrame_MaxHeight_double_Original(
            pThis, std::numeric_limits<double>::infinity());
    }
    return TaskbarFrame_Height_double_Original(pThis, value);
}
void* TaskbarController_OnGroupingModeChanged_Original;
LONG GetTaskbarFrameOffset() {
  Wh_Log(L".");
    static LONG taskbarFrameOffset = []() -> LONG {
#if defined(_M_X64)
        const BYTE* p =
            (const BYTE*)TaskbarController_OnGroupingModeChanged_Original;
        if (p && p[0] == 0x48 && p[1] == 0x83 && p[2] == 0xEC &&
            (p[4] == 0x48 || p[4] == 0x4C) && p[5] == 0x8B &&
            (p[6] & 0xC0) == 0x80) {
  Wh_Log(L".");
            LONG offset = *(LONG*)(p + 7);
            Wh_Log(L"taskbarFrameOffset=0x%X", offset);
            return (offset < 0 || offset > 0xFFFF) ? 0 : offset;
        }
#elif defined(_M_ARM64)
        const DWORD* start =
            (const DWORD*)TaskbarController_OnGroupingModeChanged_Original;
        const DWORD* end = start + 10;
        std::regex regex1(R"(ldr\s+x\d+, \[x\d+, #0x([0-9a-f]+)\])");
        for (const DWORD* p = start; p != end; p++) {
  Wh_Log(L".");
            WH_DISASM_RESULT result1;
            if (!Wh_Disasm((void*)p, &result1)) {
  Wh_Log(L".");
                break;
            }
            std::string_view s1 = result1.text;
            if (s1 == "ret") {
  Wh_Log(L".");
                break;
            }
            std::match_results<std::string_view::const_iterator> match1;
            if (!std::regex_match(s1.begin(), s1.end(), match1, regex1)) {
  Wh_Log(L".");
                continue;
            }
            LONG offset = std::stoull(match1[1], nullptr, 16);
            Wh_Log(L"taskbarFrameOffset=0x%X", offset);
            return (offset < 0 || offset > 0xFFFF) ? 0 : offset;
        }
#else
#error "Unsupported architecture"
#endif
        Wh_Log(L"taskbarFrameOffset not found");
        return 0;
    }();
    return taskbarFrameOffset;
}
void TaskbarController_OnGroupingModeChanged_InitOffsets() {
  Wh_Log(L".");
    GetTaskbarFrameOffset();
}
using TaskbarController_UpdateFrameHeight_t = void(WINAPI*)(void* pThis);
TaskbarController_UpdateFrameHeight_t
    TaskbarController_UpdateFrameHeight_Original;
void WINAPI TaskbarController_UpdateFrameHeight_Hook(void* pThis) {
  Wh_Log(L".");
    LONG taskbarFrameOffset = GetTaskbarFrameOffset();
    if (!taskbarFrameOffset) {
  Wh_Log(L".");
        Wh_Log(L"Error: taskbarFrameOffset is invalid");
        TaskbarController_UpdateFrameHeight_Original(pThis);
        return;
    }
    void* taskbarFrame = *(void**)((BYTE*)pThis + taskbarFrameOffset);
    if (!taskbarFrame) {
  Wh_Log(L".");
        Wh_Log(L"Error: taskbarFrame is null");
        TaskbarController_UpdateFrameHeight_Original(pThis);
        return;
    }
    FrameworkElement taskbarFrameElement = nullptr;
    ((IUnknown**)taskbarFrame)[1]->QueryInterface(
        winrt::guid_of<FrameworkElement>(),
        winrt::put_abi(taskbarFrameElement));
    if (!taskbarFrameElement) {
  Wh_Log(L".");
        Wh_Log(L"Error: taskbarFrameElement is null");
        TaskbarController_UpdateFrameHeight_Original(pThis);
        return;
    }
    taskbarFrameElement.MaxHeight(std::numeric_limits<double>::infinity());
    TaskbarController_UpdateFrameHeight_Original(pThis);
    auto contentGrid = Media::VisualTreeHelper::GetParent(taskbarFrameElement)
                           .try_as<FrameworkElement>();
    if (contentGrid) {
  Wh_Log(L".");
        double height = taskbarFrameElement.Height();
        double contentGridHeight = contentGrid.Height();
        if (contentGridHeight > 0 && contentGridHeight != height) {
  Wh_Log(L".");
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
  Wh_Log(L".");
    g_inSystemTrayController_UpdateFrameSize = true;
    SystemTraySecondaryController_UpdateFrameSize_Original(pThis);
    g_inSystemTrayController_UpdateFrameSize = false;
}
using SystemTrayFrame_Height_t = void(WINAPI*)(void* pThis, double value);
SystemTrayFrame_Height_t SystemTrayFrame_Height_Original;
void WINAPI SystemTrayFrame_Height_Hook(void* pThis, double value) {
  Wh_Log(L".");
    if (g_inSystemTrayController_UpdateFrameSize) {
  Wh_Log(L".");
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
  Wh_Log(L".");
    g_hookCallCounter++;
    int ret = TaskbarFrame_MeasureOverride_Original(pThis, size, resultSize);
    g_pendingMeasureOverride = false;
    g_hookCallCounter--;
    return ret;
}
using TaskListButton_UpdateButtonPadding_t = void(WINAPI*)(void* pThis);
TaskListButton_UpdateButtonPadding_t
    TaskListButton_UpdateButtonPadding_Original;
void WINAPI TaskListButton_UpdateButtonPadding_Hook(void* pThis) {
  Wh_Log(L".");
    Wh_Log(L"> hasDynamicIconScaling=%d", g_hasDynamicIconScaling);
    if (!g_hasDynamicIconScaling || g_unloading) {
  Wh_Log(L".");
        TaskListButton_UpdateButtonPadding_Original(pThis);
        return;
    }
    double* iconHeight = nullptr;
    double prevIconHeight;
    if (size_t iconHeightOffset = GetIconHeightOffset()) {
  Wh_Log(L".");
        iconHeight = (double*)((BYTE*)pThis + iconHeightOffset);
        prevIconHeight = *iconHeight;
        double newIconHeight = g_smallIconSize ? 16 : 24;
        Wh_Log(L"Setting iconHeight: %f->%f", prevIconHeight, newIconHeight);
        *iconHeight = newIconHeight;
    }
    TaskListButton_UpdateButtonPadding_Original(pThis);
    if (iconHeight) {
  Wh_Log(L".");
        *iconHeight = prevIconHeight;
    }
}
using TaskListButton_OverlayIcon_t = void(WINAPI*)(void* pThis, void* param1);
TaskListButton_OverlayIcon_t TaskListButton_OverlayIcon_Original;
void WINAPI TaskListButton_OverlayIcon_Hook(void* pThis, void* param1) {
  Wh_Log(L".");
    Wh_Log(L"> hasDynamicIconScaling=%d", g_hasDynamicIconScaling);
    if (!g_hasDynamicIconScaling || g_unloading) {
  Wh_Log(L".");
        TaskListButton_OverlayIcon_Original(pThis, param1);
        return;
    }
    double* iconHeight = nullptr;
    double prevIconHeight;
    if (size_t iconHeightOffset = GetIconHeightOffset()) {
  Wh_Log(L".");
        iconHeight = (double*)((BYTE*)pThis + iconHeightOffset);
        prevIconHeight = *iconHeight;
        double newIconHeight = 24;
        Wh_Log(L"Setting iconHeight: %f->%f", prevIconHeight, newIconHeight);
        *iconHeight = newIconHeight;
    }
    TaskListButton_OverlayIcon_Original(pThis, param1);
    if (iconHeight) {
  Wh_Log(L".");
        *iconHeight = prevIconHeight;
    }
}
using TaskListButton_UpdateBadge_t = void(WINAPI*)(void* pThis);
TaskListButton_UpdateBadge_t TaskListButton_UpdateBadge_Original;
void WINAPI TaskListButton_UpdateBadge_Hook(void* pThis) {
  Wh_Log(L".");
    Wh_Log(L"> hasDynamicIconScaling=%d", g_hasDynamicIconScaling);
    if (!g_hasDynamicIconScaling || g_unloading) {
  Wh_Log(L".");
        TaskListButton_UpdateBadge_Original(pThis);
        return;
    }
    double* iconHeight = nullptr;
    double prevIconHeight;
    if (size_t iconHeightOffset = GetIconHeightOffset()) {
  Wh_Log(L".");
        iconHeight = (double*)((BYTE*)pThis + iconHeightOffset);
        prevIconHeight = *iconHeight;
        double newIconHeight = 24;
        Wh_Log(L"Setting iconHeight: %f->%f", prevIconHeight, newIconHeight);
        *iconHeight = newIconHeight;
    }
    TaskListButton_UpdateBadge_Original(pThis);
    if (iconHeight) {
  Wh_Log(L".");
        *iconHeight = prevIconHeight;
    }
}
void* TaskListButton_UpdateIconColumnDefinition_Original;
LONG GetMediumTaskbarButtonExtentOffset() {
  Wh_Log(L".");
    static LONG mediumTaskbarButtonExtentOffset = []() -> LONG {
#if defined(_M_X64)
        const BYTE* start =
            (const BYTE*)TaskListButton_UpdateIconColumnDefinition_Original;
        const BYTE* end = start + 0x200;
        for (const BYTE* p = start; p != end; p++) {
  Wh_Log(L".");
            if (p[0] == 0xF2 && p[1] == 0x0F && p[2] == 0x10 &&
                (p[3] & 0x81) == 0x81) {
  Wh_Log(L".");
                LONG offset = *(LONG*)(p + 4);
                Wh_Log(L"mediumTaskbarButtonExtentOffset=0x%X", offset);
                return (offset < 0 || offset > 0xFFFF) ? 0 : offset;
            }
            if (p[0] == 0xF2 && p[1] == 0x44 && p[2] == 0x0F && p[3] == 0x10 &&
                (p[4] & 0x81) == 0x81) {
  Wh_Log(L".");
                LONG offset = *(LONG*)(p + 5);
                Wh_Log(L"mediumTaskbarButtonExtentOffset=0x%X", offset);
                return (offset < 0 || offset > 0xFFFF) ? 0 : offset;
            }
        }
#elif defined(_M_ARM64)
        const DWORD* start =
            (const DWORD*)TaskListButton_UpdateIconColumnDefinition_Original;
        const DWORD* end = start + 0x80;
        std::regex regex1(R"(fsub\s+d\d+, (d\d+), d\d+)");
        const DWORD* cmdWithReg1 = nullptr;
        std::string reg1;
        for (const DWORD* p = start; p != end; p++) {
  Wh_Log(L".");
            WH_DISASM_RESULT result1;
            if (!Wh_Disasm((void*)p, &result1)) {
  Wh_Log(L".");
                break;
            }
            std::string_view s1 = result1.text;
            if (s1 == "ret") {
  Wh_Log(L".");
                break;
            }
            std::match_results<std::string_view::const_iterator> match1;
            if (std::regex_match(s1.begin(), s1.end(), match1, regex1)) {
  Wh_Log(L".");
                cmdWithReg1 = p;
                reg1 = match1[1];
                break;
            }
        }
        if (cmdWithReg1) {
  Wh_Log(L".");
            std::regex regex2(R"(ldr\s+(d\d+), \[x\d+, #0x([0-9a-f]+)\])");
            for (const DWORD* p = start; p != cmdWithReg1; p++) {
  Wh_Log(L".");
                WH_DISASM_RESULT result1;
                if (!Wh_Disasm((void*)p, &result1)) {
  Wh_Log(L".");
                    break;
                }
                std::string_view s1 = result1.text;
                if (s1 == "ret") {
  Wh_Log(L".");
                    break;
                }
                std::match_results<std::string_view::const_iterator> match1;
                if (!std::regex_match(s1.begin(), s1.end(), match1, regex2) ||
                    match1[1] != reg1) {
  Wh_Log(L".");
                    continue;
                }
                LONG offset = std::stoull(match1[2], nullptr, 16);
                Wh_Log(L"mediumTaskbarButtonExtentOffset=0x%X", offset);
                return (offset < 0 || offset > 0xFFFF) ? 0 : offset;
            }
        }
#else
#error "Unsupported architecture"
#endif
        Wh_Log(L"Error: mediumTaskbarButtonExtentOffset not found");
        return 0;
    }();
    return mediumTaskbarButtonExtentOffset;
}
void TaskListButton_UpdateIconColumnDefinition_InitOffsets() {
  Wh_Log(L".");
    GetMediumTaskbarButtonExtentOffset();
}
using TaskListButton_UpdateVisualStates_t = void(WINAPI*)(void* pThis);
TaskListButton_UpdateVisualStates_t TaskListButton_UpdateVisualStates_Original;
void WINAPI TaskListButton_UpdateVisualStates_Hook(void* pThis) {
  Wh_Log(L".");
    if (TaskListButton_UpdateIconColumnDefinition_Original &&
        (g_applyingSettings || g_taskbarButtonWidthCustomized)) {
  Wh_Log(L".");
        LONG mediumTaskbarButtonExtentOffset =
            GetMediumTaskbarButtonExtentOffset();
        if (mediumTaskbarButtonExtentOffset) {
  Wh_Log(L".");
            bool updateButtonPadding = false;
            double* mediumTaskbarButtonExtent =
                (double*)((BYTE*)pThis + mediumTaskbarButtonExtentOffset);
            if (*mediumTaskbarButtonExtent >= 1 &&
                *mediumTaskbarButtonExtent < 10000) {
  Wh_Log(L".");
                double newValue =
                    g_unloading ? 44 : g_settings_tbiconsize.taskbarButtonWidth;
                if (newValue != *mediumTaskbarButtonExtent) {
  Wh_Log(L".");
                    Wh_Log(
                        L"Updating MediumTaskbarButtonExtent for "
                        L"TaskListButton: %f->%f",
                        *mediumTaskbarButtonExtent, newValue);
                    *mediumTaskbarButtonExtent = newValue;
                    updateButtonPadding = true;
                }
            }
            double* smallTaskbarButtonExtent =
                g_hasDynamicIconScaling ? mediumTaskbarButtonExtent - 1
                                        : nullptr;
            if (smallTaskbarButtonExtent && *smallTaskbarButtonExtent >= 1 &&
                *smallTaskbarButtonExtent < 10000) {
  Wh_Log(L".");
                double newValue =
                    g_unloading ? 32 : g_settings_tbiconsize.taskbarButtonWidthSmall;
                if (newValue != *smallTaskbarButtonExtent) {
  Wh_Log(L".");
                    Wh_Log(
                        L"Updating SmallTaskbarButtonExtent for "
                        L"TaskListButton: %f->%f",
                        *smallTaskbarButtonExtent, newValue);
                    *smallTaskbarButtonExtent = newValue;
                    updateButtonPadding = true;
                }
            }
            if (updateButtonPadding) {
  Wh_Log(L".");
                g_taskbarButtonWidthCustomized = true;
                TaskListButton_UpdateButtonPadding_Hook(pThis);
            }
        } else {
            Wh_Log(L"Error: mediumTaskbarButtonExtentOffset is invalid");
        }
    }
    TaskListButton_UpdateVisualStates_Original(pThis);
    if (g_applyingSettings && !g_hasDynamicIconScaling) {
  Wh_Log(L".");
        FrameworkElement taskListButtonElement = nullptr;
        ((IUnknown*)pThis + 3)
            ->QueryInterface(winrt::guid_of<FrameworkElement>(),
                             winrt::put_abi(taskListButtonElement));
        if (taskListButtonElement) {
  Wh_Log(L".");
            if (auto iconPanelElement =
                    FindChildByName(taskListButtonElement, L"IconPanel")) {
  Wh_Log(L".");
                if (auto iconElement =
                        FindChildByName(iconPanelElement, L"Icon")) {
  Wh_Log(L".");
                    double iconSize = g_unloading ? 24 : g_settings_tbiconsize.iconSize;
                    iconElement.Width(iconSize);
                    iconElement.Height(iconSize);
                }
            }
        }
    }
}
using LaunchListItemViewModel_IconHeight_t = void(WINAPI*)(void* pThis,
                                                           double iconHeight);
LaunchListItemViewModel_IconHeight_t
    LaunchListItemViewModel_IconHeight_Original;
void WINAPI LaunchListItemViewModel_IconHeight_Hook(void* pThis,
                                                    double iconHeight) {
  Wh_Log(L".");
    Wh_Log(L"> iconHeight=%f", iconHeight);
    g_smallIconSize = iconHeight == g_settings_tbiconsize.iconSizeSmall &&
                      iconHeight != g_settings_tbiconsize.iconSize;
    LaunchListItemViewModel_IconHeight_Original(pThis, iconHeight);
}
using ExperienceToggleButton_UpdateButtonPadding_t = void(WINAPI*)(void* pThis);
ExperienceToggleButton_UpdateButtonPadding_t
    ExperienceToggleButton_UpdateButtonPadding_Original;
void WINAPI ExperienceToggleButton_UpdateButtonPadding_Hook(void* pThis) {
  Wh_Log(L".");
    ExperienceToggleButton_UpdateButtonPadding_Original(pThis);
    if (g_hasDynamicIconScaling && g_unloading) {
  Wh_Log(L".");
        return;
    }
    FrameworkElement toggleButtonElement = nullptr;
    ((IUnknown**)pThis)[1]->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(toggleButtonElement));
    if (!toggleButtonElement) {
  Wh_Log(L".");
        return;
    }
    auto panelElement =
        FindChildByName(toggleButtonElement, L"ExperienceToggleButtonRootPanel")
            .try_as<Controls::Grid>();
    if (!panelElement) {
  Wh_Log(L".");
        return;
    }
    double defaultWidthExtra = -4;
    auto className = winrt::get_class_name(toggleButtonElement);
    if (className == L"Taskbar.ExperienceToggleButton") {
  Wh_Log(L".");
        auto automationId = Automation::AutomationProperties::GetAutomationId(
            toggleButtonElement);
        if (automationId == L"StartButton") {
  Wh_Log(L".");
            defaultWidthExtra = -3;
        }
    } else if (className == L"Taskbar.SearchBoxButton") {
  Wh_Log(L".");
        if (panelElement.Margin() != Thickness{}) {
  Wh_Log(L".");
            return;
        }
    } else {
        return;
    }
    double buttonWidth = panelElement.Width();
    if (!(buttonWidth > 0)) {
  Wh_Log(L".");
        return;
    }
    auto buttonPadding = panelElement.Padding();
    double defaultWidth = g_smallIconSize ? 32 : 44;
    double overrideWidth =
        g_unloading ? defaultWidth
                    : (g_smallIconSize ? g_settings_tbiconsize.taskbarButtonWidthSmall
                                       : g_settings_tbiconsize.taskbarButtonWidth);
    double newWidth = overrideWidth + buttonPadding.Left + buttonPadding.Right +
                      defaultWidthExtra;
    if (newWidth != buttonWidth) {
  Wh_Log(L".");
        Wh_Log(L"Updating MediumTaskbarButtonExtent for %s: %f->%f",
               className.c_str(), buttonWidth, newWidth);
        panelElement.Width(newWidth);
    }
}
using SearchButtonBase_UpdateButtonPadding_t = void(WINAPI*)(void* pThis);
SearchButtonBase_UpdateButtonPadding_t
    SearchButtonBase_UpdateButtonPadding_Original;
void WINAPI SearchButtonBase_UpdateButtonPadding_Hook(void* pThis) {
  Wh_Log(L".");
    SearchButtonBase_UpdateButtonPadding_Original(pThis);
    if (g_hasDynamicIconScaling && g_unloading) {
  Wh_Log(L".");
        return;
    }
    FrameworkElement toggleButtonElement = nullptr;
    ((IUnknown**)pThis)[1]->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(toggleButtonElement));
    if (!toggleButtonElement) {
  Wh_Log(L".");
        return;
    }
    auto panelElement =
        FindChildByName(toggleButtonElement, L"SearchBoxButtonRootPanel")
            .try_as<Controls::Grid>();
    if (!panelElement) {
  Wh_Log(L".");
        return;
    }
    if (FindChildByName(panelElement, L"SearchBoxTextBlock")) {
  Wh_Log(L".");
        return;
    }
    double buttonWidth = panelElement.Width();
    if (!(buttonWidth > 0)) {
  Wh_Log(L".");
        return;
    }
    auto buttonPadding = panelElement.Padding();
    double defaultWidth = g_smallIconSize ? 32 : 44;
    double overrideWidth =
        g_unloading ? defaultWidth
                    : (g_smallIconSize ? g_settings_tbiconsize.taskbarButtonWidthSmall
                                       : g_settings_tbiconsize.taskbarButtonWidth);
    double newWidth =
        overrideWidth + buttonPadding.Left + buttonPadding.Right - 4;
    if (newWidth != buttonWidth) {
  Wh_Log(L".");
        Wh_Log(L"Updating MediumTaskbarButtonExtent: %f->%f", buttonWidth,
               newWidth);
        panelElement.Width(newWidth);
    }
}
using AugmentedEntryPointButton_UpdateButtonPadding_t =
    void(WINAPI*)(void* pThis);
AugmentedEntryPointButton_UpdateButtonPadding_t
    AugmentedEntryPointButton_UpdateButtonPadding_Original;
void WINAPI AugmentedEntryPointButton_UpdateButtonPadding_Hook(void* pThis) {
  Wh_Log(L".");
    g_inAugmentedEntryPointButton_UpdateButtonPadding = true;
    AugmentedEntryPointButton_UpdateButtonPadding_Original(pThis);
    g_inAugmentedEntryPointButton_UpdateButtonPadding = false;
}
using RepeatButton_Width_t = void(WINAPI*)(void* pThis, double width);
RepeatButton_Width_t RepeatButton_Width_Original;
void WINAPI RepeatButton_Width_Hook(void* pThis, double width) {
  Wh_Log(L".");
    Wh_Log(L"> width=%f", width);
    RepeatButton_Width_Original(pThis, width);
    if (!g_inAugmentedEntryPointButton_UpdateButtonPadding) {
  Wh_Log(L".");
        return;
    }
    FrameworkElement button = nullptr;
    (*(IUnknown**)pThis)
        ->QueryInterface(winrt::guid_of<FrameworkElement>(),
                         winrt::put_abi(button));
    if (!button) {
  Wh_Log(L".");
        return;
    }
    FrameworkElement augmentedEntryPointContentGrid =
        FindChildByName(button, L"AugmentedEntryPointContentGrid");
    if (!augmentedEntryPointContentGrid) {
  Wh_Log(L".");
        return;
    }
    double marginValue = static_cast<double>(40 - g_settings_tbiconsize.iconSize) / 2;
    if (marginValue < 0) {
  Wh_Log(L".");
        marginValue = 0;
    }
    EnumChildElements(augmentedEntryPointContentGrid, [marginValue](
                                                          FrameworkElement
                                                              child) {
  Wh_Log(L".");
        if (winrt::get_class_name(child) != L"Windows.UI.Xaml.Controls.Grid") {
  Wh_Log(L".");
            return false;
        }
        FrameworkElement panelGrid =
            FindChildByClassName(child, L"Windows.UI.Xaml.Controls.Grid");
        if (!panelGrid) {
  Wh_Log(L".");
            return false;
        }
        FrameworkElement panel = FindChildByClassName(
            panelGrid, L"AdaptiveCards.Rendering.Uwp.WholeItemsPanel");
        if (!panel) {
  Wh_Log(L".");
            return false;
        }
        Wh_Log(L"Processing %f x %f widget", panelGrid.Width(),
               panelGrid.Height());
        double labelsTopBorderExtraMargin = 0;
        bool widePanel = panelGrid.Width() > panelGrid.Height();
        if (widePanel) {
  Wh_Log(L".");
            auto margin = Thickness{3, 3, 3, 3};
            if (!g_unloading && marginValue < 3) {
  Wh_Log(L".");
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
  Wh_Log(L".");
                margin.Left = marginValue;
                margin.Top = marginValue;
                margin.Right = marginValue;
                margin.Bottom = marginValue;
                if (g_taskbarHeight < 48) {
  Wh_Log(L".");
                    margin.Top -= static_cast<double>(48 - g_taskbarHeight) / 2;
                    if (margin.Top < 0) {
  Wh_Log(L".");
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
  Wh_Log(L".");
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
  Wh_Log(L".");
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
  Wh_Log(L".");
            Wh_Log(L"Setting MaxWidth=%f, MaxHeight=%f for large badge",
                   badgeMaxValue, badgeMaxValue);
            badgeLarge.MaxWidth(badgeMaxValue);
            badgeLarge.MaxHeight(badgeMaxValue);
        }
        FrameworkElement labelsBorder = tickerGrid;
        if ((labelsBorder = FindChildByName(labelsBorder, L"LargeTicker2"))) {
  Wh_Log(L".");
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
  Wh_Log(L".");
    auto ret = SHAppBarMessage_Original(dwMessage, pData);
    if (dwMessage == ABM_QUERYPOS && ret && g_taskbarHeight) {
  Wh_Log(L".");
        pData->rc.top =
            pData->rc.bottom -
            MulDiv(g_taskbarHeight, GetDpiForWindow(pData->hWnd), 96);
    }
    return ret;
}
using SendMessageTimeoutW_t = decltype(&SendMessageTimeoutW);
SendMessageTimeoutW_t SendMessageTimeoutW_Original;
LRESULT WINAPI SendMessageTimeoutW_Hook(HWND hWnd,
                                        UINT Msg,
                                        WPARAM wParam,
                                        LPARAM lParam,
                                        UINT fuFlags,
                                        UINT uTimeout,
                                        PDWORD_PTR lpdwResult) {
  Wh_Log(L".");
    if (g_shellIconLoaderV2_LoadAsyncIcon__ResumeCoro_ThreadId ==
            GetCurrentThreadId() &&
        !g_unloading && Msg == WM_GETICON && wParam == ICON_BIG &&
        (g_smallIconSize ? g_settings_tbiconsize.iconSizeSmall : g_settings_tbiconsize.iconSize) <=
            16) {
  Wh_Log(L".");
        wParam = ICON_SMALL2;
    }
    LRESULT ret = SendMessageTimeoutW_Original(hWnd, Msg, wParam, lParam,
                                               fuFlags, uTimeout, lpdwResult);
    return ret;
}
void LoadSettingsTBIconSize() {
  Wh_Log(L".");
  g_settings_tbiconsize.iconSize = Wh_GetIntSetting(L"TaskbarIconSize");
  if (g_settings_tbiconsize.iconSize <= 0) g_settings_tbiconsize.iconSize = 44;
  g_settings_tbiconsize.iconSize=g_settings_tbiconsize.iconSize;
  g_settings_tbiconsize.taskbarHeight = Wh_GetIntSetting(L"TaskbarHeight");
  g_settings_tbiconsize.taskbarHeight = Wh_GetIntSetting(L"TaskbarHeight");
  if (g_settings_tbiconsize.taskbarHeight <= 0) g_settings_tbiconsize.taskbarHeight = 78;
  g_settings_tbiconsize.taskbarHeight = abs(g_settings_tbiconsize.taskbarHeight);
  if (g_settings_tbiconsize.taskbarHeight > 200) g_settings_tbiconsize.taskbarHeight = 200;
  if (g_settings_tbiconsize.taskbarHeight < 44) g_settings_tbiconsize.taskbarHeight = 44;
  int TaskbarOffsetY = abs(Wh_GetIntSetting(L"TaskbarOffsetY"));
  if (TaskbarOffsetY < 0) TaskbarOffsetY = 6;
  int heightExpansion = ((Wh_GetIntSetting(L"FlatTaskbarBottomCorners") || Wh_GetIntSetting(L"FullWidthTaskbarBackground")) ? 0 : (abs(TaskbarOffsetY) * 2));
  g_settings_tbiconsize.taskbarHeight = g_settings_tbiconsize.taskbarHeight + heightExpansion;
  int value = Wh_GetIntSetting(L"TaskbarButtonSize");
  if (value <= 0) value = 74;
  g_settings_tbiconsize.taskbarButtonWidth = value;
}
HWND FindCurrentProcessTaskbarWnd() {
  Wh_Log(L".");
    HWND hTaskbarWnd = nullptr;
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD dwProcessId;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
                dwProcessId == GetCurrentProcessId() &&
                GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
  Wh_Log(L".");
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hTaskbarWnd));
    return hTaskbarWnd;
}
bool ProtectAndMemcpy(DWORD protect, void* dst, const void* src, size_t size) {
  Wh_Log(L".");
    DWORD oldProtect;
    if (!VirtualProtect(dst, size, protect, &oldProtect)) {
  Wh_Log(L".");
        return false;
    }
    memcpy(dst, src, size);
    VirtualProtect(dst, size, oldProtect, &oldProtect);
    return true;
}
void ApplySettingsTBIconSize(int taskbarHeight) {
  Wh_Log(L".");
    if (taskbarHeight < 2) {
  Wh_Log(L".");
        taskbarHeight = 2;
    }
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
  Wh_Log(L".");
        Wh_Log(L"No taskbar found");
        g_taskbarHeight = taskbarHeight;
        return;
    }
    Wh_Log(L"Applying settings for taskbar %08X",
           (DWORD)(DWORD_PTR)hTaskbarWnd);
    if (!g_taskbarHeight) {
  Wh_Log(L".");
        RECT taskbarRect{};
        GetWindowRect(hTaskbarWnd, &taskbarRect);
        g_taskbarHeight = MulDiv(taskbarRect.bottom - taskbarRect.top, 96,
                                 GetDpiForWindow(hTaskbarWnd));
    }
    g_applyingSettings = true;
    if (taskbarHeight == g_taskbarHeight) {
  Wh_Log(L".");
        g_pendingMeasureOverride = true;
        g_taskbarHeight = taskbarHeight - 1;
        if (!TaskbarConfiguration_GetFrameSize_Original &&
            double_48_value_Original) {
  Wh_Log(L".");
            double tempTaskbarHeight = g_taskbarHeight;
            ProtectAndMemcpy(PAGE_READWRITE, double_48_value_Original,
                             &tempTaskbarHeight, sizeof(double));
        }
        SendMessage(hTaskbarWnd, WM_SETTINGCHANGE, SPI_SETLOGICALDPIOVERRIDE,
                    0);
        for (int i = 0; i < 100; i++) {
  Wh_Log(L".");
            if (!g_pendingMeasureOverride) {
  Wh_Log(L".");
                break;
            }
            Sleep(100);
        }
    }
    g_pendingMeasureOverride = true;
    g_taskbarHeight = taskbarHeight;
    if (!TaskbarConfiguration_GetFrameSize_Original &&
        double_48_value_Original) {
  Wh_Log(L".");
        double tempTaskbarHeight = g_taskbarHeight;
        ProtectAndMemcpy(PAGE_READWRITE, double_48_value_Original,
                         &tempTaskbarHeight, sizeof(double));
    }
    SendMessage(hTaskbarWnd, WM_SETTINGCHANGE, SPI_SETLOGICALDPIOVERRIDE, 0);
    for (int i = 0; i < 100; i++) {
  Wh_Log(L".");
        if (!g_pendingMeasureOverride) {
  Wh_Log(L".");
            break;
        }
        Sleep(100);
    }
    HWND hReBarWindow32 =
        FindWindowEx(hTaskbarWnd, nullptr, L"ReBarWindow32", nullptr);
    if (hReBarWindow32) {
  Wh_Log(L".");
        HWND hMSTaskSwWClass =
            FindWindowEx(hReBarWindow32, nullptr, L"MSTaskSwWClass", nullptr);
        if (hMSTaskSwWClass) {
  Wh_Log(L".");
            SendMessage(hMSTaskSwWClass, 0x452, 3, 0);
        }
    }
    g_applyingSettings = false;
}
bool HookTaskbarViewDllSymbols(HMODULE module) {
  Wh_Log(L".");
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
                &ResourceDictionary_Lookup_TaskbarView_Original,
                ResourceDictionary_Lookup_TaskbarView_Hook,
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
                {LR"(public: double __cdecl winrt::Taskbar::implementation::TaskbarConfiguration::GetIconHeightInViewPixels(void))"},
                &TaskbarConfiguration_GetIconHeightInViewPixels_method_Original,
                TaskbarConfiguration_GetIconHeightInViewPixels_method_Hook,
                true,  // From KB5044384 (October 2024).
            },
            {
                {LR"(public: void __cdecl winrt::Taskbar::implementation::TaskListButton::IconHeight(double))"},
                &TaskListButton_IconHeight_Original,
                nullptr,
                true,  // From KB5058499 (May 2025).
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
                &TaskbarController_OnGroupingModeChanged_Original,
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
                {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateButtonPadding(void))"},
                &TaskListButton_UpdateButtonPadding_Original,
                TaskListButton_UpdateButtonPadding_Hook,
            },
            {
                {LR"(public: void __cdecl winrt::Taskbar::implementation::TaskListButton::OverlayIcon(struct winrt::Windows::Storage::Streams::IRandomAccessStream const &))"},
                &TaskListButton_OverlayIcon_Original,
                TaskListButton_OverlayIcon_Hook,
            },
            {
                {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateBadge(void))"},
                &TaskListButton_UpdateBadge_Original,
                TaskListButton_UpdateBadge_Hook,
            },
            {
                {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateIconColumnDefinition(void))"},
                &TaskListButton_UpdateIconColumnDefinition_Original,
                nullptr,
                true,  // Missing in older Windows 11 versions.
            },
            {
                {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateVisualStates(void))"},
                &TaskListButton_UpdateVisualStates_Original,
                TaskListButton_UpdateVisualStates_Hook,
            },
            {
                {LR"(public: virtual void __cdecl winrt::Taskbar::implementation::LaunchListItemViewModel::IconHeight(double))"},
                &LaunchListItemViewModel_IconHeight_Original,
                LaunchListItemViewModel_IconHeight_Hook,
                true,
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
  Wh_Log(L".");
        Wh_Log(L"HookSymbols failed");
        return false;
    }
    if (TaskListButton_IconHeight_Original) {
  Wh_Log(L".");
        TaskListButton_IconHeight_InitOffsets();
    }
#ifdef _M_ARM64
    if (TaskbarConfiguration_UpdateFrameSize_SymbolAddress) {
  Wh_Log(L".");
        TaskbarConfiguration_UpdateFrameSize_InitOffsets();
        WindhawkUtils::Wh_SetFunctionHookT(
            TaskbarConfiguration_UpdateFrameSize_SymbolAddress,
            TaskbarConfiguration_UpdateFrameSize_Hook,
            &TaskbarConfiguration_UpdateFrameSize_Original);
    }
#endif
    if (SystemTrayController_UpdateFrameSize_SymbolAddress) {
  Wh_Log(L".");
        SystemTrayController_UpdateFrameSize_InitOffsets();
        WindhawkUtils::Wh_SetFunctionHookT(
            SystemTrayController_UpdateFrameSize_SymbolAddress,
            SystemTrayController_UpdateFrameSize_Hook,
            &SystemTrayController_UpdateFrameSize_Original);
    }
    if (TaskbarController_OnGroupingModeChanged_Original) {
  Wh_Log(L".");
        TaskbarController_OnGroupingModeChanged_InitOffsets();
    }
    if (TaskListButton_UpdateIconColumnDefinition_Original) {
  Wh_Log(L".");
        TaskListButton_UpdateIconColumnDefinition_InitOffsets();
    }
    constexpr UINT kDynamicIconScaling = 29785184;
    if (TaskbarConfiguration_GetIconHeightInViewPixels_method_Original &&
        IsOsFeatureEnabled(kDynamicIconScaling).value_or(true)) {
  Wh_Log(L".");
        g_hasDynamicIconScaling = true;
        Wh_Log(L"Dynamic icon scaling is enabled");
    }
    return true;
}
bool HookSearchUxUiDllSymbols(HMODULE module) {
  Wh_Log(L".");
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: __cdecl winrt::impl::consume_Windows_Foundation_Collections_IMap<struct winrt::Windows::UI::Xaml::ResourceDictionary,struct winrt::Windows::Foundation::IInspectable,struct winrt::Windows::Foundation::IInspectable>::Lookup(struct winrt::Windows::Foundation::IInspectable const &)const )"},
            &ResourceDictionary_Lookup_SearchUxUi_Original,
            ResourceDictionary_Lookup_SearchUxUi_Hook,
        },
        {
            {LR"(protected: virtual void __cdecl winrt::SearchUx::SearchUI::implementation::SearchButtonBase::UpdateButtonPadding(void))"},
            &SearchButtonBase_UpdateButtonPadding_Original,
            SearchButtonBase_UpdateButtonPadding_Hook,
        },
    };
    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
  Wh_Log(L".");
        Wh_Log(L"HookSymbols failed");
        return false;
    }
    return true;
}
bool HookTaskbarDllSymbolsTBIconSize() {
  Wh_Log(L".");
    HMODULE module =
        LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
  Wh_Log(L".");
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
            {LR"(static  ShellIconLoaderV2::LoadAsyncIcon$_ResumeCoro$1())"},
            &ShellIconLoaderV2_LoadAsyncIcon__ResumeCoro_Original,
            ShellIconLoaderV2_LoadAsyncIcon__ResumeCoro_Hook,
            true,
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
  Wh_Log(L".");
        Wh_Log(L"HookSymbols failed");
        return false;
    }
    return true;
}
HMODULE GetTaskbarViewModuleHandle() {
  Wh_Log(L".");
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) {
  Wh_Log(L".");
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }
    return module;
}
HMODULE GetSearchUxUiModuleHandle() {
  Wh_Log(L".");
    return GetModuleHandle(L"SearchUx.UI.dll");
}
using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
  Wh_Log(L".");
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (!module) {
  Wh_Log(L".");
        return module;
    }
    if (!g_taskbarViewDllLoadedTBIconSize && GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoadedTBIconSize.exchange(true)) {
  Wh_Log(L".");
        Wh_Log(L"Loaded %s", lpLibFileName);
        if (HookTaskbarViewDllSymbols(module)) {
  Wh_Log(L".");
            Wh_ApplyHookOperations();
        }
    }
    if (!g_searchUxUiDllLoaded && GetSearchUxUiModuleHandle() == module &&
        !g_searchUxUiDllLoaded.exchange(true)) {
  Wh_Log(L".");
        Wh_Log(L"Loaded %s", lpLibFileName);
        if (HookSearchUxUiDllSymbols(module)) {
  Wh_Log(L".");
            Wh_ApplyHookOperations();
        }
    }
    return module;
}
BOOL Wh_ModInitTBIconSize() {
  Wh_Log(L".");
    LoadSettingsTBIconSize();
    if (!HookTaskbarDllSymbolsTBIconSize()) {
  Wh_Log(L".");
        return FALSE;
    }
    bool delayLoadingNeeded = false;
    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
  Wh_Log(L".");
        g_taskbarViewDllLoadedTBIconSize = true;
        if (!HookTaskbarViewDllSymbols(taskbarViewModule)) {
  Wh_Log(L".");
            return FALSE;
        }
    } else {
        Wh_Log(L"Taskbar view module not loaded yet");
        delayLoadingNeeded = true;
    }
    if (HMODULE searchUxUiModule = GetSearchUxUiModuleHandle()) {
  Wh_Log(L".");
        g_searchUxUiDllLoaded = true;
        if (!HookSearchUxUiDllSymbols(searchUxUiModule)) {
  Wh_Log(L".");
            return FALSE;
        }
    } else {
        Wh_Log(L"Search UX UI module not loaded yet");
        delayLoadingNeeded = true;
    }
    if (delayLoadingNeeded) {
  Wh_Log(L".");
        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW =
            (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule,
                                                      "LoadLibraryExW");
        WindhawkUtils::Wh_SetFunctionHookT(pKernelBaseLoadLibraryExW,
                                           LoadLibraryExW_Hook,
                                           &LoadLibraryExW_Original);
    }
    WindhawkUtils::Wh_SetFunctionHookT(SHAppBarMessage, SHAppBarMessage_Hook,
                                       &SHAppBarMessage_Original);
    WindhawkUtils::Wh_SetFunctionHookT(SendMessageTimeoutW,
                                       SendMessageTimeoutW_Hook,
                                       &SendMessageTimeoutW_Original);
    return TRUE;
}
void Wh_ModAfterInitTBIconSize() {
  Wh_Log(L".");
    if (!g_taskbarViewDllLoadedTBIconSize) {
  Wh_Log(L".");
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
  Wh_Log(L".");
            if (!g_taskbarViewDllLoadedTBIconSize.exchange(true)) {
  Wh_Log(L".");
                Wh_Log(L"Got Taskbar.View.dll");
                if (HookTaskbarViewDllSymbols(taskbarViewModule)) {
  Wh_Log(L".");
                    Wh_ApplyHookOperations();
                }
            }
        }
    }
    if (!g_searchUxUiDllLoaded) {
  Wh_Log(L".");
        if (HMODULE searchUxUiModule = GetSearchUxUiModuleHandle()) {
  Wh_Log(L".");
            if (!g_searchUxUiDllLoaded.exchange(true)) {
  Wh_Log(L".");
                Wh_Log(L"Got SearchUx.UI.dll");
                if (HookSearchUxUiDllSymbols(searchUxUiModule)) {
  Wh_Log(L".");
                    Wh_ApplyHookOperations();
                }
            }
        }
    }
    ApplySettingsTBIconSize(g_settings_tbiconsize.taskbarHeight);
}
void Wh_ModBeforeUninitTBIconSize() {
  Wh_Log(L".");
    g_unloading = true;
    ApplySettingsTBIconSize(g_originalTaskbarHeight ? g_originalTaskbarHeight : 48);
}
void Wh_ModUninitTBIconSize() {
  Wh_Log(L".");
    while (g_hookCallCounter > 0) {
  Wh_Log(L".");
        Sleep(100);
    }
}
void Wh_ModSettingsChangedTBIconSize() {
  Wh_Log(L".");
    LoadSettingsTBIconSize();
    ApplySettingsTBIconSize(g_settings_tbiconsize.taskbarHeight);
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
bool ApplyStyle(FrameworkElement const& element, std::wstring monitorName);
bool InitializeDebounce();
DispatcherTimer debounceTimer{nullptr};
#include <windhawk_utils.h>
#include <atomic>
#include <functional>
#include <string>
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
;bool MoveFlyoutNotificationCenter=true;} g_settings_startbuttonposition;
std::atomic<bool> g_taskbarViewDllLoadedStartButtonPosition;
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
void* TaskbarHost_FrameHeight_Original;
using CSecondaryTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis,
                                                           void** result);
CSecondaryTaskBand_GetTaskbarHost_t CSecondaryTaskBand_GetTaskbarHost_Original;
using std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;
XamlRoot XamlRootFromTaskbarHostSharedPtr(void* taskbarHostSharedPtr[2]) {
  Wh_Log(L".");
    if (!taskbarHostSharedPtr[0] && !taskbarHostSharedPtr[1]) {
  Wh_Log(L".");
        return nullptr;
    }
    size_t taskbarElementIUnknownOffset = 0x48;
#if defined(_M_X64)
    {
        const BYTE* b = (const BYTE*)TaskbarHost_FrameHeight_Original;
        if (b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC && b[4] == 0x48 &&
            b[5] == 0x83 && b[6] == 0xC1 && b[7] <= 0x7F) {
  Wh_Log(L".");
            taskbarElementIUnknownOffset = b[7];
        } else {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        }
    }
#elif defined(_M_ARM64)
#else
#error "Unsupported architecture"
#endif
    auto* taskbarElementIUnknown =
        *(IUnknown**)((BYTE*)taskbarHostSharedPtr[0] +
                      taskbarElementIUnknownOffset);
    FrameworkElement taskbarElement = nullptr;
    taskbarElementIUnknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(taskbarElement));
    auto result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;
    std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);
    return result;
}
XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
  Wh_Log(L".");
    HWND hTaskSwWnd = (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
    if (!hTaskSwWnd) {
  Wh_Log(L".");
        return nullptr;
    }
    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *(void**)taskBandForTaskListWndSite !=
                    CTaskBand_ITaskListWndSite_vftable;
         i++) {
  Wh_Log(L".");
        if (i == 20) {
  Wh_Log(L".");
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
  Wh_Log(L".");
    HWND hTaskSwWnd =
        (HWND)FindWindowEx(hSecondaryTaskbarWnd, nullptr, L"WorkerW", nullptr);
    if (!hTaskSwWnd) {
  Wh_Log(L".");
        return nullptr;
    }
    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *(void**)taskBandForTaskListWndSite !=
                    CSecondaryTaskBand_ITaskListWndSite_vftable;
         i++) {
  Wh_Log(L".");
        if (i == 20) {
  Wh_Log(L".");
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
  Wh_Log(L".");
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        void* procParam;
    };
    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
  Wh_Log(L".");
        return false;
    }
    if (dwThreadId == GetCurrentThreadId()) {
  Wh_Log(L".");
        proc(procParam);
        return true;
    }
    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
  Wh_Log(L".");
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
  Wh_Log(L".");
                    RUN_FROM_WINDOW_THREAD_PARAM* param =
                        (RUN_FROM_WINDOW_THREAD_PARAM*)cwp->lParam;
                    param->proc(param->procParam);
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
  Wh_Log(L".");
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
  Wh_Log(L".");
    Wh_Log(L"Applying settings");
    EnumThreadWindows(
        GetCurrentThreadId(),
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            WCHAR szClassName[32];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
  Wh_Log(L".");
                return TRUE;
            }
            XamlRoot xamlRoot = nullptr;
            if (_wcsicmp(szClassName, L"Shell_TrayWnd") == 0) {
  Wh_Log(L".");
                xamlRoot = GetTaskbarXamlRoot(hWnd);
            } else if (_wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0) {
  Wh_Log(L".");
                xamlRoot = GetSecondaryTaskbarXamlRoot(hWnd);
            } else {
                return TRUE;
            }
            if (!xamlRoot) {
  Wh_Log(L".");g_already_requested_debounce_initializing=false;
                Wh_Log(L"Getting XamlRoot failed");
                return TRUE;
            }
const auto xamlRootContent = xamlRoot.Content().try_as<FrameworkElement>();
if (!xamlRootContent) {
  Wh_Log(L".");
g_already_requested_debounce_initializing=false;
return TRUE;
}
if (!debounceTimer) {
  Wh_Log(L".");
     xamlRootContent.Dispatcher().TryRunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::High, [xamlRootContent]() {
  Wh_Log(L".");
     InitializeDebounce();
  });
  return TRUE;
}
if (xamlRootContent && xamlRootContent.Dispatcher()) {
  Wh_Log(L".");
std::wstring monitorName = GetMonitorName(hWnd);
  xamlRootContent.Dispatcher().TryRunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::High, [xamlRootContent,monitorName]() {
  Wh_Log(L".");
    if (!ApplyStyle(xamlRootContent,monitorName)) {
  Wh_Log(L".");
      Wh_Log(L"ApplyStyles failed");
    }
  });
  return TRUE;
}
            return TRUE;
        },
        0);
}
void ApplySettingsStartButtonPosition(HWND hTaskbarWnd) {
  Wh_Log(L".");
    RunFromWindowThread(
        hTaskbarWnd, [](void* pParam) {
  Wh_Log(L"."); ApplySettingsFromTaskbarThread(); }, 0);
}
using IUIElement_Arrange_t =
    HRESULT(WINAPI*)(void* pThis, const winrt::Windows::Foundation::Rect* rect);
IUIElement_Arrange_t IUIElement_Arrange_Original;
HRESULT WINAPI
IUIElement_Arrange_Hook(void* pThis,
                        const winrt::Windows::Foundation::Rect* rect) {
  Wh_Log(L".");
    auto original = [=] { return IUIElement_Arrange_Original(pThis, rect); };
    if (g_unloading) {
  Wh_Log(L".");
        return original();
    }
    FrameworkElement element = nullptr;
    (*(IUnknown**)pThis)
        ->QueryInterface(winrt::guid_of<FrameworkElement>(),
                         winrt::put_abi(element));
    if (!element) {
  Wh_Log(L".");
        return original();
    }
    auto className = winrt::get_class_name(element);
    if (className != L"Taskbar.ExperienceToggleButton") {
  Wh_Log(L".");
        return original();
    }
    auto automationId =
        Automation::AutomationProperties::GetAutomationId(element);
    if (automationId != L"StartButton") {
  Wh_Log(L".");
        return original();
    }
    auto taskbarFrameRepeater =
        Media::VisualTreeHelper::GetParent(element).as<FrameworkElement>();
    auto widgetElement =
        EnumChildElements(taskbarFrameRepeater, [](FrameworkElement child) {
  Wh_Log(L".");
            auto childClassName = winrt::get_class_name(child);
            if (childClassName != L"Taskbar.AugmentedEntryPointButton") {
  Wh_Log(L".");
                return false;
            }
            if (child.Name() != L"AugmentedEntryPointButton") {
  Wh_Log(L".");
                return false;
            }
            auto margin = child.Margin();
            auto offset = child.ActualOffset();
            if (offset.x != margin.Left || offset.y != 0) {
  Wh_Log(L".");
                return false;
            }
            return true;
        });
    if (!widgetElement) {
  Wh_Log(L".");
        element.Dispatcher().TryRunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::High,[element]() {
  Wh_Log(L".");
                double width = element.ActualWidth();
                double minX = std::numeric_limits<double>::infinity();
                auto taskbarFrameRepeater =
                    Media::VisualTreeHelper::GetParent(element)
                        .as<FrameworkElement>();
                EnumChildElements(taskbarFrameRepeater,
                                  [&element, &minX](FrameworkElement child) {
  Wh_Log(L".");
                                      if (child == element) {
  Wh_Log(L".");
                                          return false;
                                      }
                                      auto offset = child.ActualOffset();
                                      if (offset.x >= 0 && offset.x < minX) {
  Wh_Log(L".");
                                          minX = offset.x;
                                      }
                                      return false;
                                  });
                if (minX < width) {
  Wh_Log(L".");
                    Thickness margin = element.Margin();
                    element.Margin(margin);
                } else if (minX > width * 2) {
  Wh_Log(L".");
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
  Wh_Log(L".");
    AugmentedEntryPointButton_UpdateButtonPadding_Original(pThis);
    if (g_unloading) {
  Wh_Log(L".");
        return;
    }
    FrameworkElement button = nullptr;
    ((IUnknown**)pThis)[1]->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(button));
    if (!button) {
  Wh_Log(L".");
        return;
    }
    button.Dispatcher().TryRunAsync(
        winrt::Windows::UI::Core::CoreDispatcherPriority::High, [button]() {
  Wh_Log(L".");
            auto offset = button.ActualOffset();
            if (offset.x != 0 || offset.y != 0) {
  Wh_Log(L".");
                return;
            }
            auto margin = button.Margin();
            margin.Left = 34;
            button.Margin(margin);
        });
}
using TrayUI__Hide_t = void(WINAPI*)(void* pThis);
TrayUI__Hide_t TrayUI__Hide_Original;
void WINAPI TrayUI__Hide_Hook(void* pThis) {
  Wh_Log(L".");
  TrayUI__Hide_Original(pThis);
  ApplySettingsFromTaskbarThreadIfRequired();
}
using CSecondaryTray__AutoHide_t = void(WINAPI*)(void* pThis, bool param1);
CSecondaryTray__AutoHide_t CSecondaryTray__AutoHide_Original;
void WINAPI CSecondaryTray__AutoHide_Hook(void* pThis, bool param1) {
  Wh_Log(L".");
  CSecondaryTray__AutoHide_Original(pThis, param1);
  ApplySettingsFromTaskbarThreadIfRequired();
}
using TrayUI_WndProc_t = LRESULT(WINAPI*)(void* pThis, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam, bool* flag);
TrayUI_WndProc_t TrayUI_WndProc_Original;
LRESULT WINAPI TrayUI_WndProc_Hook(void* pThis, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam, bool* flag) {
  Wh_Log(L".");
  ApplySettingsFromTaskbarThreadIfRequired();
  return TrayUI_WndProc_Original(pThis, hWnd, Msg, wParam, lParam, flag);
}
using CSecondaryTray_v_WndProc_t = LRESULT(WINAPI*)(void* pThis, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
CSecondaryTray_v_WndProc_t CSecondaryTray_v_WndProc_Original;
LRESULT WINAPI CSecondaryTray_v_WndProc_Hook(void* pThis, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
  Wh_Log(L".");
  ApplySettingsFromTaskbarThreadIfRequired();
  return CSecondaryTray_v_WndProc_Original(pThis, hWnd, Msg, wParam, lParam);
}
using CTaskBand__ProcessWindowDestroyed_t = void(WINAPI*)(void* pThis, void* pHwnd);
CTaskBand__ProcessWindowDestroyed_t CTaskBand__ProcessWindowDestroyed_Original;
void WINAPI CTaskBand__ProcessWindowDestroyed_Hook(void* pThis, void* pHwnd) {
  Wh_Log(L".");
  Wh_Log(L"CTaskBand::CTaskBand__ProcessWindowDestroyed_Hook Hook");
  CTaskBand__ProcessWindowDestroyed_Original(pThis, pHwnd);
  ApplySettingsFromTaskbarThreadIfRequired();
}
using CTaskBand__InsertItem_t = long(WINAPI*)(void* pThis, void* pHwnd, void** ppTaskItem, void* pHwnd1, void* pHwnd2);
CTaskBand__InsertItem_t CTaskBand__InsertItem_Original;
long WINAPI CTaskBand__InsertItem_Hook(void* pThis, void* pHwnd, void** ppTaskItem, void* pHwnd1, void* pHwnd2) {
  Wh_Log(L".");
  Wh_Log(L"CTaskBand::_InsertItem Hook");
  auto original_call = CTaskBand__InsertItem_Original(pThis, pHwnd, ppTaskItem, pHwnd1, pHwnd2);
  ApplySettingsFromTaskbarThreadIfRequired();
  return original_call;
}
using CTaskBand__UpdateAllIcons_t = void(WINAPI*)(void* pThis);
CTaskBand__UpdateAllIcons_t CTaskBand__UpdateAllIcons_Original;
void WINAPI CTaskBand__UpdateAllIcons_Hook(void* pThis) {
  Wh_Log(L".");
  Wh_Log(L"CTaskBand::_UpdateAllIcons Hook");
  CTaskBand__UpdateAllIcons_Original(pThis);
  ApplySettingsFromTaskbarThreadIfRequired();
}
using CTaskBand__TaskOrderChanged_t = void(WINAPI*)(void* pThis, void* pTaskGroup, int param);
CTaskBand__TaskOrderChanged_t CTaskBand__TaskOrderChanged_Original;
void WINAPI CTaskBand__TaskOrderChanged_Hook(void* pThis, void* pTaskGroup, int param) {
  Wh_Log(L".");
  Wh_Log(L"CTaskBand::TaskOrderChanged Hook");
  CTaskBand__TaskOrderChanged_Original(pThis, pTaskGroup, param);
  ApplySettingsFromTaskbarThreadIfRequired();
}
using CImpWndProc__WndProc_t = __int64(WINAPI*)(void* pThis, void* pHwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam);
CImpWndProc__WndProc_t CImpWndProc__WndProc_Original;
__int64 WINAPI CImpWndProc__WndProc_Hook(void* pThis, void* pHwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam) {
  Wh_Log(L".");
  ApplySettingsFromTaskbarThreadIfRequired();
  return CImpWndProc__WndProc_Original(pThis, pHwnd, msg, wParam, lParam);
}
using CTaskBand__WndProc_t = __int64(WINAPI*)(void* pThis, void* pHwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam);
CTaskBand__WndProc_t CTaskBand__WndProc_Original;
__int64 WINAPI CTaskBand__WndProc_Hook(void* pThis, void* pHwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam) {
  Wh_Log(L".");
  ApplySettingsFromTaskbarThreadIfRequired();
  return CTaskBand__WndProc_Original(pThis, pHwnd, msg, wParam, lParam);
}
using CTaskListWnd__WndProc_t = __int64(WINAPI*)(void* pThis, void* pHwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam);
CTaskListWnd__WndProc_t CTaskListWnd__WndProc_Original;
__int64 WINAPI CTaskListWnd__WndProc_Hook(void* pThis, void* pHwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam) {
  Wh_Log(L".");
  ApplySettingsFromTaskbarThreadIfRequired();
  return CTaskListWnd__WndProc_Original(pThis, pHwnd, msg, wParam, lParam);
}
using CSecondaryTaskBand__WndProc_t = __int64(WINAPI*)(void* pThis, void* pHwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam);
CSecondaryTaskBand__WndProc_t CSecondaryTaskBand__WndProc_Original;
__int64 WINAPI CSecondaryTaskBand__WndProc_Hook(void* pThis, void* pHwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam) {
  Wh_Log(L".");
  ApplySettingsFromTaskbarThreadIfRequired();
  return CSecondaryTaskBand__WndProc_Original(pThis, pHwnd, msg, wParam, lParam);
}
using CTraySearchControl__WndProc_t = __int64(WINAPI*)(void* pThis, void* pHwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam);
CTraySearchControl__WndProc_t CTraySearchControl__WndProc_Original;
__int64 WINAPI CTraySearchControl__WndProc_Hook(void* pThis, void* pHwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam) {
  Wh_Log(L".");
  ApplySettingsFromTaskbarThreadIfRequired();
  return CTraySearchControl__WndProc_Original(pThis, pHwnd, msg, wParam, lParam);
}
interface ITaskGroup;
interface ITaskItem;
using CTaskBand__UpdateItemIcon_WithArgs_t = void(WINAPI*)(void* pThis, ITaskGroup* param1, ITaskItem* param2);
CTaskBand__UpdateItemIcon_WithArgs_t CTaskBand__UpdateItemIcon_WithArgs_Original;
void WINAPI CTaskBand__UpdateItemIcon_WithArgs_Hook(void* pThis, ITaskGroup* param1, ITaskItem* param2) {
  Wh_Log(L".");
  Wh_Log(L"Method called: CTaskBand__UpdateItemIcon");
  CTaskBand__UpdateItemIcon_WithArgs_Original(pThis, param1, param2);
  ApplySettingsFromTaskbarThreadIfRequired();
}
using CTaskBand_RemoveIcon_WithArgs_t = void(WINAPI*)(void* pThis, ITaskItem* param1);
CTaskBand_RemoveIcon_WithArgs_t CTaskBand_RemoveIcon_WithArgs_Original;
void WINAPI CTaskBand_RemoveIcon_WithArgs_Hook(void* pThis, ITaskItem* param1) {
  Wh_Log(L".");
  Wh_Log(L"Method called: CTaskBand_RemoveIcon");
  CTaskBand_RemoveIcon_WithArgs_Original(pThis, param1);
  ApplySettingsFromTaskbarThreadIfRequired();
}
using ITaskbarSettings_get_Alignment_t = HRESULT(WINAPI*)(void* pThis, int* alignment);
ITaskbarSettings_get_Alignment_t ITaskbarSettings_get_Alignment_Original;
HRESULT WINAPI ITaskbarSettings_get_Alignment_Hook(void* pThis, int* alignment) {
  Wh_Log(L".");
  HRESULT ret = ITaskbarSettings_get_Alignment_Original(pThis, alignment);
  Wh_Log(L"Method called: ITaskbarSettings_get_Alignment_Hook alignment: %d", *alignment);
  if (SUCCEEDED(ret)) {
  Wh_Log(L".");
    *alignment = 1;
  }
  return ret;
}
#include <windowsx.h>
using CTaskListWnd_ComputeJumpViewPosition_t = HRESULT(WINAPI*)(void* pThis, void* taskBtnGroup, int param2, winrt::Windows::Foundation::Point* point, HorizontalAlignment* horizontalAlignment, VerticalAlignment* verticalAlignment);
CTaskListWnd_ComputeJumpViewPosition_t CTaskListWnd_ComputeJumpViewPosition_Original;
HRESULT WINAPI CTaskListWnd_ComputeJumpViewPosition_Hook(void* pThis, void* taskBtnGroup, int param2, winrt::Windows::Foundation::Point* point, HorizontalAlignment* horizontalAlignment, VerticalAlignment* verticalAlignment) {
  Wh_Log(L".");
  HRESULT ret = CTaskListWnd_ComputeJumpViewPosition_Original(pThis, taskBtnGroup, param2, point, horizontalAlignment, verticalAlignment);
  DWORD messagePos = GetMessagePos();
  POINT pt{
      GET_X_LPARAM(messagePos),
      GET_Y_LPARAM(messagePos),
  };
  point->X = pt.x;
  return ret;
}
using TrayUI__OnDPIChanged_WithoutArgs_t = void(WINAPI*)(void* pThis);
TrayUI__OnDPIChanged_WithoutArgs_t TrayUI__OnDPIChanged_WithoutArgs_Original;
void WINAPI TrayUI__OnDPIChanged_WithoutArgs_Hook(void* pThis) {
  Wh_Log(L".");
               TrayUI__OnDPIChanged_WithoutArgs_Original(pThis);
               g_invalidateDimensions = true;
            }
bool HookTaskbarDllSymbolsStartButtonPosition() {
  Wh_Log(L".");
    HMODULE module =
        LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
  Wh_Log(L".");
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }
    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {{{LR"(public: virtual void __cdecl CTaskBand::RemoveIcon(struct ITaskItem *))"}, &CTaskBand_RemoveIcon_WithArgs_Original, CTaskBand_RemoveIcon_WithArgs_Hook},
    {{LR"(protected: void __cdecl CTaskBand::_UpdateItemIcon(struct ITaskGroup *,struct ITaskItem *))"}, &CTaskBand__UpdateItemIcon_WithArgs_Original, CTaskBand__UpdateItemIcon_WithArgs_Hook},
    {
        {LR"(protected: static __int64 __cdecl CImpWndProc::s_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
        &CImpWndProc__WndProc_Original,
        CImpWndProc__WndProc_Hook,
    },
    {
        {LR"(protected: static __int64 __cdecl CTraySearchControl::s_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
        &CTraySearchControl__WndProc_Original,
        CTraySearchControl__WndProc_Hook,
    },
    {
        {LR"(private: virtual __int64 __cdecl CSecondaryTaskBand::v_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
        &CSecondaryTaskBand__WndProc_Original,
        CSecondaryTaskBand__WndProc_Hook,
    },
    {
        {LR"(protected: virtual __int64 __cdecl CTaskBand::v_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
        &CTaskBand__WndProc_Original,
        CTaskBand__WndProc_Hook,
    },
    {
        {LR"(protected: virtual __int64 __cdecl CTaskListWnd::v_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
        &CTaskListWnd__WndProc_Original,
        CTaskListWnd__WndProc_Hook,
    },
    /////////////////////////////////////
    {
        {LR"(protected: long __cdecl CTaskBand::_InsertItem(struct HWND__ *,struct ITaskItem * *,struct HWND__ *,struct HWND__ *))"},
        &CTaskBand__InsertItem_Original,
        CTaskBand__InsertItem_Hook,
    },
    {
        {LR"(protected: void __cdecl CTaskBand::_UpdateAllIcons(void))"},
        &CTaskBand__UpdateAllIcons_Original,
        CTaskBand__UpdateAllIcons_Hook,
    },
    {
        {LR"(public: virtual void __cdecl CTaskBand::TaskOrderChanged(struct ITaskGroup *,int))"},
        &CTaskBand__TaskOrderChanged_Original,
        CTaskBand__TaskOrderChanged_Hook,
    },
    {
        {LR"(protected: void __cdecl CTaskBand::_ProcessWindowDestroyed(struct HWND__ *))"},
        &CTaskBand__ProcessWindowDestroyed_Original,
        CTaskBand__ProcessWindowDestroyed_Hook,
    },
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
        {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::WindowsUdk::UI::Shell::implementation::TaskbarSettings,struct winrt::WindowsUdk::UI::Shell::ITaskbarSettings>::get_Alignment(int *))"},
        &ITaskbarSettings_get_Alignment_Original,
        ITaskbarSettings_get_Alignment_Hook,
    },
    {
        {LR"(protected: long __cdecl CTaskListWnd::_ComputeJumpViewPosition(struct ITaskBtnGroup *,int,struct Windows::Foundation::Point &,enum Windows::UI::Xaml::HorizontalAlignment &,enum Windows::UI::Xaml::VerticalAlignment &)const )"},
        &CTaskListWnd_ComputeJumpViewPosition_Original,
        CTaskListWnd_ComputeJumpViewPosition_Hook,
    },
    {{LR"(public: void __cdecl TrayUI::_OnDPIChanged(void))"}, &TrayUI__OnDPIChanged_WithoutArgs_Original, TrayUI__OnDPIChanged_WithoutArgs_Hook},
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
            {LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
            &TaskbarHost_FrameHeight_Original,
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
using TaskbarTelemetry_StartItemEntranceAnimation_t = void(WINAPI*)(const bool&);
static TaskbarTelemetry_StartItemEntranceAnimation_t orig_StartItemEntranceAnimation = nullptr;
using TaskbarTelemetry_StartItemPlateEntranceAnimation_t = void(WINAPI*)(const bool&);
static TaskbarTelemetry_StartItemPlateEntranceAnimation_t orig_StartItemPlateEntranceAnimation = nullptr;
void WINAPI Hook_StartItemEntranceAnimation_call(const bool& b) {
  Wh_Log(L".");
  Wh_Log(L"[Hook] TaskbarTelemetry::StartItemEntranceAnimation(%d)", b);
  orig_StartItemEntranceAnimation(b);
  ApplySettingsDebounced(50);
}
void WINAPI Hook_StartItemPlateEntranceAnimation_call(const bool& b) {
  Wh_Log(L".");
  Wh_Log(L"[Hook] TaskbarTelemetry::StartItemPlateEntranceAnimation(%d)", b);
  orig_StartItemPlateEntranceAnimation(b);
  ApplySettingsDebounced(50);
}
using TaskbarTelemetry_StartEntranceAnimationCompleted_WithoutArgs_t = void(WINAPI*)(void* pThis);
TaskbarTelemetry_StartEntranceAnimationCompleted_WithoutArgs_t TaskbarTelemetry_StartEntranceAnimationCompleted_WithoutArgs_Original;
static void WINAPI TaskbarTelemetry_StartEntranceAnimationCompleted_WithoutArgs_Hook(void* pThis) {
  Wh_Log(L".");
  Wh_Log(L"Method called: TaskbarTelemetry_StartEntranceAnimationCompleted");
  TaskbarTelemetry_StartEntranceAnimationCompleted_WithoutArgs_Original(pThis);
  ApplySettingsDebounced(300);
  return;
}
using TaskbarTelemetry_StartHideAnimationCompleted_WithoutArgs_t = void(WINAPI*)(void* pThis);
TaskbarTelemetry_StartHideAnimationCompleted_WithoutArgs_t TaskbarTelemetry_StartHideAnimationCompleted_WithoutArgs_Original;
static void WINAPI TaskbarTelemetry_StartHideAnimationCompleted_WithoutArgs_Hook(void* pThis) {
  Wh_Log(L".");
  TaskbarTelemetry_StartHideAnimationCompleted_WithoutArgs_Original(pThis);
  Wh_Log(L"Method called: TaskbarTelemetry_StartHideAnimationCompleted");
  ApplySettingsDebounced(300);
  return;
}
bool HookTaskbarViewDllSymbolsStartButtonPosition(HMODULE module) {
  Wh_Log(L".");
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {{{LR"(public: static void __cdecl TaskbarTelemetry::StartItemEntranceAnimation<bool const &>(bool const &))"}, &orig_StartItemEntranceAnimation, Hook_StartItemEntranceAnimation_call},
    {{LR"(public: static void __cdecl TaskbarTelemetry::StartItemPlateEntranceAnimation<bool const &>(bool const &))"}, &orig_StartItemPlateEntranceAnimation, Hook_StartItemPlateEntranceAnimation_call},
    {{LR"(public: static void __cdecl TaskbarTelemetry::StartHideAnimationCompleted(void))"}, &TaskbarTelemetry_StartHideAnimationCompleted_WithoutArgs_Original, TaskbarTelemetry_StartHideAnimationCompleted_WithoutArgs_Hook},
    {{LR"(public: static void __cdecl TaskbarTelemetry::StartEntranceAnimationCompleted(void))"}, &TaskbarTelemetry_StartEntranceAnimationCompleted_WithoutArgs_Original, TaskbarTelemetry_StartEntranceAnimationCompleted_WithoutArgs_Hook},
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
  Wh_Log(L".");
    if (!g_taskbarViewDllLoadedStartButtonPosition && GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoadedStartButtonPosition.exchange(true)) {
  Wh_Log(L".");
        Wh_Log(L"Loaded %s", lpLibFileName);
        if (HookTaskbarViewDllSymbolsStartButtonPosition(module)) {
  Wh_Log(L".");
            Wh_ApplyHookOperations();
        }
    }
}
using LoadLibraryExW_t = decltype(&LoadLibraryExW);
HMODULE WINAPI LoadLibraryExW_Hook_StartButtonPosition(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
  Wh_Log(L".");
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module) {
  Wh_Log(L".");
        HandleLoadedModuleIfTaskbarView(module, lpLibFileName);
    }
    return module;
}
std::wstring GetProcessFileName(DWORD dwProcessId) {
  Wh_Log(L".");
    HANDLE hProcess =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcessId);
    if (!hProcess) {
  Wh_Log(L".");
        return std::wstring{};
    }
    WCHAR processPath[MAX_PATH];
    DWORD dwSize = ARRAYSIZE(processPath);
    if (!QueryFullProcessImageName(hProcess, 0, processPath, &dwSize)) {
  Wh_Log(L".");
        CloseHandle(hProcess);
        return std::wstring{};
    }
    CloseHandle(hProcess);
    PCWSTR processFileNameUpper = wcsrchr(processPath, L'\\');
    if (!processFileNameUpper) {
  Wh_Log(L".");
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
  Wh_Log(L".");
    auto original = [=]() {
  Wh_Log(L".");
        return DwmSetWindowAttribute_Original(hwnd, dwAttribute, pvAttribute,
                                              cbAttribute);
    };
    if (dwAttribute != DWMWA_CLOAK || cbAttribute != sizeof(BOOL)) {
  Wh_Log(L".");
        return original();
    }
    BOOL cloak = *(BOOL*)pvAttribute;
    if (cloak) {
  Wh_Log(L".");
        return original();
    }
    Wh_Log(L"> %08X", (DWORD)(DWORD_PTR)hwnd);
    DWORD processId = 0;
    if (!hwnd || !GetWindowThreadProcessId(hwnd, &processId)) {
  Wh_Log(L".");
        return original();
    }
    TCHAR className[256];GetClassName(hwnd, className, 256);std::wstring windowClassName(className);
std::wstring processFileName = GetProcessFileName(processId);
Wh_Log(L"process: %s, windowClassName: %s",processFileName.c_str(),windowClassName.c_str());
    enum class Target {
        StartMenu,
        SearchHost,ShellExperienceHost,
    };
    Target target;
    if (_wcsicmp(processFileName.c_str(), L"StartMenuExperienceHost.exe") ==
        0) {
  Wh_Log(L".");
        target = Target::StartMenu;
    } else if (_wcsicmp(processFileName.c_str(), L"SearchHost.exe") == 0) {
  Wh_Log(L".");
        target = Target::SearchHost;
    }else if (_wcsicmp(processFileName.c_str(), L"ShellExperienceHost.exe") == 0) {
  Wh_Log(L".");
        target = Target::ShellExperienceHost;
    }  else {
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
    auto monitorName = GetMonitorName(monitor);
    auto iterationTbStates = g_taskbarStates.find(monitorName);
    if (iterationTbStates == g_taskbarStates.end()) {
  Wh_Log(L".");
      return original();
    }
    TaskbarState& taskbarState = iterationTbStates->second;
    RECT targetRect;
    if (!GetWindowRect(hwnd, &targetRect)) {
  Wh_Log(L".");
        return original();
    }
    int x = targetRect.left;
    int y = targetRect.top;
    int cx = targetRect.right - targetRect.left;
    int cy = targetRect.bottom - targetRect.top;
    float dpiScale = monitorDpiX / 96.0f;
    float absStartX = taskbarState.lastStartButtonXCalculated * dpiScale;
    float absRootWidth = taskbarState.lastRootWidth * dpiScale;
    float absTargetWidth = taskbarState.lastTargetWidth * dpiScale;
     Wh_Log(L"original: taskbarState.lastLeftMostEdgeTray: %f, lastStartButtonXCalculated: %f g_lastRootWidth %f cx: %d, x:%d;cy: %d; y: %d; target:%d g_lastTargetWidth: %f, absStartX: %f; absRootWidth: %f; absTargetWidth: %f",
           taskbarState.lastLeftMostEdgeTray,
          taskbarState.lastStartButtonXCalculated,
          taskbarState.lastRootWidth,
          cx,
          x,
          cy,
          y,
          target,
          taskbarState.lastTargetWidth,
          absStartX,
          absRootWidth,
          absTargetWidth);
    if (target == Target::StartMenu) {
  Wh_Log(L".");
    g_lastRecordedStartMenuWidth = static_cast<int>(Wh_GetIntValue(L"lastRecordedStartMenuWidth", g_lastRecordedStartMenuWidth) * dpiScale);
      if (g_settings_startbuttonposition.startMenuOnTheLeft && !g_unloading) {
  Wh_Log(L".");
        g_startMenuWnd = hwnd;
        g_startMenuOriginalWidth = cx;
        x = static_cast<int>(absRootWidth / 2.0f - absStartX - absTargetWidth+ (g_settings.userDefinedAlignFlyoutInner?g_lastRecordedStartMenuWidth/2.0f : 0.0f));
        x = std::min(0, std::max(static_cast<int>(((-absRootWidth + g_lastRecordedStartMenuWidth) / 2.0f) + (12 * dpiScale)), x));
      } else {
        if (g_startMenuOriginalWidth) {
  Wh_Log(L".");
          cx = g_startMenuOriginalWidth;
        }
        g_startMenuWnd = nullptr;
        g_startMenuOriginalWidth = 0;
        x = 0;
      }
    } else if (target == Target::SearchHost) {
  Wh_Log(L".");
      if (g_settings_startbuttonposition.startMenuOnTheLeft && !g_unloading) {
  Wh_Log(L".");
        g_searchMenuWnd = hwnd;
        g_searchMenuOriginalX = x;
        x = static_cast<int>(absStartX - (g_settings.userDefinedAlignFlyoutInner? ( 12 * dpiScale) :( cx / 2.0f)));
        x = std::max(0, std::min(x, static_cast<int>(absRootWidth - cx)));
      } else {
       x = g_unloading && IsStartMenuOrbLeftAligned() ? g_searchMenuOriginalX : (absRootWidth-cx)/2;
       g_searchMenuWnd = nullptr;
       g_searchMenuOriginalX = 0;
      }
    } else if (target == Target::ShellExperienceHost) {
  Wh_Log(L".");
        int lastRecordedTrayRightMostEdgeForMonitor = taskbarState.lastRightMostEdgeTray;
        if (y != 0) {
  Wh_Log(L".");
          return original();
        }
        if (g_settings_startbuttonposition.MoveFlyoutNotificationCenter && !g_unloading) {
  Wh_Log(L".");
          x = static_cast<int>(lastRecordedTrayRightMostEdgeForMonitor * dpiScale - (g_settings.userDefinedAlignFlyoutInner ? (cx - (12 * dpiScale)) : (cx / 2.0f)));
          x = std::max(0, std::min(x, static_cast<int>(absRootWidth - cx)));
        } else {
          x = static_cast<int>(absRootWidth - cx);
        }
    }
     Wh_Log(L"Recalc: taskbarState.lastLeftMostEdgeTray: %f, lastStartButtonXCalculated: %f g_lastRootWidth %f cx: %d, x:%d;cy: %d; y: %d; target:%d g_lastTargetWidth: %f, absStartX: %f; absRootWidth: %f; absTargetWidth: %f",
               taskbarState.lastLeftMostEdgeTray,
              taskbarState.lastStartButtonXCalculated,
              taskbarState.lastRootWidth,
              cx,
              x,
              cy,
              y,
              target,
              taskbarState.lastTargetWidth,
              absStartX,
              absRootWidth,
              absTargetWidth);
SetWindowPos(hwnd, nullptr, x, y, cx, cy, SWP_NOZORDER | SWP_NOACTIVATE);
    return original();
}
void RestoreMenuPositions() {
  Wh_Log(L".");
    if (g_startMenuWnd && g_startMenuOriginalWidth) {
  Wh_Log(L".");
        RECT rect;
        if (GetWindowRect(g_startMenuWnd, &rect)) {
  Wh_Log(L".");
            int x = rect.left;
            int y = rect.top;
            int cx = rect.right - rect.left;
            int cy = rect.bottom - rect.top;
            if (g_startMenuOriginalWidth != cx) {
  Wh_Log(L".");
                cx = g_startMenuOriginalWidth;
                SetWindowPos(g_startMenuWnd, nullptr, x, y, cx, cy,
                             SWP_NOZORDER | SWP_NOACTIVATE);
            }
        }
        g_startMenuWnd = nullptr;
        g_startMenuOriginalWidth = 0;
    }
    if (g_searchMenuWnd && g_searchMenuOriginalX) {
  Wh_Log(L".");
        RECT rect;
        if (GetWindowRect(g_searchMenuWnd, &rect)) {
  Wh_Log(L".");
            int x = rect.left;
            int y = rect.top;
            int cx = rect.right - rect.left;
            int cy = rect.bottom - rect.top;
            if (g_searchMenuOriginalX != x) {
  Wh_Log(L".");
                x = g_searchMenuOriginalX;
                SetWindowPos(g_searchMenuWnd, nullptr, x, y, cx, cy,
                             SWP_NOZORDER | SWP_NOACTIVATE);
            }
        }
        g_searchMenuWnd = nullptr;
        g_searchMenuOriginalX = 0;
    }
}
void LoadSettingsStartButtonPosition() {
  Wh_Log(L".");
    g_settings_startbuttonposition.startMenuOnTheLeft = Wh_GetIntSetting(L"MoveFlyoutStartMenu");
g_settings_startbuttonposition.MoveFlyoutNotificationCenter = Wh_GetIntSetting(L"MoveFlyoutNotificationCenter");
    g_settings_startbuttonposition.startMenuWidth = 660;
}
BOOL Wh_ModInitStartButtonPosition() {
  Wh_Log(L".");
    LoadSettingsStartButtonPosition();
    if (!HookTaskbarDllSymbolsStartButtonPosition()) {
  Wh_Log(L".");
        return FALSE;
    }
    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
  Wh_Log(L".");
        g_taskbarViewDllLoadedStartButtonPosition = true;
        if (!HookTaskbarViewDllSymbolsStartButtonPosition(taskbarViewModule)) {
  Wh_Log(L".");
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
    HMODULE dwmapiModule =
        LoadLibraryEx(L"dwmapi.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (dwmapiModule) {
  Wh_Log(L".");
        auto pDwmSetWindowAttribute =
            (decltype(&DwmSetWindowAttribute))GetProcAddress(
                dwmapiModule, "DwmSetWindowAttribute");
        if (pDwmSetWindowAttribute) {
  Wh_Log(L".");
            WindhawkUtils::Wh_SetFunctionHookT(pDwmSetWindowAttribute,
                                               DwmSetWindowAttribute_Hook,
                                               &DwmSetWindowAttribute_Original);
        }
    }
    return TRUE;
}
void Wh_ModAfterInitStartButtonPosition() {
  Wh_Log(L".");
    if (!g_taskbarViewDllLoadedStartButtonPosition) {
  Wh_Log(L".");
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
  Wh_Log(L".");
            if (!g_taskbarViewDllLoadedStartButtonPosition.exchange(true)) {
  Wh_Log(L".");
                Wh_Log(L"Got Taskbar.View.dll");
                if (HookTaskbarViewDllSymbolsStartButtonPosition(taskbarViewModule)) {
  Wh_Log(L".");
                    Wh_ApplyHookOperations();
                }
            }
        }
    }
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd) {
  Wh_Log(L".");
        ApplySettingsStartButtonPosition(hTaskbarWnd);
    }
}
void Wh_ModBeforeUninitStartButtonPosition() {
  Wh_Log(L".");
    g_unloading = true;
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd) {
  Wh_Log(L".");
        ApplySettingsStartButtonPosition(hTaskbarWnd);
    }
}
void Wh_ModUninitStartButtonPosition() {
  Wh_Log(L".");if(true)return;
    RestoreMenuPositions();
}
void Wh_ModSettingsChangedStartButtonPosition() {
  Wh_Log(L".");
    RestoreMenuPositions();
    LoadSettingsStartButtonPosition();
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
////  ____    __    ____  __  .__   __.  _______    ______     ______  __  ___   ////
////  \   \  /  \  /   / |  | |  \ |  | |       \  /  __  \   /      ||  |/  /   ////
////   \   \/    \/   /  |  | |   \|  | |  .--.  ||  |  |  | |  ,----'|  '  /    ////
////    \            /   |  | |  . `  | |  |  |  ||  |  |  | |  |     |    <     ////
////     \    /\    /    |  | |  |\   | |  '--'  ||  `--'  | |  `----.|  .  \    ////
////      \__/  \__/     |__| |__| \__| |_______/  \______/   \______||__|\__\   ////
////                                                                             ////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
#include <dwmapi.h>
#include <chrono>
#include <string>
#include <regex>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <limits>
#include <utility>
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <functional>
#undef GetCurrentTime
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.Animation.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>
#include <commctrl.h>
#include <roapi.h>
#include <winstring.h>
#include <string_view>
#include <vector>
#include <atomic>
#include <winrt/Windows.Graphics.Imaging.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.Storage.Search.h>
#include <chrono>
#include <thread>
#include <windows.h>
#include <psapi.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
using namespace winrt::Windows::UI::Xaml;
std::wstring EscapeXmlAttribute(std::wstring_view data) {
  Wh_Log(L".");
  std::wstring buffer;
  buffer.reserve(data.size());
  for (wchar_t c : data) buffer.append((c == L'&') ? L"&amp;" : (c == L'\"') ? L"&quot;" : (c == L'<') ? L"&lt;" : (c == L'>') ? L"&gt;" : std::wstring(1, c));
  return buffer;
}
Style GetStyleFromXamlSetters(const std::wstring_view type, const std::wstring_view xamlStyleSetters, std::wstring& outXaml) {
  Wh_Log(L".");
  std::wstring xaml =
      LR"(<ResourceDictionary
    xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
    xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
    xmlns:d="http://schemas.microsoft.com/expression/blend/2008"
    xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
    xmlns:muxc="using:Microsoft.UI.Xaml.Controls")";
  if (auto pos = type.rfind('.'); pos != type.npos) {
  Wh_Log(L".");
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
  Wh_Log(L".");
  if(!obj) return;
  std::wstring outXamlResult;
  try {
    std::wstring xamlSetter = L"<Setter Property=\"";
    xamlSetter += EscapeXmlAttribute(propertyName);
    xamlSetter += L"\"";
    if (isXamlValue) {
  Wh_Log(L".");
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
  Wh_Log(L".");
      auto setter = style.Setters().GetAt(i).as<Setter>();
      obj.SetValue(setter.Property(), setter.Value());
    }
  } catch (const std::exception& ex) {
  Wh_Log(L".");
    if (!outXamlResult.empty()) {
  Wh_Log(L".");
      Wh_Log(L"Error: %S. Xaml Result: %s", ex.what(), outXamlResult.c_str());
    } else {
      Wh_Log(L"Error: %S", ex.what());
    }
  } catch (const winrt::hresult_error& ex) {
  Wh_Log(L".");
    if (!outXamlResult.empty()) {
  Wh_Log(L".");
      Wh_Log(L"Error %08X: %s. Xaml Result: %s", ex.code(), ex.message().c_str(), outXamlResult.c_str());
    } else {
      Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
    }
  } catch (...) {
  Wh_Log(L".");
    if (!outXamlResult.empty()) {
  Wh_Log(L".");
      Wh_Log(L"Unknown error occurred while setting property. Xaml Result: %s", outXamlResult.c_str());
    } else {
      Wh_Log(L"Unknown error occurred while setting property.");
    }
  }
}
void SetElementPropertyFromString(FrameworkElement obj, const std::wstring& type, const std::wstring& propertyName, const std::wstring& propertyValue) {
  Wh_Log(L"."); return SetElementPropertyFromString(obj, type, propertyName, propertyValue, false); }
#include <regex>
std::vector<std::wstring> SplitAndTrim(const std::optional<std::wstring>& input) {
  Wh_Log(L".");
  std::vector<std::wstring> result;
  if (!input.has_value() || input->empty()) {
  Wh_Log(L".");
    return result;
  }
  std::wstringstream ss(*input);
  std::wstring item;
  while (std::getline(ss, item, L';')) {
  Wh_Log(L".");
    size_t start = item.find_first_not_of(L" \t");
    size_t end = item.find_last_not_of(L" \t");
    if (start != std::wstring::npos && end != std::wstring::npos) {
  Wh_Log(L".");
      std::wstring trimmed = item.substr(start, end - start + 1);
      if (!trimmed.empty()) {
  Wh_Log(L".");
        result.push_back(trimmed);
      }
    }
  }
  return result;
}
bool RegexMatchInsensitive(const std::wstring& haystack, const std::wstring& pattern) {
  Wh_Log(L".");
  try {
    std::wregex regexPattern(pattern, std::regex_constants::icase);
    return std::regex_search(haystack, regexPattern);
  } catch (const std::regex_error&) {
  Wh_Log(L".");
    return false;
  }
}
std::atomic<bool> g_scheduled_low_priority_update = false;
int debounceDelayMs = 300;
winrt::event_token debounceToken{};
void ApplySettings(HWND hTaskbarWnd);
bool InitializeDebounce() {
  Wh_Log(L".");
  if (debounceTimer) return true;
  g_already_requested_debounce_initializing = true;
  debounceTimer = DispatcherTimer();
  debounceTimer.Interval(winrt::Windows::Foundation::TimeSpan(std::chrono::milliseconds(debounceDelayMs)));
  debounceToken = debounceTimer.Tick([](winrt::Windows::Foundation::IInspectable const&, winrt::Windows::Foundation::IInspectable const&) {
  Wh_Log(L".");
    g_scheduled_low_priority_update = false;
    debounceTimer.Stop();
    if (auto debounceHwnd = FindCurrentProcessTaskbarWnd()) {
  Wh_Log(L".");
      Wh_Log(L"Debounce triggered");
      ApplySettings(debounceHwnd);
    }
  });
  return false;
}
void CleanupDebounce() {
  Wh_Log(L".");
  g_already_requested_debounce_initializing = false;
  if (debounceTimer) {
  Wh_Log(L".");
    if (auto debounceHwnd = FindCurrentProcessTaskbarWnd()) {
  Wh_Log(L".");
      RunFromWindowThread(
          debounceHwnd,
          [](void* pParam) {
  Wh_Log(L".");
            g_scheduled_low_priority_update = false;
            debounceTimer.Stop();
            debounceTimer.Tick(debounceToken);
            debounceTimer = nullptr;
          },
          0);
    }
  }
}
void ApplySettingsDebounced(int delayMs) {
  Wh_Log(L".");
  HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
  if (!hTaskbarWnd) {
  Wh_Log(L".");
    Wh_Log(L"ApplySettingsDebounced aborted: could not find hTaskbarWnd");
    return;
  }
  if (!debounceTimer) {
  Wh_Log(L".");
    if (!g_already_requested_debounce_initializing) {
  Wh_Log(L".");
      g_already_requested_debounce_initializing = true;
      ApplySettings(hTaskbarWnd);
      Wh_Log(L"ApplySettingsDebounced aborted: debounceTimer is null; initializing");
    }
    return;
  }
  bool lowPriority = false;
  if (delayMs <= 0) {
  Wh_Log(L".");
    lowPriority = true;
    delayMs = 700;
  }
  debounceDelayMs = delayMs;
  if (!lowPriority) RunFromWindowThread(hTaskbarWnd, [](void* pParam) {
  Wh_Log(L"."); debounceTimer.Stop(); }, 0);
  RunFromWindowThread(
      hTaskbarWnd,
      [](void* pParam) {
  Wh_Log(L".");
        debounceTimer.Interval(winrt::Windows::Foundation::TimeSpan{std::chrono::milliseconds(debounceDelayMs)});
        debounceTimer.Start();
      },
      0);
}
void ApplySettingsDebounced() {
  Wh_Log(L"."); ApplySettingsDebounced(100); }
bool IsWeirdFrameworkElement(winrt::Windows::UI::Xaml::FrameworkElement const& element) {
  Wh_Log(L".");
  if (!element) return false;
  auto transform = element.TransformToVisual(nullptr);
  winrt::Windows::Foundation::Rect rect = transform.TransformBounds(winrt::Windows::Foundation::Rect(0, 0, element.ActualWidth(), element.ActualHeight()));
  return rect.X < 0 || rect.Y < 0;
}
bool IsTaskbarWidgetsEnabled() {
  Wh_Log(L".");
    DWORD value = 0;
    DWORD size = sizeof(value);
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
                      L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced",
                      0, KEY_READ, &hKey) == ERROR_SUCCESS) {
  Wh_Log(L".");
        if (RegQueryValueExW(hKey, L"TaskbarDa", nullptr, nullptr,
                             reinterpret_cast<LPBYTE>(&value), &size) == ERROR_SUCCESS) {
  Wh_Log(L".");
            RegCloseKey(hKey);
            return value == 1;
        }
        RegCloseKey(hKey);
    }
    return false;
}
using StartDocked__StartSizingFrame_UpdateWindowRegion_WithArgs_t = void(WINAPI*)(void* pThis, winrt::Windows::Foundation::Size param1);
StartDocked__StartSizingFrame_UpdateWindowRegion_WithArgs_t StartDocked__StartSizingFrame_UpdateWindowRegion_WithArgs_Original;
void WINAPI StartDocked__StartSizingFrame_UpdateWindowRegion_WithArgs_Hook(void* pThis, winrt::Windows::Foundation::Size param1) {
  Wh_Log(L".");
  StartDocked__StartSizingFrame_UpdateWindowRegion_WithArgs_Original(pThis, param1);
  Wh_Log(L"Method called: StartDocked__StartSizingFrame_UpdateWindowRegion (Width: %.2f, Height: %.2f)", param1.Width, param1.Height);
  if (g_lastRecordedStartMenuWidth != param1.Width) {
  Wh_Log(L".");
    g_lastRecordedStartMenuWidth = static_cast<int>(param1.Width);
    Wh_SetIntValue(L"lastRecordedStartMenuWidth", g_lastRecordedStartMenuWidth);
  }
}
std::atomic<int64_t> g_update_flag_set_time_ms = 0;
int64_t NowMs() {
  Wh_Log(L"."); return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count(); }
void ResetFlagAfterDelay() {
  Wh_Log(L".");
  std::this_thread::sleep_for(std::chrono::milliseconds(1400));
  int64_t now = NowMs();
  int64_t set_time = g_update_flag_set_time_ms.load();
  if (g_scheduled_low_priority_update && (now - set_time >= 1400)) {
  Wh_Log(L".");
    g_scheduled_low_priority_update = false;
  }
}
void ApplySettingsFromTaskbarThreadIfRequired() {
  Wh_Log(L".");
  if (!g_scheduled_low_priority_update) {
  Wh_Log(L".");
    g_scheduled_low_priority_update = true;
    g_update_flag_set_time_ms = NowMs();
    std::thread(ResetFlagAfterDelay).detach();
    Wh_Log(L"Scheduled low priority update");
    ApplySettingsDebounced(-1);
  }
}
void SetDividerForElement(FrameworkElement const& element, float const& panelHeight, bool dividerVisible, bool dividerShouldBeOnLeft = g_settings.userDefinedDividerLeftAligned) {
  Wh_Log(L".");
  if (!element) return;
  if (panelHeight <= 0.0f) return;
  auto visual = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(element);
  if (!visual) return;
  auto compositor = visual.Compositor();
  if (!compositor) return;
  auto shapeVisual = compositor.CreateShapeVisual();
  if (!shapeVisual) return;
  dividerVisible = dividerVisible && !g_unloading;
  if (dividerVisible) {
  Wh_Log(L".");
    auto lineGeometry = compositor.CreateLineGeometry();
    if (!lineGeometry) return;
    auto lineShape = compositor.CreateSpriteShape(lineGeometry);
    if (!lineShape) return;
    float borderThicknessFloat = static_cast<float>(g_settings.userDefinedAppsDividerThickness) * 2.0f;
    float scaledHeight = panelHeight * g_settings.userDefinedAppsDividerVerticalScale;
    float yOffset = (panelHeight - scaledHeight) * 0.5f;
    auto size = visual.Size();
    float xOffset = (dividerShouldBeOnLeft) ? 0.0f : (size.x - borderThicknessFloat / 2.0f);
    shapeVisual.Size({borderThicknessFloat, scaledHeight});
    shapeVisual.Offset({xOffset, yOffset, 0.0f});
    lineGeometry.Start({0.0f, 0.0f});
    lineGeometry.End({0.0f, scaledHeight});
    winrt::Windows::UI::Color borderColor = {g_settings.userDefinedTaskbarBorderOpacity, static_cast<BYTE>(g_settings.borderColorR), static_cast<BYTE>(g_settings.borderColorG), static_cast<BYTE>(g_settings.borderColorB)};
    auto strokeBrush = compositor.CreateColorBrush(borderColor);
    if (!strokeBrush) return;
    lineShape.StrokeBrush(strokeBrush);
    lineShape.StrokeThickness(borderThicknessFloat);
    shapeVisual.Shapes().Append(lineShape);
  }
  winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::SetElementChildVisual(element, shapeVisual);
}
void ChangeControlCenterIconSize(FrameworkElement const& systemTrayFrameGrid) {
  Wh_Log(L".");
  if (!g_settings.userDefinedStyleTrayArea) return;
  if (auto ControlCenterButton = FindChildByName(systemTrayFrameGrid, L"ControlCenterButton")) {
  Wh_Log(L".");
    if (auto innerGrid = FindChildByClassName(ControlCenterButton, L"Windows.UI.Xaml.Controls.Grid")) {
  Wh_Log(L".");
      if (auto ContentPresenter = FindChildByName(innerGrid, L"ContentPresenter")) {
  Wh_Log(L".");
        if (auto innerItemPresenter = FindChildByClassName(ContentPresenter, L"Windows.UI.Xaml.Controls.ItemsPresenter")) {
  Wh_Log(L".");
          if (auto innerStackPanel = FindChildByClassName(innerItemPresenter, L"Windows.UI.Xaml.Controls.StackPanel")) {
  Wh_Log(L".");
            auto userDefinedTrayIconSizeStr = std::to_wstring(g_settings.userDefinedTrayIconSize);
            int childCount = Media::VisualTreeHelper::GetChildrenCount(innerStackPanel);
            for (int i = 0; i < childCount; ++i) {
  Wh_Log(L".");
              auto child = Media::VisualTreeHelper::GetChild(innerStackPanel, i).try_as<FrameworkElement>();
              if (!child) continue;
              auto SystemTrayIcon = FindChildByName(child, L"SystemTrayIcon");
              if (!SystemTrayIcon) continue;
              auto ContainerGrid = FindChildByName(SystemTrayIcon, L"ContainerGrid");
              if (!ContainerGrid) continue;
              auto ContentGrid = FindChildByName(ContainerGrid, L"ContentGrid");
              if (!ContentGrid) continue;
              auto TextIconContent = FindChildByClassName(ContentGrid, L"SystemTray.TextIconContent");
              if (!TextIconContent) continue;
              auto ContainerGridInner = FindChildByName(TextIconContent, L"ContainerGrid");
              if (!ContainerGridInner) continue;
              if (auto Layer = FindChildByName(ContainerGridInner, L"Underlay")) {
  Wh_Log(L".");
                if (auto InnerTextBlock = FindChildByName(Layer, L"InnerTextBlock")) {
  Wh_Log(L".");
                  SetElementPropertyFromString(InnerTextBlock, L"Windows.UI.Xaml.Controls.TextBlock", L"FontSize", userDefinedTrayIconSizeStr);
                }
              }
              if (auto Layer = FindChildByName(ContainerGridInner, L"Base")) {
  Wh_Log(L".");
                if (auto InnerTextBlock = FindChildByName(Layer, L"InnerTextBlock")) {
  Wh_Log(L".");
                  SetElementPropertyFromString(InnerTextBlock, L"Windows.UI.Xaml.Controls.TextBlock", L"FontSize", userDefinedTrayIconSizeStr);
                }
              }
              if (auto Layer = FindChildByName(ContainerGridInner, L"AccentOverlay")) {
  Wh_Log(L".");
                if (auto InnerTextBlock = FindChildByName(Layer, L"InnerTextBlock")) {
  Wh_Log(L".");
                  SetElementPropertyFromString(InnerTextBlock, L"Windows.UI.Xaml.Controls.TextBlock", L"FontSize", userDefinedTrayIconSizeStr);
                }
              }
            }
          }
        }
      }
    }
  }
}
void ProcessStackPanelChildren(FrameworkElement const& stackPanel, float const& panelHeight) {
  Wh_Log(L".");
  if (!g_settings.userDefinedStyleTrayArea) return;
  auto userDefinedTaskButtonCornerRadius = std::to_wstring(g_settings.userDefinedTaskButtonCornerRadius);
  int childCount = Media::VisualTreeHelper::GetChildrenCount(stackPanel);
  for (int i = 0; i < childCount; ++i) {
  Wh_Log(L".");
    auto child = Media::VisualTreeHelper::GetChild(stackPanel, i).try_as<FrameworkElement>();
    if (!child) continue;
    auto notifyItemIcon = FindChildByName(child, L"NotifyItemIcon");
    if (!notifyItemIcon) continue;
    auto containerGrid = FindChildByName(notifyItemIcon, L"ContainerGrid");
    if (!containerGrid) continue;
    auto innerContentPresenter = FindChildByName(containerGrid, L"ContentPresenter");
    if (!innerContentPresenter) continue;
    auto contentGrid = FindChildByName(innerContentPresenter, L"ContentGrid");
    if (!contentGrid) continue;
    auto imageIconContent = FindChildByClassName(contentGrid, L"SystemTray.ImageIconContent");
    if (!imageIconContent) continue;
    auto innerContainerGrid = FindChildByName(imageIconContent, L"ContainerGrid");
    if (!innerContainerGrid) continue;
    auto image = FindChildByClassName(innerContainerGrid, L"Windows.UI.Xaml.Controls.Image");
    if (!image) continue;
    auto imageCtrl = image.try_as<winrt::Windows::UI::Xaml::Controls::Image>();
    if (!imageCtrl) continue;
    if (g_settings.userDefinedStyleTrayArea) {
  Wh_Log(L".");
      child.Width(g_settings.userDefinedTrayButtonSize);
      child.Height(g_settings.userDefinedTaskbarHeight);
      SetElementPropertyFromString(containerGrid, L"Windows.UI.Xaml.Controls.Grid", L"CornerRadius", userDefinedTaskButtonCornerRadius);
      image.Width(g_settings.userDefinedTrayIconSize);
      image.Height(g_settings.userDefinedTrayIconSize);
    }
  }
}
void StyleNativeDividerElement(winrt::Windows::UI::Xaml::FrameworkElement const& element) {
  Wh_Log(L".");
  if (!element) return;
  using namespace winrt::Windows::UI::Xaml::Hosting;
  using namespace winrt::Windows::Foundation::Numerics;
  element.Opacity(g_unloading ? 1.0f : std::min(1.0f, static_cast<float>(g_settings.userDefinedTaskbarBorderOpacity / 255.0f)));
  element.Width(std::max(0.0, g_settings.userDefinedAppsDividerThickness * 0.99));
  if (auto visual = ElementCompositionPreview::GetElementVisual(element)) {
  Wh_Log(L".");
    if (auto compositor = visual.Compositor()) {
  Wh_Log(L".");
      visual.CenterPoint({0.0f, static_cast<float>(element.ActualHeight()) / 2.0f, 0.0f});
      visual.Scale({1.0f, g_unloading ? 1.0f : g_settings.userDefinedAppsDividerVerticalScale, 1.0f});
    }
  }
  PCWSTR hex = Wh_GetStringSetting(L"TaskbarBorderColorHex");
  PCWSTR originalHex = hex;
  if (!hex || *hex == L'\0') {
  Wh_Log(L".");
    hex = L"#ffffff";
    originalHex = nullptr;
  }
  if (*hex == L'#') ++hex;
  std::wstring fillBrush = L"<SolidColorBrush Color=\"#" + std::wstring(hex) + L"\"/>";
  SetElementPropertyFromString(element, L"Windows.UI.Xaml.Shapes.Rectangle", L"Fill", fillBrush.c_str(), true);
  if (originalHex) {
  Wh_Log(L".");
    Wh_FreeStringSetting(originalHex);
  }
}
double CalculateValidChildrenWidth(FrameworkElement element, int& childrenCount, TaskbarState& state) {
  Wh_Log(L".");
  if (!element) return 0.0;
  const float tbHeightFloat = static_cast<float>(g_settings.userDefinedTaskbarHeight);
  auto userDefinedTaskButtonCornerRadius = std::to_wstring(g_settings.userDefinedTaskButtonCornerRadius);
  auto userDefinedTaskbarIconSize = std::to_wstring(g_settings.userDefinedTaskbarIconSize);
  double totalWidth = 0.0;
  childrenCount = 0;
  int childrenCountTentative = Media::VisualTreeHelper::GetChildrenCount(element);
  for (int i = 0; i < childrenCountTentative; i++) {
  Wh_Log(L".");
    auto child = Media::VisualTreeHelper::GetChild(element, i).try_as<FrameworkElement>();
    if (!child) {
  Wh_Log(L".");
      Wh_Log(L"Failed to get child %d of %d", i + 1, childrenCountTentative);
      continue;
    }
    auto transform = child.TransformToVisual(element);
    auto rect = transform.TransformBounds(winrt::Windows::Foundation::Rect(0, 0, child.ActualWidth(), child.ActualHeight()));
    // exclude "weird" rectangles (aka recycled views)
    if (rect.X < 0 || rect.Y < 0) {
  Wh_Log(L".");
      continue;
    }
    auto className = winrt::get_class_name(child);
    SetElementPropertyFromString(child, className.c_str(), L"CornerRadius", userDefinedTaskButtonCornerRadius);
    if (className == L"Taskbar.TaskListButton" || className == L"Taskbar.ExperienceToggleButton" || className == L"Taskbar.OverflowToggleButton") {
  Wh_Log(L".");
      child.MinWidth(g_settings.userDefinedTaskbarButtonSize);
      //   child.Width(g_settings.userDefinedTaskbarButtonSize);
      auto innerElementChild = FindChildByClassName(child, L"Taskbar.TaskListButtonPanel");
      if (innerElementChild) {
  Wh_Log(L".");
        innerElementChild.MinWidth(g_settings.userDefinedTaskbarButtonSize);
        // innerElementChild.Width(g_settings.userDefinedTaskbarButtonSize);
      }
    }
    if (className == L"Taskbar.SearchBoxButton") {
  Wh_Log(L".");
      child.MinWidth(g_settings.userDefinedTaskbarButtonSize);
      auto innerElementChild = FindChildByClassName(child, L"Taskbar.TaskListButtonPanel");
      if (innerElementChild) {
  Wh_Log(L".");
        innerElementChild.MinWidth(g_settings.userDefinedTaskbarButtonSize);
      }
    } else if (className == L"Taskbar.ExperienceToggleButton") {
  Wh_Log(L".");
      state.lastStartButtonXActual = rect.X - rect.Width;
    } else if (className == L"Taskbar.TaskListButton") {
  Wh_Log(L".");
      auto innerElementChild = FindChildByClassName(child, L"Taskbar.TaskListLabeledButtonPanel");
      if (innerElementChild) {
  Wh_Log(L".");
        auto iconElementChild = FindChildByName(innerElementChild, L"Icon");
        if (iconElementChild) {
  Wh_Log(L".");
          iconElementChild.Width(g_settings.userDefinedTaskbarIconSize);
          iconElementChild.Height(g_settings.userDefinedTaskbarIconSize);
          auto currentIconAppName = child.GetValue(winrt::Windows::UI::Xaml::Automation::AutomationProperties::NameProperty());
          const std::wstring currentIconAppNameStr = winrt::unbox_value<winrt::hstring>(currentIconAppName).c_str();
          // Wh_Log(L"bbwi: %s", currentIconAppNameStr);
          SetDividerForElement(child, tbHeightFloat, false);
          if (!currentIconAppNameStr.empty()) {
  Wh_Log(L".");
            for (const auto& pat : g_settings.userDefinedDividedAppNames) {
  Wh_Log(L".");
              if (RegexMatchInsensitive(currentIconAppNameStr, pat)) {
  Wh_Log(L".");
                SetDividerForElement(child, tbHeightFloat, true);
                break;
              }
            }
          }
        }
      }
    } else if (className == L"Taskbar.AugmentedEntryPointButton") {
  Wh_Log(L".");  // widget element
      child.Margin(Thickness{0, 0, 0, 0});
      auto ExperienceToggleButtonRootPanelElement = FindChildByName(child, L"ExperienceToggleButtonRootPanel");
      if (ExperienceToggleButtonRootPanelElement) {
  Wh_Log(L".");
        ExperienceToggleButtonRootPanelElement.Margin(Thickness{0, 0, 0, 0});
      }
      continue;
    } else if (className == L"Taskbar.OverflowToggleButton") {
  Wh_Log(L".");  // overflow button
      if (auto OverflowToggleButtonRootPanel = FindChildByName(child, L"OverflowToggleButtonRootPanel")) {
  Wh_Log(L".");
        if (auto RightOverflowButtonDivider = FindChildByName(OverflowToggleButtonRootPanel, L"RightOverflowButtonDivider")) {
  Wh_Log(L".");
          if (g_settings.userDefinedTrayAreaDivider) {
  Wh_Log(L".");
            RightOverflowButtonDivider.Opacity(0);
          } else {
            StyleNativeDividerElement(RightOverflowButtonDivider);
          }
        }
      }
    }
    if (auto iconPanelElement = FindChildByName(child, L"IconPanel")) {
  Wh_Log(L".");
      if (auto mostRecentlyUsedDivider = FindChildByName(iconPanelElement, L"MostRecentlyUsedDivider")) {
  Wh_Log(L".");
        StyleNativeDividerElement(mostRecentlyUsedDivider);
      }
      if (auto progressIndicator = FindChildByName(iconPanelElement, L"ProgressIndicator")) {
  Wh_Log(L".");
        if (auto layoutRoot = FindChildByName(progressIndicator, L"LayoutRoot")) {
  Wh_Log(L".");
          if (auto progressBarRoot = FindChildByName(layoutRoot, L"ProgressBarRoot")) {
  Wh_Log(L".");
            if (auto border = FindChildByClassName(progressBarRoot, L"Windows.UI.Xaml.Controls.Border")) {
  Wh_Log(L".");
              if (auto grid = FindChildByClassName(border, L"Windows.UI.Xaml.Controls.Grid")) {
  Wh_Log(L".");
                grid.Height(3.8);
                if (auto progressBarTrack = FindChildByName(grid, L"ProgressBarTrack")) {
  Wh_Log(L".");
                  progressBarTrack.Opacity(0.5);
                }
              }
            }
          }
        }
      } else if (auto runningIndicator = FindChildByName(iconPanelElement, L"RunningIndicator")) {
  Wh_Log(L".");
        runningIndicator.Height(3.5);
        runningIndicator.Opacity(1);
      }
    }
    totalWidth += rect.Width;
    childrenCount++;
  }
  return totalWidth;
}
void UpdateGlobalSettings() {
  Wh_Log(L".");
  auto getInt = [&](PCWSTR key) {
  Wh_Log(L"."); return Wh_GetIntSetting(key); };
  auto clamp = [](int v, int lo, int hi) {
  Wh_Log(L"."); return v < lo ? lo : v > hi ? hi : v; };
  // Booleans
  g_settings.userDefinedFlatTaskbarBottomCorners = (getInt(L"FlatTaskbarBottomCorners") != 0);
  g_settings.userDefinedFullWidthTaskbarBackground = (getInt(L"FullWidthTaskbarBackground") != 0) || g_unloading;
  if (g_settings.userDefinedFullWidthTaskbarBackground) g_settings.userDefinedFlatTaskbarBottomCorners = true;
  g_settings.userDefinedIgnoreShowDesktopButton = (getInt(L"IgnoreShowDesktopButton") != 0);
  g_settings.userDefinedTrayAreaDivider = (getInt(L"TrayAreaDivider") != 0) && !g_unloading;
  g_settings.userDefinedStyleTrayArea = (getInt(L"StyleTrayArea") != 0);
  g_settings.userDefinedAlignFlyoutInner = (getInt(L"AlignFlyoutInner") != 0);
  g_settings.userDefinedCustomizeTaskbarBackground = (getInt(L"CustomizeTaskbarBackground") != 0);
  PCWSTR appsDividerAlignment = Wh_GetStringSetting(L"AppsDividerAlignment");
  g_settings.userDefinedDividerLeftAligned = (_wcsicmp(appsDividerAlignment, L"left") == 0);
  Wh_FreeStringSetting(appsDividerAlignment);
  // Gaps & Padding (non-negative)
  g_settings.userDefinedTrayTaskGap = g_unloading ? 0 : std::max(0, getInt(L"TrayTaskGap"));
  g_settings.userDefinedTaskbarBackgroundHorizontalPadding = g_unloading ? 0 : std::max(0, getInt(L"TaskbarBackgroundHorizontalPadding"));
  // Offset Y (negative up, zero if flat or unloading)
  int offsetY = abs(getInt(L"TaskbarOffsetY"));
  g_settings.userDefinedTaskbarOffsetY = (g_unloading || g_settings.userDefinedFlatTaskbarBottomCorners) ? 0 : -offsetY;
  // Height & Sizes
  int h = clamp(abs(getInt(L"TaskbarHeight")), 44, 200);
  g_settings.userDefinedTaskbarHeight = g_unloading ? 44 : h;
  g_settings.userDefinedTaskbarIconSize = g_unloading ? 24 : std::max(24, getInt(L"TaskbarIconSize"));
  g_settings.userDefinedTrayIconSize = std::max(15, getInt(L"TrayIconSize"));
  g_settings.userDefinedTaskbarButtonSize = g_unloading ? 44 : std::max(44, getInt(L"TaskbarButtonSize"));
  g_settings.userDefinedTrayButtonSize = std::max(20, getInt(L"TrayButtonSize"));
  // Corner radii
  float tcr = float(fmax(0.0f, getInt(L"TaskbarCornerRadius")));
  tcr = fmin(tcr, g_settings.userDefinedTaskbarHeight / 2.0f);
  g_settings.userDefinedTaskbarCornerRadius = g_unloading ? 0.0f : tcr;
  int btnCr = clamp(abs(getInt(L"TaskButtonCornerRadius")), 0, g_settings.userDefinedTaskbarHeight / 2);
  g_settings.userDefinedTaskButtonCornerRadius = g_unloading ? 0 : btnCr;
  // Opacities & tints (0–100)
  int bgOp = clamp(abs(getInt(L"TaskbarBackgroundOpacity")), 0, 100);
  g_settings.userDefinedTaskbarBackgroundOpacity = bgOp;
  g_settings.userDefinedTaskbarBackgroundTint = clamp(abs(getInt(L"TaskbarBackgroundTint")), 0, 100);
  g_settings.userDefinedTaskbarBackgroundLuminosity = clamp(abs(getInt(L"TaskbarBackgroundLuminosity")), 0, 100);
  // Border opacity: 0–255
  int bOp = clamp(abs(getInt(L"TaskbarBorderOpacity")), 0, 100);
  g_settings.userDefinedTaskbarBorderOpacity = uint8_t(round(bOp * 2.55f));
  // Border thickness: 0.0–10.0 (10% of [0–100])
  g_settings.userDefinedTaskbarBorderThickness = g_unloading ? 0.0f : (clamp(abs(getInt(L"TaskbarBorderThickness")), 0, 100) * 0.1f);
  g_settings.userDefinedAppsDividerThickness = g_unloading ? 0.0f : (clamp(abs(getInt(L"AppsDividerThickness")), 0, 100) * 0.1f);
  g_settings.userDefinedAppsDividerVerticalScale = g_unloading ? 0.0f : (clamp(abs(getInt(L"AppsDividerVerticalScale")), 0, 100) / 100.0f);
  // Border color
  PCWSTR hex = Wh_GetStringSetting(L"TaskbarBorderColorHex");
  PCWSTR originalHex = hex;
  if (!hex || *hex == L'\0') {
  Wh_Log(L".");
    hex = L"#ffffff";
    originalHex = nullptr;
  }
  if (*hex == L'#') ++hex;
  unsigned int r = 255, g = 255, b = 255;
  if (swscanf_s(hex, L"%02x%02x%02x", &r, &g, &b) != 3) {
  Wh_Log(L".");
    r = g = b = 255;
  }
  g_settings.borderColorR = r;
  g_settings.borderColorG = g;
  g_settings.borderColorB = b;
  if (originalHex) {
  Wh_Log(L".");
    Wh_FreeStringSetting(originalHex);
  }
  // String list
  PCWSTR dividerAppNames = Wh_GetStringSetting(L"DividedAppNames");
  g_settings.userDefinedDividedAppNames = SplitAndTrim(dividerAppNames);
  Wh_FreeStringSetting(dividerAppNames);
}
bool HasInvalidSettings() {
  Wh_Log(L".");
  if (g_settings.userDefinedTrayTaskGap < 0) return true;
  if (g_settings.userDefinedTaskbarBackgroundHorizontalPadding < 0) return true;
  if ((int)g_settings.userDefinedTaskbarOffsetY < 0 && !g_settings.userDefinedFlatTaskbarBottomCorners) return true;
  if (g_settings.userDefinedTaskbarHeight < 44 || g_settings.userDefinedTaskbarHeight > 200) return true;
  if (g_settings.userDefinedTaskbarIconSize <= 0) return true;
  if (g_settings.userDefinedTrayIconSize <= 0) return true;
  if (g_settings.userDefinedTaskbarButtonSize <= 0) return true;
  if (g_settings.userDefinedTrayButtonSize <= 0) return true;
  if (g_settings.userDefinedTaskbarCornerRadius < 0.0f || g_settings.userDefinedTaskbarCornerRadius > (g_settings.userDefinedTaskbarHeight / 2.0f)) return true;
  if (g_settings.userDefinedTaskButtonCornerRadius < 0 || g_settings.userDefinedTaskButtonCornerRadius > (g_settings.userDefinedTaskbarHeight / 2)) return true;
  if (g_settings.userDefinedTaskbarBackgroundOpacity > 100) return true;
  if (g_settings.userDefinedTaskbarBackgroundTint > 100) return true;
  if (g_settings.userDefinedTaskbarBackgroundLuminosity > 100) return true;
  if (g_settings.userDefinedTaskbarBorderOpacity > 255) return true;
  if (g_settings.userDefinedTaskbarBorderThickness < 0.0 || g_settings.userDefinedTaskbarBorderThickness > 10.0) return true;
  return false;
}
void LogAllSettings() {
  Wh_Log(L".");
  Wh_Log(L"setting %d %s", g_settings.userDefinedTrayTaskGap, L"userDefinedTrayTaskGap");
  Wh_Log(L"setting %d %s", g_settings.userDefinedTaskbarBackgroundHorizontalPadding, L"userDefinedTaskbarBackgroundHorizontalPadding");
  Wh_Log(L"setting %d %s", g_settings.userDefinedTaskbarOffsetY, L"userDefinedTaskbarOffsetY");
  Wh_Log(L"setting %d %s", g_settings.userDefinedTaskbarHeight, L"userDefinedTaskbarHeight");
  Wh_Log(L"setting %d %s", g_settings.userDefinedTaskbarIconSize, L"userDefinedTaskbarIconSize");
  Wh_Log(L"setting %d %s", g_settings.userDefinedTrayIconSize, L"userDefinedTrayIconSize");
  Wh_Log(L"setting %d %s", g_settings.userDefinedTaskbarButtonSize, L"userDefinedTaskbarButtonSize");
  Wh_Log(L"setting %d %s", g_settings.userDefinedTrayButtonSize, L"userDefinedTrayButtonSize");
  Wh_Log(L"setting %d %s", (int)g_settings.userDefinedTaskbarCornerRadius, L"userDefinedTaskbarCornerRadius");
  Wh_Log(L"setting %d %s", g_settings.userDefinedTaskButtonCornerRadius, L"userDefinedTaskButtonCornerRadius");
  Wh_Log(L"setting %d %s", g_settings.userDefinedFlatTaskbarBottomCorners ? 1 : 0, L"userDefinedFlatTaskbarBottomCorners");
  Wh_Log(L"setting %d %s", g_settings.userDefinedTaskbarBackgroundOpacity, L"userDefinedTaskbarBackgroundOpacity");
  Wh_Log(L"setting %d %s", g_settings.userDefinedTaskbarBackgroundTint, L"userDefinedTaskbarBackgroundTint");
  Wh_Log(L"setting %d %s", g_settings.userDefinedTaskbarBackgroundLuminosity, L"userDefinedTaskbarBackgroundLuminosity");
  Wh_Log(L"setting %d %s", g_settings.userDefinedTaskbarBorderOpacity, L"userDefinedTaskbarBorderOpacity");
  Wh_Log(L"setting %d %s", (int)(g_settings.userDefinedTaskbarBorderThickness * 100.0 / 10.0), L"userDefinedTaskbarBorderThickness (scaled)");
  Wh_Log(L"setting %d %s", g_settings.userDefinedFullWidthTaskbarBackground ? 1 : 0, L"userDefinedFullWidthTaskbarBackground");
  Wh_Log(L"setting %d %s", g_settings.userDefinedIgnoreShowDesktopButton ? 1 : 0, L"userDefinedIgnoreShowDesktopButton");
  Wh_Log(L"setting %d %s", g_settings.userDefinedStyleTrayArea ? 1 : 0, L"userDefinedStyleTrayArea");
  Wh_Log(L"setting %d %s", g_settings.userDefinedTrayAreaDivider ? 1 : 0, L"userDefinedTrayAreaDivider");
  Wh_Log(L"setting %d %s", g_settings.borderColorR, L"borderColorR");
  Wh_Log(L"setting %d %s", g_settings.borderColorG, L"borderColorG");
  Wh_Log(L"setting %d %s", g_settings.borderColorB, L"borderColorB");
}
bool ApplyStyle(FrameworkElement const& xamlRootContent, std::wstring monitorName) {
  Wh_Log(L".");
  if (!xamlRootContent) {
  Wh_Log(L".");
    Wh_Log(L"xamlRootContent is null");
    return false;
  }
  auto& state = g_taskbarStates[monitorName];
  Wh_Log(L"ApplyStyle for monitor: %s", monitorName.c_str());
  g_scheduled_low_priority_update = false;
  auto now = std::chrono::steady_clock::now();
  if (!g_unloading && now - state.lastApplyStyleTime < std::chrono::milliseconds(200)) {
  Wh_Log(L".");
    return true;
  }
  state.lastApplyStyleTime = now;
  if (!xamlRootContent) return false;
  auto taskFrame = FindChildByClassName(xamlRootContent, L"Taskbar.TaskbarFrame");
  if (!taskFrame) {
  Wh_Log(L".");
    Wh_Log(L"Failed to find Taskbar.TaskbarFrame");
    return false;
  }
  auto rootGridTaskBar = FindChildByName(taskFrame, L"RootGrid");
  if (!rootGridTaskBar) {
  Wh_Log(L".");
    Wh_Log(L"Failed to find RootGrid in taskFrame");
    return false;
  }
  auto taskbarFrameRepeater = FindChildByName(rootGridTaskBar, L"TaskbarFrameRepeater");
  if (!taskbarFrameRepeater) {
  Wh_Log(L".");
    Wh_Log(L"Failed to find TaskbarFrameRepeater in rootGridTaskBar");
    return false;
  }
  auto trayFrame = FindChildByClassName(xamlRootContent, L"SystemTray.SystemTrayFrame");
  if (!trayFrame) {
  Wh_Log(L".");
    Wh_Log(L"Failed to find SystemTray.SystemTrayFrame");
    return false;
  }
  auto systemTrayFrameGrid = FindChildByName(trayFrame, L"SystemTrayFrameGrid");
  if (!systemTrayFrameGrid) {
  Wh_Log(L".");
    Wh_Log(L"Failed to find SystemTrayFrameGrid in trayFrame");
    return false;
  }
  auto showDesktopButton = FindChildByName(systemTrayFrameGrid, L"ShowDesktopStack");
  if (!showDesktopButton) {
  Wh_Log(L".");
    Wh_Log(L"Failed to find ShowDesktopStack in systemTrayFrameGrid");
    return false;
  }
  auto taskbarBackground = FindChildByClassName(rootGridTaskBar, L"Taskbar.TaskbarBackground");
  if (!taskbarBackground) {
  Wh_Log(L".");
    Wh_Log(L"Failed to find Taskbar.TaskbarBackground in rootGridTaskBar");
    return false;
  }
  auto backgroundFillParent = FindChildByClassName(taskbarBackground, L"Windows.UI.Xaml.Controls.Grid");
  if (!backgroundFillParent) {
  Wh_Log(L".");
    Wh_Log(L"Failed to find backgroundFillParent in taskbarBackground");
    return false;
  }
  auto backgroundFillChild = FindChildByName(backgroundFillParent, L"BackgroundFill");
  if (!backgroundFillChild) {
  Wh_Log(L".");
    Wh_Log(L"Failed to find BackgroundFill in backgroundFillParent");
    return false;
  }
  auto notificationAreaIcons = FindChildByName(systemTrayFrameGrid, L"NotificationAreaIcons");
  if (!notificationAreaIcons) {
  Wh_Log(L".");
    Wh_Log(L"Failed to find NotificationAreaIcons in systemTrayFrameGrid");
    return false;
  }
  auto itemsPresenter = FindChildByClassName(notificationAreaIcons, L"Windows.UI.Xaml.Controls.ItemsPresenter");
  if (!itemsPresenter) {
  Wh_Log(L".");
    Wh_Log(L"Failed to find ItemsPresenter in notificationAreaIcons");
    return false;
  }
  auto stackPanel = FindChildByClassName(itemsPresenter, L"Windows.UI.Xaml.Controls.StackPanel");
  if (!stackPanel) {
  Wh_Log(L".");
    Wh_Log(L"Failed to find StackPanel in itemsPresenter");
    return false;
  }
  bool widgetPresent = IsTaskbarWidgetsEnabled();
  auto widgetElement = widgetPresent ? FindChildByClassName(taskbarFrameRepeater, L"Taskbar.AugmentedEntryPointButton") : nullptr;
  auto widgetMainView = widgetElement ? FindChildByName(widgetElement, L"ExperienceToggleButtonRootPanel") : widgetElement;
  widgetPresent = widgetPresent && widgetMainView != nullptr;
  auto widgetElementWidth = widgetPresent && widgetMainView ? widgetMainView.ActualWidth() : 0;
  if (widgetPresent && widgetElementWidth <= 0) {
  Wh_Log(L".");
    Wh_Log(L"Error: widgetPresent && widgetElementWidth<=0");
    return false;
  }
  auto widgetElementInnerChild = widgetPresent ? FindChildByClassName(widgetElement, L"Taskbar.TaskListButtonPanel") : nullptr;
  auto widgetElementVisibleWidth = widgetElementInnerChild ? widgetElementInnerChild.ActualWidth() : 0;
  auto widgetElementVisibleHeight = widgetElementInnerChild ? widgetElementInnerChild.ActualHeight() : 0;
  if (widgetElementInnerChild && widgetElementVisibleWidth <= 0) {
  Wh_Log(L".");
    Wh_Log(L"Error: widgetElementInnerChild && widgetElementVisibleWidth<=0");
    return false;
  }
  if (widgetElementInnerChild && widgetElementVisibleHeight <= 0) {
  Wh_Log(L".");
    Wh_Log(L"Error: widgetElementInnerChild && widgetElementVisibleHeight<=0");
    return false;
  }
  auto overflowButton = FindChildByClassName(taskbarFrameRepeater, L"Taskbar.OverflowToggleButton");
  bool isOverflowing = overflowButton != nullptr && !IsWeirdFrameworkElement(overflowButton);
  double rootWidth = xamlRootContent.ActualWidth();
  state.lastRootWidth = static_cast<float>(rootWidth);
  if (!g_unloading && rootWidth < 100) {
  Wh_Log(L".");
    Wh_Log(L"root width is too small");
    return false;
  }
  int childrenCountTaskbar = 0;
  const double childrenWidthTaskbarDbl = CalculateValidChildrenWidth(taskbarFrameRepeater, childrenCountTaskbar, state);
  if (!g_unloading && childrenWidthTaskbarDbl <= 0) {
  Wh_Log(L".");
    Wh_Log(L"Error: childrenWidthTaskbarDbl <= 0");
    return false;
  }
  signed int rightMostEdgeTaskbar = static_cast<signed int>((rootWidth / 2.0) + (childrenWidthTaskbarDbl / 2.0));
  unsigned int childrenWidthTaskbar = static_cast<unsigned int>(childrenWidthTaskbarDbl);
  if (!g_unloading && childrenCountTaskbar < 1) {
  Wh_Log(L".");
    Wh_Log(L"Error: childrenCountTaskbar < 1");
    return false;
  }
  if (!g_unloading && childrenWidthTaskbar <= 10) {
  Wh_Log(L".");
    Wh_Log(L"Error: childrenWidthTaskbar <= 10");
    return false;
  }
  if (!g_unloading && rightMostEdgeTaskbar < 0) {
  Wh_Log(L".");
    Wh_Log(L"Error: rightMostEdgeTaskbar < 0");
    return false;
  }
  bool rightMostEdgeChangedTaskbar = (state.lastTaskbarData.rightMostEdge != rightMostEdgeTaskbar);
  if (!isOverflowing && (rightMostEdgeChangedTaskbar || state.lastTaskbarData.rightMostEdge == 0.0)) {
  Wh_Log(L".");
    state.lastTaskbarData.childrenCount = childrenCountTaskbar;
    state.lastTaskbarData.rightMostEdge = rightMostEdgeTaskbar;
    state.lastTaskbarData.childrenWidth = childrenWidthTaskbar;
  }
  trayFrame.Clip(nullptr);
  if (trayFrame.GetValue(FrameworkElement::HorizontalAlignmentProperty()).as<winrt::Windows::Foundation::IReference<HorizontalAlignment>>().Value() == HorizontalAlignment::Center) {
  Wh_Log(L".");
    trayFrame.SetValue(FrameworkElement::HorizontalAlignmentProperty(), winrt::box_value(HorizontalAlignment::Right));
  }
  int childrenCountTray = 0;
  double trayFrameWidthDbl = CalculateValidChildrenWidth(systemTrayFrameGrid, childrenCountTray, state);
  if (!g_unloading && trayFrameWidthDbl <= 0) {
  Wh_Log(L".");
    Wh_Log(L"Error: trayFrameWidthDbl <= 0");
    return false;
  }
  if (!g_unloading && childrenCountTray <= 0) {
  Wh_Log(L".");
    Wh_Log(L"Error: childrenCountTray <= 0");
    return false;
  }
  float showDesktopButtonWidth = static_cast<float>(g_settings.userDefinedIgnoreShowDesktopButton ? showDesktopButton.ActualWidth() : 0);
  int trayGapPlusExtras = g_settings.userDefinedTrayTaskGap + widgetElementVisibleWidth + (widgetPresent ? -6 + g_settings.userDefinedTrayTaskGap : 0);
  const unsigned int trayFrameWidth = static_cast<unsigned int>(trayFrameWidthDbl + trayGapPlusExtras);
  if (!g_unloading && childrenCountTray == 0) {
  Wh_Log(L".");
    Wh_Log(L"Error: childrenCountTray == 0");
    return false;
  }
  if (!g_unloading && trayFrameWidth <= 1) {
  Wh_Log(L".");
    Wh_Log(L"Error: trayFrameWidth <= 1");
    return false;
  }
  float centeredTray = (rootWidth - trayFrameWidth) / 2.0f;
  if (!g_unloading && centeredTray <= 1) {
  Wh_Log(L".");
    Wh_Log(L"Error: centeredTray <= 1");
    return false;
  }
  float newXOffsetTray = centeredTray + (childrenWidthTaskbar / 2.0f) + trayGapPlusExtras + showDesktopButtonWidth;
  // tray animations
  auto systemTrayFrameGridVisual = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(systemTrayFrameGrid);
  if (!systemTrayFrameGridVisual) {
  Wh_Log(L".");
    Wh_Log(L"Error: !SystemTrayFrameGridVisual");
    return false;
  }
  auto originalOffset = systemTrayFrameGridVisual.Offset();
  if (state.initOffsetX == -1) {
  Wh_Log(L".");
    state.initOffsetX = originalOffset.x;
  }
  auto taskbarFrameRepeaterVisual = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(taskbarFrameRepeater);
  if (!taskbarFrameRepeaterVisual) {
  Wh_Log(L".");
    Wh_Log(L"Error: !taskbarFrameRepeaterVisual");
    return false;
  }
  float targetTaskFrameOffsetX = newXOffsetTray - rightMostEdgeTaskbar - trayGapPlusExtras;
  // 5 pixels tolerance
  if (!g_invalidateDimensions && !g_unloading && abs(newXOffsetTray - systemTrayFrameGridVisual.Offset().x) <= 5 && childrenWidthTaskbar == state.lastChildrenWidthTaskbar && trayFrameWidth == state.lastTrayFrameWidth && abs(targetTaskFrameOffsetX - taskbarFrameRepeaterVisual.Offset().x) <= 5) {
  Wh_Log(L".");
    Wh_Log(L"newXOffsetTray is within 5 pixels of systemTrayFrameGridVisual offset %f, childrenWidthTaskbar and trayFrameWidth didn't change: %d, %d", systemTrayFrameGridVisual.Offset().x, childrenWidthTaskbar, state.lastTrayFrameWidth);
    return true;
  }
  if (childrenWidthTaskbar < 1) {
  Wh_Log(L".");
    state.lastChildrenWidthTaskbar = 1;
  } else {
    state.lastChildrenWidthTaskbar = static_cast<unsigned int>(childrenWidthTaskbar);
  }
  if (trayFrameWidth < 1) {
  Wh_Log(L".");
    state.lastTrayFrameWidth = 1;
  } else {
    state.lastTrayFrameWidth = static_cast<unsigned int>(trayFrameWidth);
  }
  signed int userDefinedTaskbarOffsetY = (g_settings.userDefinedFlatTaskbarBottomCorners || g_settings.userDefinedFullWidthTaskbarBackground) ? 0 : g_settings.userDefinedTaskbarOffsetY;
  float targetWidth = g_unloading ? rootWidth : (childrenWidthTaskbar + trayFrameWidth + (g_settings.userDefinedTaskbarBackgroundHorizontalPadding * 2));
  if (targetWidth < 1) {
  Wh_Log(L".");
    Wh_Log(L"Error: targetWidth<1");
    return false;
  }
  state.lastStartButtonXCalculated = (rootWidth - targetWidth) / 2.0f;
  auto heightValue = (g_settings.userDefinedTaskbarHeight + abs(userDefinedTaskbarOffsetY < 0 ? (userDefinedTaskbarOffsetY * 2) : 0));
  if (heightValue < g_settings.userDefinedTaskbarHeight / 2) {
  Wh_Log(L".");
    Wh_Log(L"Error: heightValue<g_settings.userDefinedTaskbarHeight/2");
    return false;
  }
  if (g_invalidateDimensions) {
  Wh_Log(L".");
    g_invalidateDimensions = false;
    if (g_settings.userDefinedTaskbarHeight <= 0) {
  Wh_Log(L".");
      Wh_Log(L"Invalid size detected! Panel Height");
      return false;
    }
    if (heightValue <= 0) {
  Wh_Log(L".");
      Wh_Log(L"Invalid size detected! Panel Height");
      return false;
    }
    trayFrame.Height(g_settings.userDefinedTaskbarHeight);
    trayFrame.MaxHeight(g_settings.userDefinedTaskbarHeight);
    taskFrame.Height(heightValue);
    taskFrame.MaxHeight(heightValue);
    taskbarFrameRepeater.Height(g_settings.userDefinedTaskbarHeight);
    taskbarFrameRepeater.MaxHeight(g_settings.userDefinedTaskbarHeight);
    taskbarFrameRepeater.Width(rootWidth);
    taskbarFrameRepeater.MaxWidth(rootWidth);
  }
  if (auto taskbarFrameRepeaterVisualCompositor = taskbarFrameRepeaterVisual.Compositor()) {
  Wh_Log(L".");
    if (!g_unloading) {
  Wh_Log(L".");
      auto taskbarFrameRepeaterVisualAnimation = taskbarFrameRepeaterVisualCompositor.CreateVector3KeyFrameAnimation();
      auto animationControllerTaskbarFrameRepeaterVisual = taskbarFrameRepeaterVisual.TryGetAnimationController(L"Offset");
      taskbarFrameRepeaterVisualAnimation.InsertKeyFrame(1.0f, winrt::Windows::Foundation::Numerics::float3{targetTaskFrameOffsetX, taskbarFrameRepeaterVisual.Offset().y, taskbarFrameRepeaterVisual.Offset().z});
      taskbarFrameRepeaterVisual.StartAnimation(L"Offset", taskbarFrameRepeaterVisualAnimation);
    } else {
      taskbarFrameRepeaterVisual.Offset({0.0f, 0.0f, 0.0f});
    }
  }
  bool movingInwards = originalOffset.x > newXOffsetTray;
  auto taskbarVisual = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(taskbarFrameRepeater);
  auto trayVisualCompositor = systemTrayFrameGridVisual.Compositor();
  if (trayVisualCompositor) {
  Wh_Log(L".");
    if (!g_unloading) {
  Wh_Log(L".");
      float targetOffsetXTray = static_cast<float>(newXOffsetTray - trayGapPlusExtras - (rootWidth - trayFrameWidth));
      auto trayAnimation = trayVisualCompositor.CreateVector3KeyFrameAnimation();
      trayAnimation.InsertKeyFrame(1.0f, winrt::Windows::Foundation::Numerics::float3{targetOffsetXTray, systemTrayFrameGridVisual.Offset().y, systemTrayFrameGridVisual.Offset().z});
      if (movingInwards) {
  Wh_Log(L".");
        trayAnimation.DelayTime(winrt::Windows::Foundation::TimeSpan(std::chrono::milliseconds(childrenCountTaskbar * 4)));
      }
      systemTrayFrameGridVisual.StartAnimation(L"Offset", trayAnimation);
    } else {
      systemTrayFrameGridVisual.Offset({0.0f, 0.0f, 0.0f});
    }
  }
  if (widgetPresent && widgetMainView) {
  Wh_Log(L".");
    if (widgetElement) {
  Wh_Log(L".");
      auto widgetVisualParent = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(widgetElement);
      if (widgetVisualParent && widgetVisualParent.Offset().x != 0.0f) {
  Wh_Log(L".");
        widgetVisualParent.Offset({0.0f, widgetVisualParent.Offset().y, widgetVisualParent.Offset().z});
      }
    }
    auto widgetVisual = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(widgetMainView);
    if (widgetVisual) {
  Wh_Log(L".");
      if (!g_unloading) {
  Wh_Log(L".");
        auto compositorWidget = widgetVisual.Compositor();
        if (compositorWidget) {
  Wh_Log(L".");
          float targetOffsetXWidget = static_cast<float>(rightMostEdgeTaskbar - 8) + g_settings.userDefinedTrayTaskGap;
          auto widgetOffsetAnimation = compositorWidget.CreateVector3KeyFrameAnimation();
          widgetOffsetAnimation.InsertKeyFrame(1.0f, winrt::Windows::Foundation::Numerics::float3{static_cast<float>(targetOffsetXWidget), static_cast<float>(abs(g_settings.userDefinedTaskbarHeight - widgetElementVisibleHeight)), taskbarVisual.Offset().z});
          if (movingInwards) {
  Wh_Log(L".");
            widgetOffsetAnimation.DelayTime(winrt::Windows::Foundation::TimeSpan(std::chrono::milliseconds(childrenCountTaskbar * 4)));
          }
          widgetVisual.StartAnimation(L"Offset", widgetOffsetAnimation);
        }
      } else {
        widgetVisual.Offset({0.0f, 0.0f, 0.0f});
      }
    }
  }
  if (state.lastTargetWidth <= 10) {
  Wh_Log(L".");
    state.lastTargetWidth = static_cast<float>(rootWidth);
    if (!g_unloading && state.lastTargetWidth <= 0) {
  Wh_Log(L".");
      Wh_Log(L"Error: g_unloading && state.lastTargetWidth <= 0");
      return false;
    }
  }
  const float targetWidthRect = !g_settings.userDefinedFullWidthTaskbarBackground ? targetWidth : static_cast<float>(rootWidth);
  if (!g_unloading && targetWidthRect <= 0) {
  Wh_Log(L".");
    Wh_Log(L"Error: targetWidthRect<=0");
    return false;
  }
  int rightMostEdgeTray = static_cast<int>((rootWidth - targetWidth) / 2 + targetWidth);
  if (state.lastRightMostEdgeTray != rightMostEdgeTray) {
  Wh_Log(L".");
    state.lastRightMostEdgeTray = rightMostEdgeTray;
    Wh_SetIntValue((L"lastRightMostEdgeTray_" + monitorName).c_str(), rightMostEdgeTray);
  }
  float leftMostEdgeTray = rightMostEdgeTray - trayFrameWidth;
  if (leftMostEdgeTray != state.lastLeftMostEdgeTray) {
  Wh_Log(L".");
    state.lastLeftMostEdgeTray = leftMostEdgeTray;
    Wh_SetIntValue((L"lastLeftMostEdgeTray_" + monitorName).c_str(), static_cast<int>(leftMostEdgeTray));
  }
  const auto targetHeightPrelim = (!g_settings.userDefinedFullWidthTaskbarBackground ? g_settings.userDefinedTaskbarHeight : xamlRootContent.ActualHeight());
  if (!g_unloading && targetHeightPrelim <= 0) {
  Wh_Log(L".");
    Wh_Log(L"Error: targetHeightPrelim<=0");
    return false;
  }
  const auto clipHeight = static_cast<float>(targetHeightPrelim + ((g_settings.userDefinedFlatTaskbarBottomCorners) ? (targetHeightPrelim - g_settings.userDefinedTaskbarCornerRadius) : 0.0f));
  if (!g_unloading && clipHeight <= 0) {
  Wh_Log(L".");
    Wh_Log(L"Error: clipHeight<=0");
    return false;
  }
  ProcessStackPanelChildren(stackPanel, clipHeight);
  ChangeControlCenterIconSize(systemTrayFrameGrid);
  auto trayOverflowArrowNotifyIconStack = FindChildByName(systemTrayFrameGrid, L"NotifyIconStack");
  if (trayOverflowArrowNotifyIconStack) {
  Wh_Log(L".");
    SetDividerForElement(trayOverflowArrowNotifyIconStack, clipHeight, g_settings.userDefinedTrayAreaDivider, true);
  } else {
    SetDividerForElement(stackPanel, clipHeight, g_settings.userDefinedTrayAreaDivider, true);
  }
  //  if (widgetPresent && widgetElementInnerChild) {
  Wh_Log(L".");
  //    SetDividerForElement(widgetElementInnerChild, clipHeight, widgetPresent && g_settings.userDefinedTrayAreaDivider, true);
  //  }
  if (!taskbarBackground) return false;
  auto taskbarStroke = FindChildByName(backgroundFillParent, L"BackgroundStroke");
  if (taskbarStroke) {
  Wh_Log(L".");
    taskbarStroke.Opacity(g_unloading ? 1.0 : 0.0);
  }
  auto screenEdgeStroke = FindChildByName(rootGridTaskBar, L"ScreenEdgeStroke");
  if (screenEdgeStroke) {
  Wh_Log(L".");
    screenEdgeStroke.Opacity(g_unloading ? 1.0 : 0.0);
  }
  auto userDefinedTaskbarBackgroundLuminosity = std::to_wstring(g_settings.userDefinedTaskbarBackgroundLuminosity / 100.0f);
  auto userDefinedTaskbarBackgroundOpacity = std::to_wstring(g_settings.userDefinedTaskbarBackgroundOpacity / 100.0f);
  auto userDefinedTaskbarBackgroundTint = std::to_wstring(g_settings.userDefinedTaskbarBackgroundTint / 100.0f);
  if (g_settings.userDefinedCustomizeTaskbarBackground) {
  Wh_Log(L".");
    SetElementPropertyFromString(backgroundFillChild, L"Windows.UI.Xaml.Shapes.Rectangle", L"Fill", L"<AcrylicBrush TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" FallbackColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\"" + userDefinedTaskbarBackgroundTint + L"\" TintLuminosityOpacity=\"" + userDefinedTaskbarBackgroundLuminosity + L"\" Opacity=\"" + userDefinedTaskbarBackgroundOpacity + L"\"/>", true);
//    For custom brush
//    auto compositor = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(backgroundFillChild).Compositor();
//    float blurAmount = float(g_settings.userDefinedTaskbarBackgroundLuminosity);
//    winrt::Windows::Foundation::Numerics::float4 tint = {0,0,0,0};
//    auto blurBrush = winrt::make<XamlBlurBrush>(compositor, blurAmount, tint);
//    auto rectangle = backgroundFillChild.try_as<winrt::Windows::UI::Xaml::Shapes::Rectangle>();
//    if (rectangle){
//    rectangle.Fill(blurBrush);
//    }
  }
  // you can also try SystemAccentColor
  auto backgroundFillVisual = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(backgroundFillChild);
  auto compositorTaskBackground = backgroundFillVisual.Compositor();
  // borders and corners
  if (!g_unloading) {
  Wh_Log(L".");
    if (backgroundFillVisual) {
  Wh_Log(L".");
      if (compositorTaskBackground) {
  Wh_Log(L".");
        auto roundedRect = compositorTaskBackground.CreateRoundedRectangleGeometry();
        roundedRect.CornerRadius({g_settings.userDefinedTaskbarCornerRadius, g_settings.userDefinedTaskbarCornerRadius});
        auto borderGeometry = compositorTaskBackground.CreateRoundedRectangleGeometry();
        borderGeometry.CornerRadius({g_settings.userDefinedTaskbarCornerRadius - static_cast<float>(g_settings.userDefinedTaskbarBorderThickness) / 2.0f, g_settings.userDefinedTaskbarCornerRadius - static_cast<float>(g_settings.userDefinedTaskbarBorderThickness) / 2.0f});
        roundedRect.Size({!g_settings.userDefinedFullWidthTaskbarBackground ? state.lastTargetWidth : targetWidthRect, clipHeight});
        borderGeometry.Offset({static_cast<float>(g_settings.userDefinedTaskbarBorderThickness / 2.0f), static_cast<float>(g_settings.userDefinedTaskbarBorderThickness / 2.0f)});
        auto shapeVisualBorderControl = compositorTaskBackground.CreateShapeVisual();
        shapeVisualBorderControl.Size({!g_settings.userDefinedFullWidthTaskbarBackground ? state.lastTargetWidth : targetWidthRect, clipHeight});
        auto geometricClip = compositorTaskBackground.CreateGeometricClip(roundedRect);
        auto borderShape = compositorTaskBackground.CreateSpriteShape(borderGeometry);
        winrt::Windows::UI::Color borderColor = {g_settings.userDefinedTaskbarBorderOpacity, static_cast<BYTE>(g_settings.borderColorR), static_cast<BYTE>(g_settings.borderColorG), static_cast<BYTE>(g_settings.borderColorB)};
        borderShape.StrokeBrush(compositorTaskBackground.CreateColorBrush(borderColor));
        borderShape.StrokeThickness(g_settings.userDefinedTaskbarBorderThickness);
        borderShape.FillBrush(nullptr);
        const float userDefinedTaskbarBorderThicknessFloat = static_cast<float>(g_settings.userDefinedTaskbarBorderThickness);
        borderGeometry.Size({static_cast<float>(targetWidthRect - userDefinedTaskbarBorderThicknessFloat), static_cast<float>(clipHeight - userDefinedTaskbarBorderThicknessFloat)});
        backgroundFillVisual.Clip(geometricClip);
        shapeVisualBorderControl.Shapes().Append(borderShape);
        winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::SetElementChildVisual(backgroundFillChild, shapeVisualBorderControl);
        if (!g_settings.userDefinedFullWidthTaskbarBackground) {
  Wh_Log(L".");
          float offsetXRect = (rootWidth - targetWidth) / 2;
          float newOffsetYRect = userDefinedTaskbarOffsetY <= 0 ? static_cast<float>(abs(userDefinedTaskbarOffsetY)) : 0.0f;
          // size animation
          auto sizeAnimationRect = compositorTaskBackground.CreateVector2KeyFrameAnimation();
          sizeAnimationRect.InsertKeyFrame(0.0f, {(std::abs(state.lastTargetWidth - rootWidth) <= 5 ? targetWidthRect : state.lastTargetWidth), clipHeight});
          sizeAnimationRect.InsertKeyFrame(1.0f, {targetWidthRect, clipHeight});
          auto sizeAnimationBorderGeometry = compositorTaskBackground.CreateVector2KeyFrameAnimation();
          sizeAnimationBorderGeometry.InsertKeyFrame(0.0f, {(std::abs(state.lastTargetWidth - rootWidth) <= 5 ? targetWidthRect : state.lastTargetWidth) - userDefinedTaskbarBorderThicknessFloat, clipHeight - userDefinedTaskbarBorderThicknessFloat});
          sizeAnimationBorderGeometry.InsertKeyFrame(1.0f, {targetWidthRect - userDefinedTaskbarBorderThicknessFloat, clipHeight - userDefinedTaskbarBorderThicknessFloat});
          roundedRect.StartAnimation(L"Size", sizeAnimationRect);
          shapeVisualBorderControl.StartAnimation(L"Size", sizeAnimationRect);
          borderGeometry.StartAnimation(L"Size", sizeAnimationBorderGeometry);
          //   // centering the clip animation
          float refinedStateLastTargetOffsetX = state.lastTargetOffsetX == 0 ? offsetXRect : state.lastTargetOffsetX;
          roundedRect.Offset({refinedStateLastTargetOffsetX, state.lastTargetOffsetY});
          shapeVisualBorderControl.Offset({refinedStateLastTargetOffsetX, state.lastTargetOffsetY, 0.0f});
          auto offsetAnimationRect = compositorTaskBackground.CreateVector2KeyFrameAnimation();
          offsetAnimationRect.InsertKeyFrame(0.0f, {refinedStateLastTargetOffsetX, state.lastTargetOffsetY});
          offsetAnimationRect.InsertKeyFrame(1.0f, {offsetXRect, newOffsetYRect});
          auto offsetAnimationRect3V = compositorTaskBackground.CreateVector3KeyFrameAnimation();
          offsetAnimationRect3V.InsertKeyFrame(0.0f, {refinedStateLastTargetOffsetX, state.lastTargetOffsetY, 0.0f});
          offsetAnimationRect3V.InsertKeyFrame(1.0f, {offsetXRect, newOffsetYRect, 0.0f});
          roundedRect.StartAnimation(L"Offset", offsetAnimationRect);
          shapeVisualBorderControl.StartAnimation(L"Offset", offsetAnimationRect3V);
          state.lastTargetOffsetX = offsetXRect;
          state.lastTargetOffsetY = newOffsetYRect;
        } else {
          state.lastTargetOffsetX = 0;
          state.lastTargetOffsetY = 0;
          roundedRect.Offset({state.lastTargetOffsetX, state.lastTargetOffsetY});
          shapeVisualBorderControl.Offset({state.lastTargetOffsetX, state.lastTargetOffsetY, 0.0f});
        }
      }
    }
  } else {
    if (backgroundFillVisual) {
  Wh_Log(L".");
      backgroundFillVisual.Clip(nullptr);
    }
    if (compositorTaskBackground) {
  Wh_Log(L".");
      winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::SetElementChildVisual(backgroundFillChild, compositorTaskBackground.CreateShapeVisual());
    }
  }
  state.wasOverflowing = isOverflowing;
  state.lastTargetWidth = targetWidthRect;
  state.lastTargetWidth = targetWidth;
  return true;
}
void ApplySettings(HWND hTaskbarWnd) {
  Wh_Log(L".");
  RunFromWindowThread(hTaskbarWnd, [](void* pParam) {
  Wh_Log(L"."); ApplySettingsFromTaskbarThread(); }, 0);
}
void RefreshSettings() {
  Wh_Log(L".");
  g_invalidateDimensions = true;
  Wh_ModSettingsChangedTBIconSize();
  Wh_ModSettingsChangedStartButtonPosition();
  UpdateGlobalSettings();
}
void ResetGlobalVars() {
  Wh_Log(L".");
  g_invalidateDimensions = true;
  for (auto& [key, state] : g_taskbarStates) {
  Wh_Log(L".");
    state.lastTaskbarData.childrenCount = 0;
    state.lastTaskbarData.rightMostEdge = 0;
    // state.lastTaskbarData.childrenWidth = 0;
    state.lastChildrenWidthTaskbar = 0;
    // state.lastTrayFrameWidth = 0;
    state.wasOverflowing = false;
  }
}
bool g_PartialMode = false;
void Wh_ModSettingsChanged() {
  Wh_Log(L".");
  if (g_PartialMode) {
  Wh_Log(L".");
    return;
  }
  Wh_Log(L"Settings Changed");
  ResetGlobalVars();
  RefreshSettings();
  ApplySettingsFromTaskbarThread();
}
bool IsExplorer() {
  Wh_Log(L".");
  wchar_t processPath[MAX_PATH];
  if (GetModuleFileName(NULL, processPath, MAX_PATH)) {
  Wh_Log(L".");
    const wchar_t* processName = wcsrchr(processPath, L'\\');
    if (processName && _wcsicmp(processName + 1, L"explorer.exe") == 0) {
  Wh_Log(L".");
      return true;
    }
  }
  return false;
}
using SetWindowPos_t = BOOL(WINAPI*)(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags);
SetWindowPos_t SetWindowPos_Original = nullptr;
std::wstring GetProcessExeName(DWORD processId) {
  Wh_Log(L".");
  std::wstring result = L"<unknown>";
  HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
  if (hProcess) {
  Wh_Log(L".");
    WCHAR path[MAX_PATH];
    DWORD size = ARRAYSIZE(path);
    if (QueryFullProcessImageNameW(hProcess, 0, path, &size)) {
  Wh_Log(L".");
      std::wstring fullPath = path;
      size_t pos = fullPath.find_last_of(L"\\/");
      result = (pos != std::wstring::npos) ? fullPath.substr(pos + 1) : fullPath;
    }
    CloseHandle(hProcess);
  }
  return result;
}
BOOL WINAPI SetWindowPos_Hook(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags) {
  Wh_Log(L".");
  DWORD processId = 0;
  bool userDefinedMoveFlyoutControlCenter = Wh_GetIntSetting(L"MoveFlyoutControlCenter");
  if (!hWnd || !GetWindowThreadProcessId(hWnd, &processId)) {
  Wh_Log(L".");
    return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
  }
  WCHAR className[256] = L"<unknown>";
  GetClassNameW(hWnd, className, ARRAYSIZE(className));
  std::wstring windowClassName = className;
  std::wstring processFileName = GetProcessExeName(processId);
  if (true) {
  Wh_Log(L".");
    Wh_Log(L"[SetWindowPos] PID: %lu | EXE: %s | Class: %s | HWND: 0x%p | Pos: (%d,%d) Size: %dx%d Flags: 0x%08X", processId, processFileName.c_str(), windowClassName.c_str(), hWnd, X, Y, cx, cy, uFlags);
  }
  if (!g_unloading && userDefinedMoveFlyoutControlCenter && _wcsicmp(processFileName.c_str(), L"ShellHost.exe") == 0 && _wcsicmp(windowClassName.c_str(), L"ControlCenterWindow") == 0) {
  Wh_Log(L".");
    HMONITOR monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitorInfo{
        .cbSize = sizeof(MONITORINFO),
    };
    GetMonitorInfo(monitor, &monitorInfo);
    auto monitorName = GetMonitorName(monitor);
    int lastRecordedTrayRightMostEdgeForMonitor = Wh_GetIntValue((L"lastRightMostEdgeTray_" + monitorName).c_str(), -1);
    if (lastRecordedTrayRightMostEdgeForMonitor > 0) {
  Wh_Log(L".");
      UINT monitorDpiX = 96;
      UINT monitorDpiY = 96;
      GetDpiForMonitor(monitor, MDT_DEFAULT, &monitorDpiX, &monitorDpiY);
      float dpiScale = monitorDpiX / 96.0f;
      X = static_cast<int>(lastRecordedTrayRightMostEdgeForMonitor * dpiScale + (12 * dpiScale) - (Wh_GetIntSetting(L"AlignFlyoutInner") ? cx : (cx / 2.0f)));
      Wh_Log(L"[SetWindowPos] New X %d", X);
    } else {
      Wh_Log(L"[SetWindowPos] No reference state for monitor %s", monitorName.c_str());
    }
  }
  return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}
BOOL Wh_ModInit() {
  Wh_Log(L".");
  Wh_Log(L"======================================================");
  HMODULE moduleUser32 = LoadLibraryW(L"user32.dll");
  if (moduleUser32) {
  Wh_Log(L".");
    auto pSetWindowPos = (SetWindowPos_t)GetProcAddress(moduleUser32, "SetWindowPos");
    if (pSetWindowPos) {
  Wh_Log(L".");
      if (WindhawkUtils::Wh_SetFunctionHookT(pSetWindowPos, SetWindowPos_Hook, &SetWindowPos_Original)) {
  Wh_Log(L".");
        Wh_Log(L"Successfully hooked SetWindowPos");
      } else {
        Wh_Log(L"Failed to hook SetWindowPos");
      }
    } else {
      Wh_Log(L"Failed to get address of SetWindowPos");
    }
  } else {
    Wh_Log(L"Failed to load user32.dll");
  }
  if (!IsExplorer()) {
  Wh_Log(L".");
    g_PartialMode = true;
    Wh_Log(L"Not explorer.exe; setting g_PartialMode to true");
    HMODULE moduleStartDocked = GetModuleHandle(L"StartDocked.dll");
    if (moduleStartDocked) {
  Wh_Log(L".");
      WindhawkUtils::SYMBOL_HOOK hook[] = {{{LR"(private: void __cdecl StartDocked::StartSizingFrame::UpdateWindowRegion(class Windows::Foundation::Size))"}, &StartDocked__StartSizingFrame_UpdateWindowRegion_WithArgs_Original, StartDocked__StartSizingFrame_UpdateWindowRegion_WithArgs_Hook}};
      return WindhawkUtils::HookSymbols(moduleStartDocked, hook, ARRAYSIZE(hook));
    }
    return true;
  }
  g_unloading = false;
  if (!Wh_ModInitTBIconSize()) {
  Wh_Log(L".");
    Wh_Log(L"Wh_ModInitTBIconSize failed");
    return FALSE;
  }
  if (!Wh_ModInitStartButtonPosition()) {
  Wh_Log(L".");
    Wh_Log(L"Wh_ModInitStartButtonPosition failed");
    return FALSE;
  }
  return TRUE;
}
void Wh_ModAfterInit() {
  Wh_Log(L".");
  if (g_PartialMode) {
  Wh_Log(L".");
    g_lastRecordedStartMenuWidth = Wh_GetIntValue(L"lastRecordedStartMenuWidth", g_lastRecordedStartMenuWidth);
    return;
  }
  Wh_ModAfterInitTBIconSize();
  Wh_ModSettingsChanged();
  ApplySettingsDebounced(300);
}
void Wh_ModBeforeUninit() {
  Wh_Log(L".");
  if (g_PartialMode) {
  Wh_Log(L".");
    return;
  }
  g_unloading = true;
  Wh_ModBeforeUninitTBIconSize();
  Wh_ModBeforeUninitStartButtonPosition();
  RefreshSettings();
  HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
  if (hTaskbarWnd) {
  Wh_Log(L".");
    ApplySettings(hTaskbarWnd);
  }
}
void Wh_ModUninit() {
  Wh_Log(L".");
  if (g_PartialMode) {
  Wh_Log(L".");
    return;
  }
  Wh_ModUninitTBIconSize();
  ResetGlobalVars();
  CleanupDebounce();
  Wh_Log(L"... detached");
}