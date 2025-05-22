
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



#include <windhawk_utils.h>
#include <windhawk_api.h>
#include <string>
#include <atomic>
#include <functional>
#include <limits>
#include <optional>
#include <regex>
#include <dwmapi.h>
#include <commctrl.h>
#include <roapi.h>
#include <winstring.h>
#include <sstream>
#include <string_view>
#include <winrt/base.h>
#include <chrono>
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


#include <winrt/Windows.UI.Xaml.Shapes.h>

// Auto-generated hook definitions and functions
using TaskbarTelemetry_StartRunningIndicatorSizeChangeAnimation_WithArgs_t = void(WINAPI*)(bool const & param1);
TaskbarTelemetry_StartRunningIndicatorSizeChangeAnimation_WithArgs_t TaskbarTelemetry_StartRunningIndicatorSizeChangeAnimation_WithArgs_Original;
static void WINAPI TaskbarTelemetry_StartRunningIndicatorSizeChangeAnimation_WithArgs_Hook(bool const & param1) {
                Wh_Log(L"Method called: TaskbarTelemetry_StartRunningIndicatorSizeChangeAnimation");
                return TaskbarTelemetry_StartRunningIndicatorSizeChangeAnimation_WithArgs_Original(param1);
            }

bool HookFunctionTaskbarTelemetry_StartRunningIndicatorSizeChangeAnimation_WithArgs() {
    HMODULE module = LoadLibrary(L"Taskbar.View.dll");
    if (!module) {
        Wh_Log(L"Failed to load Taskbar.View.dll for TaskbarTelemetry_StartRunningIndicatorSizeChangeAnimation_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(public: static void __cdecl TaskbarTelemetry::StartRunningIndicatorSizeChangeAnimation<bool const &>(bool const &))"},
                                         &TaskbarTelemetry_StartRunningIndicatorSizeChangeAnimation_WithArgs_Original,
                                         TaskbarTelemetry_StartRunningIndicatorSizeChangeAnimation_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}

winrt::event_token m_sizeChangedToken{};
using winrt__Taskbar__implementation__SharedAnimations_StartRunningIndicatorSizeChangeAnimation_WithArgs_t = void(WINAPI*)(void* pThis, winrt::Windows::UI::Xaml::UIElement const & param1, winrt::Windows::Foundation::Size param2, winrt::Windows::Foundation::Size param3, class std::chrono::duration<__int64 , std::ratio<1 , 10000000> > param6);
winrt__Taskbar__implementation__SharedAnimations_StartRunningIndicatorSizeChangeAnimation_WithArgs_t winrt__Taskbar__implementation__SharedAnimations_StartRunningIndicatorSizeChangeAnimation_WithArgs_Original;
void WINAPI winrt__Taskbar__implementation__SharedAnimations_StartRunningIndicatorSizeChangeAnimation_WithArgs_Hook(void* pThis, winrt::Windows::UI::Xaml::UIElement const & param1, winrt::Windows::Foundation::Size to, winrt::Windows::Foundation::Size from, class std::chrono::duration<__int64 , std::ratio<1 , 10000000> > param6) {
// RunningIndicator

if (const auto xaml = param1.try_as<winrt::Windows::UI::Xaml::FrameworkElement>()) {


 auto compositor = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(xaml).Compositor();
auto visual = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(xaml);

// Set ScaleX (and maintain current ScaleY and ScaleZ if needed)


  float actualWidth = static_cast<float>(xaml.ActualWidth());
  float actualHeight = static_cast<float>(xaml.ActualHeight());
  if(actualWidth>0.0f && actualHeight>0.0f)
 visual.CenterPoint({ static_cast<float>(actualWidth / 2.0f), static_cast<float>(actualHeight / 2.0f), 0.0f });

  bool expanding = to.Width > actualWidth;
  Wh_Log(L"Method called: winrt__Taskbar__implementation__SharedAnimations_StartRunningIndicatorSizeChangeAnimation  %f %f expanding: %d",

         static_cast<float>(xaml.ActualWidth()), static_cast<float>(xaml.Width()), expanding);

    xaml.SizeChanged([](winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::SizeChangedEventArgs const& args){
}
);

//   if (expanding) {
//     to.Width = 10.0f;
//     from.Width = 4.0f;
//     to.Height = 4.0f;
//     from.Height = 4.0f;
//   } else {
//     from.Width = 10.0f;
//     to.Width = 4.0f;
//     to.Height = 4.0f;
//     from.Height = 4.0f;
//   }

float scaleX =expanding?1.0f: actualHeight / actualWidth;
visual.Scale({ scaleX, 1.0f, visual.Scale().z });


//    xaml.Width(from.Width);
}




                    Wh_Log(L"from size: Width = %f, Height = %f", from.Width, from.Height);
    Wh_Log(L"to size: Width = %f, Height = %f", to.Width, to.Height);
  //winrt__Taskbar__implementation__SharedAnimations_StartRunningIndicatorSizeChangeAnimation_WithArgs_Original(pThis, param1, to, from,  param6);

            }



using winrt__Taskbar__implementation__TaskListButton_UpdateRunningIndicatorSize_WithoutArgs_t = void(WINAPI*)(void* pThis);
winrt__Taskbar__implementation__TaskListButton_UpdateRunningIndicatorSize_WithoutArgs_t winrt__Taskbar__implementation__TaskListButton_UpdateRunningIndicatorSize_WithoutArgs_Original;
void WINAPI winrt__Taskbar__implementation__TaskListButton_UpdateRunningIndicatorSize_WithoutArgs_Hook(void* pThis) {
                Wh_Log(L"Method called: winrt__Taskbar__implementation__TaskListButton_UpdateRunningIndicatorSize");
                winrt__Taskbar__implementation__TaskListButton_UpdateRunningIndicatorSize_WithoutArgs_Original(pThis);
            }


bool HookFunctionwinrt__Taskbar__implementation__TaskListButton_UpdateRunningIndicatorSize_WithoutArgs() {
    HMODULE module = LoadLibrary(L"Taskbar.View.dll");
    if (!module) {
        Wh_Log(L"Failed to load Taskbar.View.dll for winrt__Taskbar__implementation__TaskListButton_UpdateRunningIndicatorSize_WithoutArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateRunningIndicatorSize(void))"},
                                         &winrt__Taskbar__implementation__TaskListButton_UpdateRunningIndicatorSize_WithoutArgs_Original,
                                         winrt__Taskbar__implementation__TaskListButton_UpdateRunningIndicatorSize_WithoutArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}





bool HookFunctionwinrt__Taskbar__implementation__SharedAnimations_StartRunningIndicatorSizeChangeAnimation_WithArgs() {
    HMODULE module = LoadLibrary(L"Taskbar.View.dll");
    if (!module) {
        Wh_Log(L"Failed to load Taskbar.View.dll for winrt__Taskbar__implementation__SharedAnimations_StartRunningIndicatorSizeChangeAnimation_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(public: void __cdecl winrt::Taskbar::implementation::SharedAnimations::StartRunningIndicatorSizeChangeAnimation(struct winrt::Windows::UI::Xaml::UIElement const &,struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size,class std::chrono::duration<__int64,struct std::ratio<1,10000000> >)const)"},
                                         &winrt__Taskbar__implementation__SharedAnimations_StartRunningIndicatorSizeChangeAnimation_WithArgs_Original,
                                         winrt__Taskbar__implementation__SharedAnimations_StartRunningIndicatorSizeChangeAnimation_WithArgs_Hook } };
bool success=false;
    if(! WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook))){
        Wh_Log(L"Failed to hook StartRunningIndicatorSizeChangeAnimation; trying with decorated symbol");


    tagWH_FIND_SYMBOL sym;

        void* findhandle=Wh_FindFirstSymbol(module, NULL, &sym);
        do{
          // sym.symbolDecorated is PCWSTR (aka const wchar_t *)
// if (wcsstr(sym.symbolDecorated, L"StartRunningIndicatorSizeChangeAnimation")) {
//     Wh_Log(L"%s", sym.symbolDecorated);
// }

            if(wcsstr(sym.symbolDecorated,LR"(?StartRunningIndicatorSizeChangeAnimation@SharedAnimations@implementation@Taskbar@winrt@@QEBAXAEBUUIElement@Xaml@UI@Windows@4@USize@Foundation@84@1V?$duration@_JU?$ratio@$00$0JIJGIA@@std@@@chrono@std@@@Z)")){
                Wh_Log(L"Foudn something: %s", sym.symbol);

                if(Wh_SetFunctionHook(
    (void*)sym.address,
    (void*)winrt__Taskbar__implementation__SharedAnimations_StartRunningIndicatorSizeChangeAnimation_WithArgs_Hook,
    (void**)&winrt__Taskbar__implementation__SharedAnimations_StartRunningIndicatorSizeChangeAnimation_WithArgs_Original
)
){
    if(Wh_ApplyHookOperations())
Wh_Log(L"Hook successful");
success=true;

                }else{
                    Wh_Log(L"Hook found but failed");



                }

            };
        }while(Wh_FindNextSymbol(findhandle, &sym));
        Wh_FindCloseSymbol(findhandle);


    }else{
        success=true;
    }

    return success;
}


using winrt__Taskbar__implementation__TaskListButton_InitializeRunningIndicator_WithoutArgs_t = void(WINAPI*)(void* pThis);
winrt__Taskbar__implementation__TaskListButton_InitializeRunningIndicator_WithoutArgs_t winrt__Taskbar__implementation__TaskListButton_InitializeRunningIndicator_WithoutArgs_Original;
void WINAPI winrt__Taskbar__implementation__TaskListButton_InitializeRunningIndicator_WithoutArgs_Hook(void* pThis) {
                Wh_Log(L"Method called: winrt__Taskbar__implementation__TaskListButton_InitializeRunningIndicator");
                winrt__Taskbar__implementation__TaskListButton_InitializeRunningIndicator_WithoutArgs_Original(pThis);
            }


bool HookFunctionwinrt__Taskbar__implementation__TaskListButton_InitializeRunningIndicator_WithoutArgs() {
    HMODULE module = LoadLibrary(L"Taskbar.View.dll");
    if (!module) {
        Wh_Log(L"Failed to load Taskbar.View.dll for winrt__Taskbar__implementation__TaskListButton_InitializeRunningIndicator_WithoutArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::InitializeRunningIndicator(void))"},
                                         &winrt__Taskbar__implementation__TaskListButton_InitializeRunningIndicator_WithoutArgs_Original,
                                         winrt__Taskbar__implementation__TaskListButton_InitializeRunningIndicator_WithoutArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


BOOL Wh_ModInit() {
    bool anyHooked = false;
     if (!HookFunctionwinrt__Taskbar__implementation__TaskListButton_InitializeRunningIndicator_WithoutArgs()) {
        Wh_Log(L"Method hooking failed: winrt__Taskbar__implementation__TaskListButton_InitializeRunningIndicator_WithoutArgs");
    } else {
        anyHooked = true;
    }
   if (!HookFunctionTaskbarTelemetry_StartRunningIndicatorSizeChangeAnimation_WithArgs()) {
        Wh_Log(L"Method hooking failed: TaskbarTelemetry_StartRunningIndicatorSizeChangeAnimation_WithArgs");
    } else {
        anyHooked = true;
    }
if (!HookFunctionwinrt__Taskbar__implementation__TaskListButton_UpdateRunningIndicatorSize_WithoutArgs()) {
        Wh_Log(L"Method hooking failed: winrt__Taskbar__implementation__TaskListButton_UpdateRunningIndicatorSize_WithoutArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionwinrt__Taskbar__implementation__SharedAnimations_StartRunningIndicatorSizeChangeAnimation_WithArgs()) {
        Wh_Log(L"Method hooking failed: winrt__Taskbar__implementation__SharedAnimations_StartRunningIndicatorSizeChangeAnimation_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!anyHooked) {         Wh_Log(L"Nothing was hooked");
        return FALSE;
    }
    return TRUE;
}

void Wh_ModUninit() {}
void Wh_ModSettingsChanged() {}





//winrt::event_token sizeChangedToken;
//auto weakIndicator = winrt::make_weak(runningIndicator);
//sizeChangedToken = runningIndicator.SizeChanged(
//    [weakIndicator, &sizeChangedToken](winrt::Windows::Foundation::IInspectable const& /*sender*/,
//                                       winrt::Windows::UI::Xaml::SizeChangedEventArgs const& /*args*/) mutable
//    {
//        if (auto indicator = weakIndicator.get())
//        {
//            std::wstringstream wss;
//wss << L"SizeChanged one-shot handler executed at address: 0x"
//    << std::hex << std::uppercase
//    << reinterpret_cast<uintptr_t>(winrt::get_abi(&sizeChangedToken));
//
//Wh_Log(L"%s",wss.str().c_str());
//            indicator.SizeChanged(sizeChangedToken);
//
//            float actualWidth  = static_cast<float>(indicator.ActualWidth());
//            float actualHeight = static_cast<float>(indicator.ActualHeight());
//            float halfIconSize = g_settings.userDefinedTaskbarIconSize / 2.0f;
//
//            auto visual = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(indicator);
//            if (!visual)
//                return;
//
//            if (actualWidth <= 0.0f || actualHeight <= 0.0f || actualWidth >= halfIconSize)
//            {
//                visual.Clip(nullptr);
//                return;
//            }
//
//            auto compositor = visual.Compositor();
//            auto roundedRect = compositor.CreateRoundedRectangleGeometry();
//
//            float size = std::max(3.0f, actualHeight);
//            roundedRect.CornerRadius({ size / 2.0f, size / 2.0f });
//            roundedRect.Size({ size, size });
//            roundedRect.Offset({ static_cast<float>(indicator.Width()) / 2.0f, 0.0f });
//
//            auto geometricClip = compositor.CreateGeometricClip(roundedRect);
//            visual.Clip(geometricClip);
//        }
//    });
//     runningIndicator.SizeChanged( [](winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::SizeChangedEventArgs const& args){
// if(sender){
// auto const& runningIndicator = sender.template try_as<winrt::Windows::UI::Xaml::FrameworkElement>();
// if(runningIndicator){
//  float actualWidth=std::max(0.0f,static_cast<float>(runningIndicator.ActualWidth()));
// auto compositor = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(runningIndicator).Compositor();
// auto visual = winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::GetElementVisual(runningIndicator);
//         if(actualWidth>0.0f && actualWidth<g_settings.userDefinedTaskbarIconSize/2.0f){

//           auto roundedRect = compositor.CreateRoundedRectangleGeometry();

//         float size=std::max(3.0f,static_cast<float>(runningIndicator.ActualHeight()));
//         roundedRect.CornerRadius({size/2.0f,size/2.0f});
//               roundedRect.Size({size,size});
//       roundedRect.Offset({static_cast<float>(runningIndicator.Width())/2.0f,0.0f});
//         auto geometricClip = compositor.CreateGeometricClip(roundedRect);

//         visual.Clip(geometricClip);
//         }else{
//               visual.Clip(nullptr);
//         }

// Wh_Log(L"asdasd ");
// }
//      }

//   });

