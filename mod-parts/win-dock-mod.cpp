using StartDocked__StartSizingFrame_UpdateWindowRegion_WithArgs_t = void(WINAPI*)(void* pThis, winrt::Windows::Foundation::Size param1);
StartDocked__StartSizingFrame_UpdateWindowRegion_WithArgs_t StartDocked__StartSizingFrame_UpdateWindowRegion_WithArgs_Original;
void WINAPI StartDocked__StartSizingFrame_UpdateWindowRegion_WithArgs_Hook(void* pThis, winrt::Windows::Foundation::Size param1) {
  StartDocked__StartSizingFrame_UpdateWindowRegion_WithArgs_Original(pThis, param1);
  Wh_Log(L"Method called: StartDocked__StartSizingFrame_UpdateWindowRegion (Width: %.2f, Height: %.2f)", param1.Width, param1.Height);
  if (g_lastRecordedStartMenuWidth != param1.Width) {
    g_lastRecordedStartMenuWidth = static_cast<int>(param1.Width);
    Wh_SetIntValue(L"lastRecordedStartMenuWidth", g_lastRecordedStartMenuWidth);
  }
}

bool g_invalidateDimensions = true;
std::atomic<int64_t> g_update_flag_set_time_ms = 0;
int64_t NowMs() { return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count(); }

void ResetFlagAfterDelay() {
  std::this_thread::sleep_for(std::chrono::milliseconds(1400));
  int64_t now = NowMs();
  int64_t set_time = g_update_flag_set_time_ms.load();
  if (g_scheduled_low_priority_update && (now - set_time >= 1400)) {
    g_scheduled_low_priority_update = false;
  }
}

void ApplySettingsFromTaskbarThreadIfRequired() {
  if (!g_scheduled_low_priority_update) {
    g_scheduled_low_priority_update = true;
    g_update_flag_set_time_ms = NowMs();
    std::thread(ResetFlagAfterDelay).detach();
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

double CalculateValidChildrenWidth(FrameworkElement element, int& childrenCount) {
  if (!element) return 0.0;
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
    if (rect.Y < 0) {
      continue;
    }
    auto className = winrt::get_class_name(child);
    SetElementPropertyFromString(child, className.c_str(), L"CornerRadius", userDefinedTaskButtonCornerRadius);

    if (className == L"Taskbar.TaskListButton" || className == L"Taskbar.ExperienceToggleButton" || className == L"Taskbar.OverflowToggleButton") {
      child.MinWidth(g_settings.userDefinedTaskbarButtonSize);
      //   child.Width(g_settings.userDefinedTaskbarButtonSize);

      auto innerElementChild = FindChildByClassName(child, L"Taskbar.TaskListButtonPanel");
      if (innerElementChild) {
        innerElementChild.MinWidth(g_settings.userDefinedTaskbarButtonSize);
        // innerElementChild.Width(g_settings.userDefinedTaskbarButtonSize);
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
        if(g_settings.userDefinedTrayAreaDivider){
            RightOverflowButtonDivider.Opacity(0);
        }else{
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
    childrenCount++;
  }
  return totalWidth;
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
  g_settings.userDefinedDividerLeftAligned = (_wcsicmp(appsDividerAlignment,L"left")==0);
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
  PCWSTR dividerAppNames=Wh_GetStringSetting(L"DividedAppNames");
  g_settings.userDefinedDividedAppNames = SplitAndTrim(dividerAppNames);
  Wh_FreeStringSetting(dividerAppNames);

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
bool ApplyStyle(FrameworkElement const& xamlRootContent, std::wstring monitorName) {
  if (!xamlRootContent) {
    Wh_Log(L"xamlRootContent is null");
    return false;
  }

  auto& state = g_taskbarStates[monitorName];

  Wh_Log(L"ApplyStyle for monitor: %s", monitorName.c_str());

  g_scheduled_low_priority_update = false;

  auto now = std::chrono::steady_clock::now();
  if (!g_unloading && now - state.lastApplyStyleTime < std::chrono::milliseconds(200)) {
    return true;
  }
  state.lastApplyStyleTime = now;

  if (!xamlRootContent) return false;


  auto taskFrame = FindChildByClassName(xamlRootContent, L"Taskbar.TaskbarFrame");
  if (!taskFrame) {
    Wh_Log(L"Failed to find Taskbar.TaskbarFrame");
    return false;
  }

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

  if (!g_unloading && rootWidth < 100) {
    Wh_Log(L"root width is too small");
    return false;
  }

  int childrenCountTaskbar = 0;
  const double childrenWidthTaskbarDbl = CalculateValidChildrenWidth(taskbarFrameRepeater, childrenCountTaskbar);

  if (!g_unloading && childrenWidthTaskbarDbl <= 0) {
    Wh_Log(L"Error: childrenWidthTaskbarDbl <= 0");
    return false;
  }
  unsigned int childrenWidthTaskbar = static_cast<unsigned int>(childrenWidthTaskbarDbl);
  signed int rightMostEdgeTaskbar = childrenWidthTaskbar+10;

  if (!g_unloading && childrenCountTaskbar < 1) {
    Wh_Log(L"Error: childrenCountTaskbar < 1");
    return false;
  }

  if (!g_unloading && childrenWidthTaskbar <= 10) {
    Wh_Log(L"Error: childrenWidthTaskbar <= 10");
    return false;
  }

  if (!g_unloading && rightMostEdgeTaskbar < 0) {
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
  if (trayFrame.GetValue(FrameworkElement::HorizontalAlignmentProperty()).as<winrt::Windows::Foundation::IReference<HorizontalAlignment>>().Value() == HorizontalAlignment::Center) {
    trayFrame.SetValue(FrameworkElement::HorizontalAlignmentProperty(), winrt::box_value(HorizontalAlignment::Right));
  }

  int childrenCountTray = 0;
  double trayFrameWidthDbl = CalculateValidChildrenWidth(systemTrayFrameGrid, childrenCountTray);

  if (!g_unloading && trayFrameWidthDbl <= 0) {
    Wh_Log(L"Error: trayFrameWidthDbl <= 0");
    return false;
  }

  if (!g_unloading && childrenCountTray <= 0) {
    Wh_Log(L"Error: childrenCountTray <= 0");
    return false;
  }

  float showDesktopButtonWidth = static_cast<float>(g_settings.userDefinedIgnoreShowDesktopButton ? showDesktopButton.ActualWidth() : 0);
  int trayGapPlusExtras = g_settings.userDefinedTrayTaskGap + widgetElementVisibleWidth + (widgetPresent ? -6+g_settings.userDefinedTrayTaskGap : 0);
  const unsigned int trayFrameWidth = static_cast<unsigned int>(trayFrameWidthDbl - showDesktopButtonWidth + trayGapPlusExtras);

  if (!g_unloading && childrenCountTray == 0) {
    Wh_Log(L"Error: childrenCountTray == 0");
    return false;
  }

  if (!g_unloading && trayFrameWidth <= 1) {
    Wh_Log(L"Error: trayFrameWidth <= 1");
    return false;
  }

  float centeredTray = (rootWidth - trayFrameWidth) / 2.0f;

  if (!g_unloading && centeredTray <= 1) {
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
  state.lastTargetTaskFrameOffsetX = targetTaskFrameOffsetX;
  // 5 pixels tolerance
  if (!g_invalidateDimensions && !g_unloading && abs(newXOffsetTray - systemTrayFrameGridVisual.Offset().x) <= 5 && childrenWidthTaskbar == state.lastChildrenWidthTaskbar && trayFrameWidth == state.lastTrayFrameWidth && abs(targetTaskFrameOffsetX - taskbarFrameRepeaterVisual.Offset().x) <= 5) {
    Wh_Log(L"newXOffsetTray is within 5 pixels of systemTrayFrameGridVisual offset %f, childrenWidthTaskbar and trayFrameWidth didn't change: %d, %d", systemTrayFrameGridVisual.Offset().x, childrenWidthTaskbar, state.lastTrayFrameWidth);
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
  state.lastStartButtonX = (rootWidth - targetWidth) / 2.0f;

  auto heightValue = (g_settings.userDefinedTaskbarHeight + abs(userDefinedTaskbarOffsetY < 0 ? (userDefinedTaskbarOffsetY * 2) : 0));

  if (heightValue < g_settings.userDefinedTaskbarHeight / 2) {
    Wh_Log(L"Error: heightValue<g_settings.userDefinedTaskbarHeight/2");
    return false;
  }
  if (g_invalidateDimensions) {
    g_invalidateDimensions=false;
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
    if (!g_unloading) {
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
    if (!g_unloading) {
      float targetOffsetXTray = static_cast<float>(newXOffsetTray - trayGapPlusExtras - (rootWidth - trayFrameWidth));
      auto trayAnimation = trayVisualCompositor.CreateVector3KeyFrameAnimation();
      trayAnimation.InsertKeyFrame(1.0f, winrt::Windows::Foundation::Numerics::float3{targetOffsetXTray, systemTrayFrameGridVisual.Offset().y, systemTrayFrameGridVisual.Offset().z});
      if (movingInwards) {
        trayAnimation.DelayTime(winrt::Windows::Foundation::TimeSpan(std::chrono::milliseconds(childrenCountTaskbar * 4)));
      }
      systemTrayFrameGridVisual.StartAnimation(L"Offset", trayAnimation);
    } else {
      systemTrayFrameGridVisual.Offset({0.0f, 0.0f, 0.0f});
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
          float targetOffsetXWidget = static_cast<float>(rightMostEdgeTaskbar -8) +g_settings.userDefinedTrayTaskGap;
          auto widgetOffsetAnimation = compositorWidget.CreateVector3KeyFrameAnimation();
          widgetOffsetAnimation.InsertKeyFrame(1.0f, winrt::Windows::Foundation::Numerics::float3{static_cast<float>(targetOffsetXWidget), static_cast<float>(abs(g_settings.userDefinedTaskbarHeight - widgetElementVisibleHeight)), taskbarVisual.Offset().z});
          if (movingInwards) {
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

  int rightMostEdgeTray = static_cast<int>((rootWidth - targetWidth) / 2 + targetWidth);
  if (state.lastRightMostEdgeTray != rightMostEdgeTray) {
    state.lastRightMostEdgeTray = rightMostEdgeTray;
    Wh_SetIntValue((L"lastRightMostEdgeTray_" + monitorName).c_str(), rightMostEdgeTray);
  }
  float leftMostEdgeTray = rightMostEdgeTray - trayFrameWidth;
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

  auto userDefinedTaskbarBackgroundLuminosity = std::to_wstring(g_settings.userDefinedTaskbarBackgroundLuminosity / 100.0f);
  auto userDefinedTaskbarBackgroundOpacity = std::to_wstring(g_settings.userDefinedTaskbarBackgroundOpacity / 100.0f);
  auto userDefinedTaskbarBackgroundTint = std::to_wstring(g_settings.userDefinedTaskbarBackgroundTint / 100.0f);
  if (g_settings.userDefinedCustomizeTaskbarBackground) {
    SetElementPropertyFromString(backgroundFillChild, L"Windows.UI.Xaml.Shapes.Rectangle", L"Fill",
                                 L"<AcrylicBrush TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" FallbackColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\"" + userDefinedTaskbarBackgroundTint + L"\" TintLuminosityOpacity=\"" + userDefinedTaskbarBackgroundLuminosity +
                                     L"\" Opacity=\"" + userDefinedTaskbarBackgroundOpacity + L"\"/>",
                                 true);
  }

  // you can also try SystemAccentColor
  auto backgroundFillVisual = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(backgroundFillChild);
  auto compositorTaskBackground = backgroundFillVisual.Compositor();

  // borders and corners
  if (!g_unloading) {
    if (backgroundFillVisual) {
      if (compositorTaskBackground) {
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
      backgroundFillVisual.Clip(nullptr);
    }
    if (compositorTaskBackground) {
      winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::SetElementChildVisual(backgroundFillChild, compositorTaskBackground.CreateShapeVisual());
    }
  }

  state.wasOverflowing = isOverflowing;
  state.lastTargetWidth = targetWidthRect;
  state.lastTargetWidth = targetWidth;
  return true;
}

void ApplySettings(HWND hTaskbarWnd) {
  RunFromWindowThread(hTaskbarWnd, [](void* pParam) { ApplySettingsFromTaskbarThread(); }, 0);
}

void RefreshSettings() {
  g_invalidateDimensions = true;
  Wh_ModSettingsChangedTBIconSize();
  Wh_ModSettingsChangedStartButtonPosition();
  UpdateGlobalSettings();
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

      X = static_cast<int>(lastRecordedTrayRightMostEdgeForMonitor * dpiScale + (12 * dpiScale) - (Wh_GetIntSetting(L"AlignFlyoutInner") ? cx : (cx / 2.0f)));
      Wh_Log(L"[SetWindowPos] New X %d", X);
    } else {
      Wh_Log(L"[SetWindowPos] No reference state for monitor %s", monitorName.c_str());
    }
  }

  return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}
BOOL Wh_ModInit() {
  Wh_Log(L"======================================================");
  HMODULE moduleUser32 = LoadLibraryW(L"user32.dll");
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
  Wh_ModSettingsChanged();
  ApplySettingsDebounced(300);
}

void Wh_ModBeforeUninit() {
  if (g_PartialMode) {
    return;
  }
  g_unloading = true;
  Wh_ModBeforeUninitTBIconSize();
  Wh_ModBeforeUninitStartButtonPosition();
  RefreshSettings();
  HWND hTaskbarWnd = GetTaskbarWnd();
  if (hTaskbarWnd) {
    ApplySettings(hTaskbarWnd);
  }
}

void Wh_ModUninit() {
  if (g_PartialMode) {
    return;
  }
  Wh_ModUninitTBIconSize();
  ResetGlobalVars();
  CleanupDebounce();

  Wh_Log(L"... detached");
}
