

bool g_invalidateDimensions = true;

void ApplySettingsFromTaskbarThreadIfRequired() {
  if (!g_scheduled_low_priority_update) {
    g_scheduled_low_priority_update = true;
    Wh_Log(L"Scheduled low priority update");  // it's low priority in the debounce
                                               // sense, not the required sense.
    ApplySettingsDebounced(-1);
  }
}

void SetDividerForElement(FrameworkElement const& element, float const& panelHeight, bool dividerVisible) {
  if (!element) return;

  if (panelHeight <= 0.0f) return;

  auto visual = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(element);
  if (!visual) return;
  auto compositor = visual.Compositor();  // :contentReference[oaicite:0]{index=0}
  if (!compositor) return;

  auto shapeVisual = compositor.CreateShapeVisual();
  if (!shapeVisual) return;

  if (dividerVisible) {
    auto borderThicknessFloat = static_cast<float>(g_settings.userDefinedTaskbarBorderThickness) * 2.0f;
    shapeVisual.Size({borderThicknessFloat, panelHeight});
    shapeVisual.Offset({0.0f, 0.0f, 0.0f});
    auto lineGeometry = compositor.CreateLineGeometry();
    lineGeometry.Start({0.0f, 0.0f});
    lineGeometry.End({0.0f, panelHeight});
    auto lineShape = compositor.CreateSpriteShape(lineGeometry);
    winrt::Windows::UI::Color borderColor = {g_settings.userDefinedTaskbarBorderOpacity, static_cast<BYTE>(g_settings.borderColorR), static_cast<BYTE>(g_settings.borderColorG), static_cast<BYTE>(g_settings.borderColorB)};

    lineShape.StrokeBrush(compositor.CreateColorBrush(borderColor));

    lineShape.StrokeThickness(borderThicknessFloat);
    shapeVisual.Shapes().Append(lineShape);
  }
  winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::SetElementChildVisual(element, shapeVisual);  // :contentReference[oaicite:3]{index=3}
}

void ProcessStackPanelChildren(FrameworkElement const& stackPanel, float const& panelHeight) {
  if (!g_settings.userDefinedStyleTrayArea && g_trayIcons.empty()) return;

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

//    unsigned int targetImageSize = g_settings.userDefinedStyleTrayArea ? g_settings.userDefinedTrayIconSize : image.ActualWidth();
//    if (targetImageSize <= 1) {
//      continue;
//    }
//    auto currentIconAppName = notifyItemIcon.GetValue(winrt::Windows::UI::Xaml::Automation::AutomationProperties::NameProperty());
//    const std::wstring currentIconAppNameStr = winrt::unbox_value<winrt::hstring>(currentIconAppName).c_str();
//    if (currentIconAppNameStr.empty()) continue;
//    for (const auto& [iconName, iconPath] : g_trayIcons) {
//      if (RegexMatchInsensitive(currentIconAppNameStr, iconName)) {
//        SetImageFromFileAsync(imageCtrl, iconPath, targetImageSize, true);
//        break;
//      }
//    }
  }
}

double CalculateValidChildrenWidth(FrameworkElement element, int& childrenCount) {
  const float tbHeightFloat = static_cast<float>(g_settings.userDefinedTaskbarHeight);
  auto userDefinedTaskButtonCornerRadius = std::to_wstring(g_settings.userDefinedTaskButtonCornerRadius);
  auto userDefinedTaskbarIconSize = std::to_wstring(g_settings.userDefinedTaskbarIconSize);
  double totalWidth = 0.0;
  childrenCount = 0;
  int childrenCountTentative = Media::VisualTreeHelper::GetChildrenCount(element);
  for (int i = 0; i < childrenCountTentative; i++) {
    auto child = Media::VisualTreeHelper::GetChild(element, i).try_as<FrameworkElement>();
    if (!child) {
      Wh_Log(L"Failed to get child %d of %d", i + 1, childrenCountTentative);
      continue;
    }
    auto transform = child.TransformToVisual(nullptr);
    auto rect = transform.TransformBounds(winrt::Windows::Foundation::Rect(0, 0, child.ActualWidth(), child.ActualHeight()));
    // exclude "weird" rectangles (aka recycled views)
    if (rect.X < 0 | rect.Y < 0) {
      continue;
    }
    auto className = winrt::get_class_name(child);
    SetElementPropertyFromString(child, className.c_str(), L"CornerRadius", userDefinedTaskButtonCornerRadius);

    if (className == L"Taskbar.TaskListButton" || className == L"Taskbar.ExperienceToggleButton" || className == L"Taskbar.OverflowToggleButton") {
      child.MinWidth(g_settings.userDefinedTaskbarButtonSize);
      child.Width(g_settings.userDefinedTaskbarButtonSize);

      auto innerElementChild = FindChildByClassName(child, L"Taskbar.TaskListButtonPanel");
      if (innerElementChild) {
        innerElementChild.MinWidth(g_settings.userDefinedTaskbarButtonSize);
        innerElementChild.Width(g_settings.userDefinedTaskbarButtonSize);
      }
    }
    if (className == L"Taskbar.SearchBoxButton") {
      child.MinWidth(g_settings.userDefinedTaskbarButtonSize);
      auto innerElementChild = FindChildByClassName(child, L"Taskbar.TaskListButtonPanel");
      if (innerElementChild) {
        innerElementChild.MinWidth(g_settings.userDefinedTaskbarButtonSize);
      }
    }
    // todo: check this
    if (className == L"Taskbar.TaskListButton") {
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
//            if (auto imageControl = iconElementChild.try_as<winrt::Windows::UI::Xaml::Controls::Image>()) {
//              for (const auto& [iconName, iconPath] : g_taskbarIcons) {
//                if (RegexMatchInsensitive(currentIconAppNameStr, iconName)) {
//                  SetImageFromFileAsync(imageControl, iconPath, g_settings.userDefinedTaskbarButtonSize, false);
//                  break;
//                }
//              }
//            }
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
    }

    totalWidth += rect.Width;
    childrenCount++;
  }
  return totalWidth;
}
void UpdateGlobalSettings() {
  int value;

  value = Wh_GetIntSetting(L"FlatTaskbarBottomCorners");
  if (value == 0)
    g_settings.userDefinedFlatTaskbarBottomCorners = false;
  else
    g_settings.userDefinedFlatTaskbarBottomCorners = true;

  value = Wh_GetIntSetting(L"FullWidthTaskbarBackground");
  if (value == 0)
    g_settings.userDefinedFullWidthTaskbarBackground = false;
  else
    g_settings.userDefinedFullWidthTaskbarBackground = true;

  if (g_settings.userDefinedFullWidthTaskbarBackground) {
    g_settings.userDefinedFlatTaskbarBottomCorners = true;
  }

  value = Wh_GetIntSetting(L"IgnoreShowDesktopButton");
  g_settings.userDefinedIgnoreShowDesktopButton = value != 0;

  value = Wh_GetIntSetting(L"TrayAreaDivider");
  g_settings.userDefinedTrayAreaDivider = value != 0;

  value = Wh_GetIntSetting(L"StyleTrayArea");
  g_settings.userDefinedStyleTrayArea = value != 0;

  value = Wh_GetIntSetting(L"TrayTaskGap");
  if (value <= 0) value = 20;
  g_settings.userDefinedTrayTaskGap = value;

  value = Wh_GetIntSetting(L"TaskbarBackgroundHorizontalPadding");
  if (value <= 0) value = 6;
  g_settings.userDefinedTaskbarBackgroundHorizontalPadding = value;

  value = abs(Wh_GetIntSetting(L"TaskbarOffsetY"));
  if (value < 0) value = 6;
  g_settings.userDefinedTaskbarOffsetY = -1 * abs(value);

  if (g_settings.userDefinedFlatTaskbarBottomCorners) {
    g_settings.userDefinedTaskbarOffsetY = 0;
  }

  value = Wh_GetIntSetting(L"TaskbarHeight");
  if (value <= 0) value = 78;
  value = abs(value);
  if (value > 200) value = 200;
  if (value < 44) value = 44;
  g_settings.userDefinedTaskbarHeight = value;

  value = Wh_GetIntSetting(L"TaskbarIconSize");
  if (value <= 0) value = 44;
  g_settings.userDefinedTaskbarIconSize = value;

  value = Wh_GetIntSetting(L"TrayIconSize");
  if (value <= 0) value = 30;
  g_settings.userDefinedTrayIconSize = value;

  value = Wh_GetIntSetting(L"TaskbarButtonSize");
  if (value <= 0) value = 74;
  g_settings.userDefinedTaskbarButtonSize = value;

  value = Wh_GetIntSetting(L"TrayButtonSize");
  if (value <= 0) value = 45;
  g_settings.userDefinedTrayButtonSize = value;

  value = Wh_GetIntSetting(L"TaskbarCornerRadius");
  if (value <= 0) value = 24;
  g_settings.userDefinedTaskbarCornerRadius = static_cast<float>(abs(value));
  if (g_settings.userDefinedTaskbarCornerRadius > g_settings.userDefinedTaskbarHeight / 2.0f) g_settings.userDefinedTaskbarCornerRadius = g_settings.userDefinedTaskbarHeight / 2.0f;

  value = Wh_GetIntSetting(L"TaskButtonCornerRadius");
  if (value <= 0) value = 16;
  value = abs(value);
  if (value > (int)(g_settings.userDefinedTaskbarHeight / 2)) value = g_settings.userDefinedTaskbarHeight / 2;
  g_settings.userDefinedTaskButtonCornerRadius = value;

  value = Wh_GetIntSetting(L"TaskbarBackgroundOpacity");
  value = abs(value);
  if (value < 0)
    value = 0;
  else if (value > 100)
    value = 100;
  g_settings.userDefinedTaskbarBackgroundOpacity = value > 0 ? value : 100;

  value = Wh_GetIntSetting(L"TaskbarBackgroundTint");
  value = abs(value);
  if (value > 100) value = 100;
  g_settings.userDefinedTaskbarBackgroundTint = value;

  value = Wh_GetIntSetting(L"TaskbarBackgroundLuminosity");
  value = abs(value);
  if (value > 100) value = 100;
  g_settings.userDefinedTaskbarBackgroundLuminosity = value > 0 ? value : 30;

  value = Wh_GetIntSetting(L"TaskbarBorderOpacity");
  value = abs(value);
  if (value > 100) value = 100;
  g_settings.userDefinedTaskbarBorderOpacity = static_cast<uint8_t>(value * 2.55f);
  if (g_settings.userDefinedTaskbarBorderOpacity > 255) g_settings.userDefinedTaskbarBorderOpacity = 255;

  value = Wh_GetIntSetting(L"TaskbarBorderThickness");
  value = abs(value);
  if (value < 0) value = 0;
  if (value > 100) value = 100;
  g_settings.userDefinedTaskbarBorderThickness = (10.0 / 100.0) * value;

  PCWSTR userDefinedTaskbarBorderHexColorString = Wh_GetStringSetting(L"TaskbarBorderColorHex");
  if (!userDefinedTaskbarBorderHexColorString || wcslen(userDefinedTaskbarBorderHexColorString) == 0) {
    userDefinedTaskbarBorderHexColorString = L"#ffffff";
  }
  PCWSTR hexStr = userDefinedTaskbarBorderHexColorString;
  if (*hexStr == L'#') {
    hexStr++;
  }
  unsigned int r = 255, g = 255, b = 255;
  if (swscanf_s(hexStr, L"%02x%02x%02x", &r, &g, &b) != 3) {
    r = g = b = 255;
  }
  g_settings.borderColorR = r;
  g_settings.borderColorG = g;
  g_settings.borderColorB = b;

  g_settings.userDefinedDividedAppNames = SplitAndTrim(Wh_GetStringSetting(L"DividedAppNames"));
}
bool HasInvalidSettings() {
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
};

static std::unordered_map<size_t, TaskbarState> g_taskbarStates;

bool ApplyStyle(FrameworkElement const& xamlRootContent) {
  if (!xamlRootContent) return false;

  size_t key = reinterpret_cast<size_t>(winrt::get_abi(xamlRootContent));
  auto& state = g_taskbarStates[key];

  Wh_Log(L"ApplyStyle on key %zu", key);

  g_scheduled_low_priority_update = false;

  auto now = std::chrono::steady_clock::now();
  if (now - state.lastApplyStyleTime < std::chrono::milliseconds(200)) {
    return true;
  }
  state.lastApplyStyleTime = now;

  if (!xamlRootContent) return false;

  bool invalidateLayoutRequest = g_invalidateDimensions;
  g_invalidateDimensions = false;

  auto taskFrame = FindChildByClassName(xamlRootContent, L"Taskbar.TaskbarFrame");
  if (!taskFrame) return false;

  auto rootGridTaskBar = FindChildByName(taskFrame, L"RootGrid");
  if (!rootGridTaskBar) return false;

  auto taskbarFrameRepeater = FindChildByName(rootGridTaskBar, L"TaskbarFrameRepeater");
  if (!taskbarFrameRepeater) return false;

  auto trayFrame = FindChildByClassName(xamlRootContent, L"SystemTray.SystemTrayFrame");
  if (!trayFrame) return false;

  auto systemTrayFrameGrid = FindChildByName(trayFrame, L"SystemTrayFrameGrid");
  if (!systemTrayFrameGrid) return false;

  auto showDesktopButton = FindChildByName(systemTrayFrameGrid, L"ShowDesktopStack");
  if (!showDesktopButton) return false;

  auto taskbarBackground = FindChildByClassName(rootGridTaskBar, L"Taskbar.TaskbarBackground");
  if (!taskbarBackground) return false;

  auto backgroundFillParent = FindChildByClassName(taskbarBackground, L"Windows.UI.Xaml.Controls.Grid");
  if (!backgroundFillParent) return false;

  auto backgroundFillChild = FindChildByName(backgroundFillParent, L"BackgroundFill");
  if (!backgroundFillChild) return false;

  auto notificationAreaIcons = FindChildByName(systemTrayFrameGrid, L"NotificationAreaIcons");
  if (!notificationAreaIcons) return false;

  auto itemsPresenter = FindChildByClassName(notificationAreaIcons, L"Windows.UI.Xaml.Controls.ItemsPresenter");
  if (!itemsPresenter) return false;

  auto stackPanel = FindChildByClassName(itemsPresenter, L"Windows.UI.Xaml.Controls.StackPanel");
  if (!stackPanel) return false;

  auto widgetElement = FindChildByClassName(taskbarFrameRepeater, L"Taskbar.AugmentedEntryPointButton");
  bool widgetPresent = widgetElement != nullptr && winrt::unbox_value<bool>(widgetElement.GetValue(UIElement::CanBeScrollAnchorProperty()));
  auto widgetElementWidth = widgetPresent ? widgetElement.ActualWidth() : 0;

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

  if (rootWidth < 100) {
    Wh_Log(L"root width is too small");
    return false;
  }

  int childrenCountTaskbar = 0;
  const double childrenWidthTaskbarDbl = CalculateValidChildrenWidth(taskbarFrameRepeater, childrenCountTaskbar);

  if (childrenWidthTaskbarDbl <= 0) {
    Wh_Log(L"Error: childrenWidthTaskbarDbl <= 0");
    return false;
  }
  signed int rightMostEdgeTaskbar = static_cast<signed int>((rootWidth / 2.0) + (childrenWidthTaskbarDbl / 2.0));
  unsigned int childrenWidthTaskbar = static_cast<unsigned int>(childrenWidthTaskbarDbl);

  if (childrenCountTaskbar < 1) {
    Wh_Log(L"Error: childrenCountTaskbar < 1");
    return false;
  }

  if (childrenWidthTaskbar <= 10) {
    Wh_Log(L"Error: childrenWidthTaskbar <= 10");
    return false;
  }

  if (rightMostEdgeTaskbar < 0) {
    Wh_Log(L"Error: rightMostEdgeTaskbar < 0");
    return false;
  }
  bool rightMostEdgeChangedTaskbar = (state.lastTaskbarData.rightMostEdge != rightMostEdgeTaskbar);

  if (!isOverflowing && (rightMostEdgeChangedTaskbar || state.lastTaskbarData.rightMostEdge == 0.0)) {
    state.lastTaskbarData.childrenCount = childrenCountTaskbar;
    state.lastTaskbarData.rightMostEdge = rightMostEdgeTaskbar;
    state.lastTaskbarData.childrenWidth = childrenWidthTaskbar;
  }

trayFrame.Clip(nullptr);
if (trayFrame.GetValue(FrameworkElement::HorizontalAlignmentProperty()).as<winrt::Windows::Foundation::IReference<HorizontalAlignment>>().Value() == HorizontalAlignment::Center){
    trayFrame.SetValue(FrameworkElement::HorizontalAlignmentProperty(), winrt::box_value(HorizontalAlignment::Right));
}

  int childrenCountTray = 0;
  double trayFrameWidthDbl = CalculateValidChildrenWidth(systemTrayFrameGrid, childrenCountTray);

  if (trayFrameWidthDbl <= 0) {
    Wh_Log(L"Error: trayFrameWidthDbl <= 0");
    return false;
  }

  if (childrenCountTray <= 0) {
    Wh_Log(L"Error: childrenCountTray <= 0");
    return false;
  }

  float showDesktopButtonWidth = static_cast<float>(g_settings.userDefinedIgnoreShowDesktopButton ? showDesktopButton.ActualWidth() : 0);
  int trayGapPlusExtras = g_settings.userDefinedTrayTaskGap + widgetElementVisibleWidth + (widgetPresent ? g_settings.userDefinedTrayTaskGap : 0);

  if (trayGapPlusExtras < 0) {
    Wh_Log(L"Error: trayGapPlusExtras < 0");
    return false;
  }

  const unsigned int trayFrameWidth = static_cast<unsigned int>(trayFrameWidthDbl - showDesktopButtonWidth + trayGapPlusExtras);

  if (childrenCountTray == 0) {
    Wh_Log(L"Error: childrenCountTray == 0");
    return false;
  }

  if (trayFrameWidth <= 1) {
    Wh_Log(L"Error: trayFrameWidth <= 1");
    return false;
  }

  float centeredTray = (rootWidth - trayFrameWidth) / 2.0f;

  if (centeredTray <= 1) {
    Wh_Log(L"Error: centeredTray <= 1");
    return false;
  }
  float newXOffsetTray = centeredTray + (childrenWidthTaskbar / 2.0f) + trayGapPlusExtras;
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

  auto taskbarFrameRepeaterVisual = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(taskbarFrameRepeater);
  if (!taskbarFrameRepeaterVisual) {
    Wh_Log(L"Error: !taskbarFrameRepeaterVisual");
    return false;
  }

  float targetTaskFrameOffsetX = newXOffsetTray - rightMostEdgeTaskbar - trayGapPlusExtras;

  // 5 pixels tolerance
  if (!invalidateLayoutRequest && !g_unloading && abs(newXOffsetTray - systemTrayFrameGridVisual.Offset().x) <= 5 && childrenWidthTaskbar == state.lastChildrenWidthTaskbar && trayFrameWidth == state.lastTrayFrameWidth &&
      abs(targetTaskFrameOffsetX - taskbarFrameRepeaterVisual.Offset().x) <= 5) {
    Wh_Log(L"newXOffsetTray is within 5 pixels of systemTrayFrameGridVisual offset %f", systemTrayFrameGridVisual.Offset().x);
    Wh_Log(L"childrenWidthTaskbar and trayFrameWidth didn't change: %d, %d", childrenWidthTaskbar, state.lastTrayFrameWidth);
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

  signed int userDefinedTaskbarOffsetY = (g_settings.userDefinedFlatTaskbarBottomCorners || g_settings.userDefinedFullWidthTaskbarBackground) ? 0 : g_settings.userDefinedTaskbarOffsetY;
  float targetWidth = g_unloading ? rootWidth : (childrenWidthTaskbar + trayFrameWidth + (g_settings.userDefinedTaskbarBackgroundHorizontalPadding * 2));

  if (targetWidth < 1) {
    Wh_Log(L"Error: targetWidth<1");
    return false;
  }

  auto heightValue = (g_settings.userDefinedTaskbarHeight + abs(userDefinedTaskbarOffsetY < 0 ? (userDefinedTaskbarOffsetY * 2) : 0));

  if (heightValue < g_settings.userDefinedTaskbarHeight / 2) {
    Wh_Log(L"Error: heightValue<g_settings.userDefinedTaskbarHeight/2");
    return false;
  }
  if (invalidateLayoutRequest) {
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

    taskbarFrameRepeater.Width(rootWidth);
    taskbarFrameRepeater.MaxWidth(rootWidth);
  }

  auto taskbarFrameRepeaterVisualCompositor = taskbarFrameRepeaterVisual.Compositor();
  if (taskbarFrameRepeaterVisualCompositor) {
    auto taskbarFrameRepeaterVisualAnimation = taskbarFrameRepeaterVisualCompositor.CreateVector3KeyFrameAnimation();
    auto animationControllerTaskbarFrameRepeaterVisual = taskbarFrameRepeaterVisual.TryGetAnimationController(L"Offset");

    taskbarFrameRepeaterVisualAnimation.InsertKeyFrame(1.0f, winrt::Windows::Foundation::Numerics::float3{targetTaskFrameOffsetX, taskbarFrameRepeaterVisual.Offset().y, taskbarFrameRepeaterVisual.Offset().z});
    taskbarFrameRepeaterVisual.StartAnimation(L"Offset", taskbarFrameRepeaterVisualAnimation);
  }

  bool movingInwards = originalOffset.x > newXOffsetTray;

  auto taskbarVisual = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(taskbarFrameRepeater);

  auto trayVisualCompositor = systemTrayFrameGridVisual.Compositor();
  if (trayVisualCompositor) {
    float targetOffsetXTray = (g_unloading ? (float)state.initOffsetX : static_cast<float>(rightMostEdgeTaskbar+targetTaskFrameOffsetX - (rootWidth - trayFrameWidth)));
    auto trayAnimation = trayVisualCompositor.CreateVector3KeyFrameAnimation();

    trayAnimation.InsertKeyFrame(1.0f, winrt::Windows::Foundation::Numerics::float3{targetOffsetXTray, systemTrayFrameGridVisual.Offset().y, systemTrayFrameGridVisual.Offset().z});
    if (movingInwards) {
      trayAnimation.DelayTime(winrt::Windows::Foundation::TimeSpan(std::chrono::milliseconds(childrenCountTaskbar * 4)));
    }

    systemTrayFrameGridVisual.StartAnimation(L"Offset", trayAnimation);
  }

  if (widgetPresent) {
    float centered_widget = widgetElementVisibleWidth + ((rootWidth - widgetElementWidth) / 2.0f);

    if (centered_widget <= 0) {
      Wh_Log(L"Error: centered_widget<=0");
      return false;
    }
    auto widgetVisual = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(widgetElement);
    if (widgetVisual) {
      auto compositorWidget = widgetVisual.Compositor();
      if (compositorWidget) {
        float targetOffsetXWidget = static_cast<float>(centered_widget + (childrenWidthTaskbar / 2.0f) - (widgetElementWidth - widgetElementVisibleWidth)) + g_settings.userDefinedTrayTaskGap;
        auto widgetOffsetAnimation = compositorWidget.CreateVector3KeyFrameAnimation();

        widgetOffsetAnimation.InsertKeyFrame(1.0f, winrt::Windows::Foundation::Numerics::float3{targetOffsetXWidget, static_cast<float>(abs(g_settings.userDefinedTaskbarHeight - widgetElementVisibleHeight)), taskbarVisual.Offset().z});
        if (movingInwards) {
          widgetOffsetAnimation.DelayTime(winrt::Windows::Foundation::TimeSpan(std::chrono::milliseconds(childrenCountTaskbar * 4)));
        }
        widgetVisual.StartAnimation(L"Offset", widgetOffsetAnimation);
      }
    }
  }

  // borders and corners
  if (state.lastTargetWidth <= 10) {
    state.lastTargetWidth = static_cast<float>(rootWidth);

    if (state.lastTargetWidth <= 0) {
      return false;
    }
  }
  const float targetWidthRect = !g_settings.userDefinedFullWidthTaskbarBackground ? targetWidth : static_cast<float>(rootWidth);
  if (targetWidthRect <= 0) {
    Wh_Log(L"Error: targetWidthRect<=0");
    return false;
  }

  const auto targetHeightPrelim = (!g_settings.userDefinedFullWidthTaskbarBackground ? g_settings.userDefinedTaskbarHeight : xamlRootContent.ActualHeight());
  if (targetHeightPrelim <= 0) {
    Wh_Log(L"Error: targetHeightPrelim<=0");
    return false;
  }
  const auto clipHeight = static_cast<float>(targetHeightPrelim + ((g_settings.userDefinedFlatTaskbarBottomCorners) ? (targetHeightPrelim - g_settings.userDefinedTaskbarCornerRadius) : 0.0f));

  if (clipHeight <= 0) {
    Wh_Log(L"Error: clipHeight<=0");
    return false;
  }
  ProcessStackPanelChildren(stackPanel, clipHeight);
  auto trayOverflowArrowNotifyIconStack = FindChildByName(systemTrayFrameGrid, L"NotifyIconStack");
  if (trayOverflowArrowNotifyIconStack) {
    SetDividerForElement(trayOverflowArrowNotifyIconStack, clipHeight, g_settings.userDefinedTrayAreaDivider);
  } else {
    SetDividerForElement(stackPanel, clipHeight, g_settings.userDefinedTrayAreaDivider);
  }

  if (widgetElement) {
    SetDividerForElement(widgetElement, clipHeight, widgetPresent && g_settings.userDefinedTrayAreaDivider);
  }

  if (!taskbarBackground) return false;

  auto taskbarStroke = FindChildByName(backgroundFillParent, L"BackgroundStroke");
  if (taskbarStroke) {
    taskbarStroke.Opacity(g_unloading ? 1.0 : 0.0);
  }

  auto screenEdgeStroke = FindChildByName(rootGridTaskBar, L"ScreenEdgeStroke");
  if (screenEdgeStroke) {
    screenEdgeStroke.Opacity(g_unloading ? 1.0 : 0.0);
  }

  auto userDefinedTaskbarBackgroundLuminosity = std::to_wstring(g_settings.userDefinedTaskbarBackgroundLuminosity / 100.0f);
  auto userDefinedTaskbarBackgroundOpacity = std::to_wstring(g_settings.userDefinedTaskbarBackgroundOpacity / 100.0f);
  auto userDefinedTaskbarBackgroundTint = std::to_wstring(g_settings.userDefinedTaskbarBackgroundTint / 100.0f);
  SetElementPropertyFromString(backgroundFillChild, L"Windows.UI.Xaml.Shapes.Rectangle", L"Fill",
                               L"<AcrylicBrush TintColor=\"{ThemeResource "
                               L"CardStrokeColorDefaultSolid}\" TintOpacity=\"" +
                                   userDefinedTaskbarBackgroundTint + L"\" TintLuminosityOpacity=\"" + userDefinedTaskbarBackgroundLuminosity + L"\" Opacity=\"" + userDefinedTaskbarBackgroundOpacity + L"\"/>",
                               true);

  auto backgroundFillVisual = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(backgroundFillChild);
  if (backgroundFillVisual) {
    auto compositorTaskBackground = backgroundFillVisual.Compositor();
    if (compositorTaskBackground) {
      auto roundedRect = compositorTaskBackground.CreateRoundedRectangleGeometry();
      roundedRect.CornerRadius({g_settings.userDefinedTaskbarCornerRadius, g_settings.userDefinedTaskbarCornerRadius});

      auto borderGeometry = compositorTaskBackground.CreateRoundedRectangleGeometry();
      borderGeometry.CornerRadius({g_settings.userDefinedTaskbarCornerRadius, g_settings.userDefinedTaskbarCornerRadius});

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
        float offsetXRect = (rootWidth - targetWidth) / 2;
        float newOffsetYRect = userDefinedTaskbarOffsetY < 0 ? static_cast<float>(abs(userDefinedTaskbarOffsetY)) : 0.0f;

        // size animation

        auto sizeAnimationRect = compositorTaskBackground.CreateVector2KeyFrameAnimation();
        sizeAnimationRect.InsertKeyFrame(0.0f, {state.lastTargetWidth, clipHeight});
        sizeAnimationRect.InsertKeyFrame(1.0f, {targetWidthRect, clipHeight});

        auto sizeAnimationBorderGeometry = compositorTaskBackground.CreateVector2KeyFrameAnimation();
        sizeAnimationBorderGeometry.InsertKeyFrame(0.0f, {state.lastTargetWidth - userDefinedTaskbarBorderThicknessFloat, clipHeight - userDefinedTaskbarBorderThicknessFloat});
        sizeAnimationBorderGeometry.InsertKeyFrame(1.0f, {targetWidthRect - userDefinedTaskbarBorderThicknessFloat, clipHeight - userDefinedTaskbarBorderThicknessFloat});

        roundedRect.StartAnimation(L"Size", sizeAnimationRect);
        shapeVisualBorderControl.StartAnimation(L"Size", sizeAnimationRect);
        borderGeometry.StartAnimation(L"Size", sizeAnimationBorderGeometry);

        //   // centering the clip animation

        roundedRect.Offset({state.lastTargetOffsetX, state.lastTargetOffsetY});
        shapeVisualBorderControl.Offset({state.lastTargetOffsetX, state.lastTargetOffsetY, 0.0f});

        auto offsetAnimationRect = compositorTaskBackground.CreateVector2KeyFrameAnimation();
        offsetAnimationRect.InsertKeyFrame(0.0f, {state.lastTargetOffsetX, state.lastTargetOffsetY});
        offsetAnimationRect.InsertKeyFrame(1.0f, {offsetXRect, newOffsetYRect});

        auto offsetAnimationRect3V = compositorTaskBackground.CreateVector3KeyFrameAnimation();
        offsetAnimationRect3V.InsertKeyFrame(0.0f, {state.lastTargetOffsetX, state.lastTargetOffsetY, 0.0f});
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

  state.wasOverflowing = isOverflowing;
  state.lastTargetWidth = targetWidthRect;
 g_invalidateCustomIcons = false;
  return true;
}

void ApplySettings(HWND hTaskbarWnd) {
  RunFromWindowThread(hTaskbarWnd, [](void* pParam) { ApplySettingsFromTaskbarThread(); }, 0);
}

void RefreshSettings() {
  Wh_ModSettingsChangedTBIconSize();
  Wh_ModSettingsChangedStartButtonPosition();

  g_invalidateDimensions = true;

  UpdateGlobalSettings();
}

void ResetGlobalVars() {
  g_invalidateDimensions = true;
 g_invalidateCustomIcons = true;

  for (auto& [key, state] : g_taskbarStates) {
    state.lastTaskbarData.childrenCount = 0;
    state.lastTaskbarData.rightMostEdge = 0;
    // state.lastTaskbarData.childrenWidth = 0;
    state.lastChildrenWidthTaskbar = 0;
    // state.lastTrayFrameWidth = 0;
    state.wasOverflowing = false;
  }
}

void Wh_ModSettingsChanged() {
  Wh_Log(L"Settings Changed");
  ResetGlobalVars();
  RefreshSettings();
  ApplySettingsFromTaskbarThread();
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

  g_unloading = false;

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
  Wh_ModAfterInitTBIconSize();
  HWND hTaskbarWnd = GetTaskbarWnd();
  if (hTaskbarWnd) {
    Wh_ModSettingsChanged();
    ApplySettings(hTaskbarWnd);
    Wh_ModSettingsChanged();
    ApplySettingsDebounced(1000);
  }

//  const std::wstring customTaskbarIconsDir = Wh_GetStringSetting(L"CustomTaskbarIconsDir");
//  const std::wstring customTrayIconsDir = Wh_GetStringSetting(L"CustomTrayIconsDir");
  // if (!customTaskbarIconsDir.empty() || !customTrayIconsDir.empty()) {
  //   InitializeWatchers(customTaskbarIconsDir,customTrayIconsDir);
  // }
}

void Wh_ModBeforeUninit() {
  g_unloading = true;
  Wh_ModBeforeUninitTBIconSize();
  ApplySettingsFromTaskbarThread();
//  CleanupWatchers();
}

void Wh_ModUninit() {
  CleanupDebounce();
  Wh_ModUninitTBIconSize();
  ResetGlobalVars();

  Wh_Log(L"... detached");
}
