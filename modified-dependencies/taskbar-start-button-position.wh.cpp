
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////       _______..___________.     ___      .______      .___________..______    __    __  .___________..___________.  ______   .__   __. .______     ______        _______. __  .___________. __    ______   .__   __.   ////
////      /       ||           |    /   \     |   _  \     |           ||   _  \  |  |  |  | |           ||           | /  __  \  |  \ |  | |   _  \   /  __  \      /       ||  | |           ||  |  /  __  \  |  \ |  |   ////
////     |   (----``---|  |----`   /  ^  \    |  |_)  |    `---|  |----`|  |_)  | |  |  |  | `---|  |----``---|  |----`|  |  |  | |   \|  | |  |_)  | |  |  |  |    |   (----`|  | `---|  |----`|  | |  |  |  | |   \|  |   ////
////      \   \        |  |       /  /_\  \   |      /         |  |     |   _  <  |  |  |  |     |  |         |  |     |  |  |  | |  . `  | |   ___/  |  |  |  |     \   \    |  |     |  |     |  | |  |  |  | |  . `  |   ////
////  .----)   |       |  |      /  _____  \  |  |\  \----.    |  |     |  |_)  | |  `--'  |     |  |         |  |     |  `--'  | |  |\   | |  |      |  `--'  | .----)   |   |  |     |  |     |  | |  `--'  | |  |\   |   ////
////  |_______/        |__|     /__/     \__\ | _| `._____|    |__|     |______/   \______/      |__|         |__|      \______/  |__| \__| | _|       \______/  |_______/    |__|     |__|     |__|  \______/  |__| \__|   ////
////                                                                                                                                                                                                                        ////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ApplyStyle(XamlRoot xamlRoot);
#include <windhawk_utils.h>
#include <atomic>
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
} g_settings_startbuttonposition;
HWND g_startMenuWnd;
int g_startMenuOriginalWidth;
HWND g_searchMenuWnd;
int g_searchMenuOriginalX;
STDAPI GetDpiForMonitor(HMONITOR hmonitor,
                        MONITOR_DPI_TYPE dpiType,
                        UINT* dpiX,
                        UINT* dpiY);
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
void ApplySettingsStartButtonPosition(HWND hTaskbarWnd) {
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
    auto widgetElement =
        EnumChildElements(taskbarFrameRepeater, [](FrameworkElement child) {
            auto childClassName = winrt::get_class_name(child);
            if (childClassName != L"Taskbar.AugmentedEntryPointButton") {
                return false;
            }
            if (child.Name() != L"AugmentedEntryPointButton") {
                return false;
            }
            auto margin = child.Margin();
            auto offset = child.ActualOffset();
            if (offset.x != margin.Left || offset.y != 0) {
                return false;
            }
            return true;
        });
    if (!widgetElement) {
        element.Dispatcher().TryRunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::High,[element]() {
 ApplySettingsFromTaskbarThread(); 
                double width = element.ActualWidth();
                double minX = std::numeric_limits<double>::infinity();
                auto taskbarFrameRepeater =
                    Media::VisualTreeHelper::GetParent(element)
                        .as<FrameworkElement>();
                EnumChildElements(taskbarFrameRepeater,
                                  [&element, &minX](FrameworkElement child) {
                                      if (child == element) {
                                          return false;
                                      }
                                      auto offset = child.ActualOffset();
                                      if (offset.x >= 0 && offset.x < minX) {
                                          minX = offset.x;
                                      }
                                      return false;
                                  });
                if (minX < width) {
                    Thickness margin = element.Margin();
                    
                    element.Margin(margin);
                } else if (minX > width * 2) {
                    Thickness margin = element.Margin();
                    
                    element.Margin(margin);
                }
            });
    }
    winrt::Windows::Foundation::Rect newRect = *rect;
    newRect.X = 0;
    return original();
}
using AugmentedEntryPointButton_UpdateButtonPadding_t =
    void(WINAPI*)(void* pThis);
void WINAPI AugmentedEntryPointButton_UpdateButtonPadding_Hook_StartButtonPosition(void* pThis) {
    
    AugmentedEntryPointButton_UpdateButtonPadding_Original(pThis);
    if (g_unloading) {
        return;
    }
    FrameworkElement button = nullptr;
    ((IUnknown**)pThis)[1]->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(button));
    if (!button) {
        return;
    }
    button.Dispatcher().TryRunAsync(
        winrt::Windows::UI::Core::CoreDispatcherPriority::High, [button]() {
            auto offset = button.ActualOffset();
            if (offset.x != 0 || offset.y != 0) {
                return;
            }
            auto margin = button.Margin();
            margin.Left = 34;
            button.Margin(margin);
        });
}
bool HookTaskbarDllSymbolsStartButtonPosition() {
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
bool HookTaskbarViewDllSymbolsStartButtonPosition(HMODULE module) {
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IUIElement<struct winrt::Windows::UI::Xaml::IUIElement>::Arrange(struct winrt::Windows::Foundation::Rect const &)const )"},
            &IUIElement_Arrange_Original,
            IUIElement_Arrange_Hook,
        },
        {
            {LR"(protected: virtual void __cdecl winrt::Taskbar::implementation::AugmentedEntryPointButton::UpdateButtonPadding(void))"},
            &AugmentedEntryPointButton_UpdateButtonPadding_Original,
            AugmentedEntryPointButton_UpdateButtonPadding_Hook_StartButtonPosition,
        },
    };
    return HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks));
}
void HandleLoadedModuleIfTaskbarView(HMODULE module, LPCWSTR lpLibFileName) {
    if (!g_taskbarViewDllLoaded && GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoaded.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);
        if (HookTaskbarViewDllSymbolsStartButtonPosition(module)) {
            Wh_ApplyHookOperations();
        }
    }
}
using LoadLibraryExW_t = decltype(&LoadLibraryExW);
HMODULE WINAPI LoadLibraryExW_Hook_StartButtonPosition(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module) {
        HandleLoadedModuleIfTaskbarView(module, lpLibFileName);
    }
    return module;
}
std::wstring GetProcessFileName(DWORD dwProcessId) {
    HANDLE hProcess =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcessId);
    if (!hProcess) {
        return std::wstring{};
    }
    WCHAR processPath[MAX_PATH];
    DWORD dwSize = ARRAYSIZE(processPath);
    if (!QueryFullProcessImageName(hProcess, 0, processPath, &dwSize)) {
        CloseHandle(hProcess);
        return std::wstring{};
    }
    CloseHandle(hProcess);
    PCWSTR processFileNameUpper = wcsrchr(processPath, L'\\');
    if (!processFileNameUpper) {
        return std::wstring{};
    }
    processFileNameUpper++;
    return processFileNameUpper;
}
using DwmSetWindowAttribute_t = decltype(&DwmSetWindowAttribute);
DwmSetWindowAttribute_t DwmSetWindowAttribute_Original;
HRESULT WINAPI DwmSetWindowAttribute_Hook(HWND hwnd,
                                          DWORD dwAttribute,
                                          LPCVOID pvAttribute,
                                          DWORD cbAttribute) {
    auto original = [=]() {
        return DwmSetWindowAttribute_Original(hwnd, dwAttribute, pvAttribute,
                                              cbAttribute);
    };
    if (dwAttribute != DWMWA_CLOAK || cbAttribute != sizeof(BOOL)) {
        return original();
    }
    BOOL cloak = *(BOOL*)pvAttribute;
    if (cloak) {
        return original();
    }
    Wh_Log(L"> %08X", (DWORD)(DWORD_PTR)hwnd);
    DWORD processId = 0;
    if (!hwnd || !GetWindowThreadProcessId(hwnd, &processId)) {
        return original();
    }
    std::wstring processFileName = GetProcessFileName(processId);
    enum class Target {
        StartMenu,
        SearchHost,
    };
    Target target;
    if (_wcsicmp(processFileName.c_str(), L"StartMenuExperienceHost.exe") ==
        0) {
        target = Target::StartMenu;
    } else if (_wcsicmp(processFileName.c_str(), L"SearchHost.exe") == 0) {
        target = Target::SearchHost;
    } else {
        return original();
    }
    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    UINT monitorDpiX = 96;
    UINT monitorDpiY = 96;
    GetDpiForMonitor(monitor, MDT_DEFAULT, &monitorDpiX, &monitorDpiY);
    MONITORINFO monitorInfo{
        .cbSize = sizeof(MONITORINFO),
    };
    GetMonitorInfo(monitor, &monitorInfo);
    RECT targetRect;
    if (!GetWindowRect(hwnd, &targetRect)) {
        return original();
    }
    int x = targetRect.left;
    int y = targetRect.top;
    int cx = targetRect.right - targetRect.left;
    int cy = targetRect.bottom - targetRect.top;
    if (target == Target::StartMenu) {
        int cxNew;
        if (g_settings_startbuttonposition.startMenuOnTheLeft) {
            cxNew = MulDiv(
                g_settings_startbuttonposition.startMenuWidth ? g_settings_startbuttonposition.startMenuWidth : 660,
                monitorDpiX, 96);
            g_startMenuWnd = hwnd;
            g_startMenuOriginalWidth = cx;
        } else {
            if (!g_startMenuOriginalWidth) {
                return original();
            }
            cxNew = g_startMenuOriginalWidth;
            g_startMenuWnd = nullptr;
            g_startMenuOriginalWidth = 0;
        }
        if (cxNew == cx) {
            return original();
        }
        cx = cxNew;
    } else if (target == Target::SearchHost) {
        int xNew;
        if (g_settings_startbuttonposition.startMenuOnTheLeft) {
            if (x == monitorInfo.rcWork.left) {
                return original();
            }
            xNew = monitorInfo.rcWork.left;
            g_searchMenuWnd = hwnd;
            g_searchMenuOriginalX = x;
        } else {
            if (!g_searchMenuOriginalX) {
                return original();
            }
            xNew = g_searchMenuOriginalX;
            g_searchMenuWnd = nullptr;
            g_searchMenuOriginalX = 0;
        }
        if (xNew == x) {
            return original();
        }
        x = xNew;
    }
    SetWindowPos(hwnd, nullptr, x, y, cx, cy, SWP_NOZORDER | SWP_NOACTIVATE);
    return original();
}
void LoadSettingsStartButtonPosition() {
    g_settings_startbuttonposition.startMenuOnTheLeft = 0;
    g_settings_startbuttonposition.startMenuWidth = 0;
}
BOOL Wh_ModInitStartButtonPosition() {
    
    LoadSettingsStartButtonPosition();
    if (!HookTaskbarDllSymbolsStartButtonPosition()) {
        return FALSE;
    }
    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoaded = true;
        if (!HookTaskbarViewDllSymbolsStartButtonPosition(taskbarViewModule)) {
            return FALSE;
        }
    } else {
        Wh_Log(L"Taskbar view module not loaded yet");
        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW =
            (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule,
                                                      "LoadLibraryExW");
        WindhawkUtils::Wh_SetFunctionHookT(pKernelBaseLoadLibraryExW,
                                           LoadLibraryExW_Hook_StartButtonPosition,
                                           &LoadLibraryExW_Original);
    }
    HMODULE dwmapiModule = LoadLibrary(L"dwmapi.dll");
    if (dwmapiModule) {
        auto pDwmSetWindowAttribute =
            (decltype(&DwmSetWindowAttribute))GetProcAddress(
                dwmapiModule, "DwmSetWindowAttribute");
        if (pDwmSetWindowAttribute) {
            WindhawkUtils::Wh_SetFunctionHookT(pDwmSetWindowAttribute,
                                               DwmSetWindowAttribute_Hook,
                                               &DwmSetWindowAttribute_Original);
        }
    }
    return TRUE;
}
void Wh_ModAfterInitStartButtonPosition() {
    
    if (!g_taskbarViewDllLoaded) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewDllLoaded.exchange(true)) {
                Wh_Log(L"Got Taskbar.View.dll");
                if (HookTaskbarViewDllSymbolsStartButtonPosition(taskbarViewModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }
    HWND hTaskbarWnd = GetTaskbarWnd();
    if (hTaskbarWnd) {
        ApplySettingsStartButtonPosition(hTaskbarWnd);
    }
}
void Wh_ModBeforeUninitStartButtonPosition() {
    
    g_unloading = true;
    HWND hTaskbarWnd = GetTaskbarWnd();
    if (hTaskbarWnd) {
        ApplySettingsStartButtonPosition(hTaskbarWnd);
    }
}
void Wh_ModUninitStartButtonPosition() {
    
    if (g_startMenuWnd && g_startMenuOriginalWidth) {
        RECT rect;
        if (GetWindowRect(g_startMenuWnd, &rect)) {
            int x = rect.left;
            int y = rect.top;
            int cx = rect.right - rect.left;
            int cy = rect.bottom - rect.top;
            if (g_startMenuOriginalWidth != cx) {
                cx = g_startMenuOriginalWidth;
                SetWindowPos(g_startMenuWnd, nullptr, x, y, cx, cy,
                             SWP_NOZORDER | SWP_NOACTIVATE);
            }
        }
    }
    if (g_searchMenuWnd && g_searchMenuOriginalX) {
        RECT rect;
        if (GetWindowRect(g_searchMenuWnd, &rect)) {
            int x = rect.left;
            int y = rect.top;
            int cx = rect.right - rect.left;
            int cy = rect.bottom - rect.top;
            if (g_searchMenuOriginalX != x) {
                x = g_searchMenuOriginalX;
                SetWindowPos(g_searchMenuWnd, nullptr, x, y, cx, cy,
                             SWP_NOZORDER | SWP_NOACTIVATE);
            }
        }
    }
}
void Wh_ModSettingsChangedStartButtonPosition() {
    
    LoadSettingsStartButtonPosition();
}