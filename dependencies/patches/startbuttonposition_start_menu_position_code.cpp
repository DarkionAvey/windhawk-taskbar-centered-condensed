float dpiScale = monitorDpiX / 96.0f;
float dpiScaleY = monitorDpiY / 96.0f;
if (windowDpiX != monitorDpiX) {
  cx = MulDiv(cx, monitorDpiX, windowDpiX);
}
if (windowDpiY != monitorDpiY) {
  cy = MulDiv(cy, monitorDpiY, windowDpiY);
}
const bool alignFlyoutInner = GetUserDefinedAlignFlyoutInner();
float absStartX = taskbarState.lastStartButtonXCalculated * dpiScale;
float absRootWidth = taskbarState.lastRootWidth * dpiScale;
float absTargetWidth = taskbarState.lastTargetWidth * dpiScale;
const int monitorLeft = monitorInfo.rcMonitor.left;
int taskbarAlignedY = y;
bool hasTaskbarAlignedY = TryCalculateFlyoutYAboveTaskbar(monitorInfo, cy, dpiScaleY, taskbarAlignedY);
Wh_Log(L"original: monitor=%s left=%d sourceDpi=%ux%u targetDpi=%ux%u taskbarState.lastLeftMostEdgeTray: %f, lastStartButtonXCalculated: %f g_lastRootWidth %f cx: %d, x:%d;cy: %d; y: %d; target:%d g_lastTargetWidth: %f, absStartX: %f; absRootWidth: %f; absTargetWidth: %f", monitorName.c_str(), monitorLeft, windowDpiX, windowDpiY, monitorDpiX, monitorDpiY, taskbarState.lastLeftMostEdgeTray, taskbarState.lastStartButtonXCalculated, taskbarState.lastRootWidth, cx, x, cy, y, target, taskbarState.lastTargetWidth, absStartX, absRootWidth, absTargetWidth);
const int flyoutInnerPaddingPx = GetFlyoutInnerPaddingPx(dpiScale);
if (target == DwmTarget::StartMenu) {
  const int recordedStartMenuWidthDip = Wh_GetIntValue(L"lastRecordedStartMenuWidth", g_lastRecordedStartMenuWidth);
  if (recordedStartMenuWidthDip > 0) {
    g_lastRecordedStartMenuWidth = recordedStartMenuWidthDip;
  }
  const int startMenuWidthPx =
      (g_lastRecordedStartMenuWidth > 0)
          ? static_cast<int>((g_lastRecordedStartMenuWidth * dpiScale) + 0.5f)
          : cx;
  if (g_settings_startbuttonposition.startMenuOnTheLeft && !g_unloading) {
    g_startMenuWnd = hwnd;
    g_startMenuOriginalWidth = cx;
    cx = startMenuWidthPx;
    constexpr int kStartMenuTransparentInsetDip = 12;
    const int startMenuTransparentInsetPx = static_cast<int>(
        (kStartMenuTransparentInsetDip * dpiScale) + 0.5f);
    int localX = static_cast<int>(
        absStartX -
        (alignFlyoutInner ? flyoutInnerPaddingPx
                          : (startMenuWidthPx / 2.0f))) +
        startMenuTransparentInsetPx;
    localX = std::max(
        -startMenuTransparentInsetPx,
        std::min(localX,
                 static_cast<int>(absRootWidth - startMenuWidthPx) +
                     startMenuTransparentInsetPx));
    x = monitorLeft + localX;
    if (hasTaskbarAlignedY) {
      y = taskbarAlignedY;
    }
  } else {
    if (g_startMenuOriginalWidth) {
      cx = g_startMenuOriginalWidth;
    }
    g_startMenuWnd = nullptr;
    g_startMenuOriginalWidth = 0;
    x = monitorLeft;
  }
} else if (target == DwmTarget::SearchHost) {
  if (g_settings_startbuttonposition.startMenuOnTheLeft && !g_unloading) {
    g_searchMenuWnd = hwnd;
    g_searchMenuOriginalX = x;
    int localX = static_cast<int>(absStartX - (alignFlyoutInner ? flyoutInnerPaddingPx : (cx / 2.0f)));
    localX = std::max(0, std::min(localX, static_cast<int>(absRootWidth - cx)));
    x = monitorLeft + localX;
    if (hasTaskbarAlignedY) {
      y = taskbarAlignedY;
    }
  } else {
    x = g_unloading && IsStartMenuOrbLeftAligned()
            ? g_searchMenuOriginalX
            : monitorLeft + static_cast<int>((absRootWidth - cx) / 2);
    g_searchMenuWnd = nullptr;
    g_searchMenuOriginalX = 0;
  }

} else if (target == DwmTarget::ShellExperienceHost) {
  int lastRecordedTrayRightMostEdgeForMonitor = taskbarState.lastRightMostEdgeTray;
  if (y != 0) {
    return original();
  }
  if (g_settings_startbuttonposition.MoveFlyoutNotificationCenter && !g_unloading) {
    int localX = static_cast<int>(lastRecordedTrayRightMostEdgeForMonitor * dpiScale - (alignFlyoutInner ? (cx - flyoutInnerPaddingPx) : (cx / 2.0f)));
    localX = std::max(0, std::min(localX, static_cast<int>(absRootWidth - cx)));
    x = monitorLeft + localX;
  } else {
    x = monitorLeft + static_cast<int>(absRootWidth - cx);
  }
}

Wh_Log(L"Recalc: monitor=%s taskbarState.lastLeftMostEdgeTray: %f, lastStartButtonXCalculated: %f g_lastRootWidth %f cx: %d, x:%d;cy: %d; y: %d; target:%d g_lastTargetWidth: %f, absStartX: %f; absRootWidth: %f; absTargetWidth: %f", monitorName.c_str(), taskbarState.lastLeftMostEdgeTray, taskbarState.lastStartButtonXCalculated, taskbarState.lastRootWidth, cx, x, cy, y, target, taskbarState.lastTargetWidth, absStartX, absRootWidth, absTargetWidth);
