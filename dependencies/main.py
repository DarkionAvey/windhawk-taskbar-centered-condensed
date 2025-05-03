from abc import ABC, abstractmethod
import os
import requests
import re

from art import *

hooks_dir = "mod-parts/hooks/"


def read_file(path):
    with open(path, 'r', encoding='utf-8') as f:
        contents = f.read()
    return contents


def generate_slash_block(name):
    name = text2art(name, font="starwars")
    padding_length = max(len(line) for line in name.splitlines()) + 12
    name = '\n'.join(['////  ' + line + '  ////' for line in name.splitlines()])

    line_of_slashes = "/" * padding_length
    total_padding = padding_length - len(name) - 2
    left_padding = total_padding // 2
    right_padding = total_padding - left_padding
    content = f"""
{line_of_slashes}
{line_of_slashes}
{line_of_slashes}
{'/' * left_padding}{name}{'/' * right_padding}
{line_of_slashes}
{line_of_slashes}
{line_of_slashes}
"""
    return content


######################################################################


class URLProcessor(ABC):
    def __init__(self, url, name, injection_order):
        self.url = url
        self.name = name
        self.injection_order = injection_order

        if os.path.exists("main.py"):
            self.output_folder = "modified-dependencies"
        else:
            self.output_folder = os.path.join("dependencies", "modified-dependencies")

        os.makedirs(self.output_folder, exist_ok=True)

    @abstractmethod
    def format_content(self, content):
        pass

    def download(self):
        response = requests.get(self.url)
        if response.status_code == 200:
            return response.text
        else:
            raise Exception(f"Failed to download {self.url}")

    def save(self, content, filename):
        filename = self.injection_order + "_" + filename
        filepath = os.path.join(self.output_folder, filename)
        with open(filepath, "w", encoding="utf-8") as file:
            file.write(content)
        print(f"Saved: {filepath}")

    def process(self):
        content = self.download()
        content = content.strip()
        content = re.sub(r"//\s+==WindhawkMod==.*?WindhawkModSettings==\s+(?=#include)", "", content, flags=re.DOTALL)

        content = re.sub("g_settings", "g_settings_" + self.name.lower(), content, flags=re.DOTALL)
        content = re.sub(r'Wh_Log\(L\">\"\);', r'', content, flags=re.DOTALL)
        content = re.sub(r"LoadSettings\(\)", r"LoadSettings" + self.name + "()", content, flags=re.DOTALL)
        content = re.sub(r"LoadLibrary\(L\"dwmapi.dll\"\);", "nullptr;", content, flags=re.MULTILINE | re.DOTALL)
        content = re.sub(r"ApplySettings\(", r"ApplySettings" + self.name + "(", content, flags=re.DOTALL)
        content = re.sub(r"HookTaskbarDllSymbols\(\)", r"HookTaskbarDllSymbols" + self.name + "()", content, flags=re.DOTALL)
        content = re.sub(r"Wh_ModInit\(\)", r"Wh_ModInit" + self.name + "()", content, flags=re.DOTALL)
        content = re.sub(r"Wh_ModAfterInit\(\)", r"Wh_ModAfterInit" + self.name + "()", content, flags=re.DOTALL)
        content = re.sub(r"Wh_ModBeforeUninit\(\)", r"Wh_ModBeforeUninit" + self.name + "()", content, flags=re.DOTALL)
        content = re.sub(r"Wh_ModUninit\(\)", r"Wh_ModUninit" + self.name + "()", content, flags=re.DOTALL)
        content = re.sub(r"Wh_ModSettingsChanged\(\)", r"Wh_ModSettingsChanged" + self.name + "()", content, flags=re.DOTALL)
        content = re.sub(r"g_taskbarViewDllLoaded", r"g_taskbarViewDllLoaded" + self.name, content, flags=re.DOTALL)
        content = re.sub(r"^\s+//\s.*?$", "\n", content, flags=re.DOTALL | re.MULTILINE)
        content = re.sub("\n+", "\n", content, flags=re.DOTALL)
        content = self.format_content(content)
        content = generate_slash_block(self.name) + content

        filename = self.url.split("/")[-1] or "output.mod"
        self.save(content, filename)


######################################################################

class TaskbarIconSizeMod(URLProcessor):
    def __init__(self):
        url = "https://raw.githubusercontent.com/ramensoftware/windhawk-mods/refs/heads/main/mods/taskbar-icon-size.wh.cpp"
        super().__init__(url, "TBIconSize", "a")

    def format_content(self, content):
        content = "void ApplySettingsDebounced(int delayMs);\nvoid ApplySettingsDebounced();\nvoid ApplySettingsFromTaskbarThreadIfRequired();\n" + content
        content = re.sub(r'Wh_GetIntSetting\(L\"IconSize\"\)', 'Wh_GetIntSetting(L"TaskbarIconSize")', content, flags=re.DOTALL)
        content = re.sub(r'Wh_GetIntSetting\(L\"TaskbarButtonWidth\"\)', 'Wh_GetIntSetting(L"TaskbarButtonSize")', content, flags=re.DOTALL)
        content = re.sub(r' = Wh_GetIntSetting\(L\"TaskbarHeight\"\);', ' = Wh_GetIntSetting(L"TaskbarHeight") + ((Wh_GetIntSetting(L"FlatTaskbarBottomCorners") || Wh_GetIntSetting(L"FullWidthTaskbarBackground"))?0:(abs(Wh_GetIntSetting(L"TaskbarOffsetY"))*2));', content, flags=re.DOTALL)
        content = re.sub(r"void LoadSettingsTBIconSize\(\) \{.*?}", r"""
void LoadSettingsTBIconSize() {
  g_settings_tbiconsize.iconSize = Wh_GetIntSetting(L"TaskbarIconSize");
  if (g_settings_tbiconsize.iconSize <= 0) g_settings_tbiconsize.iconSize = 44;
  g_settings_tbiconsize.taskbarHeight = Wh_GetIntSetting(L"TaskbarHeight");

  g_settings_tbiconsize.taskbarHeight = Wh_GetIntSetting(L"TaskbarHeight");
  if (g_settings_tbiconsize.taskbarHeight <= 0) g_settings_tbiconsize.taskbarHeight = 78;
  g_settings_tbiconsize.taskbarHeight = abs(g_settings_tbiconsize.taskbarHeight);
  if (g_settings_tbiconsize.taskbarHeight > 200) g_settings_tbiconsize.taskbarHeight = 200;
  if (g_settings_tbiconsize.taskbarHeight < 44) g_settings_tbiconsize.taskbarHeight = 44;
  int TaskbarOffsetY = abs(Wh_GetIntSetting(L"TaskbarOffsetY"));
  if (TaskbarOffsetY < 0) TaskbarOffsetY = 6;
  int heightExpansion = ((Wh_GetIntSetting(L"FlatTaskbarBottomCorners") || Wh_GetIntSetting(L"FullWidthTaskbarBackground")) ? 0 : (abs(TaskbarOffsetY) * 2));
  g_settings_tbiconsize.taskbarHeight = g_settings_tbiconsize.taskbarHeight + heightExpansion;
  int value = Wh_GetIntSetting(L"TaskbarButtonSize");
  if (value <= 0) value = 74;
  g_settings_tbiconsize.taskbarButtonWidth = value;
}
    """, content, flags=re.DOTALL)

        return content


######################################################################


class StartButtonPosition(URLProcessor):
    def __init__(self):
        url = "https://raw.githubusercontent.com/ramensoftware/windhawk-mods/refs/heads/main/mods/taskbar-start-button-position.wh.cpp"
        super().__init__(url, "StartButtonPosition", "b")

    def format_content(self, content):
        content = re.sub(r"std::atomic<bool> g_taskbarViewDllLoaded;", "", content, flags=re.DOTALL)
        content = re.sub(r"std::atomic<bool> g_unloading;", "", content, flags=re.DOTALL)
        content = re.sub(r"typedef enum MONITOR_DPI_TYPE {.*?} MONITOR_DPI_TYPE;", "", content, flags=re.DOTALL)

        content = re.sub(r"HRESULT WINAPI IUIElement_Arrange_Hook", "HRESULT WINAPI IUIElement_Arrange_Hook_" + self.name, content, flags=re.DOTALL)
        content = re.sub(r"LoadSettingsStartButtonPosition\(\);", "", content, flags=re.MULTILINE | re.DOTALL)
        content = re.sub(r"LoadLibrary\(L\"dwmapi.dll\"\);", "nullptr;", content, flags=re.MULTILINE | re.DOTALL)

        content = re.sub(r"HWND GetTaskbarWnd\(\).*?(?:^}$)", "", content, flags=re.MULTILINE | re.DOTALL)
        content = re.sub(r"FrameworkElement EnumChildElements\(.*?(?:^}$)", "", content, flags=re.MULTILINE | re.DOTALL)
        content = re.sub(r"FrameworkElement FindChildByName\(.*?(?:^}$)", "", content, flags=re.MULTILINE | re.DOTALL)
        content = re.sub(r"FrameworkElement FindChildByClassName\(.*?(?:^}$)", "", content, flags=re.MULTILINE | re.DOTALL)
        content = re.sub(r"AugmentedEntryPointButton_UpdateButtonPadding_t[\s\w\d]*?AugmentedEntryPointButton_UpdateButtonPadding_Original;", "", content, flags=re.MULTILINE | re.DOTALL)
        content = re.sub(r"AugmentedEntryPointButton_UpdateButtonPadding_Hook", "AugmentedEntryPointButton_UpdateButtonPadding_Hook_" + self.name, content, flags=re.MULTILINE | re.DOTALL)
        content = re.sub(r"HookTaskbarViewDllSymbols", "HookTaskbarViewDllSymbols" + self.name, content, flags=re.MULTILINE | re.DOTALL)
        content = re.sub(r"HMODULE GetTaskbarViewModuleHandle\(.*?(?:^}$)", "", content, flags=re.MULTILINE | re.DOTALL)
        content = re.sub(r"LoadLibraryExW_t LoadLibraryExW_Original;", "", content, flags=re.MULTILINE | re.DOTALL)
        content = re.sub(r"LoadLibraryExW_Hook", "LoadLibraryExW_Hook_" + self.name, content, flags=re.MULTILINE | re.DOTALL)
        content = re.sub(r"bool ApplyStyle\(.*?(?:^}$)", "", content, flags=re.MULTILINE | re.DOTALL)
        content = re.sub(r"element\.Dispatcher\(\)\.TryRunAsync\(\s+winrt::Windows::UI::Core::CoreDispatcherPriority::High,\s+\[element\]\(\) {", "element.Dispatcher().TryRunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::High,[element]() { \\n", content,
                         flags=re.MULTILINE | re.DOTALL)
        content = re.sub(r"Wh_GetIntSetting\(L\"startMenuOnTheLeft\"\);", "0;", content, flags=re.MULTILINE | re.DOTALL)
        content = re.sub(r"Wh_GetIntSetting\(L\"startMenuWidth\"\);", "0;", content, flags=re.MULTILINE | re.DOTALL)
        content = re.sub(r"HWND hTaskbarWnd = GetTaskbarWnd.*?}", "", content, flags=re.MULTILINE | re.DOTALL)

        content = re.sub(r"margin\.Right = 0;", "", content, flags=re.DOTALL | re.MULTILINE)
        content = re.sub(r"margin\.Right = -width;", "", content, flags=re.DOTALL)
        content = re.sub(r"return IUIElement_Arrange_Original\(pThis, &newRect\);", "return original();", content, flags=re.DOTALL)
        content = re.sub(r"if \(!ApplyStyle\(xamlRoot\)\) \{.*?}",
                         r"""
if (!debounceTimer) {
  RunFromWindowThread(hWnd, [](void* pParam) { InitializeDebounce(); }, 0);
  return TRUE;
}
auto xamlRootContent = xamlRoot.Content().try_as<FrameworkElement>();
if (!xamlRootContent || !debounceTimer) return TRUE;

if (xamlRootContent && xamlRootContent.Dispatcher()) {
  xamlRootContent.Dispatcher().TryRunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::High, [xamlRootContent]() {
    if (!ApplyStyle(xamlRootContent)) {
      Wh_Log(L"ApplyStyles failed");
    }
  });
  return TRUE;
}
""",
                         content, flags=re.DOTALL | re.MULTILINE)
        content = re.sub(r"void Wh_ModUninitStartButtonPosition\(\) {", "void Wh_ModUninitStartButtonPosition() {if(true)return;", content, flags=re.MULTILINE | re.DOTALL)

        # hooks
        content = re.sub(r"WindhawkUtils::SYMBOL_HOOK taskbarDllHooks\[\] = \{", fr"WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {{{read_file(os.path.join(hooks_dir, "taskbar.dll_sigs.cpp"))}", content, flags=re.MULTILINE | re.DOTALL)
        content = re.sub(r"WindhawkUtils::SYMBOL_HOOK symbolHooks\[\] = \{", fr"WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {{{read_file(os.path.join(hooks_dir, "Taskbar.View.dll_sigs.cpp"))}", content, flags=re.MULTILINE | re.DOTALL)

        content = re.sub(r"bool HookTaskbarDllSymbolsStartButtonPosition\(\) \{", fr"""{read_file(os.path.join(hooks_dir, "taskbar.dll_methods.cpp"))}
bool HookTaskbarDllSymbolsStartButtonPosition() {{""", content, flags=re.MULTILINE | re.DOTALL)

        content = re.sub(r"bool HookTaskbarViewDllSymbolsStartButtonPosition\(HMODULE module\) \{", fr"""{read_file(os.path.join(hooks_dir, "Taskbar.View.dll_methods.cpp"))}
bool HookTaskbarViewDllSymbolsStartButtonPosition(HMODULE module) {{""", content, flags=re.MULTILINE | re.DOTALL)

        content = "bool ApplyStyle(FrameworkElement element);\nbool InitializeDebounce();\nDispatcherTimer debounceTimer{nullptr};\n" + content
        return content


######################################################################

class TaskbarStylerMod(URLProcessor):
    def __init__(self):
        url = "https://raw.githubusercontent.com/ramensoftware/windhawk-mods/refs/heads/main/mods/windows-11-taskbar-styler.wh.cpp"
        super().__init__(url, "TaskbarStyler", "c")

    def format_content(self, content):
        content = re.sub(r"^const\sTheme\sg_.*?\}\}\;", "", content, flags=re.MULTILINE | re.DOTALL)
        content = re.sub(r"^void ProcessAllStylesFromSettings\(\) {.*?(^}$)", "void ProcessAllStylesFromSettings() {}", content, flags=re.MULTILINE | re.DOTALL)
        content = re.sub(r"RunFromWindowThread\(", "RunFromWindowThread" + self.name + "(", content, flags=re.MULTILINE | re.DOTALL)
        content = re.sub(r"// clang-format off", "", content, flags=re.DOTALL)
        content = re.sub(r"// clang-format on", "", content, flags=re.DOTALL)

        return content


######################################################################

def generate_mod_art():
    print(generate_slash_block("Dock-like"))


def process_all_mods():
    generate_mod_art()
    processors = [
        TaskbarIconSizeMod(),
        StartButtonPosition(),
        # TaskbarStylerMod()
    ]

    for processor in processors:
        processor.process()


if __name__ == "__main__":
    hooks_dir = "../mod-parts/hooks/"
    process_all_mods()
