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
  float lastStartButtonAnchorLeft{0.0f};
  float lastStartButtonAnchorTop{0.0f};
  float lastStartButtonAnchorWidth{0.0f};
  float lastStartButtonAnchorHeight{0.0f};
  bool hasLastStartButtonAnchorRect{false};
  float stableStartButtonAnchorLeft{0.0f};
  float stableStartButtonAnchorTop{0.0f};
  float stableStartButtonAnchorWidth{0.0f};
  float stableStartButtonAnchorHeight{0.0f};
  bool hasStableStartButtonAnchorRect{false};
  int startButtonAnchorStablePasses{0};
  float lastRootWidth=0.0f;
  float lastTargetTaskFrameOffsetX=0.0f;
  bool hasLastTargetTaskFrameOffsetX{false};
  float lastTargetTaskbarIslandScale{1.0f};
  float lastTaskbarIslandScaleCenterX{0.0f};
  bool hasLastTargetTaskbarIslandScale{false};
  float lastObservedRootWidth{0.0f};
  float lastObservedRootHeight{0.0f};
  float lastObservedRasterizationScale{0.0f};
  float lastObservedTaskFrameWidth{0.0f};
  float lastObservedTaskFrameHeight{0.0f};
  bool hasLastDisplayGeometrySignature{false};
  float lastTargetTrayOffsetX{0.0f};
  bool hasLastTargetTrayOffsetX{false};
  float lastTargetWidgetOffsetX{0.0f};
  float lastTargetWidgetOffsetY{0.0f};
  bool hasLastTargetWidgetOffset{false};
  float lastLeftMostEdgeTray{0};
  int lastRightMostEdgeTray{0};
  float lastBackgroundShapeTargetWidth{0.0f};
  float lastBackgroundShapeTargetOffsetX{0.0f};
  float lastBackgroundShapeTargetOffsetY{0.0f};
  float backgroundAnimationFromWidth{0.0f};
  float backgroundAnimationToWidth{0.0f};
  float backgroundAnimationFromOffsetX{0.0f};
  float backgroundAnimationToOffsetX{0.0f};
  float backgroundAnimationFromOffsetY{0.0f};
  float backgroundAnimationToOffsetY{0.0f};
  int64_t backgroundAnimationStartMs{0};
  bool hasCustomTaskbarBackgroundVisuals{false};
};
static std::unordered_map<std::wstring, TaskbarState> g_taskbarStates;


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
