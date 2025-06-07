
// ==WindhawkMod==
// @id      hooks-spy-mod
// @name    Spy for function calls
// @description     Print the names of functions being called
// @version 0.1
// @author  DarkionAvey
// @include explorer.exe
// @compilerOptions -ldwmapi -lole32 -loleaut32 -lruntimeobject -lshcore -lcomctl32 -Wl,--export-all-symbols
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
This mod prints the names of functions being called.
*/
// ==/WindhawkModReadme==

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
////   __    __    ______     ______    __  ___      _______.             _______..______   ____    ____   ////
////  |  |  |  |  /  __  \   /  __  \  |  |/  /     /       |            /       ||   _  \  \   \  /   /   ////
////  |  |__|  | |  |  |  | |  |  |  | |  '  /     |   (----` ______    |   (----`|  |_)  |  \   \/   /    ////
////  |   __   | |  |  |  | |  |  |  | |    <       \   \    |______|    \   \    |   ___/    \_    _/     ////
////  |  |  |  | |  `--'  | |  `--'  | |  .  \  .----)   |           .----)   |   |  |          |  |       ////
////  |__|  |__|  \______/   \______/  |__|\__\ |_______/            |_______/    | _|          |__|       ////
////                                                                                                       ////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <commctrl.h>
#include <dwmapi.h>
#include <roapi.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <winrt/base.h>
#include <winstring.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <functional>
#include <limits>
#include <optional>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#undef GetCurrentTime
#include <commctrl.h>
#include <roapi.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.Imaging.h>
#include <winrt/Windows.Storage.Search.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.Animation.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>
#include <winstring.h>

#include <atomic>
#include <string_view>
#include <vector>

using winrt__Taskbar__implementation__SharedAnimations_StartRunningIndicatorSizeChangeAnimation_WithArgs_t = void(WINAPI*)(void* pThis, winrt::Windows::UI::Xaml::UIElement const& param1, winrt::Windows::Foundation::Size param2, winrt::Windows::Foundation::Size param3,
                                                                                                                           class std::chrono::duration<__int64, std::ratio<1, 10000000> > param6);
winrt__Taskbar__implementation__SharedAnimations_StartRunningIndicatorSizeChangeAnimation_WithArgs_t winrt__Taskbar__implementation__SharedAnimations_StartRunningIndicatorSizeChangeAnimation_WithArgs_Original;
void WINAPI winrt__Taskbar__implementation__SharedAnimations_StartRunningIndicatorSizeChangeAnimation_WithArgs_Hook(void* pThis, winrt::Windows::UI::Xaml::UIElement const& param1, winrt::Windows::Foundation::Size to, winrt::Windows::Foundation::Size from,
                                                                                                                    class std::chrono::duration<__int64, std::ratio<1, 10000000> > param6) {
  if (const auto xaml = param1.try_as<winrt::Windows::UI::Xaml::FrameworkElement>()) {
    Wh_Log(L"Method called: winrt__Taskbar__implementation__SharedAnimations_StartRunningIndicatorSizeChangeAnimation  %f %f expanding: %d",static_cast<float>(xaml.ActualWidth()), static_cast<float>(xaml.Width()), to.Width > from.Width);

    auto visual = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(xaml);
    winrt::Windows::UI::Composition::Compositor compositor = visual.Compositor();
    bool dragged = to.Width == 0;

    float actualWidth = static_cast<float>(xaml.ActualWidth());
    float actualHeight = static_cast<float>(xaml.ActualHeight());

    if (actualWidth > 0.0f && actualHeight > 0.0f && from.Height > 0 && from.Width > 0) {
      visual.CenterPoint({actualWidth / 2.0f, actualHeight / 2.0f, 0.0f});
      bool expanding = to.Width > from.Width;
      auto animation = compositor.CreateVector3KeyFrameAnimation();
      float scaleX = !expanding ? 0.8f : (from.Height + (from.Height * 0.1f)) / std::max(1.0f, from.Width);

      if (dragged) {
        animation.InsertKeyFrame(0.0f, {0.0f, 1.0f, visual.Scale().z});
        animation.InsertKeyFrame(1.0f, {scaleX * scaleX / 2.0f, 1.0f, visual.Scale().z});
      }

      else {
        animation.InsertKeyFrame(1.0f, {scaleX, 1.0f, visual.Scale().z});
      }

      animation.Duration(winrt::Windows::Foundation::TimeSpan{std::chrono::milliseconds(250)});
      visual.StartAnimation(L"Scale", animation);
    }
  }

  Wh_Log(L"from size: Width = %f, Height = %f", from.Width, from.Height);
  Wh_Log(L"to size: Width = %f, Height = %f", to.Width, to.Height);

}

bool HookFunctionwinrt__Taskbar__implementation__SharedAnimations_StartRunningIndicatorSizeChangeAnimation_WithArgs() {
  HMODULE module = LoadLibrary(L"Taskbar.View.dll");
  if (!module) {
    Wh_Log(L"Failed to load Taskbar.View.dll for winrt__Taskbar__implementation__SharedAnimations_StartRunningIndicatorSizeChangeAnimation_WithArgs");
    return false;
  }

  WindhawkUtils::SYMBOL_HOOK hook[] = {
      {{LR"(public: void __cdecl winrt::Taskbar::implementation::SharedAnimations::StartRunningIndicatorSizeChangeAnimation(struct winrt::Windows::UI::Xaml::UIElement const &,struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size,class std::chrono::duration<__int64,struct std::ratio<1,10000000> >)const)"},
       &winrt__Taskbar__implementation__SharedAnimations_StartRunningIndicatorSizeChangeAnimation_WithArgs_Original,
       winrt__Taskbar__implementation__SharedAnimations_StartRunningIndicatorSizeChangeAnimation_WithArgs_Hook}};
  bool success = false;
  if (!WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook))) {
    Wh_Log(L"Failed to hook StartRunningIndicatorSizeChangeAnimation; trying with decorated symbol");

    tagWH_FIND_SYMBOL sym;

    void* findhandle = Wh_FindFirstSymbol(module, NULL, &sym);
    do {
      if (wcsstr(sym.symbolDecorated, LR"(?StartRunningIndicatorSizeChangeAnimation@SharedAnimations@implementation@Taskbar@winrt@@QEBAXAEBUUIElement@Xaml@UI@Windows@4@USize@Foundation@84@1V?$duration@_JU?$ratio@$00$0JIJGIA@@std@@@chrono@std@@@Z)")) {
        Wh_Log(L"Foudn something: %s", sym.symbol);

        if (Wh_SetFunctionHook((void*)sym.address, (void*)winrt__Taskbar__implementation__SharedAnimations_StartRunningIndicatorSizeChangeAnimation_WithArgs_Hook, (void**)&winrt__Taskbar__implementation__SharedAnimations_StartRunningIndicatorSizeChangeAnimation_WithArgs_Original)) {
          if (Wh_ApplyHookOperations()) Wh_Log(L"Hook successful");
          success = true;

        } else {
          Wh_Log(L"Hook found but failed");
        }
      };
    } while (Wh_FindNextSymbol(findhandle, &sym));
    Wh_FindCloseSymbol(findhandle);

  } else {
    success = true;
  }

  return success;
}

BOOL Wh_ModInit() {
  bool anyHooked = false;

  if (!HookFunctionwinrt__Taskbar__implementation__SharedAnimations_StartRunningIndicatorSizeChangeAnimation_WithArgs()) {
    Wh_Log(L"Method hooking failed: winrt__Taskbar__implementation__SharedAnimations_StartRunningIndicatorSizeChangeAnimation_WithArgs");
  } else {
    anyHooked = true;
  }

  if (!anyHooked) {
    Wh_Log(L"Nothing was hooked");
    return FALSE;
  }
  return TRUE;
}

void Wh_ModUninit() {}
void Wh_ModSettingsChanged() {}