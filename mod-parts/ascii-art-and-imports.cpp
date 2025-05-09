
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
////  ____    __    ____  __  .__   __.  _______    ______     ______  __  ___   ////
////  \   \  /  \  /   / |  | |  \ |  | |       \  /  __  \   /      ||  |/  /   ////
////   \   \/    \/   /  |  | |   \|  | |  .--.  ||  |  |  | |  ,----'|  '  /    ////
////    \            /   |  | |  . `  | |  |  |  ||  |  |  | |  |     |    <     ////
////     \    /\    /    |  | |  |\   | |  '--'  ||  `--'  | |  `----.|  .  \    ////
////      \__/  \__/     |__| |__| \__| |_______/  \______/   \______||__|\__\   ////
////                                                                             ////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

#include <dwmapi.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <functional>
#undef GetCurrentTime
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.Animation.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>
#include <commctrl.h>
#include <roapi.h>
#include <winstring.h>
#include <chrono>
#include <string>
#include <string_view>
#include <vector>
#include <regex>
#include <sstream>
#include <algorithm>
#include <atomic>
#include <unordered_map>
#include <limits>
#include <winrt/Windows.Graphics.Imaging.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.Storage.Search.h>
#include <set>
#include <utility>



using namespace winrt::Windows::UI::Xaml;

STDAPI GetDpiForMonitor(HMONITOR hmonitor, MONITOR_DPI_TYPE dpiType, UINT* dpiX, UINT* dpiY);


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
  std::vector<std::wstring> userDefinedDividedAppNames;
} g_settings;


bool g_invalidateCustomIcons = false;
static std::set<std::pair<std::wstring, std::wstring>> g_taskbarIcons;
static std::set<std::pair<std::wstring, std::wstring>> g_trayIcons;