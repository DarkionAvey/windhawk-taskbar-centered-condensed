
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
#include <chrono>
#include <string>
#include <regex>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <limits>
#include <set>
#include <utility>
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
#include <string_view>
#include <vector>
#include <atomic>
#include <winrt/Windows.Graphics.Imaging.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.Storage.Search.h>
#include <chrono>
#include <thread>
#include <windows.h>
#include <psapi.h>

using namespace winrt::Windows::UI::Xaml;

STDAPI GetDpiForMonitor(HMONITOR hmonitor, MONITOR_DPI_TYPE dpiType, UINT* dpiX, UINT* dpiY);

bool g_invalidateCustomIcons = false;
static std::set<std::pair<std::wstring, std::wstring>> g_taskbarIcons;
static std::set<std::pair<std::wstring, std::wstring>> g_trayIcons;