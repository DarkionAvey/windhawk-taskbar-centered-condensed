using StartDocked__StartSizingFrame_UpdateWindowRegion_WithArgs_t = void(WINAPI*)(void* pThis, winrt::Windows::Foundation::Size param1);
StartDocked__StartSizingFrame_UpdateWindowRegion_WithArgs_t StartDocked__StartSizingFrame_UpdateWindowRegion_WithArgs_Original;
void WINAPI StartDocked__StartSizingFrame_UpdateWindowRegion_WithArgs_Hook(void* pThis, winrt::Windows::Foundation::Size param1) {
  StartDocked__StartSizingFrame_UpdateWindowRegion_WithArgs_Original(pThis, param1);
  Wh_Log(L"Method called: StartDocked__StartSizingFrame_UpdateWindowRegion (Width: %.2f, Height: %.2f)", param1.Width, param1.Height);
  const int measuredStartMenuWidth = static_cast<int>(param1.Width + 0.5f);
  if (measuredStartMenuWidth > 0 && g_lastRecordedStartMenuWidth != measuredStartMenuWidth) {
    g_lastRecordedStartMenuWidth = measuredStartMenuWidth;
    Wh_SetIntValue(L"lastRecordedStartMenuWidth", g_lastRecordedStartMenuWidth);
  }
}

std::atomic<int64_t> g_update_flag_set_time_ms = 0;
int64_t NowMs() { return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count(); }
void ResetFlagAfterDelay() {
  std::this_thread::sleep_for(std::chrono::milliseconds(kScheduledLowPriorityFlagTtlMs));
  int64_t now = NowMs();
  int64_t set_time = g_update_flag_set_time_ms.load();
  if (g_scheduled_low_priority_update && (now - set_time >= kScheduledLowPriorityFlagTtlMs)) {
    g_scheduled_low_priority_update = false;
  }
}

void ApplySettingsFromTaskbarThreadIfRequired() {
  if (!g_unloading && DelayedApplyNowMs() < g_suppress_low_priority_apply_until_ms.load()) {
    return;
  }
  if (!g_scheduled_low_priority_update.exchange(true)) {
    g_update_flag_set_time_ms = NowMs();
    Wh_Log(L"Scheduled low priority update");
    ApplySettingsDebounced(-1);
  }
}

void SetDividerForElement(FrameworkElement const& element, float const& panelHeight, bool dividerVisible, bool dividerShouldBeOnLeft = g_settings.userDefinedDividerLeftAligned) {
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
  if (!g_settings.userDefinedStyleTrayArea) return;

  if (auto ControlCenterButton = FindChildByName(systemTrayFrameGrid, L"ControlCenterButton")) {
    if (auto innerGrid = FindChildByClassName(ControlCenterButton, L"Windows.UI.Xaml.Controls.Grid")) {
      if (auto ContentPresenter = FindChildByName(innerGrid, L"ContentPresenter")) {
        if (auto innerItemPresenter = FindChildByClassName(ContentPresenter, L"Windows.UI.Xaml.Controls.ItemsPresenter")) {
          if (auto innerStackPanel = FindChildByClassName(innerItemPresenter, L"Windows.UI.Xaml.Controls.StackPanel")) {
            auto userDefinedTrayIconSizeStr = std::to_wstring(g_settings.userDefinedTrayIconSize);

            int childCount = Media::VisualTreeHelper::GetChildrenCount(innerStackPanel);
            for (int i = 0; i < childCount; ++i) {
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
                if (auto InnerTextBlock = FindChildByName(Layer, L"InnerTextBlock")) {
                  SetElementPropertyFromString(InnerTextBlock, L"Windows.UI.Xaml.Controls.TextBlock", L"FontSize", userDefinedTrayIconSizeStr);
                }
              }

              if (auto Layer = FindChildByName(ContainerGridInner, L"Base")) {
                if (auto InnerTextBlock = FindChildByName(Layer, L"InnerTextBlock")) {
                  SetElementPropertyFromString(InnerTextBlock, L"Windows.UI.Xaml.Controls.TextBlock", L"FontSize", userDefinedTrayIconSizeStr);
                }
              }

              if (auto Layer = FindChildByName(ContainerGridInner, L"AccentOverlay")) {
                if (auto InnerTextBlock = FindChildByName(Layer, L"InnerTextBlock")) {
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
  if (!g_settings.userDefinedStyleTrayArea) return;

  auto userDefinedTaskButtonCornerRadius = std::to_wstring(g_settings.userDefinedTaskButtonCornerRadius);
  int childCount = Media::VisualTreeHelper::GetChildrenCount(stackPanel);
  for (int i = 0; i < childCount; ++i) {
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
      child.Width(g_settings.userDefinedTrayButtonSize);
      child.Height(g_settings.userDefinedTaskbarHeight);

      SetElementPropertyFromString(containerGrid, L"Windows.UI.Xaml.Controls.Grid", L"CornerRadius", userDefinedTaskButtonCornerRadius);

      image.Width(g_settings.userDefinedTrayIconSize);
      image.Height(g_settings.userDefinedTrayIconSize);
    }
  }
}
void StyleNativeDividerElement(winrt::Windows::UI::Xaml::FrameworkElement const& element) {
  if (!element) return;
  using namespace winrt::Windows::UI::Xaml::Hosting;
  using namespace winrt::Windows::Foundation::Numerics;

  element.Opacity(g_unloading ? 1.0f : std::min(1.0f, static_cast<float>(g_settings.userDefinedTaskbarBorderOpacity / 255.0f)));
  element.Width(std::max(0.0, g_settings.userDefinedAppsDividerThickness * 0.99));

  if (auto visual = ElementCompositionPreview::GetElementVisual(element)) {
    if (auto compositor = visual.Compositor()) {
      visual.CenterPoint({0.0f, static_cast<float>(element.ActualHeight()) / 2.0f, 0.0f});
      visual.Scale({1.0f, g_unloading ? 1.0f : g_settings.userDefinedAppsDividerVerticalScale, 1.0f});
    }
  }

  PCWSTR hex = Wh_GetStringSetting(L"TaskbarBorderColorHex");
  PCWSTR originalHex = hex;
  if (!hex || *hex == L'\0') {
    hex = L"#ffffff";
    originalHex = nullptr;
  }

  if (*hex == L'#') ++hex;

  std::wstring fillBrush = L"<SolidColorBrush Color=\"#" + std::wstring(hex) + L"\"/>";
  SetElementPropertyFromString(element, L"Windows.UI.Xaml.Shapes.Rectangle", L"Fill", fillBrush.c_str(), true);

  if (originalHex) {
    Wh_FreeStringSetting(originalHex);
  }
}
void InvalidateTaskbarButtonLayoutElement(FrameworkElement const& element) {
  if (!element) {
    return;
  }
  try {
    element.InvalidateMeasure();
    element.InvalidateArrange();
  } catch (...) {
  }
}

double GetEffectiveTaskbarButtonTargetWidth() {
  const double defaultWidth = g_smallIconSize
                                  ? kSystemSmallTaskbarButtonSize
                                  : kSystemMediumTaskbarButtonSize;
  return g_unloading
             ? defaultWidth
             : static_cast<double>(g_smallIconSize
                                       ? g_settings_tbiconsize.taskbarButtonWidthSmall
                                       : g_settings_tbiconsize.taskbarButtonWidth);
}

bool IsExpectedTaskbarButtonDimension(double value, double expected) {
  if (!std::isfinite(value) || value <= 0.0) {
    return false;
  }
  return std::abs(value - expected) <= kLayoutToleranceDip;
}

bool EnsureElementTaskbarButtonWidth(FrameworkElement const& element,
                                     double targetWidth,
                                     bool allowHardWidth) {
  if (!element) {
    return false;
  }

  bool changed = false;
  try {
    if (!IsExpectedTaskbarButtonDimension(element.MinWidth(), targetWidth)) {
      element.MinWidth(targetWidth);
      changed = true;
    }

    if (allowHardWidth) {
      if (!IsExpectedTaskbarButtonDimension(element.MaxWidth(), targetWidth)) {
        element.MaxWidth(targetWidth);
        changed = true;
      }

      const double currentWidth = element.Width();
      const double actualWidth = element.ActualWidth();
      const bool explicitWidthWrong =
          !std::isfinite(currentWidth) ||
          !IsExpectedTaskbarButtonDimension(currentWidth, targetWidth);
      const bool actualWidthWrong =
          actualWidth > 0.0 &&
          !IsExpectedTaskbarButtonDimension(actualWidth, targetWidth);

      if (explicitWidthWrong || actualWidthWrong) {
        element.Width(targetWidth);
        changed = true;
      }
    }

    if (changed) {
      InvalidateTaskbarButtonLayoutElement(element);
    }
  } catch (...) {
  }

  return changed;
}
bool IsStartButtonElement(FrameworkElement const& child,
                          winrt::hstring const& className) {
  if (!child || className != L"Taskbar.ExperienceToggleButton") {
    return false;
  }
  try {
    return Automation::AutomationProperties::GetAutomationId(child) == L"StartButton";
  } catch (...) {
    return false;
  }
}

bool ApplyTaskbarButtonSizeToElement(FrameworkElement const& child,
                                     winrt::hstring const& className) {
  if (!child) {
    return false;
  }

  const bool isTaskbarButton =
      className == L"Taskbar.TaskListButton" ||
      className == L"Taskbar.ExperienceToggleButton" ||
      className == L"Taskbar.OverflowToggleButton" ||
      className == L"Taskbar.SearchBoxButton";
  if (!isTaskbarButton) {
    return false;
  }

  const double targetWidth = static_cast<double>(g_settings.userDefinedTaskbarButtonSize);
  const bool allowHardWidth =
      className != L"Taskbar.SearchBoxButton" ||
      !FindChildByName(child, L"SearchBoxTextBlock");
  bool changed = EnsureElementTaskbarButtonWidth(child, targetWidth, allowHardWidth);

  FrameworkElement innerElementChild = nullptr;
  if (className == L"Taskbar.SearchBoxButton") {
    innerElementChild = FindChildByClassName(child, L"Taskbar.TaskListButtonPanel");
    if (!innerElementChild) {
      innerElementChild = FindChildByName(child, L"SearchBoxButtonRootPanel");
    }
  } else if (className == L"Taskbar.TaskListButton") {
    innerElementChild = FindChildByClassName(child, L"Taskbar.TaskListLabeledButtonPanel");
    if (!innerElementChild) {
      innerElementChild = FindChildByClassName(child, L"Taskbar.TaskListButtonPanel");
    }
  } else if (className == L"Taskbar.ExperienceToggleButton") {
    innerElementChild = FindChildByName(child, L"ExperienceToggleButtonRootPanel");
  } else if (className == L"Taskbar.OverflowToggleButton") {
    innerElementChild = FindChildByName(child, L"OverflowToggleButtonRootPanel");
  } else {
    innerElementChild = FindChildByClassName(child, L"Taskbar.TaskListButtonPanel");
  }

  if (innerElementChild) {
    changed = EnsureElementTaskbarButtonWidth(innerElementChild, targetWidth, allowHardWidth) || changed;
  }

  return changed;
}

double CalculateValidChildrenWidth(FrameworkElement element,
                                   int& childrenCount,
                                   TaskbarState& state,
                                   double* leftMostEdge = nullptr,
                                   double* rightMostEdge = nullptr,
                                   FrameworkElement boundsRelativeTo = nullptr,
                                   double* startButtonLeft = nullptr,
                                   double* startButtonTop = nullptr,
                                   double* startButtonWidth = nullptr,
                                   double* startButtonHeight = nullptr) {
  if (!element) {
    if (leftMostEdge) *leftMostEdge = 0.0;
    if (rightMostEdge) *rightMostEdge = 0.0;
       if (startButtonLeft) *startButtonLeft = 0.0;
    if (startButtonTop) *startButtonTop = 0.0;
    if (startButtonWidth) *startButtonWidth = 0.0;
    if (startButtonHeight) *startButtonHeight = 0.0;
    return 0.0;
  }
  const float tbHeightFloat = static_cast<float>(g_settings.userDefinedTaskbarHeight);
  auto userDefinedTaskButtonCornerRadius = std::to_wstring(g_settings.userDefinedTaskButtonCornerRadius);
  auto userDefinedTaskbarIconSize = std::to_wstring(g_settings.userDefinedTaskbarIconSize);
  double totalWidth = 0.0;
  double minEdge = std::numeric_limits<double>::infinity();
  double maxEdge = -std::numeric_limits<double>::infinity();
    double startLeft = 0.0;
  double startTop = 0.0;
  double startWidth = 0.0;
  double startHeight = 0.0;
  bool foundStartButton = false;
  auto transformTarget = boundsRelativeTo ? boundsRelativeTo : element;

  childrenCount = 0;
  int childrenCountTentative = Media::VisualTreeHelper::GetChildrenCount(element);
  for (int i = 0; i < childrenCountTentative; i++) {
    auto child = Media::VisualTreeHelper::GetChild(element, i).try_as<FrameworkElement>();
    if (!child) {
      Wh_Log(L"Failed to get child %d of %d", i + 1, childrenCountTentative);
      continue;
    }
    auto className = winrt::get_class_name(child);
    const bool taskbarButtonSizeChanged = ApplyTaskbarButtonSizeToElement(child, className);
    winrt::Windows::Foundation::Rect rect{};
    try {
      if (taskbarButtonSizeChanged) {
        child.UpdateLayout();
      }
     auto transform = child.TransformToVisual(transformTarget);
      rect = transform.TransformBounds(
          winrt::Windows::Foundation::Rect(0, 0, child.ActualWidth(), child.ActualHeight()));
    } catch (...) {
      // A taskbar child can be present in the tree while being recycled during
      // overflow transitions. Skip the transient child for this pass.
      continue;
    }
    // Exclude recycled/transient rectangles, but don't assume that the valid
    // task button group is centered inside TaskbarFrameRepeater. Near overflow,
    // Explorer intentionally lays it out off-center before the overflow button
    // appears, so we measure the real bounds below and translate from them.
    if (rect.Width <= 0.0 || rect.Height <= 0.0 || rect.X < -kLayoutToleranceDip || rect.Y < -kLayoutToleranceDip) {
      continue;
    }
    SetElementPropertyFromString(child, className.c_str(), L"CornerRadius", userDefinedTaskButtonCornerRadius);
    if (className == L"Taskbar.SearchBoxButton") {
      // Search only needs the common sizing helper above.
    } else if (className == L"Taskbar.ExperienceToggleButton") {
       if (IsStartButtonElement(child, className)) {
        startLeft = rect.X;
        startTop = rect.Y;
        startWidth = rect.Width;
        startHeight = rect.Height;
        foundStartButton = true;
        state.lastStartButtonXActual = static_cast<float>(rect.X);
      }
    } else if (className == L"Taskbar.TaskListButton") {
      auto innerElementChild = FindChildByClassName(child, L"Taskbar.TaskListLabeledButtonPanel");
      if (innerElementChild) {
        auto iconElementChild = FindChildByName(innerElementChild, L"Icon");
        if (iconElementChild) {
          iconElementChild.Width(g_settings.userDefinedTaskbarIconSize);
          iconElementChild.Height(g_settings.userDefinedTaskbarIconSize);
          auto currentIconAppName = child.GetValue(winrt::Windows::UI::Xaml::Automation::AutomationProperties::NameProperty());
          const std::wstring currentIconAppNameStr = winrt::unbox_value<winrt::hstring>(currentIconAppName).c_str();
          // Wh_Log(L"bbwi: %s", currentIconAppNameStr);
          SetDividerForElement(child, tbHeightFloat, false);
          if (!currentIconAppNameStr.empty()) {
            for (const auto& pat : g_settings.userDefinedDividedAppNames) {
              if (RegexMatchInsensitive(currentIconAppNameStr, pat)) {
                SetDividerForElement(child, tbHeightFloat, true);
                break;
              }
            }
          }
        }
      }
    } else if (className == L"Taskbar.AugmentedEntryPointButton") {  // widget element
      child.Margin(Thickness{0, 0, 0, 0});
      auto ExperienceToggleButtonRootPanelElement = FindChildByName(child, L"ExperienceToggleButtonRootPanel");
      if (ExperienceToggleButtonRootPanelElement) {
        ExperienceToggleButtonRootPanelElement.Margin(Thickness{0, 0, 0, 0});
      }
      continue;
    } else if (className == L"Taskbar.OverflowToggleButton") {  // overflow button
      if (auto OverflowToggleButtonRootPanel = FindChildByName(child, L"OverflowToggleButtonRootPanel")) {
        if (auto RightOverflowButtonDivider = FindChildByName(OverflowToggleButtonRootPanel, L"RightOverflowButtonDivider")) {
          if (g_settings.userDefinedTrayAreaDivider) {
            RightOverflowButtonDivider.Opacity(0);
          } else {
            StyleNativeDividerElement(RightOverflowButtonDivider);
          }
        }
      }
    }
    if (auto iconPanelElement = FindChildByName(child, L"IconPanel")) {
      if (auto mostRecentlyUsedDivider = FindChildByName(iconPanelElement, L"MostRecentlyUsedDivider")) {
        StyleNativeDividerElement(mostRecentlyUsedDivider);
      }
      if (auto progressIndicator = FindChildByName(iconPanelElement, L"ProgressIndicator")) {
        if (auto layoutRoot = FindChildByName(progressIndicator, L"LayoutRoot")) {
          if (auto progressBarRoot = FindChildByName(layoutRoot, L"ProgressBarRoot")) {
            if (auto border = FindChildByClassName(progressBarRoot, L"Windows.UI.Xaml.Controls.Border")) {
              if (auto grid = FindChildByClassName(border, L"Windows.UI.Xaml.Controls.Grid")) {
                grid.Height(3.8);
                if (auto progressBarTrack = FindChildByName(grid, L"ProgressBarTrack")) {
                  progressBarTrack.Opacity(0.5);
                }
              }
            }
          }
        }
      } else if (auto runningIndicator = FindChildByName(iconPanelElement, L"RunningIndicator")) {
        runningIndicator.Height(3.5);
        runningIndicator.Opacity(1);
      }
    }
    totalWidth += rect.Width;
     minEdge = std::min(static_cast<double>(minEdge),static_cast<double>( rect.X));
    maxEdge = std::max(static_cast<double>(maxEdge), static_cast<double>(rect.X + rect.Width));

    childrenCount++;
  }
  if (childrenCount > 0 && minEdge != std::numeric_limits<double>::infinity()) {
    if (leftMostEdge) *leftMostEdge = minEdge;
    if (rightMostEdge) *rightMostEdge = maxEdge;
  } else {
    if (leftMostEdge) *leftMostEdge = 0.0;
    if (rightMostEdge) *rightMostEdge = 0.0;
    }
  if (foundStartButton && startWidth > 0.0 && startHeight > 0.0) {
    if (startButtonLeft) *startButtonLeft = startLeft;
    if (startButtonTop) *startButtonTop = startTop;
    if (startButtonWidth) *startButtonWidth = startWidth;
    if (startButtonHeight) *startButtonHeight = startHeight;
  } else {
    if (startButtonLeft) *startButtonLeft = 0.0;
    if (startButtonTop) *startButtonTop = 0.0;
    if (startButtonWidth) *startButtonWidth = 0.0;
    if (startButtonHeight) *startButtonHeight = 0.0;
  }
  return totalWidth;
}

bool TryGetStartButtonRectRelativeTo(FrameworkElement const& taskbarFrameRepeater,
                                     FrameworkElement const& relativeTo,
                                     winrt::Windows::Foundation::Rect& rect) {
  if (!taskbarFrameRepeater || !relativeTo) {
    return false;
  }
  int childrenCount = Media::VisualTreeHelper::GetChildrenCount(taskbarFrameRepeater);
  for (int i = 0; i < childrenCount; ++i) {
    auto child = Media::VisualTreeHelper::GetChild(taskbarFrameRepeater, i).try_as<FrameworkElement>();
    if (!child) {
      continue;
    }
    auto className = winrt::get_class_name(child);
    if (!IsStartButtonElement(child, className)) {
      continue;
    }
    try {
      auto transform = child.TransformToVisual(relativeTo);
      rect = transform.TransformBounds(
          winrt::Windows::Foundation::Rect(0, 0, child.ActualWidth(), child.ActualHeight()));
      return rect.Width > 0.0 &&
             rect.Height > 0.0 &&
             std::isfinite(rect.X) &&
             std::isfinite(rect.Y) &&
             rect.X > -kLayoutToleranceDip &&
             rect.Y > -kLayoutToleranceDip;
    } catch (...) {
      return false;
    }
  }
  return false;
}


void DisableElementClip(FrameworkElement const& element) {
  if (!element) {
    return;
  }
  try {
    element.Clip(nullptr);
  } catch (...) {
  }
}

void DisableClipForAncestorChain(FrameworkElement element, int maxDepth = 8) {
  for (int i = 0; element && i < maxDepth; ++i) {
    DisableElementClip(element);
    try {
      element = Media::VisualTreeHelper::GetParent(element).try_as<FrameworkElement>();
    } catch (...) {
      break;
    }
  }
}

bool SetVirtualLayoutWidth(FrameworkElement const& element, double width) {
  if (!element || !std::isfinite(width) || width <= 0.0) {
    return false;
  }

  bool changed = false;
  try {
    if (!IsExpectedTaskbarButtonDimension(element.Width(), width)) {
      element.Width(width);
      changed = true;
    }
    if (!IsExpectedTaskbarButtonDimension(element.MinWidth(), width)) {
      element.MinWidth(width);
      changed = true;
    }
    if (!IsExpectedTaskbarButtonDimension(element.MaxWidth(), width)) {
      element.MaxWidth(width);
      changed = true;
    }
    element.HorizontalAlignment(HorizontalAlignment::Left);
    DisableElementClip(element);
    if (changed) {
      element.InvalidateMeasure();
      element.InvalidateArrange();
    }
  } catch (...) {
  }
  return changed;
}
double CalculateDynamicTaskbarVirtualSurfaceWidth(FrameworkElement const& taskbarFrameRepeater,
                                                 double rootWidth,
                                                 bool isOverflowing) {
  if (g_unloading || !std::isfinite(rootWidth) || rootWidth <= 0.0) {
    return rootWidth;
  }

  int visualChildCount = 0;
  try {
    visualChildCount = Media::VisualTreeHelper::GetChildrenCount(taskbarFrameRepeater);
  } catch (...) {
    visualChildCount = 0;
  }

  const double buttonSize = std::max<double>(
      1.0, static_cast<double>(g_settings.userDefinedTaskbarButtonSize));
  const int reserveButtons = isOverflowing
      ? kTaskbarVirtualOverflowRecoveryButtonReserve
      : kTaskbarVirtualExtraButtonReserve;
  const double buttonReserveWidth =
      static_cast<double>(std::max(visualChildCount, 1) + reserveButtons) *
      buttonSize;
  const double screenReserveWidth =
      rootWidth * kTaskbarRepeaterVirtualWidthMultiplier;
  const double virtualWidth = std::max({rootWidth,
                                        screenReserveWidth,
                                        buttonReserveWidth});
  return std::isfinite(virtualWidth) && virtualWidth > rootWidth
      ? virtualWidth
      : rootWidth;
}
void ApplyVirtualTaskbarLayoutSurface(FrameworkElement const& xamlRootContent,
                                      FrameworkElement const& taskFrame,
                                      FrameworkElement const& rootGridTaskBar,
                                      FrameworkElement const& taskbarFrameRepeater,
                                      FrameworkElement const& taskbarBackground,
                                      FrameworkElement const& backgroundFillParent,
                                      FrameworkElement const& backgroundFillChild,
                                      double virtualWidth) {
  if (!std::isfinite(virtualWidth) || virtualWidth <= 0.0) {
    return;
  }

  // Keep the physical taskbar frame at the monitor width. Only the taskbar
  // content surface is virtualized. This gives ItemsRepeater more measure space
  // without making the whole taskbar window logically wider.
  DisableElementClip(xamlRootContent);
  DisableElementClip(taskFrame);
  DisableClipForAncestorChain(rootGridTaskBar);
  SetVirtualLayoutWidth(rootGridTaskBar, virtualWidth);
  SetVirtualLayoutWidth(taskbarFrameRepeater, virtualWidth);
  SetVirtualLayoutWidth(taskbarBackground, virtualWidth);
  SetVirtualLayoutWidth(backgroundFillParent, virtualWidth);
  SetVirtualLayoutWidth(backgroundFillChild, virtualWidth);
}

void SetTaskbarOverflowButtonSuppressed(FrameworkElement const& overflowButton, bool suppress) {
  if (!overflowButton) {
    return;
  }

  try {
    if (suppress) {
      overflowButton.Opacity(0.0);
      overflowButton.IsHitTestVisible(false);
      overflowButton.MinWidth(0.0);
      overflowButton.MaxWidth(0.0);
      overflowButton.Width(0.0);
      overflowButton.Clip(nullptr);
      overflowButton.InvalidateMeasure();
      overflowButton.InvalidateArrange();
      overflowButton.UpdateLayout();
    } else {
      overflowButton.Opacity(1.0);
      overflowButton.IsHitTestVisible(true);
      overflowButton.ClearValue(FrameworkElement::WidthProperty());
      overflowButton.ClearValue(FrameworkElement::MinWidthProperty());
      overflowButton.ClearValue(FrameworkElement::MaxWidthProperty());
    }
  } catch (...) {
  }
}


// ---- Temporary taskbar geometry diagnostics -------------------------------
// Set to false after collecting logs. The throttle keeps ApplyStyle from
// flooding Windhawk logs during insertion/removal animations.
constexpr bool kDebugTaskbarGeometry = true;
constexpr int64_t kDebugTaskbarGeometryMinIntervalMs = 250;
std::atomic<int64_t> g_lastTaskbarGeometryDebugLogMs = 0;

FrameworkElement FindStartButtonElement(FrameworkElement const& taskbarFrameRepeater) {
  if (!taskbarFrameRepeater) {
    return nullptr;
  }
  int childrenCount = Media::VisualTreeHelper::GetChildrenCount(taskbarFrameRepeater);
  for (int i = 0; i < childrenCount; ++i) {
    auto child = Media::VisualTreeHelper::GetChild(taskbarFrameRepeater, i).try_as<FrameworkElement>();
    if (!child) {
      continue;
    }
    auto className = winrt::get_class_name(child);
    if (IsStartButtonElement(child, className)) {
      return child;
    }
  }
  return nullptr;
}

bool ShouldLogTaskbarGeometry(bool interesting) {
  if constexpr (!kDebugTaskbarGeometry) {
    return false;
  }
  if (!interesting) {
    return false;
  }
  const int64_t nowMs = DelayedApplyNowMs();
  int64_t lastMs = g_lastTaskbarGeometryDebugLogMs.load();
  while (nowMs - lastMs >= kDebugTaskbarGeometryMinIntervalMs) {
    if (g_lastTaskbarGeometryDebugLogMs.compare_exchange_weak(lastMs, nowMs)) {
      return true;
    }
  }
  return false;
}

bool TryGetDebugBoundsRelativeTo(FrameworkElement const& element,
                                 winrt::Windows::UI::Xaml::UIElement const& relativeTo,
                                 winrt::Windows::Foundation::Rect& rect) {
  if (!element) {
    return false;
  }
  try {
    auto transform = element.TransformToVisual(relativeTo);
    rect = transform.TransformBounds(
        winrt::Windows::Foundation::Rect(0, 0, element.ActualWidth(), element.ActualHeight()));
    return std::isfinite(rect.X) &&
           std::isfinite(rect.Y) &&
           std::isfinite(rect.Width) &&
           std::isfinite(rect.Height);
  } catch (...) {
    return false;
  }
}

void LogDebugRect(PCWSTR label, bool ok, winrt::Windows::Foundation::Rect const& rect) {
  if (!ok) {
    Wh_Log(L"    %-18s unavailable", label);
    return;
  }
  Wh_Log(L"    %-18s X=%8.2f Y=%8.2f W=%8.2f H=%8.2f R=%8.2f B=%8.2f",
         label,
         rect.X,
         rect.Y,
         rect.Width,
         rect.Height,
         rect.X + rect.Width,
         rect.Y + rect.Height);
}

void LogElementGeometry(PCWSTR tag,
                        FrameworkElement const& element,
                        FrameworkElement const& rootGridTaskBar,
                        FrameworkElement const& taskbarFrameRepeater,
                        FrameworkElement const& taskFrame) {
  if (!element) {
    Wh_Log(L"[TBGEOM] %s: <null>", tag);
    return;
  }

  std::wstring className;
  std::wstring name;
  std::wstring automationId;
  try { className = std::wstring(winrt::get_class_name(element).c_str()); } catch (...) { className = L"<class?>"; }
  try { name = std::wstring(element.Name().c_str()); } catch (...) { name = L""; }
  try { automationId = std::wstring(Automation::AutomationProperties::GetAutomationId(element).c_str()); } catch (...) { automationId = L""; }

  auto parent = Media::VisualTreeHelper::GetParent(element).try_as<FrameworkElement>();
  std::wstring parentClass;
  std::wstring parentName;
  if (parent) {
    try { parentClass = std::wstring(winrt::get_class_name(parent).c_str()); } catch (...) { parentClass = L"<class?>"; }
    try { parentName = std::wstring(parent.Name().c_str()); } catch (...) { parentName = L""; }
  } else {
    parentClass = L"<null>";
  }

  Thickness margin{};
  winrt::Windows::Foundation::Numerics::float3 actualOffset{};
  try { margin = element.Margin(); } catch (...) {}
  try { actualOffset = element.ActualOffset(); } catch (...) {}

  winrt::Windows::Foundation::Numerics::float3 visualOffset{};
  winrt::Windows::Foundation::Numerics::float2 visualSize{};
  if (auto visual = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(element)) {
    visualOffset = visual.Offset();
    visualSize = visual.Size();
  }

  Wh_Log(L"[TBGEOM] %s class=%s name=%s aid=%s parent=%s/%s",
         tag,
         className.c_str(),
         name.c_str(),
         automationId.c_str(),
         parentClass.c_str(),
         parentName.c_str());
  Wh_Log(L"    size Actual=%8.2fx%-8.2f W=%8.2f H=%8.2f MinW=%8.2f MaxW=%8.2f Margin=(%.2f,%.2f,%.2f,%.2f)",
         element.ActualWidth(),
         element.ActualHeight(),
         element.Width(),
         element.Height(),
         element.MinWidth(),
         element.MaxWidth(),
         margin.Left,
         margin.Top,
         margin.Right,
         margin.Bottom);
  Wh_Log(L"    ActualOffset=(%.2f,%.2f,%.2f) VisualOffset=(%.2f,%.2f,%.2f) VisualSize=(%.2f,%.2f)",
         actualOffset.x,
         actualOffset.y,
         actualOffset.z,
         visualOffset.x,
         visualOffset.y,
         visualOffset.z,
         visualSize.x,
         visualSize.y);

  winrt::Windows::Foundation::Rect rect{};
  LogDebugRect(L"to-parent", parent ? TryGetDebugBoundsRelativeTo(element, parent, rect) : false, rect);
  LogDebugRect(L"to-repeater", taskbarFrameRepeater ? TryGetDebugBoundsRelativeTo(element, taskbarFrameRepeater, rect) : false, rect);
  LogDebugRect(L"to-rootGrid", rootGridTaskBar ? TryGetDebugBoundsRelativeTo(element, rootGridTaskBar, rect) : false, rect);
  LogDebugRect(L"to-taskFrame", taskFrame ? TryGetDebugBoundsRelativeTo(element, taskFrame, rect) : false, rect);
  LogDebugRect(L"to-xamlRoot", TryGetDebugBoundsRelativeTo(element, winrt::Windows::UI::Xaml::UIElement{nullptr}, rect), rect);
}

void LogAncestorGeometryChain(PCWSTR tag,
                              FrameworkElement element,
                              FrameworkElement const& rootGridTaskBar,
                              int maxDepth = 8) {
  Wh_Log(L"[TBGEOM] ancestor-chain for %s", tag);
  for (int depth = 0; element && depth < maxDepth; ++depth) {
    std::wstring className;
    std::wstring name;
    try { className = std::wstring(winrt::get_class_name(element).c_str()); } catch (...) { className = L"<class?>"; }
    try { name = std::wstring(element.Name().c_str()); } catch (...) { name = L""; }

    Thickness margin{};
    winrt::Windows::Foundation::Numerics::float3 actualOffset{};
    winrt::Windows::Foundation::Numerics::float3 visualOffset{};
    try { margin = element.Margin(); } catch (...) {}
    try { actualOffset = element.ActualOffset(); } catch (...) {}
    if (auto visual = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(element)) {
      visualOffset = visual.Offset();
    }

    winrt::Windows::Foundation::Rect rect{};
    const bool okRoot = rootGridTaskBar && TryGetDebugBoundsRelativeTo(element, rootGridTaskBar, rect);
    Wh_Log(L"    #%d class=%s name=%s Actual=%6.2fx%-6.2f ActualOffset=(%.2f,%.2f,%.2f) VisualOffset=(%.2f,%.2f,%.2f) Margin=(%.2f,%.2f,%.2f,%.2f) rootRect=%s X=%.2f Y=%.2f W=%.2f H=%.2f",
           depth,
           className.c_str(),
           name.c_str(),
           element.ActualWidth(),
           element.ActualHeight(),
           actualOffset.x,
           actualOffset.y,
           actualOffset.z,
           visualOffset.x,
           visualOffset.y,
           visualOffset.z,
           margin.Left,
           margin.Top,
           margin.Right,
           margin.Bottom,
           okRoot ? L"ok" : L"--",
           okRoot ? rect.X : 0.0,
           okRoot ? rect.Y : 0.0,
           okRoot ? rect.Width : 0.0,
           okRoot ? rect.Height : 0.0);

    element = Media::VisualTreeHelper::GetParent(element).try_as<FrameworkElement>();
  }
}

void LogTaskbarGeometryProbe(PCWSTR reason,
                             std::wstring const& monitorName,
                             FrameworkElement const& xamlRootContent,
                             FrameworkElement const& taskFrame,
                             FrameworkElement const& rootGridTaskBar,
                             FrameworkElement const& taskbarFrameRepeater,
                             FrameworkElement const& trayFrame,
                             FrameworkElement const& systemTrayFrameGrid,
                             FrameworkElement const& taskbarBackground,
                             FrameworkElement const& backgroundFillChild,
                             FrameworkElement const& startButton,
                             double rootWidth,
                             double childrenWidthTaskbarDbl,
                             double taskbarLeftEdge,
                             double taskbarRightEdge,
                             double startButtonLeft,
                             double startButtonTop,
                             double startButtonWidth,
                             double startButtonHeight,
                             double predictedCenteredTaskbarLeft,
                             bool isOverflowing,
                             bool taskbarLayoutIsEdgeClamped,
                             bool useStableStartButtonAnchor,
                             float targetContentLeft,
                             float targetContentWidth,
                             float targetTaskRootOffsetX,
                             float targetOffsetXTray,
                             float targetBackgroundLeftScreen,
                             float targetBackgroundRightScreen,
                             TaskbarState const& state) {
  Wh_Log(L"[TBGEOM] ===== %s monitor=%s overflow=%d edgeClamp=%d useStableStart=%d =====",
         reason,
         monitorName.c_str(),
         isOverflowing ? 1 : 0,
         taskbarLayoutIsEdgeClamped ? 1 : 0,
         useStableStartButtonAnchor ? 1 : 0);
  Wh_Log(L"[TBGEOM] scalars rootWidth=%.2f taskChildren=%.2f measuredEdges=[%.2f..%.2f] predictedLeft=%.2f",
         rootWidth,
         childrenWidthTaskbarDbl,
         taskbarLeftEdge,
         taskbarRightEdge,
         predictedCenteredTaskbarLeft);
  Wh_Log(L"[TBGEOM] startSample X=%.2f Y=%.2f W=%.2f H=%.2f stable=%d stableRect X=%.2f Y=%.2f W=%.2f H=%.2f stablePasses=%d",
         startButtonLeft,
         startButtonTop,
         startButtonWidth,
         startButtonHeight,
         state.hasStableStartButtonAnchorRect ? 1 : 0,
         state.stableStartButtonAnchorLeft,
         state.stableStartButtonAnchorTop,
         state.stableStartButtonAnchorWidth,
         state.stableStartButtonAnchorHeight,
         state.startButtonAnchorStablePasses);
  Wh_Log(L"[TBGEOM] targets contentLeft=%.2f contentWidth=%.2f rootOffsetX=%.2f trayOffsetX=%.2f bgScreen=[%.2f..%.2f]",
         targetContentLeft,
         targetContentWidth,
         targetTaskRootOffsetX,
         targetOffsetXTray,
         targetBackgroundLeftScreen,
         targetBackgroundRightScreen);

  LogElementGeometry(L"xamlRootContent", xamlRootContent, rootGridTaskBar, taskbarFrameRepeater, taskFrame);
  LogElementGeometry(L"taskFrame", taskFrame, rootGridTaskBar, taskbarFrameRepeater, taskFrame);
  LogElementGeometry(L"rootGridTaskBar", rootGridTaskBar, rootGridTaskBar, taskbarFrameRepeater, taskFrame);
  LogElementGeometry(L"repeater", taskbarFrameRepeater, rootGridTaskBar, taskbarFrameRepeater, taskFrame);
  LogElementGeometry(L"startButton", startButton, rootGridTaskBar, taskbarFrameRepeater, taskFrame);
  LogElementGeometry(L"trayFrame", trayFrame, rootGridTaskBar, taskbarFrameRepeater, taskFrame);
  LogElementGeometry(L"trayGrid", systemTrayFrameGrid, rootGridTaskBar, taskbarFrameRepeater, taskFrame);
  LogElementGeometry(L"taskbarBackground", taskbarBackground, rootGridTaskBar, taskbarFrameRepeater, taskFrame);
  LogElementGeometry(L"backgroundFill", backgroundFillChild, rootGridTaskBar, taskbarFrameRepeater, taskFrame);
  LogAncestorGeometryChain(L"startButton", startButton, rootGridTaskBar);
  LogAncestorGeometryChain(L"repeater", taskbarFrameRepeater, rootGridTaskBar);
  Wh_Log(L"[TBGEOM] ===== end =====");
}
void UpdateGlobalSettings() {
  auto getInt = [&](PCWSTR key) { return Wh_GetIntSetting(key); };
  auto clamp = [](int v, int lo, int hi) { return v < lo ? lo : v > hi ? hi : v; };
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
  int h = ClampInt(abs(ReadPositiveIntSettingOrDefault(L"TaskbarHeight", kDefaultTaskbarHeight)),
                   kMinTaskbarHeight,
                   kMaxTaskbarHeight);
  g_settings.userDefinedTaskbarHeight = g_unloading ? kSystemMediumTaskbarButtonSize : h;
  int taskbarButtonSize = ClampInt(abs(ReadPositiveIntSettingOrDefault(L"TaskbarButtonSize", kDefaultTaskbarButtonSize)),
                                   kMinTaskbarButtonSize,
                                   kMaxTaskbarButtonSize);
  g_settings.userDefinedTaskbarButtonSize = g_unloading ? kSystemMediumTaskbarButtonSize : taskbarButtonSize;
  const int maxTaskbarIconSize =
      GetMaxTaskbarIconSizeForLayout(static_cast<int>(g_settings.userDefinedTaskbarHeight),
                                     static_cast<int>(g_settings.userDefinedTaskbarButtonSize));
  int taskbarIconSize = ClampInt(abs(ReadPositiveIntSettingOrDefault(L"TaskbarIconSize", kDefaultTaskbarIconSize)),
                                 kMinTaskbarIconSize,
                                 maxTaskbarIconSize);
  g_settings.userDefinedTaskbarIconSize = g_unloading ? kSystemMediumTaskbarIconSize : taskbarIconSize;
  g_settings.userDefinedTrayIconSize = std::max(kMinTrayIconSize, getInt(L"TrayIconSize"));
  g_settings.userDefinedTrayButtonSize = std::max(kMinTrayButtonSize, getInt(L"TrayButtonSize"));
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
  g_settings.userDefinedTaskbarBackgroundBlurAmount = clamp(abs(getInt(L"TaskbarBackgroundBlurAmount")), 0, 100);
  g_settings.userDefinedTaskbarBackgroundTintSaturation = clamp(abs(getInt(L"TaskbarBackgroundTintSaturation")), 0, 500);
  g_settings.userDefinedTaskbarBackgroundInversion = clamp(abs(getInt(L"TaskbarBackgroundInversion")), 0, 100);
  PCWSTR bgTintColor = Wh_GetStringSetting(L"TaskbarBackgroundTintColor");
  g_settings.userDefinedTaskbarBackgroundTintColor = (bgTintColor && *bgTintColor) ? bgTintColor : L"{ThemeResource CardStrokeColorDefaultSolid}";
  if (bgTintColor) {
    Wh_FreeStringSetting(bgTintColor);
  }
  PCWSTR bgFallbackColor = Wh_GetStringSetting(L"TaskbarBackgroundFallbackColor");
  g_settings.userDefinedTaskbarBackgroundFallbackColor = (bgFallbackColor && *bgFallbackColor) ? bgFallbackColor : L"{ThemeResource CardStrokeColorDefaultSolid}";
  if (bgFallbackColor) {
    Wh_FreeStringSetting(bgFallbackColor);
  }
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
    hex = L"#ffffff";
    originalHex = nullptr;
  }
  if (*hex == L'#') ++hex;
  unsigned int r = 255, g = 255, b = 255;
  if (swscanf_s(hex, L"%02x%02x%02x", &r, &g, &b) != 3) {
    r = g = b = 255;
  }
  g_settings.borderColorR = r;
  g_settings.borderColorG = g;
  g_settings.borderColorB = b;
  if (originalHex) {
    Wh_FreeStringSetting(originalHex);
  }
  // String list
  PCWSTR dividerAppNames = Wh_GetStringSetting(L"DividedAppNames");
  g_settings.userDefinedDividedAppNames = SplitAndTrim(dividerAppNames);
  Wh_FreeStringSetting(dividerAppNames);
}
bool HasInvalidSettings() {
  if (g_settings.userDefinedTrayTaskGap < 0) return true;
  if (g_settings.userDefinedTaskbarBackgroundHorizontalPadding < 0) return true;
  if ((int)g_settings.userDefinedTaskbarOffsetY < 0 && !g_settings.userDefinedFlatTaskbarBottomCorners) return true;
  if (g_settings.userDefinedTaskbarHeight < kMinTaskbarHeight || g_settings.userDefinedTaskbarHeight > kMaxTaskbarHeight) return true;
  if (g_settings.userDefinedTaskbarIconSize <= 0) return true;
  if (g_settings.userDefinedTrayIconSize <= 0) return true;
  if (g_settings.userDefinedTaskbarButtonSize <= 0) return true;
  if (g_settings.userDefinedTrayButtonSize <= 0) return true;
  if (g_settings.userDefinedTaskbarCornerRadius < 0.0f || g_settings.userDefinedTaskbarCornerRadius > (g_settings.userDefinedTaskbarHeight / 2.0f)) return true;
  if (g_settings.userDefinedTaskButtonCornerRadius < 0 || g_settings.userDefinedTaskButtonCornerRadius > (g_settings.userDefinedTaskbarHeight / 2)) return true;
  if (g_settings.userDefinedTaskbarBackgroundOpacity > 100) return true;
  if (g_settings.userDefinedTaskbarBackgroundTint > 100) return true;
  if (g_settings.userDefinedTaskbarBackgroundLuminosity > 100) return true;
  if (g_settings.userDefinedTaskbarBackgroundBlurAmount > 100) return true;
  if (g_settings.userDefinedTaskbarBackgroundTintSaturation > 500) return true;
  if (g_settings.userDefinedTaskbarBackgroundInversion > 100) return true;
  if (g_settings.userDefinedTaskbarBorderOpacity > 255) return true;
  if (g_settings.userDefinedTaskbarBorderThickness < 0.0 || g_settings.userDefinedTaskbarBorderThickness > 10.0) return true;
  return false;
}
void LogAllSettings() {
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
  Wh_Log(L"setting %d %s", g_settings.userDefinedTaskbarBackgroundBlurAmount, L"userDefinedTaskbarBackgroundBlurAmount");
  Wh_Log(L"setting %s %s", g_settings.userDefinedTaskbarBackgroundTintColor.c_str(), L"userDefinedTaskbarBackgroundTintColor");
  Wh_Log(L"setting %d %s", g_settings.userDefinedTaskbarBackgroundTintSaturation, L"userDefinedTaskbarBackgroundTintSaturation");
  Wh_Log(L"setting %d %s", g_settings.userDefinedTaskbarBackgroundInversion, L"userDefinedTaskbarBackgroundInversion");
  Wh_Log(L"setting %s %s", g_settings.userDefinedTaskbarBackgroundFallbackColor.c_str(), L"userDefinedTaskbarBackgroundFallbackColor");
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
  if (!xamlRootContent) {
    Wh_Log(L"xamlRootContent is null");
    return false;
  }
  auto& state = g_taskbarStates[monitorName];
  Wh_Log(L"ApplyStyle for monitor: %s", monitorName.c_str());
  g_scheduled_low_priority_update = false;
  bool forceStyleApply = false;
  int forceStyleApplyPasses = g_force_style_apply_passes.load();
  while (forceStyleApplyPasses > 0) {
    if (g_force_style_apply_passes.compare_exchange_weak(forceStyleApplyPasses, forceStyleApplyPasses - 1)) {
      forceStyleApply = true;
      break;
    }
  }
  bool resetAnimationTargetsThisPass = false;
  int resetAnimationTargetPasses = g_reset_animation_target_passes.load();
  while (resetAnimationTargetPasses > 0) {
    if (g_reset_animation_target_passes.compare_exchange_weak(resetAnimationTargetPasses, resetAnimationTargetPasses - 1)) {
      resetAnimationTargetsThisPass = true;
      forceStyleApply = true;
      break;
    }
  }
  if (resetAnimationTargetsThisPass) {
    ResetAnimationTargetCache(state);
  }
  if (!xamlRootContent) return false;
  const float rasterizationScale = GetRasterizationScale(xamlRootContent);
  auto snapPx = [rasterizationScale](float value) -> float {
    return SnapToPhysicalPixel(value, rasterizationScale);
  };
  auto taskFrame = FindChildByClassName(xamlRootContent, L"Taskbar.TaskbarFrame");
  if (!taskFrame) {
    Wh_Log(L"Failed to find Taskbar.TaskbarFrame");
    return false;
  }
   const bool displayGeometryChanged = CheckAndUpdateDisplayGeometrySignature(
      state, xamlRootContent, taskFrame, rasterizationScale);
  if (displayGeometryChanged && !g_unloading) {
    Wh_Log(L"Taskbar display geometry changed; invalidating cached layout targets");
    ResetAnimationTargetCache(state);
    forceStyleApply = true;
    g_invalidateDimensions = true;
    if (HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd()) {
      ArmStyleFollowupPasses(hTaskbarWnd, true);
    }
  }
  auto now = std::chrono::steady_clock::now();
  if (!forceStyleApply && !g_unloading && now - state.lastApplyStyleTime < std::chrono::milliseconds(200)) {
    return true;
  }
  state.lastApplyStyleTime = now;
  auto rootGridTaskBar = FindChildByName(taskFrame, L"RootGrid");
  if (!rootGridTaskBar) {
    Wh_Log(L"Failed to find RootGrid in taskFrame");
    return false;
  }
  auto taskbarFrameRepeater = FindChildByName(rootGridTaskBar, L"TaskbarFrameRepeater");
  if (!taskbarFrameRepeater) {
    Wh_Log(L"Failed to find TaskbarFrameRepeater in rootGridTaskBar");
    return false;
  }
  auto trayFrame = FindChildByClassName(xamlRootContent, L"SystemTray.SystemTrayFrame");
  if (!trayFrame) {
    Wh_Log(L"Failed to find SystemTray.SystemTrayFrame");
    return false;
  }
  auto systemTrayFrameGrid = FindChildByName(trayFrame, L"SystemTrayFrameGrid");
  if (!systemTrayFrameGrid) {
    Wh_Log(L"Failed to find SystemTrayFrameGrid in trayFrame");
    return false;
  }
  auto showDesktopButton = FindChildByName(systemTrayFrameGrid, L"ShowDesktopStack");
  if (!showDesktopButton) {
    Wh_Log(L"Failed to find ShowDesktopStack in systemTrayFrameGrid");
    return false;
  }
  auto taskbarBackground = FindChildByClassName(rootGridTaskBar, L"Taskbar.TaskbarBackground");
  if (!taskbarBackground) {
    Wh_Log(L"Failed to find Taskbar.TaskbarBackground in rootGridTaskBar");
    return false;
  }
  auto backgroundFillParent = FindChildByClassName(taskbarBackground, L"Windows.UI.Xaml.Controls.Grid");
  if (!backgroundFillParent) {
    Wh_Log(L"Failed to find backgroundFillParent in taskbarBackground");
    return false;
  }
  auto backgroundFillChild = FindChildByName(backgroundFillParent, L"BackgroundFill");
  if (!backgroundFillChild) {
    Wh_Log(L"Failed to find BackgroundFill in backgroundFillParent");
    return false;
  }
  auto notificationAreaIcons = FindChildByName(systemTrayFrameGrid, L"NotificationAreaIcons");
  if (!notificationAreaIcons) {
    Wh_Log(L"Failed to find NotificationAreaIcons in systemTrayFrameGrid");
    return false;
  }
  auto itemsPresenter = FindChildByClassName(notificationAreaIcons, L"Windows.UI.Xaml.Controls.ItemsPresenter");
  if (!itemsPresenter) {
    Wh_Log(L"Failed to find ItemsPresenter in notificationAreaIcons");
    return false;
  }
  auto stackPanel = FindChildByClassName(itemsPresenter, L"Windows.UI.Xaml.Controls.StackPanel");
  if (!stackPanel) {
    Wh_Log(L"Failed to find StackPanel in itemsPresenter");
    return false;
  }
  bool widgetPresent = IsTaskbarWidgetsEnabled();
  auto widgetElement = widgetPresent ? FindChildByClassName(taskbarFrameRepeater, L"Taskbar.AugmentedEntryPointButton") : nullptr;
  auto widgetMainView = widgetElement ? FindChildByName(widgetElement, L"ExperienceToggleButtonRootPanel") : widgetElement;
  widgetPresent = widgetPresent && widgetMainView != nullptr;
  auto widgetElementWidth = widgetPresent && widgetMainView ? widgetMainView.ActualWidth() : 0;
  if (widgetPresent && widgetElementWidth <= 0) {
    Wh_Log(L"Error: widgetPresent && widgetElementWidth<=0");
    return false;
  }
  auto widgetElementInnerChild = widgetPresent ? FindChildByClassName(widgetElement, L"Taskbar.TaskListButtonPanel") : nullptr;
  auto widgetElementVisibleWidth = widgetElementInnerChild ? widgetElementInnerChild.ActualWidth() : 0;
  auto widgetElementVisibleHeight = widgetElementInnerChild ? widgetElementInnerChild.ActualHeight() : 0;
  if (widgetElementInnerChild && widgetElementVisibleWidth <= 0) {
    Wh_Log(L"Error: widgetElementInnerChild && widgetElementVisibleWidth<=0");
    return false;
  }
  if (widgetElementInnerChild && widgetElementVisibleHeight <= 0) {
    Wh_Log(L"Error: widgetElementInnerChild && widgetElementVisibleHeight<=0");
    return false;
  }
  auto overflowButton = FindChildByClassName(taskbarFrameRepeater, L"Taskbar.OverflowToggleButton");
  bool isOverflowing = overflowButton != nullptr && !IsWeirdFrameworkElement(overflowButton);
  double rootWidth = xamlRootContent.ActualWidth();
  state.lastRootWidth = static_cast<float>(rootWidth);
  const double minimumRootWidth =
      std::max<double>(1.0, static_cast<double>(g_settings.userDefinedTaskbarButtonSize));
  if (!g_unloading && rootWidth < minimumRootWidth) {
    Wh_Log(L"root width is too small");
    return false;
  }

   const double taskbarVirtualSurfaceWidth = g_unloading
      ? rootWidth
      : CalculateDynamicTaskbarVirtualSurfaceWidth(taskbarFrameRepeater,
                                                  rootWidth,
                                                  isOverflowing);

  const bool useVirtualTaskbarSurface =
      !g_unloading &&
      std::isfinite(taskbarVirtualSurfaceWidth) &&
      taskbarVirtualSurfaceWidth > rootWidth + kLayoutToleranceDip;

  if (useVirtualTaskbarSurface) {
    ApplyVirtualTaskbarLayoutSurface(xamlRootContent,
                                     taskFrame,
                                     rootGridTaskBar,
                                     taskbarFrameRepeater,
                                     taskbarBackground,
                                     backgroundFillParent,
                                     backgroundFillChild,
                                     taskbarVirtualSurfaceWidth);
    SetTaskbarOverflowButtonSuppressed(overflowButton, true);
    isOverflowing = false;
  } else {
    SetTaskbarOverflowButtonSuppressed(overflowButton, false);
  }

  const double taskbarLayoutSurfaceWidth = useVirtualTaskbarSurface
      ? std::max(rootWidth, taskbarFrameRepeater.ActualWidth())
      : rootWidth;
  int childrenCountTaskbar = 0;
  double taskbarLeftEdge = 0.0;
  double taskbarRightEdge = 0.0;
  double startButtonLeft = 0.0;
  double startButtonTop = 0.0;
  double startButtonWidth = 0.0;
  double startButtonHeight = 0.0;
  const double childrenWidthTaskbarDbl = CalculateValidChildrenWidth(
      taskbarFrameRepeater, childrenCountTaskbar, state, &taskbarLeftEdge, &taskbarRightEdge);
  winrt::Windows::Foundation::Rect startButtonAnchorRect{};
  auto startButtonElement = FindStartButtonElement(taskbarFrameRepeater);
  if (TryGetStartButtonRectRelativeTo(taskbarFrameRepeater, rootGridTaskBar, startButtonAnchorRect)) {
    startButtonLeft = startButtonAnchorRect.X;
    startButtonTop = startButtonAnchorRect.Y;
    startButtonWidth = startButtonAnchorRect.Width;
    startButtonHeight = startButtonAnchorRect.Height;
  }
  if (!g_unloading && childrenWidthTaskbarDbl <= 0) {
    Wh_Log(L"Error: childrenWidthTaskbarDbl <= 0");
    return false;
  }
  double actualTaskbarBoundsWidthDbl = taskbarRightEdge - taskbarLeftEdge;
  if (actualTaskbarBoundsWidthDbl <= 0.0) {
    actualTaskbarBoundsWidthDbl = childrenWidthTaskbarDbl;
    taskbarLeftEdge = (rootWidth - actualTaskbarBoundsWidthDbl) / 2.0;
    taskbarRightEdge = taskbarLeftEdge + actualTaskbarBoundsWidthDbl;
  }
  // Use the sum of valid child widths as the logical task width. The measured
  // min/max bounds are still useful as an anchor near overflow, but during
  // insert/remove animations they can include temporary gaps from recycled
  // views and make the island width overshoot.
  unsigned int childrenWidthTaskbar = static_cast<unsigned int>(childrenWidthTaskbarDbl + 0.5);
  signed int rightMostEdgeTaskbar = static_cast<signed int>(taskbarRightEdge + 0.5);
  if (!g_unloading && childrenCountTaskbar < 1) {
    Wh_Log(L"Error: childrenCountTaskbar < 1");
    return false;
  }
  const unsigned int minimumTaskbarChildrenWidth =
      static_cast<unsigned int>(std::max<double>(1.0, g_settings.userDefinedTaskbarButtonSize * 0.25));
  if (!g_unloading && childrenWidthTaskbar <= minimumTaskbarChildrenWidth) {
    Wh_Log(L"Error: childrenWidthTaskbar is too small");
    return false;
  }
  if (!g_unloading && rightMostEdgeTaskbar < 0) {
    Wh_Log(L"Error: rightMostEdgeTaskbar < 0");
    return false;
  }
  bool rightMostEdgeChangedTaskbar = (state.lastTaskbarData.rightMostEdge != rightMostEdgeTaskbar);
  if (isOverflowing != state.wasOverflowing) {
    ResetAnimationTargetCache(state);
    forceStyleApply = true;
  }
  if (rightMostEdgeChangedTaskbar || state.lastTaskbarData.rightMostEdge == 0.0 || isOverflowing) {
    state.lastTaskbarData.childrenCount = childrenCountTaskbar;
    state.lastTaskbarData.rightMostEdge = rightMostEdgeTaskbar;
    state.lastTaskbarData.childrenWidth = childrenWidthTaskbar;
  }
  trayFrame.Clip(nullptr);
    DisableElementClip(systemTrayFrameGrid);
  DisableClipForAncestorChain(systemTrayFrameGrid);
  if (trayFrame.GetValue(FrameworkElement::HorizontalAlignmentProperty()).as<winrt::Windows::Foundation::IReference<HorizontalAlignment>>().Value() == HorizontalAlignment::Center) {
    trayFrame.SetValue(FrameworkElement::HorizontalAlignmentProperty(), winrt::box_value(HorizontalAlignment::Right));
  }
  int childrenCountTray = 0;
  double trayFrameWidthDbl = CalculateValidChildrenWidth(systemTrayFrameGrid, childrenCountTray, state);
  if (!g_unloading && trayFrameWidthDbl <= 0) {
    Wh_Log(L"Error: trayFrameWidthDbl <= 0");
    return false;
  }
  if (!g_unloading && childrenCountTray <= 0) {
    Wh_Log(L"Error: childrenCountTray <= 0");
    return false;
  }
  const double showDesktopButtonActualWidth = showDesktopButton ? showDesktopButton.ActualWidth() : 0.0;
  if (g_settings.userDefinedIgnoreShowDesktopButton && showDesktopButtonActualWidth > 0.0) {
    trayFrameWidthDbl = std::max(0.0, trayFrameWidthDbl - showDesktopButtonActualWidth);
  }
  int trayGapPlusExtras = g_settings.userDefinedTrayTaskGap + widgetElementVisibleWidth + (widgetPresent ? -6 + g_settings.userDefinedTrayTaskGap : 0);
  const unsigned int trayFrameWidth = static_cast<unsigned int>(trayFrameWidthDbl + trayGapPlusExtras);
  if (!g_unloading && childrenCountTray == 0) {
    Wh_Log(L"Error: childrenCountTray == 0");
    return false;
  }
  if (!g_unloading && trayFrameWidth <= 1) {
    Wh_Log(L"Error: trayFrameWidth <= 1");
    return false;
  }
  const float targetContentWidth = static_cast<float>(childrenWidthTaskbar + trayFrameWidth);
  const float targetContentLeft = snapPx(static_cast<float>((rootWidth - targetContentWidth) / 2.0f));
  const float targetTrayLogicalLeft = snapPx(targetContentLeft + static_cast<float>(childrenWidthTaskbar));
  float targetOffsetXTray = snapPx(targetTrayLogicalLeft - static_cast<float>(rootWidth - trayFrameWidth));
  // tray animations
  auto systemTrayFrameGridVisual = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(systemTrayFrameGrid);
  if (!systemTrayFrameGridVisual) {
    Wh_Log(L"Error: !SystemTrayFrameGridVisual");
    return false;
  }
  auto originalOffset = systemTrayFrameGridVisual.Offset();
  if (state.initOffsetX == -1) {
    state.initOffsetX = originalOffset.x;
  }
  auto rootGridTaskBarVisual = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(rootGridTaskBar);
  if (!rootGridTaskBarVisual) {
    Wh_Log(L"Error: !rootGridTaskBarVisual");
    return false;
  }
  auto taskbarFrameRepeaterVisual = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(taskbarFrameRepeater);
  if (!taskbarFrameRepeaterVisual) {
    Wh_Log(L"Error: !taskbarFrameRepeaterVisual");
    return false;
  }
  const float visualOffsetTolerance =
      std::max<float>(static_cast<float>(kLayoutToleranceDip),
                      2.0f / GetRasterizationScale(xamlRootContent));
  // Move the stable RootGrid instead of TaskbarFrameRepeater. The repeater is
  // virtualized and can expose transient/recycled child offsets during insert
  // and remove animations, which makes compositor Offset animations overshoot.
  // RootGrid remains the authoritative visual transform; the repeater itself is
  // kept horizontally neutral below.
  //
  // The Start button rect is sampled in RootGrid coordinates, but it is not used
  // as a live target. During task insertion/removal, Explorer can animate the
  // repeater's internal layout; chasing that transient Start rect causes a
  // feedback loop where RootGrid keeps catching up to Windows' own animation.
  // Only promote the sampled rect to an anchor after it has been stable for a
  // couple of ApplyStyle passes. Until then, keep using the last stable anchor
  // or the edge-clamp fallback from the root-grid version.
  const bool hasStartButtonAnchorSample =
      startButtonWidth > 0.0 &&
      startButtonHeight > 0.0 &&
      std::isfinite(startButtonLeft) &&
      std::isfinite(startButtonTop);
  const bool startButtonSampleChanged =
      hasStartButtonAnchorSample &&
      (!state.hasLastStartButtonAnchorRect ||
       std::abs(state.lastStartButtonAnchorLeft - static_cast<float>(startButtonLeft)) > visualOffsetTolerance ||
       std::abs(state.lastStartButtonAnchorTop - static_cast<float>(startButtonTop)) > visualOffsetTolerance ||
       std::abs(state.lastStartButtonAnchorWidth - static_cast<float>(startButtonWidth)) > visualOffsetTolerance ||
       std::abs(state.lastStartButtonAnchorHeight - static_cast<float>(startButtonHeight)) > visualOffsetTolerance);
  if (hasStartButtonAnchorSample) {
    if (startButtonSampleChanged) {
      state.startButtonAnchorStablePasses = 0;
    } else if (state.startButtonAnchorStablePasses < kStartButtonAnchorStablePassesRequired) {
      state.startButtonAnchorStablePasses++;
    }
    state.lastStartButtonAnchorLeft = static_cast<float>(startButtonLeft);
    state.lastStartButtonAnchorTop = static_cast<float>(startButtonTop);
    state.lastStartButtonAnchorWidth = static_cast<float>(startButtonWidth);
    state.lastStartButtonAnchorHeight = static_cast<float>(startButtonHeight);
    state.hasLastStartButtonAnchorRect = true;
    if (state.startButtonAnchorStablePasses >= kStartButtonAnchorStablePassesRequired) {
      state.stableStartButtonAnchorLeft = static_cast<float>(startButtonLeft);
      state.stableStartButtonAnchorTop = static_cast<float>(startButtonTop);
      state.stableStartButtonAnchorWidth = static_cast<float>(startButtonWidth);
      state.stableStartButtonAnchorHeight = static_cast<float>(startButtonHeight);
      state.hasStableStartButtonAnchorRect = true;
    }
  } else {
    state.hasLastStartButtonAnchorRect = false;
    state.hasStableStartButtonAnchorRect = false;
    state.startButtonAnchorStablePasses = 0;
  }
  const double predictedCenteredTaskbarLeft =
      (taskbarLayoutSurfaceWidth - static_cast<double>(childrenWidthTaskbar)) / 2.0;
  const double taskbarEdgeClampTolerance =
      std::max<double>(visualOffsetTolerance,
                       static_cast<double>(g_settings.userDefinedTaskbarButtonSize) * 0.25);
  const bool taskbarLayoutIsEdgeClamped =
      isOverflowing ||
      taskbarLeftEdge <= taskbarEdgeClampTolerance ||
      taskbarRightEdge >= taskbarLayoutSurfaceWidth - taskbarEdgeClampTolerance ||
      predictedCenteredTaskbarLeft <= taskbarEdgeClampTolerance;

  // Windows can shift the taskbar item group left before the official overflow
  // button appears, especially when the centered item group would get too close
  // to the tray/suffixed area. Do not turn that into a second live anchor state
  // machine: just broaden the original stable Start-button anchor path for this
  // specific tray-collision condition.
  const double trayFrameLeft = rootWidth - static_cast<double>(trayFrameWidth);
  const double predictedCenteredTaskbarRight =
      predictedCenteredTaskbarLeft + childrenWidthTaskbarDbl;
  const double predictedTaskbarRightOnScreen =
      targetContentLeft + static_cast<double>(childrenWidthTaskbar);
  const double predictedGapToTray =
      trayFrameLeft - predictedTaskbarRightOnScreen;
  const double minimumTaskbarToTrayGap =
      std::max<double>(static_cast<double>(visualOffsetTolerance) * 2.0,
                       static_cast<double>(g_settings.userDefinedTaskbarButtonSize) * 0.75);
  const bool taskbarLayoutIsTrayConstrained =
      !useVirtualTaskbarSurface &&
      trayFrameWidth > 0 &&
      taskbarRightEdge > taskbarLeftEdge &&
      predictedGapToTray < minimumTaskbarToTrayGap &&
      std::abs(taskbarLeftEdge - predictedCenteredTaskbarLeft) >
          visualOffsetTolerance;
  const bool stableStartAnchorMatchesCurrentSample =
      state.hasStableStartButtonAnchorRect &&
      hasStartButtonAnchorSample &&
      std::abs(state.stableStartButtonAnchorLeft -
               static_cast<float>(startButtonLeft)) <= visualOffsetTolerance &&
      std::abs(state.stableStartButtonAnchorWidth -
               static_cast<float>(startButtonWidth)) <= visualOffsetTolerance;

  const double fallbackTaskbarAnchorLeft =
      taskbarLayoutIsEdgeClamped ? taskbarLeftEdge : predictedCenteredTaskbarLeft;
  const bool useStableStartButtonAnchor =
      state.hasStableStartButtonAnchorRect &&
      (taskbarLayoutIsEdgeClamped ||
       isOverflowing ||
       (taskbarLayoutIsTrayConstrained && stableStartAnchorMatchesCurrentSample));
  const double taskbarAnchorLeft = useStableStartButtonAnchor
      ? static_cast<double>(state.stableStartButtonAnchorLeft)
      : fallbackTaskbarAnchorLeft;
  float targetTaskRootOffsetX = snapPx(targetContentLeft - static_cast<float>(taskbarAnchorLeft));
  float centeredTaskbarRightEdge = targetTrayLogicalLeft;

  const float taskbarBackgroundPadding =
      static_cast<float>(g_settings.userDefinedTaskbarBackgroundHorizontalPadding);
  float targetWidth = g_unloading
      ? static_cast<float>(rootWidth)
      : (static_cast<float>(childrenWidthTaskbar + trayFrameWidth) +
         (taskbarBackgroundPadding * 2.0f));
  if (targetWidth < 1) {
    Wh_Log(L"Error: targetWidth<1");
    return false;
  }
  const float targetBackgroundLeftScreen =
      snapPx(targetContentLeft - taskbarBackgroundPadding);
  const float targetBackgroundRightScreen =
      snapPx(targetBackgroundLeftScreen + targetWidth);
  const float targetScaleCenterScreenX =
      snapPx((targetBackgroundLeftScreen + targetBackgroundRightScreen) * 0.5f);
  const float targetTaskbarIslandScale = g_unloading
      ? 1.0f
      : CalculateTaskbarIslandScale(targetBackgroundLeftScreen,
                                    targetBackgroundRightScreen,
                                    static_cast<float>(rootWidth),
                                    targetScaleCenterScreenX,
                                    rasterizationScale);
  const float scaledBackgroundLeftScreen = snapPx(
      ApplyScaleToScreenX(targetBackgroundLeftScreen,
                          targetScaleCenterScreenX,
                          targetTaskbarIslandScale));
  const float scaledBackgroundRightScreen = snapPx(
      ApplyScaleToScreenX(targetBackgroundRightScreen,
                          targetScaleCenterScreenX,
                          targetTaskbarIslandScale));
  const bool taskbarIslandScaleTargetChanged =
      !state.hasLastTargetTaskbarIslandScale ||
      std::abs(state.lastTargetTaskbarIslandScale - targetTaskbarIslandScale) > 0.001f ||
      std::abs(state.lastTaskbarIslandScaleCenterX - targetScaleCenterScreenX) > visualOffsetTolerance;

  if (!forceStyleApply && !g_invalidateDimensions && !g_unloading &&
      std::abs(targetOffsetXTray - systemTrayFrameGridVisual.Offset().x) <= visualOffsetTolerance &&
      childrenWidthTaskbar == state.lastChildrenWidthTaskbar &&
      trayFrameWidth == state.lastTrayFrameWidth &&
      std::abs(targetTaskRootOffsetX - rootGridTaskBarVisual.Offset().x) <= visualOffsetTolerance &&
      std::abs(taskbarFrameRepeaterVisual.Offset().x) <= visualOffsetTolerance &&
      !taskbarIslandScaleTargetChanged) {
    Wh_Log(L"taskbar root/tray offsets/scale are within tolerance %f; widths didn't change: %d, %d", visualOffsetTolerance, childrenWidthTaskbar, state.lastTrayFrameWidth);
    return true;
  }
  if (childrenWidthTaskbar < 1) {
    state.lastChildrenWidthTaskbar = 1;
  } else {
    state.lastChildrenWidthTaskbar = static_cast<unsigned int>(childrenWidthTaskbar);
  }
  if (trayFrameWidth < 1) {
    state.lastTrayFrameWidth = 1;
  } else {
    state.lastTrayFrameWidth = static_cast<unsigned int>(trayFrameWidth);
  }
  const bool invalidateDimensionsThisPass = g_invalidateDimensions;
  signed int userDefinedTaskbarOffsetY = (g_settings.userDefinedFlatTaskbarBottomCorners || g_settings.userDefinedFullWidthTaskbarBackground) ? 0 : g_settings.userDefinedTaskbarOffsetY;
  if (ShouldLogTaskbarGeometry(isOverflowing || taskbarLayoutIsEdgeClamped || taskbarLayoutIsTrayConstrained || useStableStartButtonAnchor || targetTaskbarIslandScale < 0.999f || forceStyleApply)) {
    Wh_Log(L"[TBGEOM] virtualSurface=%d virtualWidth=%.2f actualRepeaterWidth=%.2f layoutSurfaceWidth=%.2f overflowSuppressed=%d",
           useVirtualTaskbarSurface ? 1 : 0,
           taskbarVirtualSurfaceWidth,
           taskbarFrameRepeater.ActualWidth(),
           taskbarLayoutSurfaceWidth,
           (overflowButton && useVirtualTaskbarSurface) ? 1 : 0);
    Wh_Log(L"[TBGEOM] scale target=%.4f centerX=%.2f rawBg=[%.2f..%.2f] scaledBg=[%.2f..%.2f]",
           targetTaskbarIslandScale,
           targetScaleCenterScreenX,
           targetBackgroundLeftScreen,
           targetBackgroundRightScreen,
           scaledBackgroundLeftScreen,
           scaledBackgroundRightScreen);
    Wh_Log(L"[TBGEOM] trayConstraint=%d predictedGapToTray=%.2f minGap=%.2f stableStartMatches=%d",
           taskbarLayoutIsTrayConstrained ? 1 : 0,
           predictedGapToTray,
           minimumTaskbarToTrayGap,
           stableStartAnchorMatchesCurrentSample ? 1 : 0);
    LogTaskbarGeometryProbe(
        L"ApplyStyle",
        monitorName,
        xamlRootContent,
        taskFrame,
        rootGridTaskBar,
        taskbarFrameRepeater,
        trayFrame,
        systemTrayFrameGrid,
        taskbarBackground,
        backgroundFillChild,
        startButtonElement,
        rootWidth,
        childrenWidthTaskbarDbl,
        taskbarLeftEdge,
        taskbarRightEdge,
        startButtonLeft,
        startButtonTop,
        startButtonWidth,
        startButtonHeight,
        predictedCenteredTaskbarLeft,
        isOverflowing,
        taskbarLayoutIsEdgeClamped,
        useStableStartButtonAnchor,
        targetContentLeft,
        targetContentWidth,
        targetTaskRootOffsetX,
        targetOffsetXTray,
        targetBackgroundLeftScreen,
        targetBackgroundRightScreen,
        state);
  }
  const float unscaledStartButtonScreenX = useStableStartButtonAnchor
      ? snapPx(state.stableStartButtonAnchorLeft + targetTaskRootOffsetX)
      : targetBackgroundLeftScreen;
  state.lastStartButtonXCalculated = snapPx(
      ApplyScaleToScreenX(unscaledStartButtonScreenX,
                          targetScaleCenterScreenX,
                          targetTaskbarIslandScale));
  auto heightValue = (g_settings.userDefinedTaskbarHeight + abs(userDefinedTaskbarOffsetY < 0 ? (userDefinedTaskbarOffsetY * 2) : 0));
  if (heightValue < g_settings.userDefinedTaskbarHeight / 2) {
    Wh_Log(L"Error: heightValue<g_settings.userDefinedTaskbarHeight/2");
    return false;
  }
  if (g_invalidateDimensions) {
    g_invalidateDimensions = false;
    if (g_settings.userDefinedTaskbarHeight <= 0) {
      Wh_Log(L"Invalid size detected! Panel Height");
      return false;
    }
    if (heightValue <= 0) {
      Wh_Log(L"Invalid size detected! Panel Height");
      return false;
    }
    trayFrame.Height(g_settings.userDefinedTaskbarHeight);
    trayFrame.MaxHeight(g_settings.userDefinedTaskbarHeight);
    taskFrame.Height(heightValue);
    taskFrame.MaxHeight(heightValue);
    taskbarFrameRepeater.Height(g_settings.userDefinedTaskbarHeight);
    taskbarFrameRepeater.MaxHeight(g_settings.userDefinedTaskbarHeight);
    ApplyVirtualTaskbarLayoutSurface(xamlRootContent,
                                     taskFrame,
                                     rootGridTaskBar,
                                     taskbarFrameRepeater,
                                     taskbarBackground,
                                     backgroundFillParent,
                                     backgroundFillChild,
                                     taskbarVirtualSurfaceWidth);
  }
  // Any previous version of the mod may have left a horizontal Offset
  // animation on the repeater. Clear only X so RootGrid owns task-area X
  // motion, but preserve the repeater's native Y offset. Windows/XAML can use
  // that Y component to vertically align the task buttons with the tray.
  if (std::abs(taskbarFrameRepeaterVisual.Offset().x) > visualOffsetTolerance) {
    auto repeaterOffset = taskbarFrameRepeaterVisual.Offset();
    taskbarFrameRepeaterVisual.StopAnimation(L"Offset");
    taskbarFrameRepeaterVisual.Offset({0.0f, repeaterOffset.y, repeaterOffset.z});
  }

  const float rootGridScaleCenterLocalX =
      snapPx(targetScaleCenterScreenX - targetTaskRootOffsetX);
  const float rootGridScaleCenterLocalY =
      snapPx(static_cast<float>(rootGridTaskBar.ActualHeight()) * 0.5f);
  const float trayScaleCenterLocalX =
      snapPx(targetScaleCenterScreenX - targetTrayLogicalLeft);
  const float trayScaleCenterLocalY =
      snapPx(static_cast<float>(systemTrayFrameGrid.ActualHeight()) * 0.5f);
  SetVisualScaleCenterAndAnimate(rootGridTaskBarVisual,
                                 targetTaskbarIslandScale,
                                 rootGridScaleCenterLocalX,
                                 rootGridScaleCenterLocalY,
                                 0.001f,
                                 !g_unloading);
  SetVisualScaleCenterAndAnimate(systemTrayFrameGridVisual,
                                 targetTaskbarIslandScale,
                                 trayScaleCenterLocalX,
                                 trayScaleCenterLocalY,
                                 0.001f,
                                 !g_unloading);
  state.lastTargetTaskbarIslandScale = targetTaskbarIslandScale;
  state.lastTaskbarIslandScaleCenterX = targetScaleCenterScreenX;
  state.hasLastTargetTaskbarIslandScale = true;

  if (auto taskRootVisualCompositor = rootGridTaskBarVisual.Compositor()) {
    if (!g_unloading) {
      const bool taskRootOffsetChanged =
          invalidateDimensionsThisPass ||
          !state.hasLastTargetTaskFrameOffsetX ||
          std::abs(state.lastTargetTaskFrameOffsetX - targetTaskRootOffsetX) > visualOffsetTolerance;
      if (taskRootOffsetChanged) {
        auto taskRootOffsetAnimation = taskRootVisualCompositor.CreateVector3KeyFrameAnimation();
        ConfigureTaskbarIslandAnimation(taskRootOffsetAnimation);
        InsertTaskbarIslandKeyFrame(taskRootOffsetAnimation, 1.0f, winrt::Windows::Foundation::Numerics::float3{targetTaskRootOffsetX, rootGridTaskBarVisual.Offset().y, rootGridTaskBarVisual.Offset().z});
        rootGridTaskBarVisual.StartAnimation(L"Offset", taskRootOffsetAnimation);
        state.lastTargetTaskFrameOffsetX = targetTaskRootOffsetX;
        state.hasLastTargetTaskFrameOffsetX = true;
      }
    } else {
      rootGridTaskBarVisual.StopAnimation(L"Offset");
      rootGridTaskBarVisual.Offset({0.0f, 0.0f, rootGridTaskBarVisual.Offset().z});
      taskbarFrameRepeaterVisual.StopAnimation(L"Offset");
      taskbarFrameRepeaterVisual.Offset({0.0f, 0.0f, taskbarFrameRepeaterVisual.Offset().z});
      state.hasLastTargetTaskFrameOffsetX = false;
    }
  }
  auto taskbarVisual = rootGridTaskBarVisual;
  auto trayVisualCompositor = systemTrayFrameGridVisual.Compositor();
    if (trayVisualCompositor) {
    if (!g_unloading) {
      const bool trayOffsetChanged =
          invalidateDimensionsThisPass ||
          !state.hasLastTargetTrayOffsetX ||
          std::abs(state.lastTargetTrayOffsetX - targetOffsetXTray) > visualOffsetTolerance;
      if (trayOffsetChanged) {
        auto trayAnimation = trayVisualCompositor.CreateVector3KeyFrameAnimation();
        ConfigureTaskbarIslandAnimation(trayAnimation);
        InsertTaskbarIslandKeyFrame(trayAnimation, 1.0f, winrt::Windows::Foundation::Numerics::float3{targetOffsetXTray, systemTrayFrameGridVisual.Offset().y, systemTrayFrameGridVisual.Offset().z});
        systemTrayFrameGridVisual.StartAnimation(L"Offset", trayAnimation);
        state.lastTargetTrayOffsetX = targetOffsetXTray;
        state.hasLastTargetTrayOffsetX = true;
      }
    } else {
      systemTrayFrameGridVisual.Offset({0.0f, 0.0f, 0.0f});
      state.hasLastTargetTrayOffsetX = false;
    }
  }
  if (widgetPresent && widgetMainView) {
    if (widgetElement) {
      auto widgetVisualParent = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(widgetElement);
      if (widgetVisualParent && widgetVisualParent.Offset().x != 0.0f) {
        widgetVisualParent.Offset({0.0f, widgetVisualParent.Offset().y, widgetVisualParent.Offset().z});
      }
    }
    auto widgetVisual = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(widgetMainView);
     if (widgetVisual) {
      if (!g_unloading) {
        auto compositorWidget = widgetVisual.Compositor();
        if (compositorWidget) {
          // Widget visual is inside the RootGrid-shifted subtree, so its local
          // target must exclude the RootGrid compositor offset. Its screen
          // position still lands after the centered task buttons.
          float targetOffsetXWidget = centeredTaskbarRightEdge - targetTaskRootOffsetX - 8.0f + g_settings.userDefinedTrayTaskGap;
          targetOffsetXWidget = snapPx(targetOffsetXWidget);
          float targetOffsetYWidget = snapPx(static_cast<float>(abs(g_settings.userDefinedTaskbarHeight - widgetElementVisibleHeight)));
          const bool widgetOffsetChanged =
              invalidateDimensionsThisPass ||
              !state.hasLastTargetWidgetOffset ||
              std::abs(state.lastTargetWidgetOffsetX - targetOffsetXWidget) > visualOffsetTolerance ||
              std::abs(state.lastTargetWidgetOffsetY - targetOffsetYWidget) > visualOffsetTolerance;
          if (widgetOffsetChanged) {
            auto widgetOffsetAnimation = compositorWidget.CreateVector3KeyFrameAnimation();
            ConfigureTaskbarIslandAnimation(widgetOffsetAnimation);
            InsertTaskbarIslandKeyFrame(widgetOffsetAnimation, 1.0f, winrt::Windows::Foundation::Numerics::float3{targetOffsetXWidget, targetOffsetYWidget, taskbarVisual.Offset().z});
            widgetVisual.StartAnimation(L"Offset", widgetOffsetAnimation);
            state.lastTargetWidgetOffsetX = targetOffsetXWidget;
            state.lastTargetWidgetOffsetY = targetOffsetYWidget;
            state.hasLastTargetWidgetOffset = true;
          }
        }
      } else {
        widgetVisual.Offset({0.0f, 0.0f, 0.0f});
        state.hasLastTargetWidgetOffset = false;
      }
    }
  }
  if (state.lastTargetWidth <= static_cast<float>(minimumTaskbarChildrenWidth)) {
    state.lastTargetWidth = static_cast<float>(rootWidth);
    if (!g_unloading && state.lastTargetWidth <= 0) {
      Wh_Log(L"Error: g_unloading && state.lastTargetWidth <= 0");
      return false;
    }
  }
  const float targetWidthRect = !g_settings.userDefinedFullWidthTaskbarBackground ? targetWidth : static_cast<float>(rootWidth);
  if (!g_unloading && targetWidthRect <= 0) {
    Wh_Log(L"Error: targetWidthRect<=0");
    return false;
  }
  int rightMostEdgeTray = static_cast<int>(scaledBackgroundRightScreen + 0.5f);
  if (state.lastRightMostEdgeTray != rightMostEdgeTray) {
    state.lastRightMostEdgeTray = rightMostEdgeTray;
    Wh_SetIntValue((L"lastRightMostEdgeTray_" + monitorName).c_str(), rightMostEdgeTray);
  }
  float leftMostEdgeTray = scaledBackgroundRightScreen - (static_cast<float>(trayFrameWidth) * targetTaskbarIslandScale);
  if (leftMostEdgeTray != state.lastLeftMostEdgeTray) {
    state.lastLeftMostEdgeTray = leftMostEdgeTray;
    Wh_SetIntValue((L"lastLeftMostEdgeTray_" + monitorName).c_str(), static_cast<int>(leftMostEdgeTray));
  }
  const auto targetHeightPrelim = (!g_settings.userDefinedFullWidthTaskbarBackground ? g_settings.userDefinedTaskbarHeight : xamlRootContent.ActualHeight());
  if (!g_unloading && targetHeightPrelim <= 0) {
    Wh_Log(L"Error: targetHeightPrelim<=0");
    return false;
  }
  const auto clipHeight = static_cast<float>(targetHeightPrelim + ((g_settings.userDefinedFlatTaskbarBottomCorners) ? (targetHeightPrelim - g_settings.userDefinedTaskbarCornerRadius) : 0.0f));
  if (!g_unloading && clipHeight <= 0) {
    Wh_Log(L"Error: clipHeight<=0");
    return false;
  }
  ProcessStackPanelChildren(stackPanel, clipHeight);
  ChangeControlCenterIconSize(systemTrayFrameGrid);
  auto trayOverflowArrowNotifyIconStack = FindChildByName(systemTrayFrameGrid, L"NotifyIconStack");
  if (trayOverflowArrowNotifyIconStack) {
    SetDividerForElement(trayOverflowArrowNotifyIconStack, clipHeight, g_settings.userDefinedTrayAreaDivider, true);
  } else {
    SetDividerForElement(stackPanel, clipHeight, g_settings.userDefinedTrayAreaDivider, true);
  }
  //  if (widgetPresent && widgetElementInnerChild) {
  //    SetDividerForElement(widgetElementInnerChild, clipHeight, widgetPresent && g_settings.userDefinedTrayAreaDivider, true);
  //  }
  if (!taskbarBackground) return false;
  auto taskbarStroke = FindChildByName(backgroundFillParent, L"BackgroundStroke");
  if (taskbarStroke) {
    taskbarStroke.Opacity(g_unloading ? 1.0 : 0.0);
  }
  auto screenEdgeStroke = FindChildByName(rootGridTaskBar, L"ScreenEdgeStroke");
  if (screenEdgeStroke) {
    screenEdgeStroke.Opacity(g_unloading ? 1.0 : 0.0);
  }
 if (g_unloading) {
    ClearWindhawkBlurFromBackgroundFill(backgroundFillChild);
  } else if (g_settings.userDefinedCustomizeTaskbarBackground) {
    ApplyWindhawkBlurToBackgroundFill(backgroundFillChild);
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
    if (backgroundFillVisual) {
      if (compositorTaskBackground) {
        const float userDefinedTaskbarBorderThicknessFloat = static_cast<float>(g_settings.userDefinedTaskbarBorderThickness);
        // BackgroundFill belongs to RootGrid, and RootGrid is now translated.
        // Convert the desired screen-space island left into RootGrid-local
        // coordinates so the island background and task/tray content stay in
        // the same coordinate system.
        const float offsetXRect = snapPx(targetBackgroundLeftScreen - targetTaskRootOffsetX);
        const float newOffsetYRect = snapPx(userDefinedTaskbarOffsetY <= 0 ? static_cast<float>(abs(userDefinedTaskbarOffsetY)) : 0.0f);
        const bool backgroundShapeTargetChanged =
            invalidateDimensionsThisPass ||
            std::abs(state.lastBackgroundShapeTargetWidth - targetWidthRect) > visualOffsetTolerance ||
            std::abs(state.lastBackgroundShapeTargetOffsetX - offsetXRect) > visualOffsetTolerance ||
            std::abs(state.lastBackgroundShapeTargetOffsetY - newOffsetYRect) > visualOffsetTolerance;
        if (backgroundShapeTargetChanged || g_settings.userDefinedFullWidthTaskbarBackground || state.lastBackgroundShapeTargetWidth <= 0.0f) {
          auto roundedRect = compositorTaskBackground.CreateRoundedRectangleGeometry();
          roundedRect.CornerRadius({g_settings.userDefinedTaskbarCornerRadius, g_settings.userDefinedTaskbarCornerRadius});
          auto borderGeometry = compositorTaskBackground.CreateRoundedRectangleGeometry();
          borderGeometry.CornerRadius({g_settings.userDefinedTaskbarCornerRadius - userDefinedTaskbarBorderThicknessFloat / 2.0f, g_settings.userDefinedTaskbarCornerRadius - userDefinedTaskbarBorderThicknessFloat / 2.0f});
          borderGeometry.Offset({userDefinedTaskbarBorderThicknessFloat / 2.0f, userDefinedTaskbarBorderThicknessFloat / 2.0f});
          float animationStartWidth = targetWidthRect;
          float animationStartOffsetX = offsetXRect;
          float animationStartOffsetY = newOffsetYRect;
          const int64_t animationNowMs = DelayedApplyNowMs();
          const bool canContinueRunningBackgroundAnimation =
              !invalidateDimensionsThisPass &&
              state.backgroundAnimationStartMs > 0 &&
              animationNowMs - state.backgroundAnimationStartMs < kTaskbarIslandAnimationDurationMs;
          if (!g_settings.userDefinedFullWidthTaskbarBackground) {
            if (canContinueRunningBackgroundAnimation) {
              animationStartWidth = EstimateAnimationValue(state.backgroundAnimationFromWidth, state.backgroundAnimationToWidth, state.backgroundAnimationStartMs, animationNowMs);
              animationStartOffsetX = EstimateAnimationValue(state.backgroundAnimationFromOffsetX, state.backgroundAnimationToOffsetX, state.backgroundAnimationStartMs, animationNowMs);
              animationStartOffsetY = EstimateAnimationValue(state.backgroundAnimationFromOffsetY, state.backgroundAnimationToOffsetY, state.backgroundAnimationStartMs, animationNowMs);
            } else if (state.lastTargetWidth > static_cast<float>(minimumTaskbarChildrenWidth) && std::abs(state.lastTargetWidth - rootWidth) > visualOffsetTolerance) {
              animationStartWidth = state.lastTargetWidth;
              animationStartOffsetX = state.lastTargetOffsetX == 0.0f ? offsetXRect : snapPx(state.lastTargetOffsetX);
              animationStartOffsetY = snapPx(state.lastTargetOffsetY);
            }
          }
          roundedRect.Size({animationStartWidth, clipHeight});
          auto shapeVisualBorderControl = compositorTaskBackground.CreateShapeVisual();
          shapeVisualBorderControl.Size({animationStartWidth, clipHeight});
          auto geometricClip = compositorTaskBackground.CreateGeometricClip(roundedRect);
          auto borderShape = compositorTaskBackground.CreateSpriteShape(borderGeometry);
          winrt::Windows::UI::Color borderColor = {g_settings.userDefinedTaskbarBorderOpacity, static_cast<BYTE>(g_settings.borderColorR), static_cast<BYTE>(g_settings.borderColorG), static_cast<BYTE>(g_settings.borderColorB)};
          borderShape.StrokeBrush(compositorTaskBackground.CreateColorBrush(borderColor));
          borderShape.StrokeThickness(g_settings.userDefinedTaskbarBorderThickness);
          borderShape.FillBrush(nullptr);
          borderGeometry.Size({static_cast<float>(animationStartWidth - userDefinedTaskbarBorderThicknessFloat), static_cast<float>(clipHeight - userDefinedTaskbarBorderThicknessFloat)});
          backgroundFillVisual.Clip(geometricClip);
          shapeVisualBorderControl.Shapes().Append(borderShape);
          winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::SetElementChildVisual(backgroundFillChild, shapeVisualBorderControl);
          if (!g_settings.userDefinedFullWidthTaskbarBackground) {
            auto sizeAnimationRect = compositorTaskBackground.CreateVector2KeyFrameAnimation();
            ConfigureTaskbarIslandAnimation(sizeAnimationRect);
            sizeAnimationRect.InsertKeyFrame(0.0f, {animationStartWidth, clipHeight});
            InsertTaskbarIslandKeyFrame(sizeAnimationRect, 1.0f, winrt::Windows::Foundation::Numerics::float2{targetWidthRect, clipHeight});
            auto sizeAnimationBorderGeometry = compositorTaskBackground.CreateVector2KeyFrameAnimation();
            ConfigureTaskbarIslandAnimation(sizeAnimationBorderGeometry);
            sizeAnimationBorderGeometry.InsertKeyFrame(0.0f, {animationStartWidth - userDefinedTaskbarBorderThicknessFloat, clipHeight - userDefinedTaskbarBorderThicknessFloat});
            InsertTaskbarIslandKeyFrame(sizeAnimationBorderGeometry, 1.0f, winrt::Windows::Foundation::Numerics::float2{targetWidthRect - userDefinedTaskbarBorderThicknessFloat, clipHeight - userDefinedTaskbarBorderThicknessFloat});
            roundedRect.StartAnimation(L"Size", sizeAnimationRect);
            shapeVisualBorderControl.StartAnimation(L"Size", sizeAnimationRect);
            borderGeometry.StartAnimation(L"Size", sizeAnimationBorderGeometry);
            roundedRect.Offset({animationStartOffsetX, animationStartOffsetY});
            shapeVisualBorderControl.Offset({animationStartOffsetX, animationStartOffsetY, 0.0f});
            auto offsetAnimationRect = compositorTaskBackground.CreateVector2KeyFrameAnimation();
            ConfigureTaskbarIslandAnimation(offsetAnimationRect);
            offsetAnimationRect.InsertKeyFrame(0.0f, {animationStartOffsetX, animationStartOffsetY});
            InsertTaskbarIslandKeyFrame(offsetAnimationRect, 1.0f, winrt::Windows::Foundation::Numerics::float2{offsetXRect, newOffsetYRect});
            auto offsetAnimationRect3V = compositorTaskBackground.CreateVector3KeyFrameAnimation();
            ConfigureTaskbarIslandAnimation(offsetAnimationRect3V);
            offsetAnimationRect3V.InsertKeyFrame(0.0f, {animationStartOffsetX, animationStartOffsetY, 0.0f});
            InsertTaskbarIslandKeyFrame(offsetAnimationRect3V, 1.0f, winrt::Windows::Foundation::Numerics::float3{offsetXRect, newOffsetYRect, 0.0f});
            roundedRect.StartAnimation(L"Offset", offsetAnimationRect);
            shapeVisualBorderControl.StartAnimation(L"Offset", offsetAnimationRect3V);
            state.backgroundAnimationFromWidth = animationStartWidth;
            state.backgroundAnimationToWidth = targetWidthRect;
            state.backgroundAnimationFromOffsetX = animationStartOffsetX;
            state.backgroundAnimationToOffsetX = offsetXRect;
            state.backgroundAnimationFromOffsetY = animationStartOffsetY;
            state.backgroundAnimationToOffsetY = newOffsetYRect;
            state.backgroundAnimationStartMs = animationNowMs;
            state.lastTargetOffsetX = offsetXRect;
            state.lastTargetOffsetY = newOffsetYRect;
          } else {
            state.lastTargetOffsetX = 0;
            state.lastTargetOffsetY = 0;
            state.backgroundAnimationStartMs = 0;
            roundedRect.Offset({state.lastTargetOffsetX, state.lastTargetOffsetY});
            shapeVisualBorderControl.Offset({state.lastTargetOffsetX, state.lastTargetOffsetY, 0.0f});
          }
          state.lastBackgroundShapeTargetWidth = targetWidthRect;
          state.lastBackgroundShapeTargetOffsetX = offsetXRect;
          state.lastBackgroundShapeTargetOffsetY = newOffsetYRect;
        }
      }
    }
  } else {
    if (backgroundFillVisual) {
      backgroundFillVisual.Clip(nullptr);
    }
    if (compositorTaskBackground) {
      winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::SetElementChildVisual(backgroundFillChild, compositorTaskBackground.CreateShapeVisual());
    }
  }
  state.wasOverflowing = isOverflowing;
  state.lastTargetWidth = targetWidthRect;
  state.lastTargetWidth = targetWidth;
  g_initial_style_apply_completed = true;
  g_initial_style_apply_not_before_ms = 0;
  return true;
}
void ApplySettings(HWND hTaskbarWnd) {
  RunFromWindowThread(hTaskbarWnd, [](void* pParam) { ApplySettingsFromTaskbarThread(); }, 0);
}
void RefreshSettings() {
  g_invalidateDimensions = true;
  const unsigned int oldTaskbarButtonSize = g_settings.userDefinedTaskbarButtonSize;
  Wh_ModSettingsChangedTBIconSize();
  Wh_ModSettingsChangedStartButtonPosition();
  UpdateGlobalSettings();
  if (!g_unloading && oldTaskbarButtonSize > 0 &&
      oldTaskbarButtonSize != g_settings.userDefinedTaskbarButtonSize) {
    RequestTaskbarButtonSizeRelayout();
  }
}
void ResetGlobalVars() {
  g_invalidateDimensions = true;
  for (auto& [key, state] : g_taskbarStates) {
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
  if (g_PartialMode) {
    return;
  }
  Wh_Log(L"Settings Changed");
  ResetGlobalVars();
  RefreshSettings();
  ApplySettingsFromTaskbarThread();
}
bool IsExplorer() {
  wchar_t processPath[MAX_PATH];
  if (GetModuleFileName(NULL, processPath, MAX_PATH)) {
    const wchar_t* processName = wcsrchr(processPath, L'\\');
    if (processName && _wcsicmp(processName + 1, L"explorer.exe") == 0) {
      return true;
    }
  }
  return false;
}
using SetWindowPos_t = BOOL(WINAPI*)(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags);
SetWindowPos_t SetWindowPos_Original = nullptr;
std::wstring GetProcessExeName(DWORD processId) {
  std::wstring result = L"<unknown>";
  HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
  if (hProcess) {
    WCHAR path[MAX_PATH];
    DWORD size = ARRAYSIZE(path);
    if (QueryFullProcessImageNameW(hProcess, 0, path, &size)) {
      std::wstring fullPath = path;
      size_t pos = fullPath.find_last_of(L"\\/");
      result = (pos != std::wstring::npos) ? fullPath.substr(pos + 1) : fullPath;
    }
    CloseHandle(hProcess);
  }
  return result;
}
BOOL WINAPI SetWindowPos_Hook(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags) {
  DWORD processId = 0;
  bool userDefinedMoveFlyoutControlCenter = Wh_GetIntSetting(L"MoveFlyoutControlCenter");
  if (!hWnd || !GetWindowThreadProcessId(hWnd, &processId)) {
    return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
  }
  WCHAR className[256] = L"<unknown>";
  GetClassNameW(hWnd, className, ARRAYSIZE(className));
  std::wstring windowClassName = className;
  std::wstring processFileName = GetProcessExeName(processId);
  if (true) {
    Wh_Log(L"[SetWindowPos] PID: %lu | EXE: %s | Class: %s | HWND: 0x%p | Pos: (%d,%d) Size: %dx%d Flags: 0x%08X", processId, processFileName.c_str(), windowClassName.c_str(), hWnd, X, Y, cx, cy, uFlags);
  }
  if (!g_unloading && userDefinedMoveFlyoutControlCenter && _wcsicmp(processFileName.c_str(), L"ShellHost.exe") == 0 && _wcsicmp(windowClassName.c_str(), L"ControlCenterWindow") == 0) {
    HMONITOR monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitorInfo{
        .cbSize = sizeof(MONITORINFO),
    };
    GetMonitorInfo(monitor, &monitorInfo);
    auto monitorName = GetMonitorName(monitor);
    int lastRecordedTrayRightMostEdgeForMonitor = Wh_GetIntValue((L"lastRightMostEdgeTray_" + monitorName).c_str(), -1);
    if (lastRecordedTrayRightMostEdgeForMonitor > 0) {
      UINT monitorDpiX = 96;
      UINT monitorDpiY = 96;
      GetDpiForMonitor(monitor, MDT_DEFAULT, &monitorDpiX, &monitorDpiY);
      float dpiScale = monitorDpiX / 96.0f;
      const int flyoutInnerPaddingPx = GetFlyoutInnerPaddingPx(dpiScale);
      X = static_cast<int>(lastRecordedTrayRightMostEdgeForMonitor * dpiScale + flyoutInnerPaddingPx - (Wh_GetIntSetting(L"AlignFlyoutInner") ? cx : (cx / 2.0f)));
      Wh_Log(L"[SetWindowPos] New X %d", X);
    } else {
      Wh_Log(L"[SetWindowPos] No reference state for monitor %s", monitorName.c_str());
    }
  }
  return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}
BOOL Wh_ModInit() {
  Wh_Log(L"======================================================");
  HMODULE moduleUser32 = GetModuleHandleW(L"user32.dll");
  if (moduleUser32) {
    auto pSetWindowPos = (SetWindowPos_t)GetProcAddress(moduleUser32, "SetWindowPos");
    if (pSetWindowPos) {
      if (WindhawkUtils::Wh_SetFunctionHookT(pSetWindowPos, SetWindowPos_Hook, &SetWindowPos_Original)) {
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
    g_PartialMode = true;
    Wh_Log(L"Not explorer.exe; setting g_PartialMode to true");
    HMODULE moduleStartDocked = GetModuleHandle(L"StartDocked.dll");
    if (moduleStartDocked) {
      WindhawkUtils::SYMBOL_HOOK hook[] = {{{LR"(private: void __cdecl StartDocked::StartSizingFrame::UpdateWindowRegion(class Windows::Foundation::Size))"}, &StartDocked__StartSizingFrame_UpdateWindowRegion_WithArgs_Original, StartDocked__StartSizingFrame_UpdateWindowRegion_WithArgs_Hook}};
      return WindhawkUtils::HookSymbols(moduleStartDocked, hook, ARRAYSIZE(hook));
    }
    return true;
  }
  g_unloading = false;
  ArmInitialExplorerStyleApplyDelay();
  if (!Wh_ModInitTBIconSize()) {
    Wh_Log(L"Wh_ModInitTBIconSize failed");
    return FALSE;
  }
  if (!Wh_ModInitStartButtonPosition()) {
    Wh_Log(L"Wh_ModInitStartButtonPosition failed");
    return FALSE;
  }
  return TRUE;
}
void Wh_ModAfterInit() {
  if (g_PartialMode) {
    g_lastRecordedStartMenuWidth = Wh_GetIntValue(L"lastRecordedStartMenuWidth", g_lastRecordedStartMenuWidth);
    return;
  }
  Wh_ModAfterInitTBIconSize();
  ResetGlobalVars();
  LoadSettingsTBIconSize();
  LoadSettingsStartButtonPosition();
  UpdateGlobalSettings();
  ScheduleInitialExplorerStyleApply();
}
void Wh_ModBeforeUninit() {
  if (g_PartialMode) {
    return;
  }
  g_unloading = true;
  Wh_ModBeforeUninitTBIconSize();
  Wh_ModBeforeUninitStartButtonPosition();
  RefreshSettings();
  HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
  if (hTaskbarWnd) {
    ApplySettings(hTaskbarWnd);
  }
}
void Wh_ModUninit() {
  if (g_PartialMode) {
    return;
  }
  CleanupDebounce();
  Wh_ModUninitTBIconSize();
  ResetGlobalVars();
  g_taskbarStates.clear();
  g_settings.userDefinedDividedAppNames.clear();
  Wh_Log(L"... detached");
}