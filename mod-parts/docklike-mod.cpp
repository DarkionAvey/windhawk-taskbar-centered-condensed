

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
////   _______    ______     ______  __  ___          __       __   __  ___  _______   ////
////  |       \  /  __  \   /      ||  |/  /         |  |     |  | |  |/  / |   ____|  ////
////  |  .--.  ||  |  |  | |  ,----'|  '  /   ______ |  |     |  | |  '  /  |  |__     ////
////  |  |  |  ||  |  |  | |  |     |    <   |______||  |     |  | |    <   |   __|    ////
////  |  '--'  ||  `--'  | |  `----.|  .  \          |  `----.|  | |  .  \  |  |____   ////
////  |_______/  \______/   \______||__|\__\         |_______||__| |__|\__\ |_______|  ////
////                                                                                   ////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

#include <dwmapi.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>

#include <functional>
#undef GetCurrentTime
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>
using namespace winrt::Windows::UI::Xaml;
STDAPI GetDpiForMonitor(HMONITOR hmonitor, MONITOR_DPI_TYPE dpiType, UINT* dpiX, UINT* dpiY);
#include <commctrl.h>
#include <roapi.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.Animation.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winstring.h>

#include <chrono>
#include <string>
#include <string_view>
#include <winrt/Windows.UI.Xaml.Automation.h.>

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
float g_lastTargetOffsetX = 0.0f, g_lastTargetWidth = 0.0f, g_lastTargetOffsetY = 0.0f;
bool g_isAnimating = false;
signed int g_initOffsetX = -1;
bool g_invalidateDimensions = true;
struct {
  int childrenCount = 0;
  double rightMostEdge = 0.0;
  double childrenWidth = 0.0;
} g_lastTaskbarData;
float g_lastModifierForTaskbarOffset = 0.0f;
bool g_wasOverflowing = false;

#include <atomic>
#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>

std::mutex debounceMutex;
std::condition_variable debounceCv;
std::atomic<bool> debounceExit(false);
std::atomic<bool> debounceRequested(false);
std::thread debounceThread;
HWND debounceHwnd = nullptr;
int debounceDelayMs = 300;

std::wstring EscapeXmlAttribute(std::wstring_view data) {
  std::wstring buffer;
  buffer.reserve(data.size());
  for (wchar_t c : data) buffer.append((c == L'&') ? L"&amp;" : (c == L'\"') ? L"&quot;" : (c == L'<') ? L"&lt;" : (c == L'>') ? L"&gt;" : std::wstring(1, c));
  return buffer;
}

Style GetStyleFromXamlSetters(const std::wstring_view type, const std::wstring_view xamlStyleSetters, std::wstring& outXaml) {
  std::wstring xaml =
      LR"(<ResourceDictionary
    xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
    xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
    xmlns:d="http://schemas.microsoft.com/expression/blend/2008"
    xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
    xmlns:muxc="using:Microsoft.UI.Xaml.Controls")";
  if (auto pos = type.rfind('.'); pos != type.npos) {
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
  std::wstring outXamlResult;
  try {
    std::wstring xamlSetter = L"<Setter Property=\"";
    xamlSetter += EscapeXmlAttribute(propertyName);
    xamlSetter += L"\"";
    if (isXamlValue) {
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
      auto setter = style.Setters().GetAt(i).as<Setter>();
      obj.SetValue(setter.Property(), setter.Value());
    }
  } catch (const std::exception& ex) {
    if (!outXamlResult.empty()) {
      Wh_Log(L"Error: %S. Xaml Result: %s", ex.what(), outXamlResult.c_str());
    } else {
      Wh_Log(L"Error: %S", ex.what());
    }
  } catch (const winrt::hresult_error& ex) {
    if (!outXamlResult.empty()) {
      Wh_Log(L"Error %08X: %s. Xaml Result: %s", ex.code(), ex.message().c_str(), outXamlResult.c_str());
    } else {
      Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
    }
  } catch (...) {
    if (!outXamlResult.empty()) {
      Wh_Log(L"Unknown error occurred while setting property. Xaml Result: %s", outXamlResult.c_str());
    } else {
      Wh_Log(L"Unknown error occurred while setting property.");
    }
  }
}

void ApplySettingsFromTaskbarThreadIfRequired() {
  if (g_invalidateDimensions) {
    ApplySettingsFromTaskbarThread();
  }
}

void SetElementPropertyFromString(FrameworkElement obj, const std::wstring& type, const std::wstring& propertyName, const std::wstring& propertyValue) { return SetElementPropertyFromString(obj, type, propertyName, propertyValue, false); }

void ProcessStackPanelChildren(FrameworkElement const& stackPanel) {
if(g_settings.userDefinedTrayAreaDivider){
  auto compositor =  winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(stackPanel).Compositor();
  auto shapeVisual = compositor.CreateShapeVisual();
  float panelHeight= static_cast<float>( stackPanel.ActualHeight());
  float panelWidth= static_cast<float>( stackPanel.ActualWidth());
  shapeVisual.Size({static_cast<float>(g_settings.userDefinedTaskbarBorderThickness),panelHeight});
  auto borderGeometry = compositor.CreateRoundedRectangleGeometry();
  borderGeometry.Size({static_cast<float>(panelWidth - g_settings.userDefinedTaskbarBorderThickness), static_cast<float>(panelHeight -g_settings.userDefinedTaskbarBorderThickness*2)});
  auto borderShape = compositor.CreateSpriteShape(borderGeometry);
  winrt::Windows::UI::Color borderColor = {g_settings.userDefinedTaskbarBorderOpacity, static_cast<BYTE>(g_settings.borderColorR), static_cast<BYTE>(g_settings.borderColorG), static_cast<BYTE>(g_settings.borderColorB)};
  borderShape.StrokeBrush(compositor.CreateColorBrush(borderColor));
  borderShape.StrokeThickness(g_settings.userDefinedTaskbarBorderThickness);
  borderShape.FillBrush(nullptr);
  borderGeometry.Offset({static_cast<float>(g_settings.userDefinedTaskbarBorderThickness / 2.0f), static_cast<float>(g_settings.userDefinedTaskbarBorderThickness / 2.0f)});
  shapeVisual.Shapes().Append(borderShape);
  shapeVisual.Offset({g_settings.userDefinedTrayTaskGap/-2.0f, static_cast<float>(g_settings.userDefinedTaskbarBorderThickness/2.0f), 0.0f});
  winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::SetElementChildVisual(stackPanel, shapeVisual);
}else{
    winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::SetElementChildVisual(stackPanel, nullptr);
}

if(!g_settings.userDefinedStyleTrayArea)return;

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


bool IsWeirdFrameworkElement(winrt::Windows::UI::Xaml::FrameworkElement const& element) {
  if (!element) return false;
  auto transform = element.TransformToVisual(nullptr);
  winrt::Windows::Foundation::Rect rect = transform.TransformBounds(winrt::Windows::Foundation::Rect(0, 0, element.ActualWidth(), element.ActualHeight()));
  return rect.X < 0 || rect.Y < 0;
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
    if(className == L"Taskbar.TaskListButton" ){
              auto innerElementChild = FindChildByClassName(child, L"Taskbar.TaskListLabeledButtonPanel");
if (innerElementChild) {
                     auto iconElementChild = FindChildByName(innerElementChild, L"Icon");

                     if (iconElementChild) {
        iconElementChild.Width(g_settings.userDefinedTaskbarIconSize);
        iconElementChild.Height(g_settings.userDefinedTaskbarIconSize);
      }

      }
    }
    SetElementPropertyFromString(child, className.c_str(), L"CornerRadius", userDefinedTaskButtonCornerRadius);

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

unsigned int g_lastChildrenWidthTaskbar = 0, g_lastTrayFrameWidth = 0;

bool ApplyStyle(XamlRoot xamlRoot) {
  g_isAnimating = true;
  auto xamlRootContent = xamlRoot.Content().try_as<FrameworkElement>();
  if (!xamlRootContent) return false;

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

ProcessStackPanelChildren(stackPanel);

  auto overflowButton = FindChildByClassName(taskbarFrameRepeater, L"Taskbar.OverflowToggleButton");
  bool isOverflowing = overflowButton != nullptr && !IsWeirdFrameworkElement(overflowButton);

  double rootWidth = xamlRoot.Size().Width;

  int childrenCountTaskbar = 0;
  const double childrenWidthTaskbarDbl = CalculateValidChildrenWidth(taskbarFrameRepeater, childrenCountTaskbar);

  signed int leftMostEdgeTaskbar = static_cast<signed int>((rootWidth/2.0)-(childrenWidthTaskbarDbl/2.0));
  signed int rightMostEdgeTaskbar = static_cast<signed int>((rootWidth/2.0)+(childrenWidthTaskbarDbl/2.0));
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
  const unsigned int trayFrameWidth = static_cast<unsigned int>(trayFrameWidthDbl-showDesktopButtonWidth+g_settings.userDefinedTrayTaskGap);

  signed int leftMostEdgeTray = static_cast<signed int>((rootWidth/2.0)-(trayFrameWidth/2.0));
  signed int rightMostEdgeTray = static_cast<signed int>((rootWidth/2.0)+(trayFrameWidth/2.0));


  if (childrenCountTray == 0 || trayFrameWidth <= 1) {
    return false;
  }

  float centeredTray = (rootWidth - trayFrameWidth) / 2.0f;
  float newXOffsetTray = centeredTray + (childrenWidthTaskbar / 2.0f)+g_settings.userDefinedTrayTaskGap;
  // tray animations
  auto trayVisual = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(trayFrame);
  auto originalOffset = trayVisual.Offset();
  if (g_initOffsetX == -1) {
    g_initOffsetX = originalOffset.x;
  }

  auto taskbarFrameRepeaterVisual = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(taskbarFrameRepeater);
  float targetTaskFrameOffsetX = newXOffsetTray - rightMostEdgeTaskbar-g_settings.userDefinedTrayTaskGap ;

// 5 pixels tolerance
if (abs(newXOffsetTray - trayVisual.Offset().x) <= 5 &&
    childrenWidthTaskbar == g_lastChildrenWidthTaskbar &&
    trayFrameWidth == g_lastTrayFrameWidth &&
    abs(targetTaskFrameOffsetX - taskbarFrameRepeaterVisual.Offset().x) <= 5)
{
    Wh_Log(L"newXOffsetTray is within 5 pixels of trayVisual offset %f", trayVisual.Offset().x);
    Wh_Log(L"childrenWidthTaskbar and trayFrameWidth didn't change: %d, %d", childrenWidthTaskbar, g_lastTrayFrameWidth);
    return true;
}

  g_lastChildrenWidthTaskbar = static_cast<unsigned int>(childrenWidthTaskbar);
  g_lastTrayFrameWidth = static_cast<unsigned int>(trayFrameWidth);

  Wh_Log(L"leftMostEdgeTaskbar: %df, rightMostEdgeTaskbar: %d, g_lastTaskbarData.rightMostEdge %d, g_lastTaskbarData.count %d", leftMostEdgeTaskbar, rightMostEdgeTaskbar, g_lastTaskbarData.rightMostEdge, g_lastTaskbarData.childrenCount);
  Wh_Log(L"leftMostEdgeTray: %d, rightMostEdgeTray: %d", leftMostEdgeTray, rightMostEdgeTray);

  signed int userDefinedTaskbarOffsetY = (g_settings.userDefinedFlatTaskbarBottomCorners||g_settings.userDefinedFullWidthTaskbarBackground) ? 0 : g_settings.userDefinedTaskbarOffsetY;
  float targetWidth = g_unloading ? rootWidth : (childrenWidthTaskbar + trayFrameWidth + (g_settings.userDefinedTaskbarBackgroundHorizontalPadding * 2));

  auto heightValue = (g_settings.userDefinedTaskbarHeight + abs(userDefinedTaskbarOffsetY < 0 ? (userDefinedTaskbarOffsetY * 2) : 0));

  if (ScrollViewer.Width() != rootWidth) {
    g_invalidateDimensions = true;
  }
  if (g_invalidateDimensions) {
    g_invalidateDimensions = false;
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

  //   leftMostEdgeTaskbar = fmax(0.0f, (rootWidth - childrenWidthTaskbar) / 2.0f);

  auto taskbarFrameRepeaterVisualAnimation = taskbarFrameRepeaterVisual.Compositor().CreateVector3KeyFrameAnimation();

  float modifierForTaskbarOffset = isOverflowing ? ((targetWidth - childrenWidthTaskbar) / 2.0f) : (childrenWidthTaskbar - g_lastTaskbarData.childrenWidth) / 2.0f;

  Wh_Log(L"modifierForTaskbarOffset: %f, isOverflowing: %d, wasOverflowing: %d", modifierForTaskbarOffset, isOverflowing, g_wasOverflowing);

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

  if (!taskbarBackground) return false;

  auto taskbarGrid = FindChildByClassName(taskbarBackground, L"Windows.UI.Xaml.Controls.Grid");
  if (taskbarGrid) {
    auto taskbarStroke = FindChildByName(taskbarGrid, L"BackgroundStroke");
    if (taskbarStroke) {
      taskbarStroke.Opacity(0);
    }
  }

  auto userDefinedTaskbarBackgroundLuminosity = std::to_wstring(g_settings.userDefinedTaskbarBackgroundLuminosity / 100.0f);
  auto userDefinedTaskbarBackgroundOpacity = std::to_wstring(g_settings.userDefinedTaskbarBackgroundOpacity / 100.0f);
  auto userDefinedTaskbarBackgroundTint = std::to_wstring(g_settings.userDefinedTaskbarBackgroundTint / 100.0f);
  SetElementPropertyFromString(backgroundFillChild, L"Windows.UI.Xaml.Shapes.Rectangle", L"Fill",
                               L"<AcrylicBrush TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\"" + userDefinedTaskbarBackgroundTint + L"\" TintLuminosityOpacity=\"" + userDefinedTaskbarBackgroundLuminosity + L"\" Opacity=\"" +
                                   userDefinedTaskbarBackgroundOpacity + L"\"/>",
                               true);

  auto backgroundFillVisual = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(backgroundFillChild);
  auto compositorTaskBackground = backgroundFillVisual.Compositor();

  ////////////////////////////////////////

const  float targetWidthRect = !g_settings.userDefinedFullWidthTaskbarBackground ? targetWidth : static_cast<float>(rootWidth);

  auto roundedRect = compositorTaskBackground.CreateRoundedRectangleGeometry();
  roundedRect.CornerRadius({g_settings.userDefinedTaskbarCornerRadius, g_settings.userDefinedTaskbarCornerRadius});

  const auto targetHeightPrelim = (!g_settings.userDefinedFullWidthTaskbarBackground ? g_settings.userDefinedTaskbarHeight : ScrollViewer.ActualHeight());

  const auto clipHeight = static_cast<float>(targetHeightPrelim + ((g_settings.userDefinedFlatTaskbarBottomCorners) ? (targetHeightPrelim - g_settings.userDefinedTaskbarCornerRadius) : 0.0f));
  roundedRect.Size({!g_settings.userDefinedFullWidthTaskbarBackground?g_lastTargetWidth:targetWidthRect , clipHeight});
  auto geometricClip = compositorTaskBackground.CreateGeometricClip(roundedRect);
  backgroundFillVisual.Clip(geometricClip);

  if (!g_settings.userDefinedFullWidthTaskbarBackground) {
    float offsetXRect = (rootWidth - targetWidth) / 2;
    float newOffsetYRect = userDefinedTaskbarOffsetY < 0 ? static_cast<float>(abs(userDefinedTaskbarOffsetY)) : 0.0f;

    // size animation
    if (roundedRect.TryGetAnimationController(L"Size")) {
      roundedRect.StopAnimation(L"Size");
    }
    auto sizeAnimationRect = compositorTaskBackground.CreateVector2KeyFrameAnimation();
    sizeAnimationRect.InsertKeyFrame(0.0f, {g_lastTargetWidth, clipHeight});
    sizeAnimationRect.InsertKeyFrame(1.0f, {targetWidthRect, clipHeight});

    if (movingInwards) {
      sizeAnimationRect.DelayTime(winrt::Windows::Foundation::TimeSpan(std::chrono::milliseconds(200)));
    }
    roundedRect.StartAnimation(L"Size", sizeAnimationRect);

    //   // centering the clip animation
    if (roundedRect.TryGetAnimationController(L"Offset")) {
      roundedRect.StopAnimation(L"Offset");
    }

    roundedRect.Offset({g_lastTargetOffsetX, g_lastTargetOffsetY});
    auto offsetAnimationRect = compositorTaskBackground.CreateVector2KeyFrameAnimation();
    offsetAnimationRect.InsertKeyFrame(0.0f, {g_lastTargetOffsetX, g_lastTargetOffsetY});
    offsetAnimationRect.InsertKeyFrame(1.0f, {offsetXRect, newOffsetYRect});
    if (movingInwards) {
      offsetAnimationRect.DelayTime(winrt::Windows::Foundation::TimeSpan(std::chrono::milliseconds(200)));
    }
    roundedRect.StartAnimation(L"Offset", offsetAnimationRect);
    g_lastTargetOffsetX = offsetXRect;
    g_lastTargetOffsetY = newOffsetYRect;
  }else{
    g_lastTargetOffsetX = 0;
    g_lastTargetOffsetY = 0;
    roundedRect.Offset({g_lastTargetOffsetX,g_lastTargetOffsetY});
  }

  // border control
  auto compositor = compositorTaskBackground;
  auto shapeVisual = compositor.CreateShapeVisual();
  shapeVisual.Size({targetWidthRect, static_cast<float>(clipHeight)});

  auto borderGeometry = compositor.CreateRoundedRectangleGeometry();
  borderGeometry.CornerRadius({g_settings.userDefinedTaskbarCornerRadius, g_settings.userDefinedTaskbarCornerRadius});
  borderGeometry.Size({static_cast<float>(targetWidthRect - g_settings.userDefinedTaskbarBorderThickness), static_cast<float>(clipHeight - g_settings.userDefinedTaskbarBorderThickness)});
  //   Wh_Log(L"userDefinedTaskbarBorderOpacity: %d", g_settings.userDefinedTaskbarBorderOpacity);

  auto borderShape = compositor.CreateSpriteShape(borderGeometry);

  winrt::Windows::UI::Color borderColor = {g_settings.userDefinedTaskbarBorderOpacity, static_cast<BYTE>(g_settings.borderColorR), static_cast<BYTE>(g_settings.borderColorG), static_cast<BYTE>(g_settings.borderColorB)};

  borderShape.StrokeBrush(compositor.CreateColorBrush(borderColor));
  borderShape.StrokeThickness(g_settings.userDefinedTaskbarBorderThickness);
  borderShape.FillBrush(nullptr);

  borderGeometry.Offset({static_cast<float>(g_settings.userDefinedTaskbarBorderThickness / 2.0f), static_cast<float>(g_settings.userDefinedTaskbarBorderThickness / 2.0f)});
  shapeVisual.Shapes().Append(borderShape);
  shapeVisual.Offset({g_lastTargetOffsetX, g_lastTargetOffsetY, 0.0f});
  winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::SetElementChildVisual(backgroundFillChild, shapeVisual);

  g_wasOverflowing = isOverflowing;
  g_lastTargetWidth = targetWidthRect;

  return true;
}

void ApplySettings(HWND hTaskbarWnd) {
  RunFromWindowThread(hTaskbarWnd, [](void* pParam) { ApplySettingsFromTaskbarThread(); }, 0);
}



void StartDebounceWorker() {
    debounceThread = std::thread([]() {
        std::unique_lock<std::mutex> lock(debounceMutex);
        while (!debounceExit) {
            if (!debounceRequested) {
                debounceCv.wait(lock);
                continue;
            }

            debounceRequested = false;
            auto wakeTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(debounceDelayMs);

            while (std::chrono::steady_clock::now() < wakeTime) {
                if (debounceExit) return;
                if (debounceRequested) {
                    debounceRequested = false;
                    wakeTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(debounceDelayMs);
                }
                debounceCv.wait_until(lock, wakeTime);
            }

            if (debounceHwnd && !debounceExit) {
                ApplySettings(debounceHwnd);
            }
        }
    });
}

void StopDebounceWorker() {
    {
        std::lock_guard<std::mutex> lock(debounceMutex);
        debounceExit = true;
        debounceCv.notify_all();
    }
    if (debounceThread.joinable()) {
        debounceThread.join();
    }
}
void ApplySettingsDebounced(int delayMs=500) {
auto hTaskbarWnd=GetTaskbarWnd();
if(!hTaskbarWnd)return;

    {
        std::lock_guard<std::mutex> lock(debounceMutex);
        debounceHwnd = hTaskbarWnd;
        debounceDelayMs = delayMs;
        debounceRequested = true;
    }
    debounceCv.notify_all();
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

// Hook function type definitions and original function pointers

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
  auto original_call=CTaskBand__InsertItem_Original(pThis, pHwnd, ppTaskItem, pHwnd1, pHwnd2);
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

// /////////////////////////////////////

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

interface ITaskGroup;interface ITaskItem;
using CTaskBand__UpdateItemIcon_WithArgs_t = void(WINAPI*)(void* pThis, ITaskGroup * param1, ITaskItem * param2);
CTaskBand__UpdateItemIcon_WithArgs_t CTaskBand__UpdateItemIcon_WithArgs_Original;
void WINAPI CTaskBand__UpdateItemIcon_WithArgs_Hook(void* pThis, ITaskGroup * param1, ITaskItem * param2) {
                Wh_Log(L"Method called: CTaskBand__UpdateItemIcon");
                CTaskBand__UpdateItemIcon_WithArgs_Original(pThis, param1, param2);
                  ApplySettingsDebounced(100);
            }

using CTaskBand_RemoveIcon_WithArgs_t = void(WINAPI*)(void* pThis, ITaskItem * param1);
CTaskBand_RemoveIcon_WithArgs_t CTaskBand_RemoveIcon_WithArgs_Original;
void WINAPI CTaskBand_RemoveIcon_WithArgs_Hook(void* pThis, ITaskItem * param1) {
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


	{ {LR"(public: virtual void __cdecl CTaskBand::RemoveIcon(struct ITaskItem *))"},
                                         &CTaskBand_RemoveIcon_WithArgs_Original,
                                         CTaskBand_RemoveIcon_WithArgs_Hook }                                          ,
    {{LR"(protected: void __cdecl CTaskBand::_UpdateItemIcon(struct ITaskGroup *,struct ITaskItem *))"},
                                         &CTaskBand__UpdateItemIcon_WithArgs_Original,
                                         CTaskBand__UpdateItemIcon_WithArgs_Hook },

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
}

void ResetGlobalVars(){
         g_lastTargetOffsetX = 0.0f, g_lastTargetWidth = 0.0f, g_lastTargetOffsetY = 0.0f;
 g_isAnimating = false;
 g_initOffsetX = -1;
 g_invalidateDimensions = true;

g_lastTaskbarData.childrenCount = 0;
g_lastTaskbarData.rightMostEdge = 0.0;
g_lastTaskbarData.childrenWidth = 0.0;
g_lastModifierForTaskbarOffset = 0.0f;
g_wasOverflowing = false;
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
  Wh_ModInitTBIconSize();
  RefreshSettings();
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

  // not needed?
  // HMODULE dwmapiModule = LoadLibrary(L"dwmapi.dll");
  // if (dwmapiModule) {
  //     FARPROC pDwmSetWindowAttribute =
  //             GetProcAddress(dwmapiModule, "DwmSetWindowAttribute");
  //     if (pDwmSetWindowAttribute) {
  //         Wh_SetFunctionHook((void *) pDwmSetWindowAttribute,
  //                            (void *) DwmSetWindowAttribute_Hook,
  //                            (void **) &DwmSetWindowAttribute_Original);
  //     }
  // }

  return TRUE;
}

void Wh_ModAfterInit() {
  Wh_ModAfterInitTBIconSize();
StartDebounceWorker();
  HWND hTaskbarWnd = GetTaskbarWnd();
  if (hTaskbarWnd) {
   ApplySettingsDebounced(3000);
  }
}

void Wh_ModBeforeUninit() {
  Wh_ModBeforeUninitTBIconSize();
StopDebounceWorker();
  g_unloading = true;
  HWND hTaskbarWnd = GetTaskbarWnd();
  if (hTaskbarWnd) {
    ApplySettings(hTaskbarWnd);
  }
}

void Wh_ModUninit() {
    Wh_ModUninitTBIconSize();
ResetGlobalVars();
    }

void Wh_ModSettingsChanged() {
    ResetGlobalVars();
  RefreshSettings();
  ApplySettingsFromTaskbarThread();
}
