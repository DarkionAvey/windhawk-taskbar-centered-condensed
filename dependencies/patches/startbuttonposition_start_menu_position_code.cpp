float dpiScale = monitorDpiX / 96.0f;
float dpiScaleY = monitorDpiY / 96.0f;
float absStartX = taskbarState.lastStartButtonXCalculated * dpiScale;
float absRootWidth = taskbarState.lastRootWidth * dpiScale;
float absTargetWidth = taskbarState.lastTargetWidth * dpiScale;
int taskbarAlignedY = y;
bool hasTaskbarAlignedY = TryCalculateFlyoutYAboveTaskbar(monitorInfo, cy, dpiScaleY, taskbarAlignedY);
Wh_Log(L"original: taskbarState.lastLeftMostEdgeTray: %f, lastStartButtonXCalculated: %f g_lastRootWidth %f cx: %d, x:%d;cy: %d; y: %d; target:%d g_lastTargetWidth: %f, absStartX: %f; absRootWidth: %f; absTargetWidth: %f", taskbarState.lastLeftMostEdgeTray, taskbarState.lastStartButtonXCalculated, taskbarState.lastRootWidth, cx, x, cy, y, target, taskbarState.lastTargetWidth, absStartX, absRootWidth, absTargetWidth);
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
    x = static_cast<int>(absRootWidth / 2.0f - absStartX - absTargetWidth + (g_settings.userDefinedAlignFlyoutInner ? startMenuWidthPx / 2.0f : 0.0f));
    x = std::min(0, std::max(static_cast<int>(((-absRootWidth + startMenuWidthPx) / 2.0f) + flyoutInnerPaddingPx), x));
     if (hasTaskbarAlignedY) {
      y = taskbarAlignedY;
    }
  } else {
    if (g_startMenuOriginalWidth) {
      cx = g_startMenuOriginalWidth;
    }
    g_startMenuWnd = nullptr;
    g_startMenuOriginalWidth = 0;
    x = 0;
  }
} else if (target == DwmTarget::SearchHost) {
  if (g_settings_startbuttonposition.startMenuOnTheLeft && !g_unloading) {
    g_searchMenuWnd = hwnd;
    g_searchMenuOriginalX = x;
    x = static_cast<int>(absStartX - (g_settings.userDefinedAlignFlyoutInner ? flyoutInnerPaddingPx : (cx / 2.0f)));
    x = std::max(0, std::min(x, static_cast<int>(absRootWidth - cx)));
    if (hasTaskbarAlignedY) {
      y = taskbarAlignedY;
    }
  } else {
    x = g_unloading && IsStartMenuOrbLeftAligned() ? g_searchMenuOriginalX : (absRootWidth - cx) / 2;
    g_searchMenuWnd = nullptr;
    g_searchMenuOriginalX = 0;
  }

} else if (target == DwmTarget::ShellExperienceHost) {
  int lastRecordedTrayRightMostEdgeForMonitor = taskbarState.lastRightMostEdgeTray;
  if (y != 0) {
    return original();
  }
  if (g_settings_startbuttonposition.MoveFlyoutNotificationCenter && !g_unloading) {
    x = static_cast<int>(lastRecordedTrayRightMostEdgeForMonitor * dpiScale - (g_settings.userDefinedAlignFlyoutInner ? (cx - flyoutInnerPaddingPx) : (cx / 2.0f)));
    x = std::max(0, std::min(x, static_cast<int>(absRootWidth - cx)));
  } else {
    x = static_cast<int>(absRootWidth - cx);
  }
}

Wh_Log(L"Recalc: taskbarState.lastLeftMostEdgeTray: %f, lastStartButtonXCalculated: %f g_lastRootWidth %f cx: %d, x:%d;cy: %d; y: %d; target:%d g_lastTargetWidth: %f, absStartX: %f; absRootWidth: %f; absTargetWidth: %f", taskbarState.lastLeftMostEdgeTray, taskbarState.lastStartButtonXCalculated, taskbarState.lastRootWidth, cx, x, cy, y, target, taskbarState.lastTargetWidth, absStartX, absRootWidth, absTargetWidth);