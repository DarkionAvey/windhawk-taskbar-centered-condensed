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
} g_settings;
struct {
  int childrenCount = 0;
  double rightMostEdge = 0.0;
  double childrenWidth = 0.0;
} g_lastTaskbarData;

bool g_invalidateDimensions = true;
bool g_wasOverflowing = false;
signed int g_initOffsetX = -1;
float g_lastTargetOffsetX = 0.0f, g_lastTargetWidth = 0.0f, g_lastTargetOffsetY = 0.0f;

unsigned int g_lastChildrenWidthTaskbar = 0, g_lastTrayFrameWidth = 0;

void ApplySettingsFromTaskbarThreadIfRequired() {
  if (g_invalidateDimensions) {
//    ApplySettingsFromTaskbarThread();
  }
}

void SetDividerForElement(FrameworkElement element, bool dividerVisible){
      if (dividerVisible) {
    auto compositor = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(element).Compositor();
    auto shapeVisual = compositor.CreateShapeVisual();
    float panelHeight = static_cast<float>(g_settings.userDefinedTaskbarHeight);
    float panelWidth = static_cast<float>(element.ActualWidth());
    shapeVisual.Size({static_cast<float>(g_settings.userDefinedTaskbarBorderThickness), static_cast<float>(panelHeight - g_settings.userDefinedTaskbarBorderThickness * 2.0f)});
    auto borderGeometry = compositor.CreateRoundedRectangleGeometry();
    borderGeometry.Size({static_cast<float>(panelWidth - g_settings.userDefinedTaskbarBorderThickness), static_cast<float>(panelHeight - g_settings.userDefinedTaskbarBorderThickness * 2.0f)});
    auto borderShape = compositor.CreateSpriteShape(borderGeometry);
    winrt::Windows::UI::Color borderColor = {g_settings.userDefinedTaskbarBorderOpacity, static_cast<BYTE>(g_settings.borderColorR), static_cast<BYTE>(g_settings.borderColorG), static_cast<BYTE>(g_settings.borderColorB)};
    borderShape.StrokeBrush(compositor.CreateColorBrush(borderColor));
    borderShape.StrokeThickness(g_settings.userDefinedTaskbarBorderThickness);
    borderShape.FillBrush(nullptr);
    borderGeometry.Offset({static_cast<float>(g_settings.userDefinedTaskbarBorderThickness / 2.0f), static_cast<float>(g_settings.userDefinedTaskbarBorderThickness / 2.0f)});
    shapeVisual.Shapes().Append(borderShape);
    shapeVisual.Offset({g_settings.userDefinedTrayTaskGap / -2.0f, static_cast<float>(g_settings.userDefinedTaskbarBorderThickness), 0.0f});
    winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::SetElementChildVisual(element, shapeVisual);
  } else {
    winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::SetElementChildVisual(element, nullptr);
  }
}

void ProcessStackPanelChildren(FrameworkElement const& stackPanel) {
    SetDividerForElement(stackPanel,g_settings.userDefinedTrayAreaDivider);

  if (!g_settings.userDefinedStyleTrayArea) return;

  auto userDefinedTaskButtonCornerRadius = std::to_wstring(g_settings.userDefinedTaskButtonCornerRadius);
  int childCount = Media::VisualTreeHelper::GetChildrenCount(stackPanel);
  for (int i = 0; i < childCount; ++i) {
    auto child = Media::VisualTreeHelper::GetChild(stackPanel, i).try_as<FrameworkElement>();
    if (!child) continue;
    child.Width(g_settings.userDefinedTrayButtonSize);
    child.Height(g_settings.userDefinedTaskbarHeight);

    auto notifyItemIcon = FindChildByName(child, L"NotifyItemIcon");
    if (!notifyItemIcon) continue;

    auto containerGrid = FindChildByName(notifyItemIcon, L"ContainerGrid");
    if (!containerGrid) continue;

    SetElementPropertyFromString(containerGrid, L"Windows.UI.Xaml.Controls.Grid", L"CornerRadius", userDefinedTaskButtonCornerRadius);

    auto innerContentPresenter = FindChildByName(containerGrid, L"ContentPresenter");
    if (!innerContentPresenter) continue;

    auto contentGrid = FindChildByName(innerContentPresenter, L"ContentGrid");
    if (!contentGrid) continue;

    auto imageIconContent = FindChildByClassName(contentGrid, L"SystemTray.ImageIconContent");
    if (!imageIconContent) continue;

    auto innerContainerGrid = FindChildByName(imageIconContent, L"ContainerGrid");
    if (!innerContainerGrid) continue;

    auto image = FindChildByClassName(innerContainerGrid, L"Windows.UI.Xaml.Controls.Image");
    if (image) {
      image.Width(g_settings.userDefinedTrayIconSize);
      image.Height(g_settings.userDefinedTrayIconSize);
    }
  }
}

double CalculateValidChildrenWidth(FrameworkElement element, int& childrenCount) {
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
    // todo: check this
    if (className == L"Taskbar.TaskListButton") {
      auto innerElementChild = FindChildByClassName(child, L"Taskbar.TaskListLabeledButtonPanel");
      if (innerElementChild) {
        auto iconElementChild = FindChildByName(innerElementChild, L"Icon");

        if (iconElementChild) {
          iconElementChild.Width(g_settings.userDefinedTaskbarIconSize);
          iconElementChild.Height(g_settings.userDefinedTaskbarIconSize);

          auto currentIconAppName = child.GetValue(winrt::Windows::UI::Xaml::Automation::AutomationProperties::NameProperty());
          auto currentIconAppNameStr = winrt::unbox_value<winrt::hstring>(currentIconAppName);
          Wh_Log(L"bbwi: %s", currentIconAppNameStr.c_str());

          if (g_unloading) {
            iconElementChild.Tag(nullptr);
          } else {
            UpdateIcons(iconElementChild, currentIconAppNameStr);
          }
        }
      }
    }
    else if(className==L"Taskbar.AugmentedEntryPointButton"){ //widget element
child.Margin(Thickness{0, 0, 0, 0});

auto ExperienceToggleButtonRootPanelElement =        FindChildByName(child, L"ExperienceToggleButtonRootPanel");

if(ExperienceToggleButtonRootPanelElement){
    ExperienceToggleButtonRootPanelElement.Margin(Thickness{0, 0, 0, 0});
}
continue;
    }

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
  }
  return totalWidth;
}

bool ApplyStyle(FrameworkElement xamlRootContent) {
  bool invalidateLayoutRequest = g_invalidateDimensions;
  g_invalidateDimensions = false;

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

  auto notificationAreaIcons = FindChildByName(systemTrayFrameGrid, L"NotificationAreaIcons");
  if (!notificationAreaIcons) return false;

  auto itemsPresenter = FindChildByClassName(notificationAreaIcons, L"Windows.UI.Xaml.Controls.ItemsPresenter");
  if (!itemsPresenter) return false;

  auto stackPanel = FindChildByClassName(itemsPresenter, L"Windows.UI.Xaml.Controls.StackPanel");
  if (!stackPanel) return false;


auto widgetElement = FindChildByClassName(taskbarFrameRepeater, L"Taskbar.AugmentedEntryPointButton");
  bool widgetPresent=widgetElement!=nullptr && winrt::unbox_value<bool>(
    widgetElement.GetValue(UIElement::CanBeScrollAnchorProperty())
);
auto widgetElementWidth=widgetPresent?widgetElement.ActualWidth():0;

auto widgetElementInnerChild =widgetPresent? FindChildByClassName(widgetElement, L"Taskbar.TaskListButtonPanel"): nullptr;
if(widgetElement){
        SetDividerForElement(widgetElement,widgetPresent&&g_settings.userDefinedTrayAreaDivider);
}


auto widgetElementVisibleWidth=widgetElementInnerChild?widgetElementInnerChild.ActualWidth():0;
auto widgetElementVisibleHeight=widgetElementInnerChild?widgetElementInnerChild.ActualHeight():0;


  ProcessStackPanelChildren(stackPanel);

  auto overflowButton = FindChildByClassName(taskbarFrameRepeater, L"Taskbar.OverflowToggleButton");
  bool isOverflowing = overflowButton != nullptr && !IsWeirdFrameworkElement(overflowButton);

  double rootWidth = xamlRootContent.ActualWidth();

  int childrenCountTaskbar = 0;
  const double childrenWidthTaskbarDbl = CalculateValidChildrenWidth(taskbarFrameRepeater, childrenCountTaskbar);

  if(childrenWidthTaskbarDbl<=0)return false;

  signed int rightMostEdgeTaskbar = static_cast<signed int>((rootWidth / 2.0) + (childrenWidthTaskbarDbl / 2.0));
  unsigned int childrenWidthTaskbar = static_cast<unsigned int>(childrenWidthTaskbarDbl);

  if (childrenCountTaskbar < 1 || childrenWidthTaskbar <= 1) {
    return false;
  }

  bool rightMostEdgeChangedTaskbar = (g_lastTaskbarData.rightMostEdge != rightMostEdgeTaskbar);

  if (!isOverflowing && (rightMostEdgeChangedTaskbar || g_lastTaskbarData.rightMostEdge == 0.0)) {
    g_lastTaskbarData.childrenCount = childrenCountTaskbar;
    g_lastTaskbarData.rightMostEdge = rightMostEdgeTaskbar;
    g_lastTaskbarData.childrenWidth = childrenWidthTaskbar;
  }

  int childrenCountTray = 0;
  double trayFrameWidthDbl = CalculateValidChildrenWidth(systemTrayFrameGrid, childrenCountTray);
  float showDesktopButtonWidth = static_cast<float>(g_settings.userDefinedIgnoreShowDesktopButton ? showDesktopButton.ActualWidth() : 0);
  int trayGapPlusExtras=g_settings.userDefinedTrayTaskGap+widgetElementVisibleWidth+(widgetPresent?g_settings.userDefinedTrayTaskGap:0);
  const unsigned int trayFrameWidth = static_cast<unsigned int>(trayFrameWidthDbl - showDesktopButtonWidth + trayGapPlusExtras);

  if (childrenCountTray == 0 || trayFrameWidth <= 1) {
    return false;
  }

  float centeredTray = (rootWidth - trayFrameWidth) / 2.0f;
  float newXOffsetTray = centeredTray + (childrenWidthTaskbar / 2.0f) + trayGapPlusExtras;
  // tray animations
  auto trayVisual = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(trayFrame);
  auto originalOffset = trayVisual.Offset();
  if (g_initOffsetX == -1) {
    g_initOffsetX = originalOffset.x;
  }

  auto taskbarFrameRepeaterVisual = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(taskbarFrameRepeater);
  float targetTaskFrameOffsetX = newXOffsetTray - rightMostEdgeTaskbar - trayGapPlusExtras;

  // 5 pixels tolerance
  if (!invalidateLayoutRequest && abs(newXOffsetTray - trayVisual.Offset().x) <= 5 && childrenWidthTaskbar == g_lastChildrenWidthTaskbar && trayFrameWidth == g_lastTrayFrameWidth && abs(targetTaskFrameOffsetX - taskbarFrameRepeaterVisual.Offset().x) <= 5) {
    Wh_Log(L"newXOffsetTray is within 5 pixels of trayVisual offset %f", trayVisual.Offset().x);
    Wh_Log(L"childrenWidthTaskbar and trayFrameWidth didn't change: %d, %d", childrenWidthTaskbar, g_lastTrayFrameWidth);
    return true;
  }

  g_lastChildrenWidthTaskbar = static_cast<unsigned int>(childrenWidthTaskbar);
  g_lastTrayFrameWidth = static_cast<unsigned int>(trayFrameWidth);

  signed int userDefinedTaskbarOffsetY = (g_settings.userDefinedFlatTaskbarBottomCorners || g_settings.userDefinedFullWidthTaskbarBackground) ? 0 : g_settings.userDefinedTaskbarOffsetY;
  float targetWidth = g_unloading ? rootWidth : (childrenWidthTaskbar + trayFrameWidth + (g_settings.userDefinedTaskbarBackgroundHorizontalPadding * 2));

  auto heightValue = (g_settings.userDefinedTaskbarHeight + abs(userDefinedTaskbarOffsetY < 0 ? (userDefinedTaskbarOffsetY * 2) : 0));

  if (ScrollViewer.Width() != rootWidth) {
    invalidateLayoutRequest = true;
  }
  if (invalidateLayoutRequest) {
    taskFrame.SetValue(FrameworkElement::WidthProperty(), winrt::box_value(std::numeric_limits<double>::quiet_NaN()));
    trayFrame.SetValue(FrameworkElement::HorizontalAlignmentProperty(), winrt::box_value(HorizontalAlignment::Center));

    ScrollViewer.Width(rootWidth);
    ScrollViewer.MaxWidth(rootWidth);
    ScrollViewer.Height(heightValue);
    ScrollViewer.MaxHeight(heightValue);

    trayFrame.Height(g_settings.userDefinedTaskbarHeight);
    trayFrame.MaxHeight(g_settings.userDefinedTaskbarHeight);

    borderView.Height(heightValue);
    borderView.MaxHeight(heightValue);

    taskFrame.Height(heightValue);
    taskFrame.MaxHeight(heightValue);

    taskbarFrameRepeater.Height(g_settings.userDefinedTaskbarHeight);
    taskbarFrameRepeater.MaxHeight(g_settings.userDefinedTaskbarHeight);

    taskbarFrameRepeater.Width(rootWidth);
    taskbarFrameRepeater.MaxWidth(rootWidth);
  }

  auto taskbarFrameRepeaterVisualAnimation = taskbarFrameRepeaterVisual.Compositor().CreateVector3KeyFrameAnimation();

  auto animationControllerTaskbarFrameRepeaterVisual = taskbarFrameRepeaterVisual.TryGetAnimationController(L"Offset");
  if (animationControllerTaskbarFrameRepeaterVisual) {
    taskbarFrameRepeaterVisual.StopAnimation(L"Offset");
  }
  taskbarFrameRepeaterVisualAnimation.InsertKeyFrame(1.0f, winrt::Windows::Foundation::Numerics::float3{targetTaskFrameOffsetX, taskbarFrameRepeaterVisual.Offset().y, taskbarFrameRepeaterVisual.Offset().z});
  taskbarFrameRepeaterVisual.StartAnimation(L"Offset", taskbarFrameRepeaterVisualAnimation);

  boolean movingInwards = originalOffset.x > newXOffsetTray;

  auto taskbarVisual = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(taskbarFrameRepeater);
  auto compositorTaskbar = taskbarVisual.Compositor();
  float targetOffsetXTray = (g_unloading ? (float)g_initOffsetX : static_cast<float>(newXOffsetTray));
  auto trayAnimation = compositorTaskbar.CreateVector3KeyFrameAnimation();

  trayAnimation.InsertKeyFrame(1.0f, winrt::Windows::Foundation::Numerics::float3{targetOffsetXTray, taskbarVisual.Offset().y, taskbarVisual.Offset().z});
  if (movingInwards) {
    trayAnimation.DelayTime(winrt::Windows::Foundation::TimeSpan(std::chrono::milliseconds(childrenCountTaskbar * 4)));
  }

  trayVisual.StartAnimation(L"Offset", trayAnimation);

if(widgetPresent){
    float centered_widget=widgetElementVisibleWidth + ((rootWidth-widgetElementWidth)/2.0f);

      auto widgetVisual = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(widgetElement);
//         auto animationControllerWidgetVisual = widgetVisual.TryGetAnimationController(L"Offset");

//         if (animationControllerWidgetVisual) {
//     animationControllerWidgetVisual.StopAnimation(L"Offset");
//   }
  auto compositorWidget = widgetVisual.Compositor();
  float targetOffsetXWidget = static_cast<float>(centered_widget+(childrenWidthTaskbar / 2.0f)- (widgetElementWidth-widgetElementVisibleWidth))+ g_settings.userDefinedTrayTaskGap;
  auto widgetOffsetAnimation = compositorWidget.CreateVector3KeyFrameAnimation();

  widgetOffsetAnimation.InsertKeyFrame(1.0f, winrt::Windows::Foundation::Numerics::float3{targetOffsetXWidget, static_cast<float>(abs(g_settings.userDefinedTaskbarHeight - widgetElementVisibleHeight)), taskbarVisual.Offset().z});
//   if (movingInwards) {
//     widgetOffsetAnimation.DelayTime(winrt::Windows::Foundation::TimeSpan(std::chrono::milliseconds(childrenCountTaskbar * 4)));
//   }
  widgetVisual.StartAnimation(L"Offset", widgetOffsetAnimation);
}



  if (!taskbarBackground) return false;

  auto taskbarStroke = FindChildByName(backgroundFillParent, L"BackgroundStroke");
  if (taskbarStroke) {
    taskbarStroke.Opacity(g_unloading ? 1 : 0);
  }

  auto userDefinedTaskbarBackgroundLuminosity = std::to_wstring(g_settings.userDefinedTaskbarBackgroundLuminosity / 100.0f);
  auto userDefinedTaskbarBackgroundOpacity = std::to_wstring(g_settings.userDefinedTaskbarBackgroundOpacity / 100.0f);
  auto userDefinedTaskbarBackgroundTint = std::to_wstring(g_settings.userDefinedTaskbarBackgroundTint / 100.0f);
  SetElementPropertyFromString(backgroundFillChild, L"Windows.UI.Xaml.Shapes.Rectangle", L"Fill",
                               L"<AcrylicBrush TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\"" + userDefinedTaskbarBackgroundTint + L"\" TintLuminosityOpacity=\"" + userDefinedTaskbarBackgroundLuminosity + L"\" Opacity=\"" +
                                   userDefinedTaskbarBackgroundOpacity + L"\"/>",
                               true);

  // borders and corners

  const float targetWidthRect = !g_settings.userDefinedFullWidthTaskbarBackground ? targetWidth : static_cast<float>(rootWidth);
  const auto targetHeightPrelim = (!g_settings.userDefinedFullWidthTaskbarBackground ? g_settings.userDefinedTaskbarHeight : ScrollViewer.ActualHeight());
  const auto clipHeight = static_cast<float>(targetHeightPrelim + ((g_settings.userDefinedFlatTaskbarBottomCorners) ? (targetHeightPrelim - g_settings.userDefinedTaskbarCornerRadius) : 0.0f));

  auto backgroundFillVisual = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(backgroundFillChild);
  auto compositorTaskBackground = backgroundFillVisual.Compositor();

  auto roundedRect = compositorTaskBackground.CreateRoundedRectangleGeometry();
  roundedRect.CornerRadius({g_settings.userDefinedTaskbarCornerRadius, g_settings.userDefinedTaskbarCornerRadius});

  auto borderGeometry = compositorTaskBackground.CreateRoundedRectangleGeometry();
  borderGeometry.CornerRadius({g_settings.userDefinedTaskbarCornerRadius, g_settings.userDefinedTaskbarCornerRadius});

  roundedRect.Size({!g_settings.userDefinedFullWidthTaskbarBackground ? g_lastTargetWidth : targetWidthRect, clipHeight});
  borderGeometry.Offset({static_cast<float>(g_settings.userDefinedTaskbarBorderThickness / 2.0f), static_cast<float>(g_settings.userDefinedTaskbarBorderThickness / 2.0f)});

  auto shapeVisualBorderControl = compositorTaskBackground.CreateShapeVisual();
  shapeVisualBorderControl.Size({static_cast<float>(!g_settings.userDefinedFullWidthTaskbarBackground ? g_lastTargetWidth : targetWidthRect), static_cast<float>(clipHeight)});

  auto geometricClip = compositorTaskBackground.CreateGeometricClip(roundedRect);
  auto borderShape = compositorTaskBackground.CreateSpriteShape(borderGeometry);
  winrt::Windows::UI::Color borderColor = {g_settings.userDefinedTaskbarBorderOpacity, static_cast<BYTE>(g_settings.borderColorR), static_cast<BYTE>(g_settings.borderColorG), static_cast<BYTE>(g_settings.borderColorB)};
  borderShape.StrokeBrush(compositorTaskBackground.CreateColorBrush(borderColor));
  borderShape.StrokeThickness(g_settings.userDefinedTaskbarBorderThickness);
  borderShape.FillBrush(nullptr);

  borderGeometry.Size({static_cast<float>(targetWidthRect - g_settings.userDefinedTaskbarBorderThickness), static_cast<float>(clipHeight - g_settings.userDefinedTaskbarBorderThickness)});

  backgroundFillVisual.Clip(geometricClip);
  shapeVisualBorderControl.Shapes().Append(borderShape);
  winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::SetElementChildVisual(backgroundFillChild, shapeVisualBorderControl);

  if (!g_settings.userDefinedFullWidthTaskbarBackground) {
    float offsetXRect = (rootWidth - targetWidth) / 2;
    float newOffsetYRect = userDefinedTaskbarOffsetY < 0 ? static_cast<float>(abs(userDefinedTaskbarOffsetY)) : 0.0f;

    // size animation
    if (roundedRect.TryGetAnimationController(L"Size")) {
      roundedRect.StopAnimation(L"Size");
    }

    if (shapeVisualBorderControl.TryGetAnimationController(L"Size")) {
      shapeVisualBorderControl.StopAnimation(L"Size");
    }

    auto sizeAnimationRect = compositorTaskBackground.CreateVector2KeyFrameAnimation();
    sizeAnimationRect.InsertKeyFrame(0.0f, {g_lastTargetWidth, clipHeight});
    sizeAnimationRect.InsertKeyFrame(1.0f, {targetWidthRect, clipHeight});

    roundedRect.StartAnimation(L"Size", sizeAnimationRect);
    shapeVisualBorderControl.StartAnimation(L"Size", sizeAnimationRect);

    //   // centering the clip animation
    if (roundedRect.TryGetAnimationController(L"Offset")) {
      roundedRect.StopAnimation(L"Offset");
    }
    if (shapeVisualBorderControl.TryGetAnimationController(L"Offset")) {
      shapeVisualBorderControl.StopAnimation(L"Offset");
    }

    roundedRect.Offset({g_lastTargetOffsetX, g_lastTargetOffsetY});
    shapeVisualBorderControl.Offset({g_lastTargetOffsetX, g_lastTargetOffsetY, 0.0f});

    auto offsetAnimationRect = compositorTaskBackground.CreateVector2KeyFrameAnimation();
    offsetAnimationRect.InsertKeyFrame(0.0f, {g_lastTargetOffsetX, g_lastTargetOffsetY});
    offsetAnimationRect.InsertKeyFrame(1.0f, {offsetXRect, newOffsetYRect});

    auto offsetAnimationRect3V = compositorTaskBackground.CreateVector3KeyFrameAnimation();
    offsetAnimationRect3V.InsertKeyFrame(0.0f, {g_lastTargetOffsetX, g_lastTargetOffsetY, 0.0f});
    offsetAnimationRect3V.InsertKeyFrame(1.0f, {offsetXRect, newOffsetYRect, 0.0f});

    roundedRect.StartAnimation(L"Offset", offsetAnimationRect);
    shapeVisualBorderControl.StartAnimation(L"Offset", offsetAnimationRect3V);
    g_lastTargetOffsetX = offsetXRect;
    g_lastTargetOffsetY = newOffsetYRect;
  } else {
    g_lastTargetOffsetX = 0;
    g_lastTargetOffsetY = 0;
    roundedRect.Offset({g_lastTargetOffsetX, g_lastTargetOffsetY});
    shapeVisualBorderControl.Offset({g_lastTargetOffsetX, g_lastTargetOffsetY, 0.0f});
  }

  g_wasOverflowing = isOverflowing;
  g_lastTargetWidth = targetWidthRect;

  return true;
}

void ApplySettings(HWND hTaskbarWnd) {
  RunFromWindowThread(hTaskbarWnd, [](void* pParam) { ApplySettingsFromTaskbarThread(); }, 0);
}

using TaskbarTelemetry_StartItemEntranceAnimation_t            = void (WINAPI *)(const bool&);
static TaskbarTelemetry_StartItemEntranceAnimation_t            orig_StartItemEntranceAnimation            = nullptr;

using TaskbarTelemetry_StartItemPlateEntranceAnimation_t       = void (WINAPI *)(const bool&);
static TaskbarTelemetry_StartItemPlateEntranceAnimation_t       orig_StartItemPlateEntranceAnimation       = nullptr;


void WINAPI Hook_StartItemEntranceAnimation_call(const bool& b) {
    Wh_Log(L"[Hook] TaskbarTelemetry::StartItemEntranceAnimation(%d)", b);
    orig_StartItemEntranceAnimation(b);
    ApplySettingsDebounced();
}

void WINAPI Hook_StartItemPlateEntranceAnimation_call(const bool& b) {
    Wh_Log(L"[Hook] TaskbarTelemetry::StartItemPlateEntranceAnimation(%d)", b);
    orig_StartItemPlateEntranceAnimation(b);
    ApplySettingsDebounced();
}

bool Hook_StartItemEntranceAnimation() {
    HMODULE module = LoadLibrary(L"Taskbar.View.dll");
    if (!module) {
        Wh_Log(L"Failed to load Taskbar.View.dll for StartItemEntranceAnimation");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = {{
        { LR"(public: static void __cdecl TaskbarTelemetry::StartItemEntranceAnimation<bool const &>(bool const &))" },
        &orig_StartItemEntranceAnimation,
        Hook_StartItemEntranceAnimation_call
    }};
    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}
bool Hook_StartItemPlateEntranceAnimation() {
    HMODULE module = LoadLibrary(L"Taskbar.View.dll");
    if (!module) {
        Wh_Log(L"Failed to load Taskbar.View.dll for StartItemPlateEntranceAnimation");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = {{
        { LR"(public: static void __cdecl TaskbarTelemetry::StartItemPlateEntranceAnimation<bool const &>(bool const &))" },
        &orig_StartItemPlateEntranceAnimation,
        Hook_StartItemPlateEntranceAnimation_call
    }};
    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}





using TrayUI__Hide_t = void(WINAPI*)(void* pThis);
TrayUI__Hide_t TrayUI__Hide_Original;
void WINAPI TrayUI__Hide_Hook(void* pThis) {
  TrayUI__Hide_Original(pThis);
  ApplySettingsDebounced();
}
using CSecondaryTray__AutoHide_t = void(WINAPI*)(void* pThis, bool param1);
CSecondaryTray__AutoHide_t CSecondaryTray__AutoHide_Original;
void WINAPI CSecondaryTray__AutoHide_Hook(void* pThis, bool param1) {
  CSecondaryTray__AutoHide_Original(pThis, param1);
  ApplySettingsDebounced();
}
using TrayUI_WndProc_t = LRESULT(WINAPI*)(void* pThis, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam, bool* flag);
TrayUI_WndProc_t TrayUI_WndProc_Original;
LRESULT WINAPI TrayUI_WndProc_Hook(void* pThis, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam, bool* flag) {
  ApplySettingsFromTaskbarThreadIfRequired();
  return TrayUI_WndProc_Original(pThis, hWnd, Msg, wParam, lParam, flag);
}
using CSecondaryTray_v_WndProc_t = LRESULT(WINAPI*)(void* pThis, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
CSecondaryTray_v_WndProc_t CSecondaryTray_v_WndProc_Original;
LRESULT WINAPI CSecondaryTray_v_WndProc_Hook(void* pThis, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
  ApplySettingsFromTaskbarThreadIfRequired();
  return CSecondaryTray_v_WndProc_Original(pThis, hWnd, Msg, wParam, lParam);
}
using CTaskBand__ProcessWindowDestroyed_t = void(WINAPI*)(void* pThis, void* pHwnd);
CTaskBand__ProcessWindowDestroyed_t CTaskBand__ProcessWindowDestroyed_Original;
void WINAPI CTaskBand__ProcessWindowDestroyed_Hook(void* pThis, void* pHwnd) {
  Wh_Log(L"CTaskBand::CTaskBand__ProcessWindowDestroyed_Hook Hook");
  CTaskBand__ProcessWindowDestroyed_Original(pThis, pHwnd);
  ApplySettingsDebounced();
}
using CTaskBand__InsertItem_t = long(WINAPI*)(void* pThis, void* pHwnd, void** ppTaskItem, void* pHwnd1, void* pHwnd2);
CTaskBand__InsertItem_t CTaskBand__InsertItem_Original;
long WINAPI CTaskBand__InsertItem_Hook(void* pThis, void* pHwnd, void** ppTaskItem, void* pHwnd1, void* pHwnd2) {
  Wh_Log(L"CTaskBand::_InsertItem Hook");
  auto original_call = CTaskBand__InsertItem_Original(pThis, pHwnd, ppTaskItem, pHwnd1, pHwnd2);
  ApplySettingsDebounced(100);
  return original_call;
}
using CTaskBand__UpdateAllIcons_t = void(WINAPI*)(void* pThis);
CTaskBand__UpdateAllIcons_t CTaskBand__UpdateAllIcons_Original;
void WINAPI CTaskBand__UpdateAllIcons_Hook(void* pThis) {
  Wh_Log(L"CTaskBand::_UpdateAllIcons Hook");
  CTaskBand__UpdateAllIcons_Original(pThis);
  ApplySettingsDebounced();
}
using CTaskBand__TaskOrderChanged_t = void(WINAPI*)(void* pThis, void* pTaskGroup, int param);
CTaskBand__TaskOrderChanged_t CTaskBand__TaskOrderChanged_Original;
void WINAPI CTaskBand__TaskOrderChanged_Hook(void* pThis, void* pTaskGroup, int param) {
  Wh_Log(L"CTaskBand::TaskOrderChanged Hook");
  CTaskBand__TaskOrderChanged_Original(pThis, pTaskGroup, param);
  ApplySettingsDebounced();
}
using CImpWndProc__WndProc_t = __int64(WINAPI*)(void* pThis, void* pHwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam);
CImpWndProc__WndProc_t CImpWndProc__WndProc_Original;
__int64 WINAPI CImpWndProc__WndProc_Hook(void* pThis, void* pHwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam) {
  ApplySettingsFromTaskbarThreadIfRequired();
  return CImpWndProc__WndProc_Original(pThis, pHwnd, msg, wParam, lParam);
}
using CTaskBand__WndProc_t = __int64(WINAPI*)(void* pThis, void* pHwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam);
CTaskBand__WndProc_t CTaskBand__WndProc_Original;
__int64 WINAPI CTaskBand__WndProc_Hook(void* pThis, void* pHwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam) {
  ApplySettingsFromTaskbarThreadIfRequired();
  return CTaskBand__WndProc_Original(pThis, pHwnd, msg, wParam, lParam);
}
using CTaskListWnd__WndProc_t = __int64(WINAPI*)(void* pThis, void* pHwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam);
CTaskListWnd__WndProc_t CTaskListWnd__WndProc_Original;
__int64 WINAPI CTaskListWnd__WndProc_Hook(void* pThis, void* pHwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam) {
  ApplySettingsFromTaskbarThreadIfRequired();
  return CTaskListWnd__WndProc_Original(pThis, pHwnd, msg, wParam, lParam);
}
using CSecondaryTaskBand__WndProc_t = __int64(WINAPI*)(void* pThis, void* pHwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam);
CSecondaryTaskBand__WndProc_t CSecondaryTaskBand__WndProc_Original;
__int64 WINAPI CSecondaryTaskBand__WndProc_Hook(void* pThis, void* pHwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam) {
  ApplySettingsFromTaskbarThreadIfRequired();
  return CSecondaryTaskBand__WndProc_Original(pThis, pHwnd, msg, wParam, lParam);
}
using CTraySearchControl__WndProc_t = __int64(WINAPI*)(void* pThis, void* pHwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam);
CTraySearchControl__WndProc_t CTraySearchControl__WndProc_Original;
__int64 WINAPI CTraySearchControl__WndProc_Hook(void* pThis, void* pHwnd, unsigned int msg, unsigned __int64 wParam, __int64 lParam) {
  ApplySettingsFromTaskbarThreadIfRequired();
  return CTraySearchControl__WndProc_Original(pThis, pHwnd, msg, wParam, lParam);
}
interface ITaskGroup;
interface ITaskItem;
using CTaskBand__UpdateItemIcon_WithArgs_t = void(WINAPI*)(void* pThis, ITaskGroup* param1, ITaskItem* param2);
CTaskBand__UpdateItemIcon_WithArgs_t CTaskBand__UpdateItemIcon_WithArgs_Original;
void WINAPI CTaskBand__UpdateItemIcon_WithArgs_Hook(void* pThis, ITaskGroup* param1, ITaskItem* param2) {
  Wh_Log(L"Method called: CTaskBand__UpdateItemIcon");
  CTaskBand__UpdateItemIcon_WithArgs_Original(pThis, param1, param2);
  ApplySettingsDebounced(100);
}
using CTaskBand_RemoveIcon_WithArgs_t = void(WINAPI*)(void* pThis, ITaskItem* param1);
CTaskBand_RemoveIcon_WithArgs_t CTaskBand_RemoveIcon_WithArgs_Original;
void WINAPI CTaskBand_RemoveIcon_WithArgs_Hook(void* pThis, ITaskItem* param1) {
  Wh_Log(L"Method called: CTaskBand_RemoveIcon");
  CTaskBand_RemoveIcon_WithArgs_Original(pThis, param1);
  ApplySettingsDebounced();
}

bool HookTaskbarDllSymbols() {
  HookTaskbarDllSymbolsTBIconSize();
  HMODULE module = LoadLibrary(L"taskbar.dll");
  if (!module) {
    Wh_Log(L"Failed to load taskbar.dll");
    return false;
  }
  WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {

      {{LR"(public: virtual void __cdecl CTaskBand::RemoveIcon(struct ITaskItem *))"}, &CTaskBand_RemoveIcon_WithArgs_Original, CTaskBand_RemoveIcon_WithArgs_Hook},
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
  Wh_ModSettingsChangedTBIconSize();
  Wh_ModSettingsChangedStartButtonPosition();

  g_invalidateDimensions = true;
  g_settings.userDefinedFlatTaskbarBottomCorners = Wh_GetIntSetting(L"FlatTaskbarBottomCorners");
  g_settings.userDefinedFullWidthTaskbarBackground = Wh_GetIntSetting(L"FullWidthTaskbarBackground");
  if (g_settings.userDefinedFullWidthTaskbarBackground) {
    g_settings.userDefinedFlatTaskbarBottomCorners = 1;
  }

  g_settings.userDefinedIgnoreShowDesktopButton = Wh_GetIntSetting(L"IgnoreShowDesktopButton");
  g_settings.userDefinedTrayAreaDivider = Wh_GetIntSetting(L"TrayAreaDivider");
  g_settings.userDefinedStyleTrayArea = Wh_GetIntSetting(L"StyleTrayArea");

  g_settings.userDefinedTrayTaskGap = Wh_GetIntSetting(L"TrayTaskGap");
  g_settings.userDefinedTaskbarBackgroundHorizontalPadding = Wh_GetIntSetting(L"TaskbarBackgroundHorizontalPadding");
  g_settings.userDefinedTaskbarOffsetY = -1 * abs(Wh_GetIntSetting(L"TaskbarOffsetY"));

  if (g_settings.userDefinedFlatTaskbarBottomCorners) {
    g_settings.userDefinedTaskbarOffsetY = 0;
  }

  g_settings.userDefinedTaskbarHeight = abs(Wh_GetIntSetting(L"TaskbarHeight"));

  g_settings.userDefinedTaskbarHeight = g_settings.userDefinedTaskbarHeight < 200 ? g_settings.userDefinedTaskbarHeight : 200;
  g_settings.userDefinedTaskbarHeight = g_settings.userDefinedTaskbarHeight < 44 ? 44 : g_settings.userDefinedTaskbarHeight;

  g_settings.userDefinedTaskbarIconSize = Wh_GetIntSetting(L"TaskbarIconSize");
  g_settings.userDefinedTrayIconSize = Wh_GetIntSetting(L"TrayIconSize");

  g_settings.userDefinedTaskbarButtonSize = Wh_GetIntSetting(L"TaskbarButtonSize");
  g_settings.userDefinedTrayButtonSize = Wh_GetIntSetting(L"TrayButtonSize");

  g_settings.userDefinedTaskbarCornerRadius = static_cast<float>(abs(Wh_GetIntSetting(L"TaskbarCornerRadius")));
  g_settings.userDefinedTaskbarCornerRadius = g_settings.userDefinedTaskbarCornerRadius < (g_settings.userDefinedTaskbarHeight / 2.0f) ? g_settings.userDefinedTaskbarCornerRadius : (g_settings.userDefinedTaskbarHeight / 2.0f);

  g_settings.userDefinedTaskButtonCornerRadius = abs(Wh_GetIntSetting(L"TaskButtonCornerRadius"));
  g_settings.userDefinedTaskButtonCornerRadius = g_settings.userDefinedTaskButtonCornerRadius < (g_settings.userDefinedTaskbarHeight / 2.0f) ? g_settings.userDefinedTaskButtonCornerRadius : (g_settings.userDefinedTaskbarHeight / 2.0f);

  g_settings.userDefinedTaskbarBackgroundOpacity = std::min(100, std::max(0, abs(Wh_GetIntSetting(L"TaskbarBackgroundOpacity"))));
  g_settings.userDefinedTaskbarBackgroundTint = std::min(100, std::max(0, abs(Wh_GetIntSetting(L"TaskbarBackgroundTint"))));
  g_settings.userDefinedTaskbarBackgroundLuminosity = std::min(100, std::max(0, abs(Wh_GetIntSetting(L"TaskbarBackgroundLuminosity"))));
  g_settings.userDefinedTaskbarBorderOpacity = abs(Wh_GetIntSetting(L"TaskbarBorderOpacity")) * 2.55;
  g_settings.userDefinedTaskbarBorderOpacity = static_cast<uint8_t>(g_settings.userDefinedTaskbarBorderOpacity > 255 ? 255 : g_settings.userDefinedTaskbarBorderOpacity);

  g_settings.userDefinedTaskbarBorderThickness = (10.0 / 100.0) * std::max(std::min(abs(Wh_GetIntSetting(L"TaskbarBorderThickness")), 100), 0);

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

  //  g_icon_scanner.onSettingsChanged(L"C:\\Program Files (x86)\\Steam\\steamapps\\common\\MyDockFinder\\png", L"C:\\Program Files (x86)\\Steam\\steamapps\\common\\MyDockFinder\\png");
}

void ResetGlobalVars() {
  g_initOffsetX = -1;
  g_invalidateDimensions = true;

  g_lastTaskbarData.childrenCount = 0;
  g_lastTaskbarData.rightMostEdge = 0.0;
  g_lastTaskbarData.childrenWidth = 0.0;
  g_wasOverflowing = false;
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

 g_unloading=false;

  if (!Wh_ModInitTBIconSize()) {
    Wh_Log(L"Wh_ModInitTBIconSize failed");
    return FALSE;
  }
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


   if (!Hook_StartItemEntranceAnimation()) {
        Wh_Log(L"Method hooking failed: StartItemEntranceAnimation");
        return FALSE;
    }

    if (!Hook_StartItemPlateEntranceAnimation()) {
        Wh_Log(L"Method hooking failed: StartItemPlateEntranceAnimation");
        return FALSE;
    }


  return TRUE;
}

void Wh_ModAfterInit() {
  Wh_ModAfterInitTBIconSize();
  HWND hTaskbarWnd = GetTaskbarWnd();
  if (hTaskbarWnd) {
    ApplySettings(hTaskbarWnd);
  }
  Wh_ModSettingsChanged();
}

void Wh_ModBeforeUninit() {
  Wh_ModBeforeUninitTBIconSize();
  g_unloading = true;
  ApplySettingsFromTaskbarThread();
}

void Wh_ModUninit() {
  Wh_ModUninitTBIconSize();
  ResetGlobalVars();
  CleanupDebounce();
}
