// ==WindhawkMod==
// @id              taskbar-centered-condensed
// @name            Tray area next to task area
// @description     Moves the tray area next to the task area of the taskbar, for Windows 11. Heavily based on m417z code, thanks m417z!
// @version         1.0.0
// @author          DarkionAvey
// @github          https://github.com/DarkionAvey/windhawk-taskbar-centered-condensed
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -ldwmapi -lole32 -loleaut32 -lruntimeobject -lshcore
// ==/WindhawkMod==

// Original code is published under The GNU General Public License v3.0.
//
// For bug reports and feeback:
// https://github.com/DarkionAvey/windhawk-taskbar-centered-condensed

// ==WindhawkModReadme==
/*
# Your Awesome Mod
This is a place for useful information about your mod. Use it to describe the
mod, explain why it's useful, and add any other relevant details. You can use
[Markdown](https://en.wikipedia.org/wiki/Markdown) to add links and
**formatting** to the readme.

This short sample customizes Microsoft Paint by forcing it to use just a single
color, and by blocking file opening. To see the mod in action:
- Compile the mod with the button on the left or with Ctrl+B.
- Run Microsoft Paint from the start menu (type "Paint") or by running
  mspaint.exe.
- Draw something and notice that the orange color is always used, regardless of
  the color you pick.
- Try opening a file and notice that it's blocked.

# Getting started
Check out the documentation
[here](https://github.com/ramensoftware/windhawk/wiki/Creating-a-new-mod).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
# Here you can define settings, in YAML format, that the mod users will be able
# to configure. Metadata values such as $name and $description are optional.
# Check out the documentation for more information:
# https://github.com/ramensoftware/windhawk/wiki/Creating-a-new-mod#settings
- NumberOption: 0
  $name: Tray task gap
  $description: The width of the gap between tray and task areas
- NumberOption: 4
  $name: Horizontal padding
  $description: The horizontal padding of the task bar
- NumberOption: 0
  $name: Vertical padding
  $description: The vertical padding of the task bar
*/
// ==/WindhawkModSettings==

// The source code of the mod starts here. This sample was inspired by the great
// article of Kyle Halladay, X64 Function Hooking by Example:
// http://kylehalladay.com/blog/2020/11/13/Hooking-By-Example.html
// If you're new to terms such as code injection and function hooking, the
// article is great to get started.

#include <windhawk_api.h>
#include <windhawk_utils.h>

#include <functional>

#include <dwmapi.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
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

STDAPI GetDpiForMonitor(HMONITOR hmonitor,
                        MONITOR_DPI_TYPE dpiType,
                        UINT* dpiX,
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
    return EnumChildElements(element, [name](FrameworkElement child) {
        return child.Name() == name;
    });
}

FrameworkElement FindChildByClassName(FrameworkElement element,
                                      PCWSTR className) {
    return EnumChildElements(element, [className](FrameworkElement child) {
        return winrt::get_class_name(child) == className;
    });
}
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>

#include <winrt/Windows.UI.Xaml.Markup.h>

#include <sstream>
#include <string>
#include <string_view>

#include <commctrl.h>
#include <roapi.h>
#include <winstring.h>

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>

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
                              const std::wstring_view xamlStyleSetters) {
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
        xaml +=
            L"\">\n"
            L"    <Style TargetType=\"windhawkstyler:";
        xaml += EscapeXmlAttribute(typeName);
        xaml += L"\">\n";
    } else {
        xaml +=
            L">\n"
            L"    <Style TargetType=\"";
        xaml += EscapeXmlAttribute(type);
        xaml += L"\">\n";
    }

    xaml += xamlStyleSetters;

    xaml +=
        L"    </Style>\n"
        L"</ResourceDictionary>";

    Wh_Log(L"======================================== XAML:");
    std::wstringstream ss(xaml);
    std::wstring line;
    while (std::getline(ss, line, L'\n')) {
        Wh_Log(L"%s", line.c_str());
    }
    Wh_Log(L"========================================");

    auto resourceDictionary =
        Markup::XamlReader::Load(xaml).as<ResourceDictionary>();

    auto [styleKey, styleInspectable] = resourceDictionary.First().Current();
    return styleInspectable.as<Style>();
}

Style GetStyleFromXamlSettersWithFallbackType(
    const std::wstring_view type,
    const std::wstring_view fallbackType,
    const std::wstring_view xamlStyleSetters) {
    try {
        return GetStyleFromXamlSetters(type, xamlStyleSetters);
    } catch (winrt::hresult_error const& ex) {
        constexpr HRESULT kStowedException = 0x802B000A;
        if (ex.code() != kStowedException || fallbackType.empty() ||
            fallbackType == type) {
            throw;
        }

        // For some types such as JumpViewUI.JumpListListViewItem, the following
        // error is returned:
        //
        // Error 802B000A: Failed to create a 'System.Type' from the text
        // 'windhawkstyler:JumpListListViewItem'. [Line: 8 Position: 12]
        //
        // Retry with a fallback type, which will allow to at least use the
        // basic properties.
        Wh_Log(L"Retrying with fallback type type due to error %08X: %s",
               ex.code(), ex.message().c_str());
        return GetStyleFromXamlSetters(fallbackType, xamlStyleSetters);
    }
}

void SetElementPropertyFromString(FrameworkElement obj,
                                  const std::wstring& type,
                                  const std::wstring& propertyName,
                                  const std::wstring& propertyValue) {
    try {
        std::wstring xamlSetter = L"<Setter Property=\"";
        xamlSetter += propertyName;
        xamlSetter += L"\" Value=\"";
        xamlSetter += propertyValue;
        xamlSetter += L"\"/>";
        auto style = GetStyleFromXamlSetters(type, xamlSetter);
        for (uint32_t i = 0; i < style.Setters().Size(); ++i) {
            auto setter = style.Setters().GetAt(i).as<Setter>();
            obj.SetValue(setter.Property(), setter.Value());
        }
    } catch (const std::exception& ex) {
        Wh_Log(L"Error: %S", ex.what());
    } catch (const winrt::hresult_error& ex) {
        Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
    } catch (...) {
        Wh_Log(L"Unknown error occurred while setting property.");
    }
}

double CalculateValidChildrenWidth(FrameworkElement element,
                                   int*& childrenCount) {
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
            // Wh_Log(L"Skipping weird child at (%.2f, %.2f)", rect.X, rect.Y);
            continue;
        }

        totalWidth += rect.Width;
    }

    return totalWidth;
}
#include <winrt/Windows.UI.Xaml.Media.Animation.h>

#include <winrt/Windows.UI.Xaml.Media.Animation.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Automation.h.>
#include <chrono>

bool g_isAnimating = false;
signed int g_initOffsetX = -1;
bool ApplyStyle(XamlRoot xamlRoot) {
    // if(g_isAnimating){
    //     return true;
    // }
    g_isAnimating = true;
    auto xamlRootContent = xamlRoot.Content().try_as<FrameworkElement>();

    // todo: move notification center
    auto taskFrame =
        FindChildByClassName(xamlRootContent, L"Taskbar.TaskbarFrame");

    if (!taskFrame) {
        return false;
    }
    auto rootGrid = FindChildByName(taskFrame, L"RootGrid");
    auto taskbarFrameRepeater =
        FindChildByName(rootGrid, L"TaskbarFrameRepeater");
    auto trayFrame =
        FindChildByClassName(xamlRootContent, L"SystemTray.SystemTrayFrame");
    if (!taskbarFrameRepeater || !trayFrame)
        return false;

    auto trayFrameWidth = trayFrame.ActualWidth();

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
    float newXOffset =
        (centered + childrenWidth / 2) + Wh_GetIntSetting(L"Tray task gap");

    boolean movingInwards = originalOffset.x > newXOffset;

    Wh_Log(L"Original Offset: %f, New offset: %f, Children %f",
           originalOffset.x, newXOffset, childrenWidth);

    auto taskbarVisual = winrt::Windows::UI::Xaml::Hosting::
        ElementCompositionPreview::GetElementVisual(taskbarFrameRepeater);
    auto compositor = taskbarVisual.Compositor();
    float targetOffsetXTray =
        (g_unloading ? (float)g_initOffsetX : static_cast<float>(newXOffset));

    auto trayAnimation = compositor.CreateVector3KeyFrameAnimation();

    trayAnimation.InsertKeyFrame(
        1.0f, winrt::Windows::Foundation::Numerics::float3{targetOffsetXTray,
                                                           0.0f, 0.0f});

    if (movingInwards) {
        trayAnimation.DelayTime(winrt::Windows::Foundation::TimeSpan(
            std::chrono::milliseconds(*childrenCount * 4)));
    }
    auto batch = compositor.CreateScopedBatch(
        winrt::Windows::UI::Composition::CompositionBatchTypes::Animation);

    trayVisual.StartAnimation(L"Offset", trayAnimation);

    batch.End();
    batch.Completed([&](auto&& sender, auto&& args) {
        Wh_Log(L"batch.Completed");
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
    auto backgroundFillChild = FindChildByName(backgroundFillParent,L"BackgroundFill");


    if (backgroundFillChild) {
        SetElementPropertyFromString(
           backgroundFillParent,
            L"Windows.UI.Xaml.Controls.Grid", L"CornerRadius", L"20");




        auto backgroundFillVisual = winrt::Windows::UI::Xaml::Hosting::
            ElementCompositionPreview::GetElementVisual(backgroundFillChild);
        auto compositor = backgroundFillVisual.Compositor();

        float targetWidth =
            g_unloading ? rootWidth : (childrenWidth + trayFrameWidth);
        float targetOffsetX = (rootWidth - targetWidth) / 2;

        Wh_Log(L"rootWidth: %f, targetWidth: %f, targetOffsetX %f", rootWidth,
               targetWidth, targetOffsetX);

        // Width animation
        auto widthAnimation = compositor.CreateScalarKeyFrameAnimation();
        widthAnimation.InsertKeyFrame(1.0f, targetWidth);
        widthAnimation.Duration(std::chrono::milliseconds(500));

        // Offset animation
        auto offsetAnimation = compositor.CreateVector3KeyFrameAnimation();
        offsetAnimation.InsertKeyFrame(
            1.0f, winrt::Windows::Foundation::Numerics::float3(
                      targetOffsetX, 
                      backgroundFillVisual.Offset().y,
                      backgroundFillVisual.Offset().z));
        offsetAnimation.Duration(std::chrono::milliseconds(500));

        auto delay = std::chrono::milliseconds(
            movingInwards ? 200 : 0);  // Adjust delay as needed
        widthAnimation.DelayTime(delay);
        offsetAnimation.DelayTime(delay);
        // Start animations
            backgroundFillVisual.StartAnimation(L"Size.X", widthAnimation);
           backgroundFillVisual.StartAnimation(L"Offset", offsetAnimation);


   auto roundedRect = compositor.CreateRoundedRectangleGeometry();
roundedRect.CornerRadius({ 20.0f, 20.0f }); // Set corner radius
roundedRect.Size({targetWidth,static_cast<float>(backgroundFillChild.ActualHeight())} ); // Match visual size


auto geometricClip = compositor.CreateGeometricClip(roundedRect);
geometricClip.Offset({0,0});
backgroundFillVisual.Clip(geometricClip);







    } else {
        Wh_Log(L"backgroundFill null");
    }

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

    auto* taskbarElementIUnknown =
        *(IUnknown**)((BYTE*)taskbarHostSharedPtr[0] +
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

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
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

HRESULT WINAPI
IUIElement_Arrange_Hook(void* pThis,
                        const winrt::Windows::Foundation::Rect* rect) {
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
                Media::VisualTreeHelper::GetParent(element)
                    .as<FrameworkElement>();

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

void Wh_ModUninit() {
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}
