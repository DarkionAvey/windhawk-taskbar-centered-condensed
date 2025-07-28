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
    DWORD value = 0;
    DWORD size = sizeof(value);
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
                     LR"(Software\Microsoft\Windows\CurrentVersion\Explorer\Advanced)",
                      0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        if (RegQueryValueExW(hKey, L"TaskbarAl", nullptr, nullptr,
                             reinterpret_cast<LPBYTE>(&value), &size) == ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return value == 0;
        }
        RegCloseKey(hKey);
    }
    return false;
}
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
        STDAPI GetDpiForMonitor(HMONITOR hmonitor,
                        MONITOR_DPI_TYPE dpiType,
                        UINT* dpiX,
                        UINT* dpiY);
size_t OffsetFromAssemblyRegex(void* func,
                               size_t defValue,
                               std::regex regex,
                               int limit = 30) {
    BYTE* p = (BYTE*)func;
    for (int i = 0; i < limit; i++) {
        WH_DISASM_RESULT result;
        if (!Wh_Disasm(p, &result)) {
            break;
        }
        p += result.length;
        std::string_view s = result.text;
        if (s == "ret") {
            break;
        }
        std::match_results<std::string_view::const_iterator> match;
        if (std::regex_match(s.begin(), s.end(), match, regex)) {
            return std::stoull(match[1], nullptr, 16);
        }
    }
    Wh_Log(L"Failed for %p", func);
    return defValue;
}
std::optional<bool> IsOsFeatureEnabled(UINT32 featureId) {
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
        HMODULE hNtDll = GetModuleHandle(L"ntdll.dll");
        return hNtDll ? (RtlQueryFeatureConfiguration_t)GetProcAddress(
                            hNtDll, "RtlQueryFeatureConfiguration")
                      : nullptr;
    }();
    if (!pRtlQueryFeatureConfiguration) {
        Wh_Log(L"RtlQueryFeatureConfiguration not found");
        return std::nullopt;
    }
    RTL_FEATURE_CONFIGURATION feature = {0};
    INT64 changeStamp = 0;
    HRESULT hr =
        pRtlQueryFeatureConfiguration(featureId, 1, &changeStamp, &feature);
    if (SUCCEEDED(hr)) {
        Wh_Log(L"RtlQueryFeatureConfiguration result for %u: %d", featureId,
               feature.enabledState);
        switch (feature.enabledState) {
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
void OverrideResourceDirectoryLookup(
    PCSTR sourceFunctionName,
    const winrt::Windows::Foundation::IInspectable* key,
    winrt::Windows::Foundation::IInspectable* value) {
    if (g_unloading) {
        return;
    }
    const auto keyString = key->try_as<winrt::hstring>();
    if (!keyString) {
        return;
    }
    double newValueDouble;
    if (*keyString == L"MediumTaskbarButtonExtent") {
        newValueDouble = g_settings_tbiconsize.taskbarButtonWidth;
    } else if (*keyString == L"SmallTaskbarButtonExtent") {
        newValueDouble = g_settings_tbiconsize.taskbarButtonWidthSmall;
    } else {
        return;
    }
    const auto valueDouble = value->try_as<double>();
    if (!valueDouble) {
        return;
    }
    if (newValueDouble != *valueDouble) {
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
    auto ret =
        ResourceDictionary_Lookup_TaskbarView_Original(pThis, result, key);
    if (!*ret) {
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
    auto ret =
        ResourceDictionary_Lookup_SearchUxUi_Original(pThis, result, key);
    if (!*ret) {
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
    [[maybe_unused]] static bool logged = [] {
        Wh_Log(L"> [%S] First call, hasDynamicIconScaling=%d",
               __PRETTY_FUNCTION__, g_hasDynamicIconScaling);
        return true;
    }();
    if (g_hasDynamicIconScaling) {
        IconUtils_GetIconSize_Original(isSmall, type, size);
        return;
    }
    IconUtils_GetIconSize_Original(isSmall, type, size);
    if (!g_unloading && !isSmall) {
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
    [[maybe_unused]] static bool logged = [] {
        Wh_Log(L"> [%S] First call, hasDynamicIconScaling=%d",
               __PRETTY_FUNCTION__, g_hasDynamicIconScaling);
        return true;
    }();
    if (g_hasDynamicIconScaling) {
        return IconContainer_IsStorageRecreationRequired_Original(pThis, param1,
                                                                  flags);
    }
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
    Wh_Log(L"> hasDynamicIconScaling=%d, nIndex=%d", g_hasDynamicIconScaling,
           nIndex);
    if (g_hasDynamicIconScaling) {
        return CIconLoadingFunctions_GetClassLongPtrW_Original(pThis, hWnd,
                                                               nIndex);
    }
    if (!g_unloading && nIndex == GCLP_HICON && g_settings_tbiconsize.iconSize <= 16) {
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
    Wh_Log(L"> hasDynamicIconScaling=%d, Msg=%u, wParam=%zu, lParam=%zu",
           g_hasDynamicIconScaling, Msg, wParam, lParam);
    if (g_hasDynamicIconScaling) {
        return CIconLoadingFunctions_SendMessageCallbackW_Original(
            pThis, hWnd, Msg, wParam, lParam, lpResultCallBack, dwData);
    }
    if (!g_unloading && Msg == WM_GETICON && wParam == ICON_BIG &&
        g_settings_tbiconsize.iconSize <= 16) {
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
    Wh_Log(L"> hasDynamicIconScaling=%d", g_hasDynamicIconScaling);
    if (g_hasDynamicIconScaling) {
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
    [[maybe_unused]] static bool logged = [] {
        Wh_Log(L"> [%S] First call, hasDynamicIconScaling=%d",
               __PRETTY_FUNCTION__, g_hasDynamicIconScaling);
        return true;
    }();
    if (g_hasDynamicIconScaling) {
        return TaskListItemViewModel_GetIconHeight_Original(pThis, param1,
                                                            iconHeight);
    }
    int ret =
        TaskListItemViewModel_GetIconHeight_Original(pThis, param1, iconHeight);
    if (!g_unloading) {
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
    Wh_Log(L"> hasDynamicIconScaling=%d", g_hasDynamicIconScaling);
    if (g_hasDynamicIconScaling) {
        return TaskListGroupViewModel_GetIconHeight_Original(pThis, param1,
                                                             iconHeight);
    }
    int ret = TaskListGroupViewModel_GetIconHeight_Original(pThis, param1,
                                                            iconHeight);
    if (!g_unloading) {
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
    Wh_Log(L"> hasDynamicIconScaling=%d, enumTaskbarSize=%d",
           g_hasDynamicIconScaling, enumTaskbarSize);
    if (g_hasDynamicIconScaling) {
        Wh_Log(L"Setting hasDynamicIconScaling to false");
        g_hasDynamicIconScaling = false;
    }
    if (!g_unloading && (enumTaskbarSize == 1 || enumTaskbarSize == 2)) {
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
    Wh_Log(L"> hasDynamicIconScaling=%d, baseHeight=%f",
           g_hasDynamicIconScaling, baseHeight);
    if (g_hasDynamicIconScaling) {
        Wh_Log(L"Setting hasDynamicIconScaling to false");
        g_hasDynamicIconScaling = false;
    }
    if (!g_unloading) {
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
    [[maybe_unused]] static bool logged = [] {
        Wh_Log(L"> [%S] First call, hasDynamicIconScaling=%d",
               __PRETTY_FUNCTION__, g_hasDynamicIconScaling);
        return true;
    }();
    double iconSize =
        TaskbarConfiguration_GetIconHeightInViewPixels_method_Original(pThis);
    if (!g_unloading) {
        return iconSize <= 16 ? g_settings_tbiconsize.iconSizeSmall : g_settings_tbiconsize.iconSize;
    }
    return iconSize;
}
using TaskListButton_IconHeight_t = void(WINAPI*)(void* pThis, double height);
TaskListButton_IconHeight_t TaskListButton_IconHeight_Original;
size_t GetIconHeightOffset() {
    static size_t iconHeightOffset = []() {
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
    GetIconHeightOffset();
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
LONG GetFrameSizeOffset() {
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
            return (offset < 0 || offset > 0xFFFF) ? 0 : offset;
        }
        Wh_Log(L"frameSizeOffset not found");
        return 0;
    }();
    return frameSizeOffset;
}
void TaskbarConfiguration_UpdateFrameSize_InitOffsets() {
    GetFrameSizeOffset();
}
TaskbarConfiguration_UpdateFrameSize_t
    TaskbarConfiguration_UpdateFrameSize_Original;
void WINAPI TaskbarConfiguration_UpdateFrameSize_Hook(void* pThis) {
    LONG frameSizeOffset = GetFrameSizeOffset();
    if (!frameSizeOffset) {
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
LONG GetLastHeightOffset() {
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
    GetLastHeightOffset();
}
SystemTrayController_UpdateFrameSize_t
    SystemTrayController_UpdateFrameSize_Original;
void WINAPI SystemTrayController_UpdateFrameSize_Hook(void* pThis) {
    LONG lastHeightOffset = GetLastHeightOffset();
    if (lastHeightOffset) {
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
    if (TaskbarFrame_MaxHeight_double_Original) {
        TaskbarFrame_MaxHeight_double_Original(
            pThis, std::numeric_limits<double>::infinity());
    }
    return TaskbarFrame_Height_double_Original(pThis, value);
}
void* TaskbarController_OnGroupingModeChanged_Original;
LONG GetTaskbarFrameOffset() {
    static LONG taskbarFrameOffset = []() -> LONG {
#if defined(_M_X64)
        const BYTE* p =
            (const BYTE*)TaskbarController_OnGroupingModeChanged_Original;
        if (p && p[0] == 0x48 && p[1] == 0x83 && p[2] == 0xEC &&
            (p[4] == 0x48 || p[4] == 0x4C) && p[5] == 0x8B &&
            (p[6] & 0xC0) == 0x80) {
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
    GetTaskbarFrameOffset();
}
using TaskbarController_UpdateFrameHeight_t = void(WINAPI*)(void* pThis);
TaskbarController_UpdateFrameHeight_t
    TaskbarController_UpdateFrameHeight_Original;
void WINAPI TaskbarController_UpdateFrameHeight_Hook(void* pThis) {
    LONG taskbarFrameOffset = GetTaskbarFrameOffset();
    if (!taskbarFrameOffset) {
        Wh_Log(L"Error: taskbarFrameOffset is invalid");
        TaskbarController_UpdateFrameHeight_Original(pThis);
        return;
    }
    void* taskbarFrame = *(void**)((BYTE*)pThis + taskbarFrameOffset);
    if (!taskbarFrame) {
        Wh_Log(L"Error: taskbarFrame is null");
        TaskbarController_UpdateFrameHeight_Original(pThis);
        return;
    }
    FrameworkElement taskbarFrameElement = nullptr;
    ((IUnknown**)taskbarFrame)[1]->QueryInterface(
        winrt::guid_of<FrameworkElement>(),
        winrt::put_abi(taskbarFrameElement));
    if (!taskbarFrameElement) {
        Wh_Log(L"Error: taskbarFrameElement is null");
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
using TaskListButton_UpdateButtonPadding_t = void(WINAPI*)(void* pThis);
TaskListButton_UpdateButtonPadding_t
    TaskListButton_UpdateButtonPadding_Original;
void WINAPI TaskListButton_UpdateButtonPadding_Hook(void* pThis) {
    Wh_Log(L"> hasDynamicIconScaling=%d", g_hasDynamicIconScaling);
    if (!g_hasDynamicIconScaling || g_unloading) {
        TaskListButton_UpdateButtonPadding_Original(pThis);
        return;
    }
    double* iconHeight = nullptr;
    double prevIconHeight;
    if (size_t iconHeightOffset = GetIconHeightOffset()) {
        iconHeight = (double*)((BYTE*)pThis + iconHeightOffset);
        prevIconHeight = *iconHeight;
        double newIconHeight = g_smallIconSize ? 16 : 24;
        Wh_Log(L"Setting iconHeight: %f->%f", prevIconHeight, newIconHeight);
        *iconHeight = newIconHeight;
    }
    TaskListButton_UpdateButtonPadding_Original(pThis);
    if (iconHeight) {
        *iconHeight = prevIconHeight;
    }
}
using TaskListButton_OverlayIcon_t = void(WINAPI*)(void* pThis, void* param1);
TaskListButton_OverlayIcon_t TaskListButton_OverlayIcon_Original;
void WINAPI TaskListButton_OverlayIcon_Hook(void* pThis, void* param1) {
    Wh_Log(L"> hasDynamicIconScaling=%d", g_hasDynamicIconScaling);
    if (!g_hasDynamicIconScaling || g_unloading) {
        TaskListButton_OverlayIcon_Original(pThis, param1);
        return;
    }
    double* iconHeight = nullptr;
    double prevIconHeight;
    if (size_t iconHeightOffset = GetIconHeightOffset()) {
        iconHeight = (double*)((BYTE*)pThis + iconHeightOffset);
        prevIconHeight = *iconHeight;
        double newIconHeight = 24;
        Wh_Log(L"Setting iconHeight: %f->%f", prevIconHeight, newIconHeight);
        *iconHeight = newIconHeight;
    }
    TaskListButton_OverlayIcon_Original(pThis, param1);
    if (iconHeight) {
        *iconHeight = prevIconHeight;
    }
}
using TaskListButton_UpdateBadge_t = void(WINAPI*)(void* pThis);
TaskListButton_UpdateBadge_t TaskListButton_UpdateBadge_Original;
void WINAPI TaskListButton_UpdateBadge_Hook(void* pThis) {
    Wh_Log(L"> hasDynamicIconScaling=%d", g_hasDynamicIconScaling);
    if (!g_hasDynamicIconScaling || g_unloading) {
        TaskListButton_UpdateBadge_Original(pThis);
        return;
    }
    double* iconHeight = nullptr;
    double prevIconHeight;
    if (size_t iconHeightOffset = GetIconHeightOffset()) {
        iconHeight = (double*)((BYTE*)pThis + iconHeightOffset);
        prevIconHeight = *iconHeight;
        double newIconHeight = 24;
        Wh_Log(L"Setting iconHeight: %f->%f", prevIconHeight, newIconHeight);
        *iconHeight = newIconHeight;
    }
    TaskListButton_UpdateBadge_Original(pThis);
    if (iconHeight) {
        *iconHeight = prevIconHeight;
    }
}
void* TaskListButton_UpdateIconColumnDefinition_Original;
LONG GetMediumTaskbarButtonExtentOffset() {
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
                return (offset < 0 || offset > 0xFFFF) ? 0 : offset;
            }
            if (p[0] == 0xF2 && p[1] == 0x44 && p[2] == 0x0F && p[3] == 0x10 &&
                (p[4] & 0x81) == 0x81) {
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
                if (!std::regex_match(s1.begin(), s1.end(), match1, regex2) ||
                    match1[1] != reg1) {
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
    GetMediumTaskbarButtonExtentOffset();
}
using TaskListButton_UpdateVisualStates_t = void(WINAPI*)(void* pThis);
TaskListButton_UpdateVisualStates_t TaskListButton_UpdateVisualStates_Original;
void WINAPI TaskListButton_UpdateVisualStates_Hook(void* pThis) {
    if (TaskListButton_UpdateIconColumnDefinition_Original &&
        (g_applyingSettings || g_taskbarButtonWidthCustomized)) {
        LONG mediumTaskbarButtonExtentOffset =
            GetMediumTaskbarButtonExtentOffset();
        if (mediumTaskbarButtonExtentOffset) {
            bool updateButtonPadding = false;
            double* mediumTaskbarButtonExtent =
                (double*)((BYTE*)pThis + mediumTaskbarButtonExtentOffset);
            if (*mediumTaskbarButtonExtent >= 1 &&
                *mediumTaskbarButtonExtent < 10000) {
                double newValue =
                    g_unloading ? 44 : g_settings_tbiconsize.taskbarButtonWidth;
                if (newValue != *mediumTaskbarButtonExtent) {
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
                double newValue =
                    g_unloading ? 32 : g_settings_tbiconsize.taskbarButtonWidthSmall;
                if (newValue != *smallTaskbarButtonExtent) {
                    Wh_Log(
                        L"Updating SmallTaskbarButtonExtent for "
                        L"TaskListButton: %f->%f",
                        *smallTaskbarButtonExtent, newValue);
                    *smallTaskbarButtonExtent = newValue;
                    updateButtonPadding = true;
                }
            }
            if (updateButtonPadding) {
                g_taskbarButtonWidthCustomized = true;
                TaskListButton_UpdateButtonPadding_Hook(pThis);
            }
        } else {
            Wh_Log(L"Error: mediumTaskbarButtonExtentOffset is invalid");
        }
    }
    TaskListButton_UpdateVisualStates_Original(pThis);
    if (g_applyingSettings && !g_hasDynamicIconScaling) {
        FrameworkElement taskListButtonElement = nullptr;
        ((IUnknown*)pThis + 3)
            ->QueryInterface(winrt::guid_of<FrameworkElement>(),
                             winrt::put_abi(taskListButtonElement));
        if (taskListButtonElement) {
            if (auto iconPanelElement =
                    FindChildByName(taskListButtonElement, L"IconPanel")) {
                if (auto iconElement =
                        FindChildByName(iconPanelElement, L"Icon")) {
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
    Wh_Log(L"> iconHeight=%f", iconHeight);
    g_smallIconSize = iconHeight == g_settings_tbiconsize.iconSizeSmall &&
                      iconHeight != g_settings_tbiconsize.iconSize;
    LaunchListItemViewModel_IconHeight_Original(pThis, iconHeight);
}
using ExperienceToggleButton_UpdateButtonPadding_t = void(WINAPI*)(void* pThis);
ExperienceToggleButton_UpdateButtonPadding_t
    ExperienceToggleButton_UpdateButtonPadding_Original;
void WINAPI ExperienceToggleButton_UpdateButtonPadding_Hook(void* pThis) {
    ExperienceToggleButton_UpdateButtonPadding_Original(pThis);
    if (g_hasDynamicIconScaling && g_unloading) {
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
    double defaultWidthExtra = -4;
    auto className = winrt::get_class_name(toggleButtonElement);
    if (className == L"Taskbar.ExperienceToggleButton") {
        auto automationId = Automation::AutomationProperties::GetAutomationId(
            toggleButtonElement);
        if (automationId == L"StartButton") {
            defaultWidthExtra = -3;
        }
    } else if (className == L"Taskbar.SearchBoxButton") {
        if (panelElement.Margin() != Thickness{}) {
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
    double defaultWidth = g_smallIconSize ? 32 : 44;
    double overrideWidth =
        g_unloading ? defaultWidth
                    : (g_smallIconSize ? g_settings_tbiconsize.taskbarButtonWidthSmall
                                       : g_settings_tbiconsize.taskbarButtonWidth);
    double newWidth = overrideWidth + buttonPadding.Left + buttonPadding.Right +
                      defaultWidthExtra;
    if (newWidth != buttonWidth) {
        Wh_Log(L"Updating MediumTaskbarButtonExtent for %s: %f->%f",
               className.c_str(), buttonWidth, newWidth);
        panelElement.Width(newWidth);
    }
}
using SearchButtonBase_UpdateButtonPadding_t = void(WINAPI*)(void* pThis);
SearchButtonBase_UpdateButtonPadding_t
    SearchButtonBase_UpdateButtonPadding_Original;
void WINAPI SearchButtonBase_UpdateButtonPadding_Hook(void* pThis) {
    SearchButtonBase_UpdateButtonPadding_Original(pThis);
    if (g_hasDynamicIconScaling && g_unloading) {
        return;
    }
    FrameworkElement toggleButtonElement = nullptr;
    ((IUnknown**)pThis)[1]->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(toggleButtonElement));
    if (!toggleButtonElement) {
        return;
    }
    auto panelElement =
        FindChildByName(toggleButtonElement, L"SearchBoxButtonRootPanel")
            .try_as<Controls::Grid>();
    if (!panelElement) {
        return;
    }
    if (FindChildByName(panelElement, L"SearchBoxTextBlock")) {
        return;
    }
    double buttonWidth = panelElement.Width();
    if (!(buttonWidth > 0)) {
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
    g_inAugmentedEntryPointButton_UpdateButtonPadding = true;
    AugmentedEntryPointButton_UpdateButtonPadding_Original(pThis);
    g_inAugmentedEntryPointButton_UpdateButtonPadding = false;
}
using RepeatButton_Width_t = void(WINAPI*)(void* pThis, double width);
RepeatButton_Width_t RepeatButton_Width_Original;
void WINAPI RepeatButton_Width_Hook(void* pThis, double width) {
    Wh_Log(L"> width=%f", width);
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
    double marginValue = static_cast<double>(40 - g_settings_tbiconsize.iconSize) / 2;
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
using SendMessageTimeoutW_t = decltype(&SendMessageTimeoutW);
SendMessageTimeoutW_t SendMessageTimeoutW_Original;
LRESULT WINAPI SendMessageTimeoutW_Hook(HWND hWnd,
                                        UINT Msg,
                                        WPARAM wParam,
                                        LPARAM lParam,
                                        UINT fuFlags,
                                        UINT uTimeout,
                                        PDWORD_PTR lpdwResult) {
    if (g_shellIconLoaderV2_LoadAsyncIcon__ResumeCoro_ThreadId ==
            GetCurrentThreadId() &&
        !g_unloading && Msg == WM_GETICON && wParam == ICON_BIG &&
        (g_smallIconSize ? g_settings_tbiconsize.iconSizeSmall : g_settings_tbiconsize.iconSize) <=
            16) {
        wParam = ICON_SMALL2;
    }
    LRESULT ret = SendMessageTimeoutW_Original(hWnd, Msg, wParam, lParam,
                                               fuFlags, uTimeout, lpdwResult);
    return ret;
}
void LoadSettingsTBIconSize() {
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
    HWND hTaskbarWnd = nullptr;
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD dwProcessId;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
                dwProcessId == GetCurrentProcessId() &&
                GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hTaskbarWnd));
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
void ApplySettingsTBIconSize(int taskbarHeight) {
    if (taskbarHeight < 2) {
        taskbarHeight = 2;
    }
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
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
        Wh_Log(L"HookSymbols failed");
        return false;
    }
    if (TaskListButton_IconHeight_Original) {
        TaskListButton_IconHeight_InitOffsets();
    }
#ifdef _M_ARM64
    if (TaskbarConfiguration_UpdateFrameSize_SymbolAddress) {
        TaskbarConfiguration_UpdateFrameSize_InitOffsets();
        WindhawkUtils::Wh_SetFunctionHookT(
            TaskbarConfiguration_UpdateFrameSize_SymbolAddress,
            TaskbarConfiguration_UpdateFrameSize_Hook,
            &TaskbarConfiguration_UpdateFrameSize_Original);
    }
#endif
    if (SystemTrayController_UpdateFrameSize_SymbolAddress) {
        SystemTrayController_UpdateFrameSize_InitOffsets();
        WindhawkUtils::Wh_SetFunctionHookT(
            SystemTrayController_UpdateFrameSize_SymbolAddress,
            SystemTrayController_UpdateFrameSize_Hook,
            &SystemTrayController_UpdateFrameSize_Original);
    }
    if (TaskbarController_OnGroupingModeChanged_Original) {
        TaskbarController_OnGroupingModeChanged_InitOffsets();
    }
    if (TaskListButton_UpdateIconColumnDefinition_Original) {
        TaskListButton_UpdateIconColumnDefinition_InitOffsets();
    }
    constexpr UINT kDynamicIconScaling = 29785184;
    if (TaskbarConfiguration_GetIconHeightInViewPixels_method_Original &&
        IsOsFeatureEnabled(kDynamicIconScaling).value_or(true)) {
        g_hasDynamicIconScaling = true;
        Wh_Log(L"Dynamic icon scaling is enabled");
    }
    return true;
}
bool HookSearchUxUiDllSymbols(HMODULE module) {
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
        Wh_Log(L"HookSymbols failed");
        return false;
    }
    return true;
}
bool HookTaskbarDllSymbolsTBIconSize() {
    HMODULE module =
        LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
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
HMODULE GetSearchUxUiModuleHandle() {
    return GetModuleHandle(L"SearchUx.UI.dll");
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
    if (!g_taskbarViewDllLoadedTBIconSize && GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoadedTBIconSize.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);
        if (HookTaskbarViewDllSymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }
    if (!g_searchUxUiDllLoaded && GetSearchUxUiModuleHandle() == module &&
        !g_searchUxUiDllLoaded.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);
        if (HookSearchUxUiDllSymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }
    return module;
}
BOOL Wh_ModInitTBIconSize() {
    LoadSettingsTBIconSize();
    if (!HookTaskbarDllSymbolsTBIconSize()) {
        return FALSE;
    }
    bool delayLoadingNeeded = false;
    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoadedTBIconSize = true;
        if (!HookTaskbarViewDllSymbols(taskbarViewModule)) {
            return FALSE;
        }
    } else {
        Wh_Log(L"Taskbar view module not loaded yet");
        delayLoadingNeeded = true;
    }
    if (HMODULE searchUxUiModule = GetSearchUxUiModuleHandle()) {
        g_searchUxUiDllLoaded = true;
        if (!HookSearchUxUiDllSymbols(searchUxUiModule)) {
            return FALSE;
        }
    } else {
        Wh_Log(L"Search UX UI module not loaded yet");
        delayLoadingNeeded = true;
    }
    if (delayLoadingNeeded) {
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
    if (!g_taskbarViewDllLoadedTBIconSize) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewDllLoadedTBIconSize.exchange(true)) {
                Wh_Log(L"Got Taskbar.View.dll");
                if (HookTaskbarViewDllSymbols(taskbarViewModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }
    if (!g_searchUxUiDllLoaded) {
        if (HMODULE searchUxUiModule = GetSearchUxUiModuleHandle()) {
            if (!g_searchUxUiDllLoaded.exchange(true)) {
                Wh_Log(L"Got SearchUx.UI.dll");
                if (HookSearchUxUiDllSymbols(searchUxUiModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }
    ApplySettingsTBIconSize(g_settings_tbiconsize.taskbarHeight);
}
void Wh_ModBeforeUninitTBIconSize() {
    g_unloading = true;
    ApplySettingsTBIconSize(g_originalTaskbarHeight ? g_originalTaskbarHeight : 48);
}
void Wh_ModUninitTBIconSize() {
    while (g_hookCallCounter > 0) {
        Sleep(100);
    }
}
void Wh_ModSettingsChangedTBIconSize() {
    LoadSettingsTBIconSize();
    ApplySettingsTBIconSize(g_settings_tbiconsize.taskbarHeight);
}