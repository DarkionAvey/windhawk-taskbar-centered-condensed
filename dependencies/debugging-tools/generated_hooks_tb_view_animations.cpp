
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
#include <chrono>

// Auto-generated hook definitions and functions

using TaskbarTelemetry_StartAepAnimatedIconOpacityAnimation_WithArgs_t = static void(WINAPI*)(void* pThis, bool const & param1);
TaskbarTelemetry_StartAepAnimatedIconOpacityAnimation_WithArgs_t TaskbarTelemetry_StartAepAnimatedIconOpacityAnimation_WithArgs_Original;
static void WINAPI TaskbarTelemetry_StartAepAnimatedIconOpacityAnimation_WithArgs_Hook(void* pThis, bool const & param1) {
                Wh_Log(L"Method called: TaskbarTelemetry_StartAepAnimatedIconOpacityAnimation");
                return TaskbarTelemetry_StartAepAnimatedIconOpacityAnimation_WithArgs_Original(pThis, param1);
            }

using TaskbarTelemetry_StartAugmentedEntryPointEntranceAnimation_WithArgs_t = static void(WINAPI*)(void* pThis, bool const & param1);
TaskbarTelemetry_StartAugmentedEntryPointEntranceAnimation_WithArgs_t TaskbarTelemetry_StartAugmentedEntryPointEntranceAnimation_WithArgs_Original;
static void WINAPI TaskbarTelemetry_StartAugmentedEntryPointEntranceAnimation_WithArgs_Hook(void* pThis, bool const & param1) {
                Wh_Log(L"Method called: TaskbarTelemetry_StartAugmentedEntryPointEntranceAnimation");
                return TaskbarTelemetry_StartAugmentedEntryPointEntranceAnimation_WithArgs_Original(pThis, param1);
            }

using TaskbarTelemetry_StartAugmentedEntryPointExitAnimation_WithArgs_t = static void(WINAPI*)(void* pThis, bool const & param1);
TaskbarTelemetry_StartAugmentedEntryPointExitAnimation_WithArgs_t TaskbarTelemetry_StartAugmentedEntryPointExitAnimation_WithArgs_Original;
static void WINAPI TaskbarTelemetry_StartAugmentedEntryPointExitAnimation_WithArgs_Hook(void* pThis, bool const & param1) {
                Wh_Log(L"Method called: TaskbarTelemetry_StartAugmentedEntryPointExitAnimation");
                return TaskbarTelemetry_StartAugmentedEntryPointExitAnimation_WithArgs_Original(pThis, param1);
            }

using TaskbarTelemetry_StartBadgeEntranceAnimation_WithArgs_t = static void(WINAPI*)(void* pThis, bool const & param1);
TaskbarTelemetry_StartBadgeEntranceAnimation_WithArgs_t TaskbarTelemetry_StartBadgeEntranceAnimation_WithArgs_Original;
static void WINAPI TaskbarTelemetry_StartBadgeEntranceAnimation_WithArgs_Hook(void* pThis, bool const & param1) {
                Wh_Log(L"Method called: TaskbarTelemetry_StartBadgeEntranceAnimation");
                return TaskbarTelemetry_StartBadgeEntranceAnimation_WithArgs_Original(pThis, param1);
            }

using TaskbarTelemetry_StartBadgeExitAnimation_WithArgs_t = static void(WINAPI*)(void* pThis, bool const & param1);
TaskbarTelemetry_StartBadgeExitAnimation_WithArgs_t TaskbarTelemetry_StartBadgeExitAnimation_WithArgs_Original;
static void WINAPI TaskbarTelemetry_StartBadgeExitAnimation_WithArgs_Hook(void* pThis, bool const & param1) {
                Wh_Log(L"Method called: TaskbarTelemetry_StartBadgeExitAnimation");
                return TaskbarTelemetry_StartBadgeExitAnimation_WithArgs_Original(pThis, param1);
            }

using TaskbarTelemetry_StartFlyoutEntranceAnimation_WithArgs_t = static void(WINAPI*)(void* pThis, bool const & param1);
TaskbarTelemetry_StartFlyoutEntranceAnimation_WithArgs_t TaskbarTelemetry_StartFlyoutEntranceAnimation_WithArgs_Original;
static void WINAPI TaskbarTelemetry_StartFlyoutEntranceAnimation_WithArgs_Hook(void* pThis, bool const & param1) {
                Wh_Log(L"Method called: TaskbarTelemetry_StartFlyoutEntranceAnimation");
                return TaskbarTelemetry_StartFlyoutEntranceAnimation_WithArgs_Original(pThis, param1);
            }

using TaskbarTelemetry_StartFlyoutExitAnimation_WithArgs_t = static void(WINAPI*)(void* pThis, bool const & param1);
TaskbarTelemetry_StartFlyoutExitAnimation_WithArgs_t TaskbarTelemetry_StartFlyoutExitAnimation_WithArgs_Original;
static void WINAPI TaskbarTelemetry_StartFlyoutExitAnimation_WithArgs_Hook(void* pThis, bool const & param1) {
                Wh_Log(L"Method called: TaskbarTelemetry_StartFlyoutExitAnimation");
                return TaskbarTelemetry_StartFlyoutExitAnimation_WithArgs_Original(pThis, param1);
            }

using TaskbarTelemetry_StartFlyoutSizeChangedAnimation_WithArgs_t = static void(WINAPI*)(void* pThis, bool const & param1);
TaskbarTelemetry_StartFlyoutSizeChangedAnimation_WithArgs_t TaskbarTelemetry_StartFlyoutSizeChangedAnimation_WithArgs_Original;
static void WINAPI TaskbarTelemetry_StartFlyoutSizeChangedAnimation_WithArgs_Hook(void* pThis, bool const & param1) {
                Wh_Log(L"Method called: TaskbarTelemetry_StartFlyoutSizeChangedAnimation");
                return TaskbarTelemetry_StartFlyoutSizeChangedAnimation_WithArgs_Original(pThis, param1);
            }

using TaskbarTelemetry_StartIconSizeChangeAnimation_WithArgs_t = static void(WINAPI*)(void* pThis, bool const & param1);
TaskbarTelemetry_StartIconSizeChangeAnimation_WithArgs_t TaskbarTelemetry_StartIconSizeChangeAnimation_WithArgs_Original;
static void WINAPI TaskbarTelemetry_StartIconSizeChangeAnimation_WithArgs_Hook(void* pThis, bool const & param1) {
                Wh_Log(L"Method called: TaskbarTelemetry_StartIconSizeChangeAnimation");
                return TaskbarTelemetry_StartIconSizeChangeAnimation_WithArgs_Original(pThis, param1);
            }

using TaskbarTelemetry_StartItemDragCompletedScaleAnimation_WithArgs_t = static void(WINAPI*)(void* pThis, bool const & param1);
TaskbarTelemetry_StartItemDragCompletedScaleAnimation_WithArgs_t TaskbarTelemetry_StartItemDragCompletedScaleAnimation_WithArgs_Original;
static void WINAPI TaskbarTelemetry_StartItemDragCompletedScaleAnimation_WithArgs_Hook(void* pThis, bool const & param1) {
                Wh_Log(L"Method called: TaskbarTelemetry_StartItemDragCompletedScaleAnimation");
                return TaskbarTelemetry_StartItemDragCompletedScaleAnimation_WithArgs_Original(pThis, param1);
            }

using TaskbarTelemetry_StartItemDragStartedScaleAnimation_WithArgs_t = static void(WINAPI*)(void* pThis, bool const & param1);
TaskbarTelemetry_StartItemDragStartedScaleAnimation_WithArgs_t TaskbarTelemetry_StartItemDragStartedScaleAnimation_WithArgs_Original;
static void WINAPI TaskbarTelemetry_StartItemDragStartedScaleAnimation_WithArgs_Hook(void* pThis, bool const & param1) {
                Wh_Log(L"Method called: TaskbarTelemetry_StartItemDragStartedScaleAnimation");
                return TaskbarTelemetry_StartItemDragStartedScaleAnimation_WithArgs_Original(pThis, param1);
            }

using TaskbarTelemetry_StartItemEntranceAnimation_WithArgs_t = static void(WINAPI*)(void* pThis, bool const & param1);
TaskbarTelemetry_StartItemEntranceAnimation_WithArgs_t TaskbarTelemetry_StartItemEntranceAnimation_WithArgs_Original;
static void WINAPI TaskbarTelemetry_StartItemEntranceAnimation_WithArgs_Hook(void* pThis, bool const & param1) {
                Wh_Log(L"Method called: TaskbarTelemetry_StartItemEntranceAnimation");
                return TaskbarTelemetry_StartItemEntranceAnimation_WithArgs_Original(pThis, param1);
            }

using TaskbarTelemetry_StartItemMinimizeAnimation_WithArgs_t = static void(WINAPI*)(void* pThis, bool const & param1);
TaskbarTelemetry_StartItemMinimizeAnimation_WithArgs_t TaskbarTelemetry_StartItemMinimizeAnimation_WithArgs_Original;
static void WINAPI TaskbarTelemetry_StartItemMinimizeAnimation_WithArgs_Hook(void* pThis, bool const & param1) {
                Wh_Log(L"Method called: TaskbarTelemetry_StartItemMinimizeAnimation");
                return TaskbarTelemetry_StartItemMinimizeAnimation_WithArgs_Original(pThis, param1);
            }

using TaskbarTelemetry_StartItemPlateEntranceAnimation_WithArgs_t = static void(WINAPI*)(void* pThis, bool const & param1);
TaskbarTelemetry_StartItemPlateEntranceAnimation_WithArgs_t TaskbarTelemetry_StartItemPlateEntranceAnimation_WithArgs_Original;
static void WINAPI TaskbarTelemetry_StartItemPlateEntranceAnimation_WithArgs_Hook(void* pThis, bool const & param1) {
                Wh_Log(L"Method called: TaskbarTelemetry_StartItemPlateEntranceAnimation");
                return TaskbarTelemetry_StartItemPlateEntranceAnimation_WithArgs_Original(pThis, param1);
            }

using TaskbarTelemetry_StartItemPressedScaleAnimation_WithArgs_t = static void(WINAPI*)(void* pThis, bool const & param1);
TaskbarTelemetry_StartItemPressedScaleAnimation_WithArgs_t TaskbarTelemetry_StartItemPressedScaleAnimation_WithArgs_Original;
static void WINAPI TaskbarTelemetry_StartItemPressedScaleAnimation_WithArgs_Hook(void* pThis, bool const & param1) {
                Wh_Log(L"Method called: TaskbarTelemetry_StartItemPressedScaleAnimation");
                return TaskbarTelemetry_StartItemPressedScaleAnimation_WithArgs_Original(pThis, param1);
            }

using TaskbarTelemetry_StartItemReleasedScaleAnimation_WithArgs_t = static void(WINAPI*)(void* pThis, bool const & param1);
TaskbarTelemetry_StartItemReleasedScaleAnimation_WithArgs_t TaskbarTelemetry_StartItemReleasedScaleAnimation_WithArgs_Original;
static void WINAPI TaskbarTelemetry_StartItemReleasedScaleAnimation_WithArgs_Hook(void* pThis, bool const & param1) {
                Wh_Log(L"Method called: TaskbarTelemetry_StartItemReleasedScaleAnimation");
                return TaskbarTelemetry_StartItemReleasedScaleAnimation_WithArgs_Original(pThis, param1);
            }

using TaskbarTelemetry_StartItemRestoreAnimation_WithArgs_t = static void(WINAPI*)(void* pThis, bool const & param1);
TaskbarTelemetry_StartItemRestoreAnimation_WithArgs_t TaskbarTelemetry_StartItemRestoreAnimation_WithArgs_Original;
static void WINAPI TaskbarTelemetry_StartItemRestoreAnimation_WithArgs_Hook(void* pThis, bool const & param1) {
                Wh_Log(L"Method called: TaskbarTelemetry_StartItemRestoreAnimation");
                return TaskbarTelemetry_StartItemRestoreAnimation_WithArgs_Original(pThis, param1);
            }

using TaskbarTelemetry_StartItemTranslationAnimation_WithArgs_t = static void(WINAPI*)(void* pThis, bool const & param1);
TaskbarTelemetry_StartItemTranslationAnimation_WithArgs_t TaskbarTelemetry_StartItemTranslationAnimation_WithArgs_Original;
static void WINAPI TaskbarTelemetry_StartItemTranslationAnimation_WithArgs_Hook(void* pThis, bool const & param1) {
                Wh_Log(L"Method called: TaskbarTelemetry_StartItemTranslationAnimation");
                return TaskbarTelemetry_StartItemTranslationAnimation_WithArgs_Original(pThis, param1);
            }

using TaskbarTelemetry_StartLabelAnimation_WithArgs_t = static void(WINAPI*)(void* pThis, bool const & param1);
TaskbarTelemetry_StartLabelAnimation_WithArgs_t TaskbarTelemetry_StartLabelAnimation_WithArgs_Original;
static void WINAPI TaskbarTelemetry_StartLabelAnimation_WithArgs_Hook(void* pThis, bool const & param1) {
                Wh_Log(L"Method called: TaskbarTelemetry_StartLabelAnimation");
                return TaskbarTelemetry_StartLabelAnimation_WithArgs_Original(pThis, param1);
            }

using TaskbarTelemetry_StartMostRecentlyUsedSwitchAnimation_WithArgs_t = static void(WINAPI*)(void* pThis, bool const & param1);
TaskbarTelemetry_StartMostRecentlyUsedSwitchAnimation_WithArgs_t TaskbarTelemetry_StartMostRecentlyUsedSwitchAnimation_WithArgs_Original;
static void WINAPI TaskbarTelemetry_StartMostRecentlyUsedSwitchAnimation_WithArgs_Hook(void* pThis, bool const & param1) {
                Wh_Log(L"Method called: TaskbarTelemetry_StartMostRecentlyUsedSwitchAnimation");
                return TaskbarTelemetry_StartMostRecentlyUsedSwitchAnimation_WithArgs_Original(pThis, param1);
            }

using TaskbarTelemetry_StartOverflowButtonEntranceAnimation_WithArgs_t = static void(WINAPI*)(void* pThis, bool const & param1);
TaskbarTelemetry_StartOverflowButtonEntranceAnimation_WithArgs_t TaskbarTelemetry_StartOverflowButtonEntranceAnimation_WithArgs_Original;
static void WINAPI TaskbarTelemetry_StartOverflowButtonEntranceAnimation_WithArgs_Hook(void* pThis, bool const & param1) {
                Wh_Log(L"Method called: TaskbarTelemetry_StartOverflowButtonEntranceAnimation");
                return TaskbarTelemetry_StartOverflowButtonEntranceAnimation_WithArgs_Original(pThis, param1);
            }

using TaskbarTelemetry_StartPaginationButtonDisabledOpacityAnimation_WithArgs_t = static void(WINAPI*)(void* pThis, bool const & param1);
TaskbarTelemetry_StartPaginationButtonDisabledOpacityAnimation_WithArgs_t TaskbarTelemetry_StartPaginationButtonDisabledOpacityAnimation_WithArgs_Original;
static void WINAPI TaskbarTelemetry_StartPaginationButtonDisabledOpacityAnimation_WithArgs_Hook(void* pThis, bool const & param1) {
                Wh_Log(L"Method called: TaskbarTelemetry_StartPaginationButtonDisabledOpacityAnimation");
                return TaskbarTelemetry_StartPaginationButtonDisabledOpacityAnimation_WithArgs_Original(pThis, param1);
            }

using TaskbarTelemetry_StartPaginationButtonEnabledOpacityAnimation_WithArgs_t = static void(WINAPI*)(void* pThis, bool const & param1);
TaskbarTelemetry_StartPaginationButtonEnabledOpacityAnimation_WithArgs_t TaskbarTelemetry_StartPaginationButtonEnabledOpacityAnimation_WithArgs_Original;
static void WINAPI TaskbarTelemetry_StartPaginationButtonEnabledOpacityAnimation_WithArgs_Hook(void* pThis, bool const & param1) {
                Wh_Log(L"Method called: TaskbarTelemetry_StartPaginationButtonEnabledOpacityAnimation");
                return TaskbarTelemetry_StartPaginationButtonEnabledOpacityAnimation_WithArgs_Original(pThis, param1);
            }

using TaskbarTelemetry_StartPaginationButtonReleasedAnimation_WithArgs_t = static void(WINAPI*)(void* pThis, bool const & param1);
TaskbarTelemetry_StartPaginationButtonReleasedAnimation_WithArgs_t TaskbarTelemetry_StartPaginationButtonReleasedAnimation_WithArgs_Original;
static void WINAPI TaskbarTelemetry_StartPaginationButtonReleasedAnimation_WithArgs_Hook(void* pThis, bool const & param1) {
                Wh_Log(L"Method called: TaskbarTelemetry_StartPaginationButtonReleasedAnimation");
                return TaskbarTelemetry_StartPaginationButtonReleasedAnimation_WithArgs_Original(pThis, param1);
            }

using TaskbarTelemetry_StartRequestingAttentionAnimation_WithArgs_t = static void(WINAPI*)(void* pThis, bool const & param1);
TaskbarTelemetry_StartRequestingAttentionAnimation_WithArgs_t TaskbarTelemetry_StartRequestingAttentionAnimation_WithArgs_Original;
static void WINAPI TaskbarTelemetry_StartRequestingAttentionAnimation_WithArgs_Hook(void* pThis, bool const & param1) {
                Wh_Log(L"Method called: TaskbarTelemetry_StartRequestingAttentionAnimation");
                return TaskbarTelemetry_StartRequestingAttentionAnimation_WithArgs_Original(pThis, param1);
            }

using TaskbarTelemetry_StartRunningIndicatorSizeChangeAnimation_WithArgs_t = static void(WINAPI*)(void* pThis, bool const & param1);
TaskbarTelemetry_StartRunningIndicatorSizeChangeAnimation_WithArgs_t TaskbarTelemetry_StartRunningIndicatorSizeChangeAnimation_WithArgs_Original;
static void WINAPI TaskbarTelemetry_StartRunningIndicatorSizeChangeAnimation_WithArgs_Hook(void* pThis, bool const & param1) {
                Wh_Log(L"Method called: TaskbarTelemetry_StartRunningIndicatorSizeChangeAnimation");
                return TaskbarTelemetry_StartRunningIndicatorSizeChangeAnimation_WithArgs_Original(pThis, param1);
            }

using TaskbarTelemetry_StartTransitionFromOverflowToTaskbarAnimation_WithArgs_t = static void(WINAPI*)(void* pThis, bool const & param1);
TaskbarTelemetry_StartTransitionFromOverflowToTaskbarAnimation_WithArgs_t TaskbarTelemetry_StartTransitionFromOverflowToTaskbarAnimation_WithArgs_Original;
static void WINAPI TaskbarTelemetry_StartTransitionFromOverflowToTaskbarAnimation_WithArgs_Hook(void* pThis, bool const & param1) {
                Wh_Log(L"Method called: TaskbarTelemetry_StartTransitionFromOverflowToTaskbarAnimation");
                return TaskbarTelemetry_StartTransitionFromOverflowToTaskbarAnimation_WithArgs_Original(pThis, param1);
            }

using TaskbarTelemetry_StartTransitionFromOverflowToTaskbarScaleAnimation_WithArgs_t = static void(WINAPI*)(void* pThis, bool const & param1);
TaskbarTelemetry_StartTransitionFromOverflowToTaskbarScaleAnimation_WithArgs_t TaskbarTelemetry_StartTransitionFromOverflowToTaskbarScaleAnimation_WithArgs_Original;
static void WINAPI TaskbarTelemetry_StartTransitionFromOverflowToTaskbarScaleAnimation_WithArgs_Hook(void* pThis, bool const & param1) {
                Wh_Log(L"Method called: TaskbarTelemetry_StartTransitionFromOverflowToTaskbarScaleAnimation");
                return TaskbarTelemetry_StartTransitionFromOverflowToTaskbarScaleAnimation_WithArgs_Original(pThis, param1);
            }


bool HookFunctionTaskbarTelemetry_StartAepAnimatedIconOpacityAnimation_WithArgs() {
    HMODULE module = LoadLibrary(L"Taskbar.View.dll");
    if (!module) {
        Wh_Log(L"Failed to load Taskbar.View.dll for TaskbarTelemetry_StartAepAnimatedIconOpacityAnimation_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(public: static void __cdecl TaskbarTelemetry::StartAepAnimatedIconOpacityAnimation(bool const &))"},
                                         &TaskbarTelemetry_StartAepAnimatedIconOpacityAnimation_WithArgs_Original,
                                         TaskbarTelemetry_StartAepAnimatedIconOpacityAnimation_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionTaskbarTelemetry_StartAugmentedEntryPointEntranceAnimation_WithArgs() {
    HMODULE module = LoadLibrary(L"Taskbar.View.dll");
    if (!module) {
        Wh_Log(L"Failed to load Taskbar.View.dll for TaskbarTelemetry_StartAugmentedEntryPointEntranceAnimation_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(public: static void __cdecl TaskbarTelemetry::StartAugmentedEntryPointEntranceAnimation(bool const &))"},
                                         &TaskbarTelemetry_StartAugmentedEntryPointEntranceAnimation_WithArgs_Original,
                                         TaskbarTelemetry_StartAugmentedEntryPointEntranceAnimation_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionTaskbarTelemetry_StartAugmentedEntryPointExitAnimation_WithArgs() {
    HMODULE module = LoadLibrary(L"Taskbar.View.dll");
    if (!module) {
        Wh_Log(L"Failed to load Taskbar.View.dll for TaskbarTelemetry_StartAugmentedEntryPointExitAnimation_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(public: static void __cdecl TaskbarTelemetry::StartAugmentedEntryPointExitAnimation(bool const &))"},
                                         &TaskbarTelemetry_StartAugmentedEntryPointExitAnimation_WithArgs_Original,
                                         TaskbarTelemetry_StartAugmentedEntryPointExitAnimation_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionTaskbarTelemetry_StartBadgeEntranceAnimation_WithArgs() {
    HMODULE module = LoadLibrary(L"Taskbar.View.dll");
    if (!module) {
        Wh_Log(L"Failed to load Taskbar.View.dll for TaskbarTelemetry_StartBadgeEntranceAnimation_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(public: static void __cdecl TaskbarTelemetry::StartBadgeEntranceAnimation(bool const &))"},
                                         &TaskbarTelemetry_StartBadgeEntranceAnimation_WithArgs_Original,
                                         TaskbarTelemetry_StartBadgeEntranceAnimation_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionTaskbarTelemetry_StartBadgeExitAnimation_WithArgs() {
    HMODULE module = LoadLibrary(L"Taskbar.View.dll");
    if (!module) {
        Wh_Log(L"Failed to load Taskbar.View.dll for TaskbarTelemetry_StartBadgeExitAnimation_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(public: static void __cdecl TaskbarTelemetry::StartBadgeExitAnimation(bool const &))"},
                                         &TaskbarTelemetry_StartBadgeExitAnimation_WithArgs_Original,
                                         TaskbarTelemetry_StartBadgeExitAnimation_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionTaskbarTelemetry_StartFlyoutEntranceAnimation_WithArgs() {
    HMODULE module = LoadLibrary(L"Taskbar.View.dll");
    if (!module) {
        Wh_Log(L"Failed to load Taskbar.View.dll for TaskbarTelemetry_StartFlyoutEntranceAnimation_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(public: static void __cdecl TaskbarTelemetry::StartFlyoutEntranceAnimation(bool const &))"},
                                         &TaskbarTelemetry_StartFlyoutEntranceAnimation_WithArgs_Original,
                                         TaskbarTelemetry_StartFlyoutEntranceAnimation_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionTaskbarTelemetry_StartFlyoutExitAnimation_WithArgs() {
    HMODULE module = LoadLibrary(L"Taskbar.View.dll");
    if (!module) {
        Wh_Log(L"Failed to load Taskbar.View.dll for TaskbarTelemetry_StartFlyoutExitAnimation_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(public: static void __cdecl TaskbarTelemetry::StartFlyoutExitAnimation(bool const &))"},
                                         &TaskbarTelemetry_StartFlyoutExitAnimation_WithArgs_Original,
                                         TaskbarTelemetry_StartFlyoutExitAnimation_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionTaskbarTelemetry_StartFlyoutSizeChangedAnimation_WithArgs() {
    HMODULE module = LoadLibrary(L"Taskbar.View.dll");
    if (!module) {
        Wh_Log(L"Failed to load Taskbar.View.dll for TaskbarTelemetry_StartFlyoutSizeChangedAnimation_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(public: static void __cdecl TaskbarTelemetry::StartFlyoutSizeChangedAnimation(bool const &))"},
                                         &TaskbarTelemetry_StartFlyoutSizeChangedAnimation_WithArgs_Original,
                                         TaskbarTelemetry_StartFlyoutSizeChangedAnimation_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionTaskbarTelemetry_StartIconSizeChangeAnimation_WithArgs() {
    HMODULE module = LoadLibrary(L"Taskbar.View.dll");
    if (!module) {
        Wh_Log(L"Failed to load Taskbar.View.dll for TaskbarTelemetry_StartIconSizeChangeAnimation_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(public: static void __cdecl TaskbarTelemetry::StartIconSizeChangeAnimation(bool const &))"},
                                         &TaskbarTelemetry_StartIconSizeChangeAnimation_WithArgs_Original,
                                         TaskbarTelemetry_StartIconSizeChangeAnimation_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionTaskbarTelemetry_StartItemDragCompletedScaleAnimation_WithArgs() {
    HMODULE module = LoadLibrary(L"Taskbar.View.dll");
    if (!module) {
        Wh_Log(L"Failed to load Taskbar.View.dll for TaskbarTelemetry_StartItemDragCompletedScaleAnimation_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(public: static void __cdecl TaskbarTelemetry::StartItemDragCompletedScaleAnimation(bool const &))"},
                                         &TaskbarTelemetry_StartItemDragCompletedScaleAnimation_WithArgs_Original,
                                         TaskbarTelemetry_StartItemDragCompletedScaleAnimation_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionTaskbarTelemetry_StartItemDragStartedScaleAnimation_WithArgs() {
    HMODULE module = LoadLibrary(L"Taskbar.View.dll");
    if (!module) {
        Wh_Log(L"Failed to load Taskbar.View.dll for TaskbarTelemetry_StartItemDragStartedScaleAnimation_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(public: static void __cdecl TaskbarTelemetry::StartItemDragStartedScaleAnimation(bool const &))"},
                                         &TaskbarTelemetry_StartItemDragStartedScaleAnimation_WithArgs_Original,
                                         TaskbarTelemetry_StartItemDragStartedScaleAnimation_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionTaskbarTelemetry_StartItemEntranceAnimation_WithArgs() {
    HMODULE module = LoadLibrary(L"Taskbar.View.dll");
    if (!module) {
        Wh_Log(L"Failed to load Taskbar.View.dll for TaskbarTelemetry_StartItemEntranceAnimation_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(public: static void __cdecl TaskbarTelemetry::StartItemEntranceAnimation(bool const &))"},
                                         &TaskbarTelemetry_StartItemEntranceAnimation_WithArgs_Original,
                                         TaskbarTelemetry_StartItemEntranceAnimation_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionTaskbarTelemetry_StartItemMinimizeAnimation_WithArgs() {
    HMODULE module = LoadLibrary(L"Taskbar.View.dll");
    if (!module) {
        Wh_Log(L"Failed to load Taskbar.View.dll for TaskbarTelemetry_StartItemMinimizeAnimation_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(public: static void __cdecl TaskbarTelemetry::StartItemMinimizeAnimation(bool const &))"},
                                         &TaskbarTelemetry_StartItemMinimizeAnimation_WithArgs_Original,
                                         TaskbarTelemetry_StartItemMinimizeAnimation_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionTaskbarTelemetry_StartItemPlateEntranceAnimation_WithArgs() {
    HMODULE module = LoadLibrary(L"Taskbar.View.dll");
    if (!module) {
        Wh_Log(L"Failed to load Taskbar.View.dll for TaskbarTelemetry_StartItemPlateEntranceAnimation_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(public: static void __cdecl TaskbarTelemetry::StartItemPlateEntranceAnimation(bool const &))"},
                                         &TaskbarTelemetry_StartItemPlateEntranceAnimation_WithArgs_Original,
                                         TaskbarTelemetry_StartItemPlateEntranceAnimation_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionTaskbarTelemetry_StartItemPressedScaleAnimation_WithArgs() {
    HMODULE module = LoadLibrary(L"Taskbar.View.dll");
    if (!module) {
        Wh_Log(L"Failed to load Taskbar.View.dll for TaskbarTelemetry_StartItemPressedScaleAnimation_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(public: static void __cdecl TaskbarTelemetry::StartItemPressedScaleAnimation(bool const &))"},
                                         &TaskbarTelemetry_StartItemPressedScaleAnimation_WithArgs_Original,
                                         TaskbarTelemetry_StartItemPressedScaleAnimation_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionTaskbarTelemetry_StartItemReleasedScaleAnimation_WithArgs() {
    HMODULE module = LoadLibrary(L"Taskbar.View.dll");
    if (!module) {
        Wh_Log(L"Failed to load Taskbar.View.dll for TaskbarTelemetry_StartItemReleasedScaleAnimation_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(public: static void __cdecl TaskbarTelemetry::StartItemReleasedScaleAnimation(bool const &))"},
                                         &TaskbarTelemetry_StartItemReleasedScaleAnimation_WithArgs_Original,
                                         TaskbarTelemetry_StartItemReleasedScaleAnimation_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionTaskbarTelemetry_StartItemRestoreAnimation_WithArgs() {
    HMODULE module = LoadLibrary(L"Taskbar.View.dll");
    if (!module) {
        Wh_Log(L"Failed to load Taskbar.View.dll for TaskbarTelemetry_StartItemRestoreAnimation_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(public: static void __cdecl TaskbarTelemetry::StartItemRestoreAnimation(bool const &))"},
                                         &TaskbarTelemetry_StartItemRestoreAnimation_WithArgs_Original,
                                         TaskbarTelemetry_StartItemRestoreAnimation_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionTaskbarTelemetry_StartItemTranslationAnimation_WithArgs() {
    HMODULE module = LoadLibrary(L"Taskbar.View.dll");
    if (!module) {
        Wh_Log(L"Failed to load Taskbar.View.dll for TaskbarTelemetry_StartItemTranslationAnimation_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(public: static void __cdecl TaskbarTelemetry::StartItemTranslationAnimation(bool const &))"},
                                         &TaskbarTelemetry_StartItemTranslationAnimation_WithArgs_Original,
                                         TaskbarTelemetry_StartItemTranslationAnimation_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionTaskbarTelemetry_StartLabelAnimation_WithArgs() {
    HMODULE module = LoadLibrary(L"Taskbar.View.dll");
    if (!module) {
        Wh_Log(L"Failed to load Taskbar.View.dll for TaskbarTelemetry_StartLabelAnimation_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(public: static void __cdecl TaskbarTelemetry::StartLabelAnimation(bool const &))"},
                                         &TaskbarTelemetry_StartLabelAnimation_WithArgs_Original,
                                         TaskbarTelemetry_StartLabelAnimation_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionTaskbarTelemetry_StartMostRecentlyUsedSwitchAnimation_WithArgs() {
    HMODULE module = LoadLibrary(L"Taskbar.View.dll");
    if (!module) {
        Wh_Log(L"Failed to load Taskbar.View.dll for TaskbarTelemetry_StartMostRecentlyUsedSwitchAnimation_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(public: static void __cdecl TaskbarTelemetry::StartMostRecentlyUsedSwitchAnimation(bool const &))"},
                                         &TaskbarTelemetry_StartMostRecentlyUsedSwitchAnimation_WithArgs_Original,
                                         TaskbarTelemetry_StartMostRecentlyUsedSwitchAnimation_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionTaskbarTelemetry_StartOverflowButtonEntranceAnimation_WithArgs() {
    HMODULE module = LoadLibrary(L"Taskbar.View.dll");
    if (!module) {
        Wh_Log(L"Failed to load Taskbar.View.dll for TaskbarTelemetry_StartOverflowButtonEntranceAnimation_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(public: static void __cdecl TaskbarTelemetry::StartOverflowButtonEntranceAnimation(bool const &))"},
                                         &TaskbarTelemetry_StartOverflowButtonEntranceAnimation_WithArgs_Original,
                                         TaskbarTelemetry_StartOverflowButtonEntranceAnimation_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionTaskbarTelemetry_StartPaginationButtonDisabledOpacityAnimation_WithArgs() {
    HMODULE module = LoadLibrary(L"Taskbar.View.dll");
    if (!module) {
        Wh_Log(L"Failed to load Taskbar.View.dll for TaskbarTelemetry_StartPaginationButtonDisabledOpacityAnimation_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(public: static void __cdecl TaskbarTelemetry::StartPaginationButtonDisabledOpacityAnimation(bool const &))"},
                                         &TaskbarTelemetry_StartPaginationButtonDisabledOpacityAnimation_WithArgs_Original,
                                         TaskbarTelemetry_StartPaginationButtonDisabledOpacityAnimation_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionTaskbarTelemetry_StartPaginationButtonEnabledOpacityAnimation_WithArgs() {
    HMODULE module = LoadLibrary(L"Taskbar.View.dll");
    if (!module) {
        Wh_Log(L"Failed to load Taskbar.View.dll for TaskbarTelemetry_StartPaginationButtonEnabledOpacityAnimation_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(public: static void __cdecl TaskbarTelemetry::StartPaginationButtonEnabledOpacityAnimation(bool const &))"},
                                         &TaskbarTelemetry_StartPaginationButtonEnabledOpacityAnimation_WithArgs_Original,
                                         TaskbarTelemetry_StartPaginationButtonEnabledOpacityAnimation_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionTaskbarTelemetry_StartPaginationButtonReleasedAnimation_WithArgs() {
    HMODULE module = LoadLibrary(L"Taskbar.View.dll");
    if (!module) {
        Wh_Log(L"Failed to load Taskbar.View.dll for TaskbarTelemetry_StartPaginationButtonReleasedAnimation_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(public: static void __cdecl TaskbarTelemetry::StartPaginationButtonReleasedAnimation(bool const &))"},
                                         &TaskbarTelemetry_StartPaginationButtonReleasedAnimation_WithArgs_Original,
                                         TaskbarTelemetry_StartPaginationButtonReleasedAnimation_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionTaskbarTelemetry_StartRequestingAttentionAnimation_WithArgs() {
    HMODULE module = LoadLibrary(L"Taskbar.View.dll");
    if (!module) {
        Wh_Log(L"Failed to load Taskbar.View.dll for TaskbarTelemetry_StartRequestingAttentionAnimation_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(public: static void __cdecl TaskbarTelemetry::StartRequestingAttentionAnimation(bool const &))"},
                                         &TaskbarTelemetry_StartRequestingAttentionAnimation_WithArgs_Original,
                                         TaskbarTelemetry_StartRequestingAttentionAnimation_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionTaskbarTelemetry_StartRunningIndicatorSizeChangeAnimation_WithArgs() {
    HMODULE module = LoadLibrary(L"Taskbar.View.dll");
    if (!module) {
        Wh_Log(L"Failed to load Taskbar.View.dll for TaskbarTelemetry_StartRunningIndicatorSizeChangeAnimation_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(public: static void __cdecl TaskbarTelemetry::StartRunningIndicatorSizeChangeAnimation(bool const &))"},
                                         &TaskbarTelemetry_StartRunningIndicatorSizeChangeAnimation_WithArgs_Original,
                                         TaskbarTelemetry_StartRunningIndicatorSizeChangeAnimation_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionTaskbarTelemetry_StartTransitionFromOverflowToTaskbarAnimation_WithArgs() {
    HMODULE module = LoadLibrary(L"Taskbar.View.dll");
    if (!module) {
        Wh_Log(L"Failed to load Taskbar.View.dll for TaskbarTelemetry_StartTransitionFromOverflowToTaskbarAnimation_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(public: static void __cdecl TaskbarTelemetry::StartTransitionFromOverflowToTaskbarAnimation(bool const &))"},
                                         &TaskbarTelemetry_StartTransitionFromOverflowToTaskbarAnimation_WithArgs_Original,
                                         TaskbarTelemetry_StartTransitionFromOverflowToTaskbarAnimation_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


bool HookFunctionTaskbarTelemetry_StartTransitionFromOverflowToTaskbarScaleAnimation_WithArgs() {
    HMODULE module = LoadLibrary(L"Taskbar.View.dll");
    if (!module) {
        Wh_Log(L"Failed to load Taskbar.View.dll for TaskbarTelemetry_StartTransitionFromOverflowToTaskbarScaleAnimation_WithArgs");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook[] = { { {LR"(public: static void __cdecl TaskbarTelemetry::StartTransitionFromOverflowToTaskbarScaleAnimation(bool const &))"},
                                         &TaskbarTelemetry_StartTransitionFromOverflowToTaskbarScaleAnimation_WithArgs_Original,
                                         TaskbarTelemetry_StartTransitionFromOverflowToTaskbarScaleAnimation_WithArgs_Hook } };

    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));
}


BOOL Wh_ModInit() {
    bool anyHooked = false;
    if (!HookFunctionTaskbarTelemetry_StartAepAnimatedIconOpacityAnimation_WithArgs()) {
        Wh_Log(L"Method hooking failed: TaskbarTelemetry_StartAepAnimatedIconOpacityAnimation_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionTaskbarTelemetry_StartAugmentedEntryPointEntranceAnimation_WithArgs()) {
        Wh_Log(L"Method hooking failed: TaskbarTelemetry_StartAugmentedEntryPointEntranceAnimation_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionTaskbarTelemetry_StartAugmentedEntryPointExitAnimation_WithArgs()) {
        Wh_Log(L"Method hooking failed: TaskbarTelemetry_StartAugmentedEntryPointExitAnimation_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionTaskbarTelemetry_StartBadgeEntranceAnimation_WithArgs()) {
        Wh_Log(L"Method hooking failed: TaskbarTelemetry_StartBadgeEntranceAnimation_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionTaskbarTelemetry_StartBadgeExitAnimation_WithArgs()) {
        Wh_Log(L"Method hooking failed: TaskbarTelemetry_StartBadgeExitAnimation_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionTaskbarTelemetry_StartFlyoutEntranceAnimation_WithArgs()) {
        Wh_Log(L"Method hooking failed: TaskbarTelemetry_StartFlyoutEntranceAnimation_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionTaskbarTelemetry_StartFlyoutExitAnimation_WithArgs()) {
        Wh_Log(L"Method hooking failed: TaskbarTelemetry_StartFlyoutExitAnimation_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionTaskbarTelemetry_StartFlyoutSizeChangedAnimation_WithArgs()) {
        Wh_Log(L"Method hooking failed: TaskbarTelemetry_StartFlyoutSizeChangedAnimation_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionTaskbarTelemetry_StartIconSizeChangeAnimation_WithArgs()) {
        Wh_Log(L"Method hooking failed: TaskbarTelemetry_StartIconSizeChangeAnimation_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionTaskbarTelemetry_StartItemDragCompletedScaleAnimation_WithArgs()) {
        Wh_Log(L"Method hooking failed: TaskbarTelemetry_StartItemDragCompletedScaleAnimation_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionTaskbarTelemetry_StartItemDragStartedScaleAnimation_WithArgs()) {
        Wh_Log(L"Method hooking failed: TaskbarTelemetry_StartItemDragStartedScaleAnimation_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionTaskbarTelemetry_StartItemEntranceAnimation_WithArgs()) {
        Wh_Log(L"Method hooking failed: TaskbarTelemetry_StartItemEntranceAnimation_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionTaskbarTelemetry_StartItemMinimizeAnimation_WithArgs()) {
        Wh_Log(L"Method hooking failed: TaskbarTelemetry_StartItemMinimizeAnimation_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionTaskbarTelemetry_StartItemPlateEntranceAnimation_WithArgs()) {
        Wh_Log(L"Method hooking failed: TaskbarTelemetry_StartItemPlateEntranceAnimation_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionTaskbarTelemetry_StartItemPressedScaleAnimation_WithArgs()) {
        Wh_Log(L"Method hooking failed: TaskbarTelemetry_StartItemPressedScaleAnimation_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionTaskbarTelemetry_StartItemReleasedScaleAnimation_WithArgs()) {
        Wh_Log(L"Method hooking failed: TaskbarTelemetry_StartItemReleasedScaleAnimation_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionTaskbarTelemetry_StartItemRestoreAnimation_WithArgs()) {
        Wh_Log(L"Method hooking failed: TaskbarTelemetry_StartItemRestoreAnimation_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionTaskbarTelemetry_StartItemTranslationAnimation_WithArgs()) {
        Wh_Log(L"Method hooking failed: TaskbarTelemetry_StartItemTranslationAnimation_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionTaskbarTelemetry_StartLabelAnimation_WithArgs()) {
        Wh_Log(L"Method hooking failed: TaskbarTelemetry_StartLabelAnimation_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionTaskbarTelemetry_StartMostRecentlyUsedSwitchAnimation_WithArgs()) {
        Wh_Log(L"Method hooking failed: TaskbarTelemetry_StartMostRecentlyUsedSwitchAnimation_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionTaskbarTelemetry_StartOverflowButtonEntranceAnimation_WithArgs()) {
        Wh_Log(L"Method hooking failed: TaskbarTelemetry_StartOverflowButtonEntranceAnimation_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionTaskbarTelemetry_StartPaginationButtonDisabledOpacityAnimation_WithArgs()) {
        Wh_Log(L"Method hooking failed: TaskbarTelemetry_StartPaginationButtonDisabledOpacityAnimation_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionTaskbarTelemetry_StartPaginationButtonEnabledOpacityAnimation_WithArgs()) {
        Wh_Log(L"Method hooking failed: TaskbarTelemetry_StartPaginationButtonEnabledOpacityAnimation_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionTaskbarTelemetry_StartPaginationButtonReleasedAnimation_WithArgs()) {
        Wh_Log(L"Method hooking failed: TaskbarTelemetry_StartPaginationButtonReleasedAnimation_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionTaskbarTelemetry_StartRequestingAttentionAnimation_WithArgs()) {
        Wh_Log(L"Method hooking failed: TaskbarTelemetry_StartRequestingAttentionAnimation_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionTaskbarTelemetry_StartRunningIndicatorSizeChangeAnimation_WithArgs()) {
        Wh_Log(L"Method hooking failed: TaskbarTelemetry_StartRunningIndicatorSizeChangeAnimation_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionTaskbarTelemetry_StartTransitionFromOverflowToTaskbarAnimation_WithArgs()) {
        Wh_Log(L"Method hooking failed: TaskbarTelemetry_StartTransitionFromOverflowToTaskbarAnimation_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!HookFunctionTaskbarTelemetry_StartTransitionFromOverflowToTaskbarScaleAnimation_WithArgs()) {
        Wh_Log(L"Method hooking failed: TaskbarTelemetry_StartTransitionFromOverflowToTaskbarScaleAnimation_WithArgs");
    } else {
        anyHooked = true;
    }

    if (!anyHooked) {
        return FALSE;
    }
    return TRUE;
}

void Wh_ModUninit() {}
void Wh_ModSettingsChanged() {}