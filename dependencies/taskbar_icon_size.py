from __future__ import annotations

import re

from .common import MOD_PARTS_DIR, read_file, read_patch
from .cpp_patcher import CppPatcher
from .url_processor import URLProcessor


class TaskbarIconSizeMod(URLProcessor):
    def __init__(self):
        url = "https://raw.githubusercontent.com/ramensoftware/windhawk-mods/refs/heads/main/mods/taskbar-icon-size.wh.cpp"
        super().__init__(url, "TBIconSize", "a")

    def format_content(self, content: str) -> str:
        patch = CppPatcher(content, source_name=self.name)

        return (
            patch.replace_literal('Wh_GetIntSetting(L"IconSize")', 'Wh_GetIntSetting(L"TaskbarIconSize")')
            .insert_after_regex(
                r"ExperienceToggleButton_UpdateButtonPadding_t[\s\n]+ExperienceToggleButton_UpdateButtonPadding_Original;",
                "double GetEffectiveTaskbarButtonTargetWidth();\n"
                "bool EnsureElementTaskbarButtonWidth(FrameworkElement const& element,\n"
                "                                     double targetWidth,\n"
                "                                     bool allowHardWidth);",
            )
            .replace_function_body(
                "void WINAPI ExperienceToggleButton_UpdateButtonPadding_Hook(void* pThis)",
                read_patch("tbiconsize_experience_toggle_button_padding_body.cpp"),
            )
            .replace_function_body(
                "void WINAPI SearchButtonBase_UpdateButtonPadding_Hook(void* pThis)",
                read_patch("tbiconsize_search_button_padding_body.cpp"),
            )
            .replace_regex(
                r"""
    for\s*\(\s*int\s+i\s*=\s*0\s*;\s*
    i\s*<\s*100\s*;\s*
    i\+\+\s*\)\s*
    \{\s*
    if\s*\(\s*!\s*g_pendingMeasureOverride\s*\)\s*
    \{\s*
    break\s*;\s*
    \}\s*
    Sleep\s*\(\s*100\s*\)\s*;\s*
    \}
    """,
                read_patch("tbiconsize_wait_for_measure_override.cpp"),
                flags=re.VERBOSE,
            )
            .replace_literal(
                "ApplySettingsTBIconSize(g_originalTaskbarHeight ? g_originalTaskbarHeight : 48);",
                "ApplySettingsTBIconSize(g_originalTaskbarHeight ? g_originalTaskbarHeight : "
                "kSystemMediumTaskbarButtonSize);",
                in_function="void Wh_ModBeforeUninitTBIconSize()",
            )
            .replace_regex(
                r"""
    while\s*\(\s*
    g_hookCallCounter\s*>\s*0
    \s*\)\s*
    \{\s*
    Sleep\s*\(\s*100\s*\)\s*;
    \s*\}
    """,
                read_patch("tbiconsize_wait_for_hook_drain.cpp"),
                in_function="void Wh_ModUninitTBIconSize()",
                flags=re.VERBOSE,
            )
            .insert_after_literal(
                "bool g_taskbarButtonWidthCustomized;",
                "\n" + read_patch("tbiconsize_layout_constants_and_helpers.cpp"),
            )
            .replace_regex(
                re.escape(
                    """    auto ret =
        ResourceDictionary_Lookup_TaskbarView_Original(pThis, result, key);
    if (!*ret) {
        return ret;
    }"""
                ),
                """    auto ret = ResourceDictionary_Lookup_TaskbarView_Original
        ? ResourceDictionary_Lookup_TaskbarView_Original(pThis, result, key)
        : nullptr;
    if (!ret || !*ret) {
        return ret;
    }""",
                in_function="""ResourceDictionary_Lookup_TaskbarView_Hook(
    void* pThis,
    void** result,
    winrt::Windows::Foundation::IInspectable* key)""",
            )
            .replace_regex(
                re.escape(
                    """    auto ret =
        ResourceDictionary_Lookup_SearchUxUi_Original(pThis, result, key);
    if (!*ret) {
        return ret;
    }"""
                ),
                """    auto ret = ResourceDictionary_Lookup_SearchUxUi_Original
        ? ResourceDictionary_Lookup_SearchUxUi_Original(pThis, result, key)
        : nullptr;
    if (!ret || !*ret) {
        return ret;
    }""",
                in_function="""ResourceDictionary_Lookup_SearchUxUi_Hook(
    void* pThis,
    void** result,
    winrt::Windows::Foundation::IInspectable* key)""",
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
                in_function="bool HookTaskbarDllSymbolsTBIconSize()",
            )
            .replace_regex(
                re.escape(
                    """    if (!HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }"""
                ),
                """if (!HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"HookSymbols failed");
        if (loadedTaskbarDllForHooking) {
            FreeLibrary(module);
        }
        return false;
    }""",
                in_function="bool HookTaskbarDllSymbolsTBIconSize()",
            )
            .replace_regex(
                r"if \(g_unloading\)",
                "if (g_unloading || !key || !value)",
                in_function="void OverrideResourceDirectoryLookup",
            )
            .insert_after_literal(
                "g_inSystemTrayController_UpdateFrameSize = false;",
                "\nApplySettingsFromTaskbarThreadGeometryChanged();",
                in_function="void WINAPI SystemTrayController_UpdateFrameSize_Hook(void* pThis)",
            )
            .replace_literal('Wh_GetIntSetting(L"TaskbarButtonWidth")', 'Wh_GetIntSetting(L"TaskbarButtonSize")')
            .replace_literal(
                "STDAPI GetDpiForMonitor",
                self._build_dpi_prefix(),
                count=1,
                label="insert shared taskbar helpers before GetDpiForMonitor",
            )
            .replace_literal(
                "ApplySettingsTBIconSize(g_settings_tbiconsize.taskbarHeight);",
                'Wh_Log(L"Deferring taskbar icon size settings until delayed initial apply");',
                in_function="void Wh_ModAfterInitTBIconSize()",
            )
            .insert_before_literal(
                "using SendMessageTimeoutW_t = decltype(&SendMessageTimeoutW);",
                "static bool TryCorrectShellHookMinRectMessageTai(UINT Msg, WPARAM wParam, LPARAM lParam);",
            )
            .insert_after_literal(
                """LRESULT ret = SendMessageTimeoutW_Original(hWnd, Msg, wParam, lParam,
                                               fuFlags, uTimeout, lpdwResult);""",
                "\nTryCorrectShellHookMinRectMessageTai(Msg, wParam, lParam);",
                in_function="""LRESULT WINAPI SendMessageTimeoutW_Hook(HWND hWnd,
                                        UINT Msg,
                                        WPARAM wParam,
                                        LPARAM lParam,
                                        UINT fuFlags,
                                        UINT uTimeout,
                                        PDWORD_PTR lpdwResult)""",
            )
            .insert_before_literal(
                "#include <winrt/Windows.Foundation.h>\n#include <winrt/Windows.UI.Xaml.Automation.h>",
                "#include <initguid.h>\n",
            )
            .insert_after_literal(
                "TaskbarController_OnGroupingModeChanged_InitOffsets();",
                "\n" + read_patch("tbiconsize_grouping_mode_hook_install.cpp"),
                in_function="bool HookTaskbarViewDllSymbols(HMODULE module, bool hookSystemTraySymbolsInline)",
            )
            .insert_after_literal(
                "g_inSystemTrayController_UpdateFrameSize = false;",
                "ApplySettingsFromTaskbarThreadGeometryChanged();",
                in_function="void WINAPI SystemTraySecondaryController_UpdateFrameSize_Hook(void* pThis)",
            )
            .insert_before_regex(
                re.escape(r"using TaskbarController_UpdateFrameHeight_t = void(WINAPI*)(void* pThis);"),
                read_patch("tbiconsize_grouping_mode_hook_declaration.cpp") + "\n",
            )
            .replace_literal(
                ' = Wh_GetIntSetting(L"TaskbarHeight");',
                ' = Wh_GetIntSetting(L"TaskbarHeight") + ((Wh_GetIntSetting(L"FlatTaskbarBottomCorners") || '
                'Wh_GetIntSetting(L"FullWidthTaskbarBackground"))?0:(abs(Wh_GetIntSetting(L"TaskbarOffsetY"))*2));',
                count=1,
                label="expand taskbar height setting",
            )
            .replace_literal(
                "return g_settings_tbiconsize.iconSize;",
                "return g_settings_tbiconsize.iconSize ;",
                count=1,
                label="force icon-size return patch",
            )
            .replace_function(
                "void LoadSettingsTBIconSize()",
                read_patch("tbiconsize_load_settings.cpp"),
                label="replace LoadSettingsTBIconSize",
            )
            .insert_before_literal(
                "LoadSettingsTBIconSize();",
                "\nconst int oldTaskbarButtonWidth = g_settings_tbiconsize.taskbarButtonWidth;\n"
                "    const int oldSmallTaskbarButtonWidth = g_settings_tbiconsize.taskbarButtonWidthSmall;\n",
                in_function="void Wh_ModSettingsChangedTBIconSize()",
            )
            .insert_after_literal(
                "LoadSettingsTBIconSize();",
                "\nif (!g_unloading &&\n"
                "        ((oldTaskbarButtonWidth > 0 && "
                "oldTaskbarButtonWidth != g_settings_tbiconsize.taskbarButtonWidth) ||\n"
                "         (oldSmallTaskbarButtonWidth > 0 && "
                "oldSmallTaskbarButtonWidth != g_settings_tbiconsize.taskbarButtonWidthSmall))) {\n"
                "        RequestTaskbarButtonSizeRelayout();\n"
                "    }\n",
                in_function="void Wh_ModSettingsChangedTBIconSize()",
            )
            .replace_regex("symbolHooks", "SystemTray_TaskbarViewDll_hooks",
                           in_function="bool HookSystemTraySymbols(HMODULE module)", count=0)
            .replace_regex("symbolHooks\\b", "TaskbarViewDll_hooks",
                           in_function="""bool HookTaskbarViewDllSymbols(HMODULE module, bool hookSystemTraySymbolsInline)""", count=0)
            .replace_regex("symbolHooksSystemTray", "SystemTray_TaskbarViewDll_hooks",
                           in_function="""bool HookTaskbarViewDllSymbols(HMODULE module,
                                         bool hookSystemTraySymbolsInline)""", count=0)
            .replace_regex("symbolHooks", "SearchUxUiDll_hooks",
                           in_function="""bool HookSearchUxUiDllSymbols(HMODULE module)""", count=0)
            .replace_regex("taskbarDllHooks", "taskbarDll_hooks",
                           in_function="""bool HookTaskbarDllSymbolsTBIconSize()""", count=0)
            .text()
        )

    def _build_dpi_prefix(self) -> str:
        return (
            f"{read_file(MOD_PARTS_DIR / 'g_settings.cpp')}\n"
            f"{read_file(MOD_PARTS_DIR / 'top-level-definitions.cpp')}\n\n"
            f"{read_patch('tbiconsize_dpi_helpers.cpp')}"
        )
