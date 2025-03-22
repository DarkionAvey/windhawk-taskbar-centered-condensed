// ==WindhawkMod==
// @id              taskbar-centered-condensed
// @name            Tray area next to task area
// @description     Mainly moves the system tray next to the task area on the taskbar, but can be used as an all-in-one, one-click mod for turning the taskbar into MacOS-style dock. Heavily based on m417z code; for Windows 11
// @version         1.0.0
// @author          DarkionAvey
// @github          https://github.com/DarkionAvey/windhawk-taskbar-centered-condensed
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -ldwmapi -lole32 -loleaut32 -lruntimeobject -lshcore
// ==/WindhawkMod==

// This mod is based on code released under the GNU General Public License v3.0.
// For bug reports and feeback:
// https://github.com/DarkionAvey/windhawk-taskbar-centered-condensed

// ==WindhawkModReadme==
/*

# Getting started
Check out the documentation
[here](https://github.com/DarkionAvey/windhawk-taskbar-centered-condensed).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*

- TrayTaskGap: 5
  $name: Tray task gap
  $description: Adjusts the space between task area and tray area.

- DynamicTaskbarWidth: true
  $name: Dynamic taskbar width
  $description: When enabled, the taskbar background adjusts dynamically to fit only the task and tray areas. When disabled, it spans the entire width of the screen, similar to the default Windows 11 behavior.

- DoNotCustomizeTaskbarBackground: false
  $name: Customize taskbar background
  $description: If disabled, all options below will be ignored (useful for custom themes)

- TaskbarBackgroundHorizontalPadding: 10
  $name: Taskbar background horizontal padding
  $description: Adjusts the horizontal padding on both sides of the taskbar background.

- TaskbarOffsetY: 1
  $name: Taskbar vertical offset
  $description: Adjusts the vertical offset of the entire taskbar. Can be negative
  
- TaskbarHeight: 80
  $name: Taskbar height
  $description: Adjusts the height of the entire taskbar.

- TaskbarIconSize: 30
  $name: Taskbar icon size
  $description: Adjusts the width and height of taskbar icons.

- TaskbarCornerRadius: 20
  $name: Corner radius of task bar
  $description: Defines the corner rounding of the taskbar.

- TaskButtonCornerRadius: 16
  $name: Corner radius of the task buttons
  $description: Defines the corner rounding of individual task buttons.

- FlatTaskbarBottomCorners: false
  $name: Flat bottom corners
  $description: If enabled, the bottom corners of the taskbar will be squared instead of rounded. Igored when taskbar offset is negative.

- TaskbarBackgroundOpacity: 100
  $name: Background opacity percentage

- TaskbarBackgroundTint: 100
  $name: Background tint percentage

- TaskbarBackgroundLuminosity: 100
  $name: Background luminosity percentage


*/
// ==/WindhawkModSettings==

#include <dwmapi.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>

#include <functional>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Automation.h>-10
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>

using namespace winrt::Windows::UI::Xaml;

struct {
  bool startMenuOnTheLeft;
  int startMenuWidth;
} g_settings;

std::atomic<bool> g_unloading;

typedef enum MONITOR_DPI_TYPE {
  MDT_EFFECTIVE_DPI = 0,
  MDT_ANGULAR_DPI = 1,
  MDT_RAW_DPI = 2,
  MDT_DEFAULT = MDT_EFFECTIVE_DPI
} MONITOR_DPI_TYPE;

STDAPI GetDpiForMonitor(HMONITOR hmonitor, MONITOR_DPI_TYPE dpiType, UINT* dpiX,
                        UINT* dpiY);

HWND GetTaskbarWnd() {
  HWND hTaskbarWnd = FindWindow(L"Shell_TrayWnd", nullptr);

  DWORD processId = 0;
  if (!hTaskbarWnd || !GetWindowThreadProcessId(hTaskbarWnd, &processId) ||
      processId != GetCurrentProcessId()) {
    return nullptr;
  }

  return hTaskbarWnd;
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
  return EnumChildElements(
      element, [name](FrameworkElement child) { return child.Name() == name; });
}

FrameworkElement FindChildByClassName(FrameworkElement element,
                                      PCWSTR className) {
  return EnumChildElements(element, [className](FrameworkElement child) {
    return winrt::get_class_name(child) == className;
  });
}
#include <commctrl.h>
#include <roapi.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winstring.h>

#include <sstream>
#include <string>
#include <string_view>

// https://stackoverflow.com/a/5665377
std::wstring EscapeXmlAttribute(std::wstring_view data) {
  std::wstring buffer;
  buffer.reserve(data.size());
  for (const auto c : data) {
    switch (c) {
      case '&':
        buffer.append(L"&amp;");
        break;
      case '\"':
        buffer.append(L"&quot;");
        break;
      // case '\'':
      //     buffer.append(L"&apos;");
      //     break;
      case '<':
        buffer.append(L"&lt;");
        break;
      case '>':
        buffer.append(L"&gt;");
        break;
      default:
        buffer.push_back(c);
        break;
    }
  }

  return buffer;
}
Style GetStyleFromXamlSetters(const std::wstring_view type,
                              const std::wstring_view xamlStyleSetters,
                              std::wstring& outXaml)
{
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

    // Save to the provided string
    outXaml = xaml;

    auto resourceDictionary =
        Markup::XamlReader::Load(xaml).as<ResourceDictionary>();

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
            xamlSetter += L">\n"
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




void SetElementPropertyFromString(FrameworkElement obj,
                                  const std::wstring& type,
                                  const std::wstring& propertyName,
                                  const std::wstring& propertyValue) {
return SetElementPropertyFromString(obj,type,propertyName, propertyValue, false);
}

double CalculateValidChildrenWidth(FrameworkElement element,
                                   int*& childrenCount) {
  auto userDefinedTaskButtonCornerRadius =
      std::to_wstring(Wh_GetIntSetting(L"TaskButtonCornerRadius"));
  auto userDefinedTaskButtonIconSize = Wh_GetIntSetting(L"TaskbarIconSize");
  auto userDefinedTaskButtonIconSizeHalf = userDefinedTaskButtonIconSize * 0.6;
  double userDefinedTaskButtonIconSizeFraction = userDefinedTaskButtonIconSize;
  auto userDefinedTaskButtonIconSizeStr =
      std::to_wstring(userDefinedTaskButtonIconSize);
  auto userDefinedTaskButtonIconSizeHalfStr =
      std::to_wstring(userDefinedTaskButtonIconSizeHalf);
  auto userDefinedTaskButtonIconSizeFractionStr =
      std::to_wstring(userDefinedTaskButtonIconSizeFraction);

  double totalWidth = 0.0;
  childrenCount = new int(Media::VisualTreeHelper::GetChildrenCount(element));

  for (int i = 0; i < *childrenCount; i++) {
    auto child = Media::VisualTreeHelper::GetChild(element, i)
                     .try_as<FrameworkElement>();
    if (!child) {
      Wh_Log(L"Failed to get child %d of %d", i + 1, childrenCount);
      continue;
    }

    auto transform = child.TransformToVisual(element);
    auto rect = transform.TransformBounds(winrt::Windows::Foundation::Rect(
        0, 0, child.ActualWidth(), child.ActualHeight()));

    // exclude "weird" rectangles (aka recycled views)
    if (rect.X < 0 | rect.Y < 0) {
      continue;
    }

    auto className = winrt::get_class_name(child);
    if (className == L"Taskbar.TaskListButton" ||
        className == L"Taskbar.ExperienceToggleButton") {
      SetElementPropertyFromString(child, className.c_str(), L"CornerRadius",
                                   userDefinedTaskButtonCornerRadius);
      SetElementPropertyFromString(child, className.c_str(), L"Width",
                                   userDefinedTaskButtonIconSizeFractionStr);

      if (auto innerElement = FindChildByName(child, L"IconPanel")) {
        SetElementPropertyFromString(innerElement, className.c_str(),
                                     L"MinWidth",
                                     userDefinedTaskButtonIconSizeFractionStr);
        SetElementPropertyFromString(innerElement, className.c_str(), L"Width",
                                     userDefinedTaskButtonIconSizeFractionStr);

        if (auto imageElement = FindChildByClassName(
                innerElement, L"Windows.UI.Xaml.Controls.Image")) {
          SetElementPropertyFromString(imageElement, className.c_str(),
                                       L"MinWidth",
                                       userDefinedTaskButtonIconSizeHalfStr);
          SetElementPropertyFromString(imageElement, className.c_str(),
                                       L"Width",
                                       userDefinedTaskButtonIconSizeHalfStr);

          SetElementPropertyFromString(imageElement, className.c_str(),
                                       L"MinHeight",
                                       userDefinedTaskButtonIconSizeHalfStr);
          SetElementPropertyFromString(imageElement, className.c_str(),
                                       L"Height",
                                       userDefinedTaskButtonIconSizeHalfStr);

          SetElementPropertyFromString(child, className.c_str(), L"MinWidth",
                                       userDefinedTaskButtonIconSizeStr);
          SetElementPropertyFromString(child, className.c_str(), L"Width",
                                       userDefinedTaskButtonIconSizeStr);
        }
        if (auto BorderElement = FindChildByClassName(
                innerElement, L"Windows.UI.Xaml.Controls.Border")) {
          SetElementPropertyFromString(
              BorderElement, className.c_str(), L"MinWidth",
              userDefinedTaskButtonIconSizeFractionStr);
          SetElementPropertyFromString(
              BorderElement, className.c_str(), L"Width",
              userDefinedTaskButtonIconSizeFractionStr);
        }
      }

    } else {
      Wh_Log(L"CalculateValidChildrenWidth: classname: %s", className.c_str());
    }

    totalWidth += rect.Width;
  }

  return totalWidth;
}
#include <winrt/Windows.UI.Xaml.Media.Animation.h>
#include <winrt/Windows.UI.Xaml.h>

#include <chrono>
#include <winrt/Windows.UI.Xaml.Automation.h.>
float g_lastTargetOffsetX = 0.0f, g_lastTargetWidth = 0.0f;
bool g_isAnimating = false;
signed int g_initOffsetX = -1;

bool ApplyStyle(XamlRoot xamlRoot) {
  // todo: move notification center
  g_isAnimating = true;
  auto xamlRootContent = xamlRoot.Content().try_as<FrameworkElement>();

  auto taskFrame =
      FindChildByClassName(xamlRootContent, L"Taskbar.TaskbarFrame");
  auto borderView = Media::VisualTreeHelper::GetParent(xamlRootContent)
                        .try_as<FrameworkElement>();
  auto ScrollViewer = Media::VisualTreeHelper::GetParent(
                          Media::VisualTreeHelper::GetParent(borderView)
                              .try_as<FrameworkElement>())
                          .try_as<FrameworkElement>();

  if (!taskFrame || !ScrollViewer) {
    return false;
  }
  auto rootGrid = FindChildByName(taskFrame, L"RootGrid");
  auto taskbarFrameRepeater =
      FindChildByName(rootGrid, L"TaskbarFrameRepeater");
  auto trayFrame =
      FindChildByClassName(xamlRootContent, L"SystemTray.SystemTrayFrame");
  if (!taskbarFrameRepeater || !trayFrame) return false;

  signed int userDefinedTaskbarOffsetY = Wh_GetIntSetting(L"TaskbarOffsetY");

  int userDefinedTaskbarHeight = Wh_GetIntSetting(L"TaskbarHeight");
  auto heightValue = std::to_wstring(
      userDefinedTaskbarHeight +
      abs(userDefinedTaskbarOffsetY < 0 ? (userDefinedTaskbarOffsetY * 2) : 0));
  auto userDefinedTaskbarHeightStr = std::to_wstring(userDefinedTaskbarHeight);

  SetElementPropertyFromString(ScrollViewer,
                               L"Windows.UI.Xaml.Controls.ScrollViewer",
                               L"Height", heightValue);
  SetElementPropertyFromString(ScrollViewer,
                               L"Windows.UI.Xaml.Controls.ScrollViewer",
                               L"MaxHeight", heightValue);

  SetElementPropertyFromString(trayFrame, L"SystemTray.SystemTrayFrame",
                               L"MaxHeight", userDefinedTaskbarHeightStr);
  SetElementPropertyFromString(trayFrame, L"SystemTray.SystemTrayFrame",
                               L"Height", userDefinedTaskbarHeightStr);

  SetElementPropertyFromString(borderView, L"Windows.UI.Xaml.Controls.Border",
                               L"Height", heightValue);
  SetElementPropertyFromString(borderView, L"Windows.UI.Xaml.Controls.Border",
                               L"MaxHeight", heightValue);

  SetElementPropertyFromString(taskFrame, L"Taskbar.TaskbarFrame", L"Height",
                               heightValue);
  SetElementPropertyFromString(taskFrame, L"Taskbar.TaskbarFrame", L"MaxHeight",
                               heightValue);

  SetElementPropertyFromString(taskbarFrameRepeater,
                               L"Microsoft.UI.Xaml.Controls.ItemsRepeater",
                               L"Height", userDefinedTaskbarHeightStr);
  SetElementPropertyFromString(taskbarFrameRepeater,
                               L"Microsoft.UI.Xaml.Controls.ItemsRepeater",
                               L"MaxHeight", userDefinedTaskbarHeightStr);

  int userDefinedTrayGap = Wh_GetIntSetting(L"TrayTaskGap");

  auto trayFrameWidth = trayFrame.ActualWidth() + userDefinedTrayGap;

  int* childrenCount = nullptr;
  double childrenWidth =
      CalculateValidChildrenWidth(taskbarFrameRepeater, childrenCount);

  double rootWidth = xamlRoot.Size().Width;

  // tray animations
  auto trayVisual = winrt::Windows::UI::Xaml::Hosting::
      ElementCompositionPreview::GetElementVisual(trayFrame);
  auto originalOffset = trayVisual.Offset();
  if (g_initOffsetX == -1) {
    g_initOffsetX = originalOffset.x;

    taskFrame.SetValue(
        FrameworkElement::WidthProperty(),
        winrt::box_value(std::numeric_limits<double>::quiet_NaN()));
    trayFrame.SetValue(FrameworkElement::HorizontalAlignmentProperty(),
                       winrt::box_value(HorizontalAlignment::Center));
  }

  float centered = (rootWidth / 2) - trayFrameWidth / 2;

  float newXOffset = (centered + childrenWidth / 2) + userDefinedTrayGap;

  boolean movingInwards = originalOffset.x > newXOffset;

  Wh_Log(L"Original Offset: %f, New offset: %f, Children %f", originalOffset.x,
         newXOffset, childrenWidth);

  auto taskbarVisual = winrt::Windows::UI::Xaml::Hosting::
      ElementCompositionPreview::GetElementVisual(taskbarFrameRepeater);
  auto compositor = taskbarVisual.Compositor();
  float targetOffsetXTray =
      (g_unloading ? (float)g_initOffsetX : static_cast<float>(newXOffset));

  auto trayAnimation = compositor.CreateVector3KeyFrameAnimation();

  trayAnimation.InsertKeyFrame(1.0f,
                               winrt::Windows::Foundation::Numerics::float3{
                                   targetOffsetXTray, taskbarVisual.Offset().y,
                                   taskbarVisual.Offset().z});

  if (movingInwards) {
    trayAnimation.DelayTime(winrt::Windows::Foundation::TimeSpan(
        std::chrono::milliseconds(*childrenCount * 4)));
  }
  auto batchTray = compositor.CreateScopedBatch(
      winrt::Windows::UI::Composition::CompositionBatchTypes::Animation);

  trayVisual.StartAnimation(L"Offset", trayAnimation);

  batchTray.End();
  batchTray.Completed([&](auto&& sender, auto&& args) {
    Wh_Log(L"batchTray.Completed");
    g_isAnimating = false;
    if (g_unloading) {
      g_initOffsetX = -1;
    }
  });

  //   taskbar

  taskbarFrameRepeater.Margin({0, 0, g_unloading ? 0 : trayFrameWidth, 0});

  auto taskbarStroke = FindChildByName(
      FindChildByClassName(
          FindChildByClassName(rootGrid, L"Taskbar.TaskbarBackground"),
          L"Windows.UI.Xaml.Controls.Grid"),
      L"BackgroundStroke");
  taskbarStroke.Opacity(0);

  auto backgroundFillParent = FindChildByClassName(
      FindChildByClassName(rootGrid, L"Taskbar.TaskbarBackground"),
      L"Windows.UI.Xaml.Controls.Grid");
  auto backgroundFillChild =
      FindChildByName(backgroundFillParent, L"BackgroundFill");
  auto showDesktopButton = FindChildByName(
      FindChildByName(trayFrame, L"SystemTrayFrameGrid"), L"ShowDesktopStack");

  if (backgroundFillChild) {
    auto backgroundFillVisual = winrt::Windows::UI::Xaml::Hosting::
        ElementCompositionPreview::GetElementVisual(backgroundFillChild);
    auto compositor = backgroundFillVisual.Compositor();
    int userDefinedBackgroundHorizontalPadding =
        Wh_GetIntSetting(L"TaskbarBackgroundHorizontalPadding");

    float targetWidth = g_unloading
                            ? rootWidth
                            : (childrenWidth + trayFrameWidth +
                               (userDefinedBackgroundHorizontalPadding * 2) -
                               showDesktopButton.ActualWidth());
    float targetOffsetX = (rootWidth - targetWidth) / 2;

    Wh_Log(L"rootWidth: %f, targetWidth: %f, targetOffsetX %f", rootWidth,
           targetWidth, targetOffsetX);

    auto userDefinedTaskbarCornerRadius =
        static_cast<float>(Wh_GetIntSetting(L"TaskbarCornerRadius"));
    bool userDefinedFlatTaskbarBottomCorners =
        Wh_GetIntSetting(L"FlatTaskbarBottomCorners");

    auto roundedRect = compositor.CreateRoundedRectangleGeometry();

    roundedRect.CornerRadius(
        {userDefinedTaskbarCornerRadius, userDefinedTaskbarCornerRadius});
    bool userDefinedTaskbarOffsetYIsNegative = userDefinedTaskbarOffsetY < 0;

    // -abs(userDefinedTaskbarOffsetY)
    auto clipHeight = static_cast<float>(
        userDefinedTaskbarHeight +
        (userDefinedFlatTaskbarBottomCorners &
                 !userDefinedTaskbarOffsetYIsNegative
             ? userDefinedTaskbarHeight - userDefinedTaskbarCornerRadius
             : 0.0f));

    roundedRect.Size({targetWidth, clipHeight});

    auto geometricClip = compositor.CreateGeometricClip(roundedRect);
    backgroundFillVisual.Clip(geometricClip);

    auto sizeAnimationRect = compositor.CreateVector2KeyFrameAnimation();
    sizeAnimationRect.InsertKeyFrame(
        0.0f, {g_lastTargetWidth, clipHeight});  // Initial size
    sizeAnimationRect.InsertKeyFrame(1.0f,
                                     {targetWidth, clipHeight});  // Target size
    g_lastTargetWidth = targetWidth;
    roundedRect.StartAnimation(L"Size", sizeAnimationRect);

    auto offsetAnimationRect = compositor.CreateVector2KeyFrameAnimation();
    offsetAnimationRect.InsertKeyFrame(
        0.0f, {g_lastTargetOffsetX, 0.0f});  // Initial offset
    offsetAnimationRect.InsertKeyFrame(
        1.0f,
        {targetOffsetX, userDefinedTaskbarOffsetY < 0
                            ? static_cast<float>(abs(userDefinedTaskbarOffsetY))
                            : 0.0f});  // Target offset
                                       // auto rootScrollViewerVisual =
    // winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(ScrollViewer);
    //  rootScrollViewerVisual.Offset(winrt::Windows::Foundation::Numerics::float3(rootScrollViewerVisual.Offset().x,
    //  userDefinedTaskbarOffsetY>0?static_cast<float>(userDefinedTaskbarOffsetY):rootScrollViewerVisual.Offset().y,
    //  rootScrollViewerVisual.Offset().z));

    g_lastTargetOffsetX = targetOffsetX;
    roundedRect.StartAnimation(L"Offset", offsetAnimationRect);

  } else {
    Wh_Log(L"backgroundFill null");
  }

//   SetElementPropertyFromString(rootGrid, L"Windows.UI.Xaml.Controls.Grid",L"BorderThickness", L"1.1");

//   SetElementPropertyFromString( Media::VisualTreeHelper::GetParent(taskbarFrameRepeater).as<FrameworkElement>(), L"Windows.UI.Xaml.Controls.Grid",L"Background", L"<AcrylicBrush TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" FallbackColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\"0\" TintLuminosityOpacity=\".35\" Opacity=\"1\"/>", true);

auto userDefinedTaskbarBackgroundLuminosity=std::to_wstring(Wh_GetIntSetting(L"TaskbarBackgroundLuminosity")/100.0f);
auto userDefinedTaskbarBackgroundOpacity=std::to_wstring(Wh_GetIntSetting(L"TaskbarBackgroundOpacity")/100.0f);
auto userDefinedTaskbarBackgroundTint=std::to_wstring(Wh_GetIntSetting(L"TaskbarBackgroundTint")/100.0f);
SetElementPropertyFromString( backgroundFillChild, L"Windows.UI.Xaml.Shapes.Rectangle",L"Fill", L"<AcrylicBrush TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\""+userDefinedTaskbarBackgroundTint+L"\" TintLuminosityOpacity=\""+userDefinedTaskbarBackgroundLuminosity+L"\" Opacity=\""+userDefinedTaskbarBackgroundOpacity+L"\"/>", true);


  return true;
}

#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.UI.Xaml.h>

void* CTaskBand_ITaskListWndSite_vftable;

void* CSecondaryTaskBand_ITaskListWndSite_vftable;

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

using CSecondaryTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis,
                                                           void** result);
CSecondaryTaskBand_GetTaskbarHost_t CSecondaryTaskBand_GetTaskbarHost_Original;

using std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

XamlRoot XamlRootFromTaskbarHostSharedPtr(void* taskbarHostSharedPtr[2]) {
  if (!taskbarHostSharedPtr[0] && !taskbarHostSharedPtr[1]) {
    return nullptr;
  }

  // Reference: TaskbarHost::FrameHeight
  constexpr size_t kTaskbarElementIUnknownOffset = 0x40;

  auto* taskbarElementIUnknown = *(IUnknown**)((BYTE*)taskbarHostSharedPtr[0] +
                                               kTaskbarElementIUnknownOffset);

  FrameworkElement taskbarElement = nullptr;
  taskbarElementIUnknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                         winrt::put_abi(taskbarElement));

  auto result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;

  std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);

  return result;
}

XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
  HWND hTaskSwWnd = (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
  if (!hTaskSwWnd) {
    return nullptr;
  }

  void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
  void* taskBandForTaskListWndSite = taskBand;
  for (int i = 0; *(void**)taskBandForTaskListWndSite !=
                  CTaskBand_ITaskListWndSite_vftable;
       i++) {
    if (i == 20) {
      return nullptr;
    }

    taskBandForTaskListWndSite = (void**)taskBandForTaskListWndSite + 1;
  }

  void* taskbarHostSharedPtr[2]{};
  CTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                    taskbarHostSharedPtr);

  return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

XamlRoot GetSecondaryTaskbarXamlRoot(HWND hSecondaryTaskbarWnd) {
  HWND hTaskSwWnd =
      (HWND)FindWindowEx(hSecondaryTaskbarWnd, nullptr, L"WorkerW", nullptr);
  if (!hTaskSwWnd) {
    return nullptr;
  }

  void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
  void* taskBandForTaskListWndSite = taskBand;
  for (int i = 0; *(void**)taskBandForTaskListWndSite !=
                  CSecondaryTaskBand_ITaskListWndSite_vftable;
       i++) {
    if (i == 20) {
      return nullptr;
    }

    taskBandForTaskListWndSite = (void**)taskBandForTaskListWndSite + 1;
  }

  void* taskbarHostSharedPtr[2]{};
  CSecondaryTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                             taskbarHostSharedPtr);

  return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

using RunFromWindowThreadProc_t = void(WINAPI*)(void* parameter);

bool RunFromWindowThread(HWND hWnd, RunFromWindowThreadProc_t proc,
                         void* procParam) {
  static const UINT runFromWindowThreadRegisteredMsg =
      RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

  struct RUN_FROM_WINDOW_THREAD_PARAM {
    RunFromWindowThreadProc_t proc;
    void* procParam;
  };

  DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
  if (dwThreadId == 0) {
    return false;
  }

  if (dwThreadId == GetCurrentThreadId()) {
    proc(procParam);
    return true;
  }

  HHOOK hook = SetWindowsHookEx(
      WH_CALLWNDPROC,
      [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
        if (nCode == HC_ACTION) {
          const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
          if (cwp->message == runFromWindowThreadRegisteredMsg) {
            RUN_FROM_WINDOW_THREAD_PARAM* param =
                (RUN_FROM_WINDOW_THREAD_PARAM*)cwp->lParam;
            param->proc(param->procParam);
          }
        }

        return CallNextHookEx(nullptr, nCode, wParam, lParam);
      },
      nullptr, dwThreadId);
  if (!hook) {
    return false;
  }

  RUN_FROM_WINDOW_THREAD_PARAM param;
  param.proc = proc;
  param.procParam = procParam;
  SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);

  UnhookWindowsHookEx(hook);

  return true;
}

void ApplySettingsFromTaskbarThread() {
  Wh_Log(L"Applying settings");

  EnumThreadWindows(
      GetCurrentThreadId(),
      [](HWND hWnd, LPARAM lParam) -> BOOL {
        WCHAR szClassName[32];
        if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
          return TRUE;
        }

        XamlRoot xamlRoot = nullptr;
        if (_wcsicmp(szClassName, L"Shell_TrayWnd") == 0) {
          xamlRoot = GetTaskbarXamlRoot(hWnd);
        } else if (_wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0) {
          xamlRoot = GetSecondaryTaskbarXamlRoot(hWnd);
        } else {
          return TRUE;
        }

        if (!xamlRoot) {
          Wh_Log(L"Getting XamlRoot failed");
          return TRUE;
        }

        if (!ApplyStyle(xamlRoot)) {
          Wh_Log(L"ApplyStyles failed");
          return TRUE;
        }

        return TRUE;
      },
      0);
}

void ApplySettings(HWND hTaskbarWnd) {
  RunFromWindowThread(
      hTaskbarWnd, [](void* pParam) { ApplySettingsFromTaskbarThread(); }, 0);
}

using IUIElement_Arrange_t =
    HRESULT(WINAPI*)(void* pThis, const winrt::Windows::Foundation::Rect* rect);
IUIElement_Arrange_t IUIElement_Arrange_Original;

HRESULT WINAPI IUIElement_Arrange_Hook(
    void* pThis, const winrt::Windows::Foundation::Rect* rect) {
  auto original = [=] { return IUIElement_Arrange_Original(pThis, rect); };

  if (g_unloading) {
    return original();
  }

  FrameworkElement element = nullptr;
  (*(IUnknown**)pThis)
      ->QueryInterface(winrt::guid_of<FrameworkElement>(),
                       winrt::put_abi(element));
  if (!element) {
    return original();
  }

  auto className = winrt::get_class_name(element);
  if (className != L"Taskbar.ExperienceToggleButton") {
    return original();
  }

  auto automationId =
      Automation::AutomationProperties::GetAutomationId(element);
  if (automationId != L"StartButton") {
    return original();
  }

  auto taskbarFrameRepeater =
      Media::VisualTreeHelper::GetParent(element).as<FrameworkElement>();

  element.Dispatcher().TryRunAsync(
      winrt::Windows::UI::Core::CoreDispatcherPriority::High, [element]() {
        auto taskbarFrameRepeater =
            Media::VisualTreeHelper::GetParent(element).as<FrameworkElement>();

        HWND hTaskbarWnd = GetTaskbarWnd();
        if (hTaskbarWnd) {
          ApplySettings(hTaskbarWnd);
        }
      });

  return original();
}

bool HookTaskbarDllSymbols() {
  HMODULE module = LoadLibrary(L"taskbar.dll");
  if (!module) {
    Wh_Log(L"Failed to load taskbar.dll");
    return false;
  }

  WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
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
  wcscat_s(
      dllPath, MAX_PATH,
      LR"(\SystemApps\MicrosoftWindows.Client.Core_cw5n1h2txyewy\Taskbar.View.dll)");
  HMODULE module =
      LoadLibraryEx(dllPath, nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
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

void LoadSettings() {
  g_settings.startMenuOnTheLeft = Wh_GetIntSetting(L"startMenuOnTheLeft");
  g_settings.startMenuWidth = Wh_GetIntSetting(L"startMenuWidth");
}

BOOL Wh_ModInit() {
  LoadSettings();
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
  HWND hTaskbarWnd = GetTaskbarWnd();
  if (hTaskbarWnd) {
    ApplySettings(hTaskbarWnd);
  }
}

void Wh_ModBeforeUninit() {
  g_unloading = true;
  HWND hTaskbarWnd = GetTaskbarWnd();
  if (hTaskbarWnd) {
    ApplySettings(hTaskbarWnd);
  }
}

void Wh_ModUninit() {}

void Wh_ModSettingsChanged() {
  HWND hTaskbarWnd = GetTaskbarWnd();
  if (hTaskbarWnd) {
    ApplySettings(hTaskbarWnd);
  }
}
