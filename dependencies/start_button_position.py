from __future__ import annotations

import re

from .common import HOOKS_DIR, MOD_PARTS_DIR, read_file, read_patch
from .cpp_patcher import CppPatcher
from .url_processor import URLProcessor


class StartButtonPosition(URLProcessor):
    def __init__(self):
        url = "https://raw.githubusercontent.com/ramensoftware/windhawk-mods/refs/heads/main/mods/taskbar-start-button-position.wh.cpp"
        super().__init__(url, "StartButtonPosition", "b")

    def format_content(self, content: str) -> str:
        patch = CppPatcher(content, source_name=self.name)

        self._remove_unused_upstream_code(patch)
        self._rename_and_disable_upstream_hooks(patch)
        self._patch_start_menu_logic(patch)
        self._insert_custom_hooks(patch)
        self._patch_dwm_targeting(patch)
        self._patch_reload_and_settings(patch)

        return (
            patch.prepend(
                "bool ApplyStyle(FrameworkElement const& element, std::wstring monitorName);\n"
                "bool InitializeDebounce();\n"
                "DispatcherTimer debounceTimer{nullptr};\n\n"
            )
            .replace_regex(
                re.escape(
                    """ RunFromWindowThread(
        hTaskbarWnd, [](void* pParam) { ApplySettingsFromTaskbarThread(); }, 0);"""
                ),
                read_patch("startbuttonposition_apply_settings_window_guard.cpp"),
                in_function="void ApplySettingsStartButtonPosition(HWND hTaskbarWnd)",
            )
            .replace_regex(
                re.escape(
                    """    HMODULE module =
        LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }"""
                ),
                """    bool loadedTaskbarDllForHooking = false;
    HMODULE module = GetModuleHandle(L"taskbar.dll");
    if (!module) {
        module = LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        loadedTaskbarDllForHooking = module != nullptr;
    }
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }""",
                in_function="bool HookTaskbarDllSymbolsStartButtonPosition()",
            )
            .replace_regex(
                re.escape("""return HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));"""),
                """ if (!HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"HookSymbols failed");
        if (loadedTaskbarDllForHooking) {
            FreeLibrary(module);
        }
        return false;
    }
    return true;""",
                in_function="bool HookTaskbarDllSymbolsStartButtonPosition()",
            )
            .insert_before_literal(
                r"using DwmSetWindowAttribute_t = decltype(&DwmSetWindowAttribute);",
                read_patch("startbuttonposition_notification_center_hack.cpp"),
            )
            .replace_regex("taskbarDllHooks", "TaskbarDll_hooks",
                           in_function="bool HookTaskbarDllSymbolsStartButtonPosition()", count=0)

            .replace_regex("symbolHooks", "TaskbarViewDll_hooks",
                           in_function="bool HookTaskbarViewDllSymbolsStartButtonPosition(HMODULE module)", count=0)

            .text()
        )

    def _remove_unused_upstream_code(self, patch: CppPatcher) -> None:
        patch.remove_function("FrameworkElement EnumChildElements(")
        patch.remove_function("FrameworkElement FindChildByName(")
        patch.remove_function("FrameworkElement FindChildByClassName(")
        patch.remove_function("HWND FindCurrentProcessTaskbarWnd(")
        patch.remove_function("HMODULE GetTaskbarViewModuleHandle(")
        patch.remove_literal("LoadLibraryExW_t LoadLibraryExW_Original;")
        patch.remove_regex(
            r"ExperienceToggleButton_UpdateButtonPadding_t[\s\w\d]*?ExperienceToggleButton_UpdateButtonPadding_Original;",
            label="remove ExperienceToggleButton original pointer",
        )
        patch.remove_function("void WINAPI ExperienceToggleButton_UpdateButtonPadding_Hook(")
        patch.remove_regex(
            r"AugmentedEntryPointButton_UpdateButtonPadding_t[\s\w\d]*?AugmentedEntryPointButton_UpdateButtonPadding_Original;",
            label="remove AugmentedEntryPointButton original pointer",
        )
        patch.remove_literal("std::atomic<bool> g_unloading;")
        patch.remove_literal("void ApplyStyle();")
        patch.remove_typedef_enum("MONITOR_DPI_TYPE")

    def _rename_and_disable_upstream_hooks(self, patch: CppPatcher) -> None:
        patch.replace_literal(
            "BOOL Wh_ModSettingsChangedStartButtonPosition(BOOL* bReload)",
            "BOOL Wh_ModSettingsChangedStartButtonPosition()",
            count=1,
        )
        patch.replace_literal(
            "AugmentedEntryPointButton_UpdateButtonPadding_Hook",
            f"AugmentedEntryPointButton_UpdateButtonPadding_Hook_{self.name}",
        )
        patch.replace_literal("HookTaskbarViewDllSymbols", f"HookTaskbarViewDllSymbols{self.name}")
        patch.replace_literal("LoadLibraryExW_Hook", f"LoadLibraryExW_Hook_{self.name}")
        patch.replace_literal(
            "} g_settings_startbuttonposition;",
            "    bool MoveFlyoutNotificationCenter=true;\n} g_settings_startbuttonposition;",
            count=1,
            label="add MoveFlyoutNotificationCenter setting",
        )
        patch.replace_literal("UIElement_Arrange_Hook", f"IUIElement_Arrange_Hook_{self.name}")

    def _patch_start_menu_logic(self, patch: CppPatcher) -> None:
        patch.replace_literal(
            "std::wstring processFileName = GetProcessFileName(processId);",
            "TCHAR className[256];\n"
            "    GetClassName(hwnd, className, 256);\n"
            "    std::wstring windowClassName(className);\n"
            "    std::wstring processFileName = GetProcessFileName(processId);\n"
            '    Wh_Log(L"process: %s, windowClassName: %s",processFileName.c_str(),windowClassName.c_str());',
            count=1,
            label="log process and window class",
        )

        patch.disable_function_at_start(
            "bool ApplyStyle(XamlRoot xamlRoot)",
            "if(true)return false;",
            label="disable upstream ApplyStyle(XamlRoot)",
        )

        patch.replace_literal(
            """if (contentClassName == L"StartMenu.StartBlendedFlexFrame") {
        ApplyStyleRedesignedStartMenu(content);
    }""",
            """if (contentClassName == L"StartMenu.StartBlendedFlexFrame") {
        ApplyStyleClassicStartMenu(content, monitor);
    }""",
            count=1,
            label="route redesigned start menu to classic position handler",
        )

        patch.replace_literal(
            "auto original = [=] { return IUIElement_Arrange_Original(pThis, rect); };",
            "auto original = [=] { return IUIElement_Arrange_Original(pThis, rect); };\nif(true)return original();",
            count=1,
            label="disable upstream arrange hook body",
        )

        patch.replace_function(
            "void ApplyStyleClassicStartMenu(FrameworkElement content, HMONITOR monitor)",
            read_patch("startbuttonposition_apply_style_classic_start_menu.cpp"),
            label="replace ApplyStyleClassicStartMenu",
        )

        patch.replace_literal(
            "frameRoot.HorizontalAlignment(HorizontalAlignment::Left);",
            "frameRoot.HorizontalAlignment(HorizontalAlignment::Center);",
            count=1,
            label="center classic start menu frame root",
        )

    def _insert_custom_hooks(self, patch: CppPatcher) -> None:
        patch.insert_after_literal(
            "WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {",
            read_file(HOOKS_DIR / "taskbar.dll_sigs.cpp"),
            label="insert taskbar.dll signatures",
        )
        patch.insert_after_literal(
            "WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {",
            read_file(HOOKS_DIR / "Taskbar.View.dll_sigs.cpp"),
            label="insert Taskbar.View.dll signatures",
        )
        patch.insert_before_literal(
            "bool HookTaskbarDllSymbolsStartButtonPosition() {",
            read_file(HOOKS_DIR / "taskbar.dll_methods.cpp") + "\n",
            label="insert taskbar.dll hook methods",
        )
        patch.insert_before_literal(
            "bool HookTaskbarViewDllSymbolsStartButtonPosition(HMODULE module) {",
            read_file(HOOKS_DIR / "Taskbar.View.dll_methods.cpp") + "\n",
            label="insert Taskbar.View.dll hook methods",
        )

    def _patch_dwm_targeting(self, patch: CppPatcher) -> None:
        patch.replace_literal(
            "HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);",
            "HMONITOR monitor = ResolveFlyoutMonitorTai(hwnd);",
            count=1,
            label="resolve flyout monitor from taskbar invocation",
        )
        patch.replace_literal(
            "GetDpiForMonitor(monitor, MDT_DEFAULT, &monitorDpiX, &monitorDpiY);",
            """if (!monitor ||
        FAILED(GetDpiForMonitor(monitor, MDT_DEFAULT, &monitorDpiX,
                                &monitorDpiY)) ||
        monitorDpiX == 0 || monitorDpiY == 0) {
        monitorDpiX = 96;
        monitorDpiY = 96;
    }""",
            count=1,
            label="validate target monitor dpi",
        )
        patch.insert_after_literal(
            "enum class DwmTarget {",
            "\n        ShellExperienceHost,",
            label="add ShellExperienceHost DwmTarget",
        )
        patch.replace_regex(
            r"target = DwmTarget::SearchHost;\s+}",
            """target = DwmTarget::SearchHost;
    }else if (_wcsicmp(processFileName.c_str(), L"ShellExperienceHost.exe") == 0) {
        target = DwmTarget::ShellExperienceHost;
    } """,
            count=1,
            label="target ShellExperienceHost",
        )
        patch.replace_literal(
            "GetMonitorInfo(monitor, &monitorInfo);",
            read_patch("startbuttonposition_monitor_taskbar_state_lookup.cpp"),
            count=1,
            label="load taskbar state for monitor",
        )
        patch.replace_literal(
            """    if (_wcsicmp(processFileName.c_str(), L"SearchHost.exe") == 0) {
        target = DwmTarget::SearchHost;
    }else if (_wcsicmp(processFileName.c_str(), L"ShellExperienceHost.exe") == 0) {
        target = DwmTarget::ShellExperienceHost;
    }  else {
        return original();
    }""",
            """    if (_wcsicmp(processFileName.c_str(), L"StartMenuExperienceHost.exe") == 0) {
        target = DwmTarget::StartMenu;
    } else if (_wcsicmp(processFileName.c_str(), L"SearchHost.exe") == 0) {
        target = DwmTarget::SearchHost;
    }else if (_wcsicmp(processFileName.c_str(), L"ShellExperienceHost.exe") == 0) {
        if (_wcsicmp(windowClassName.c_str(), L"Windows.UI.Core.CoreWindow") != 0 ||
            !IsNotificationCenterShellExperienceHostWindowTai(hwnd)) {
            return original();
        }
        target = DwmTarget::ShellExperienceHost;
    }  else {
        return original();
    }
    """,
            count=1,
            label="add StartMenuExperienceHost target branch",
        )
        patch.insert_after_literal(
            "enum class DwmTarget {",
            "\n        StartMenu,",
            label="add StartMenu DwmTarget",
        )
        patch.replace_literal("HWND g_searchMenuWnd;", "HWND g_searchMenuWnd, g_startMenuWnd;", count=1)
        patch.replace_literal(
            "int g_searchMenuOriginalX;",
            "int g_searchMenuOriginalX, g_startMenuOriginalWidth;",
            count=1,
        )
        patch.replace_regex(
            r"if \(target == DwmTarget::SearchHost\).*?\}\s+SetWindowPos",
            read_patch("startbuttonposition_start_menu_position_code.cpp")+ "\nSetWindowPos",
            count=1,
            label="replace SetWindowPos target branch",
        )
        patch.remove_literal("margin.Right = 0;")
        patch.remove_literal("margin.Right = -width;")

    def _patch_reload_and_settings(self, patch: CppPatcher) -> None:
        patch.disable_function_at_start(
            "void Wh_ModUninitStartButtonPosition()",
            "if(true)return;",
            label="disable Wh_ModUninitStartButtonPosition",
        )
        patch.replace_regex(
            r"if \(!ApplyStyle\(xamlRoot\)\) \{.*?\}",
            read_patch("startbuttonposition_apply_style_reload_block.cpp"),
            count=1,
            label="replace ApplyStyle reload block",
        )
        patch.replace_literal(
            'Wh_GetIntSetting(L"startMenuOnTheLeft");',
            'Wh_GetIntSetting(L"MoveFlyoutStartMenu");\n'
            'g_settings_startbuttonposition.MoveFlyoutNotificationCenter = '
            'Wh_GetIntSetting(L"MoveFlyoutNotificationCenter");',
            count=1,
            label="load custom flyout settings",
        )
