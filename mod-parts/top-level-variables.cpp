void ApplySettingsDebounced(int delayMs);
void ApplySettingsDebounced();
void ApplySettingsFromTaskbarThreadIfRequired();
void ApplySettingsFromTaskbarThreadImmediately();
void ApplySettingsFromTaskbarThreadGeometryChanged();
extern std::atomic<int> g_high_priority_dispatch_passes;
void RequestTaskbarButtonSizeRelayout();
void ArmInitialExplorerStyleApplyDelay();
void ScheduleInitialExplorerStyleApply();
bool g_invalidateDimensions =true;
int g_lastRecordedStartMenuWidth=0;
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


int GetFlyoutTaskbarBottomGapPx(float dpiScaleY) {
    if (g_unloading || g_settings.userDefinedFlatTaskbarBottomCorners ||
        g_settings.userDefinedFullWidthTaskbarBackground) {
        return 0;
    }
    int offsetY = static_cast<int>(g_settings.userDefinedTaskbarOffsetY);
    if (offsetY >= 0) {
        return 0;
    }
    return static_cast<int>((-offsetY * dpiScaleY) + 0.5f);
}

int GetFlyoutTaskbarHeightPx(float dpiScaleY) {
    int taskbarHeight = static_cast<int>(g_settings.userDefinedTaskbarHeight);
    if (taskbarHeight <= 0) {
        taskbarHeight = g_taskbarHeight > 0 ? g_taskbarHeight : kSystemMediumTaskbarButtonSize;
    }
    return std::max(1, static_cast<int>((taskbarHeight * dpiScaleY) + 0.5f));
}
int GetFlyoutInnerPaddingPx(float dpiScale) {
    if (dpiScale <= 0.0f) {
        dpiScale = 1.0f;
    }
    // Match the flyout inset to the visible taskbar island instead of keeping a
    // fixed 12 px offset. With the default 22 dip corner radius and 2 dip
    // horizontal padding this resolves to about 13 px, close to the old value,
    // but it scales with user styling and DPI.
    constexpr int kMaxFlyoutInnerPaddingDip = 32;
    const float logicalPadding =
        std::min<float>(kMaxFlyoutInnerPaddingDip,
                        static_cast<float>(g_settings.userDefinedTaskbarBackgroundHorizontalPadding) +
                            (g_settings.userDefinedTaskbarCornerRadius * 0.5f));
    return std::max(0, static_cast<int>((logicalPadding * dpiScale) + 0.5f));
}
bool IsVerticalTaskbar();
bool TryCalculateFlyoutYAboveTaskbar(const MONITORINFO& monitorInfo,
                                     int flyoutHeight,
                                     float dpiScaleY,
                                     int& y) {
    if (flyoutHeight <= 0 || dpiScaleY <= 0.0f || IsVerticalTaskbar()) {
        return false;
    }
    const int monitorTop = monitorInfo.rcMonitor.top;
    const int monitorBottom = monitorInfo.rcMonitor.bottom;
    if (monitorBottom <= monitorTop) {
        return false;
    }
    const int taskbarHeightPx = GetFlyoutTaskbarHeightPx(dpiScaleY);
    const int taskbarBottomGapPx = GetFlyoutTaskbarBottomGapPx(dpiScaleY);
    const int taskbarTop = monitorBottom - taskbarHeightPx - taskbarBottomGapPx;
    if (taskbarTop <= monitorTop) {
        return false;
    }
    y = taskbarTop - flyoutHeight;
    if (y < monitorTop) {
        y = monitorTop;
    }
    return true;
}
