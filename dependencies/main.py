from abc import ABC, abstractmethod
import os
import requests
import re

from art import *


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
    def __init__(self, url, name):
        self.url = url
        self.name = name
        self.output_folder = "modified-dependencies"
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
        filepath = os.path.join(self.output_folder, filename)
        with open(filepath, "w", encoding="utf-8") as file:
            file.write(content)
        print(f"Saved: {filepath}")

    def process(self):
        content = self.download()
        content = content.strip()
        content = re.sub(r"//\s+==WindhawkMod==.*?WindhawkModSettings==\s+(?=#include)", "", content, flags=re.DOTALL)

        content = self.format_content(content)

        content = re.sub("g_settings", "g_settings_" + self.name.lower(), content, flags=re.DOTALL)
        content = re.sub(r'Wh_Log\(L\">\"\);', r'', content, flags=re.DOTALL)
        content = re.sub(r"LoadSettings\(\)", r"LoadSettings" + self.name + "()", content, flags=re.DOTALL)
        content = re.sub(r"ApplySettings\(", r"ApplySettings" + self.name + "(", content, flags=re.DOTALL)
        content = re.sub(r"HookTaskbarDllSymbols\(\)", r"HookTaskbarDllSymbols" + self.name + "()", content, flags=re.DOTALL)
        content = re.sub(r"Wh_ModInit\(\)", r"Wh_ModInit" + self.name + "()", content, flags=re.DOTALL)
        content = re.sub(r"Wh_ModAfterInit\(\)", r"Wh_ModAfterInit" + self.name + "()", content, flags=re.DOTALL)
        content = re.sub(r"Wh_ModBeforeUninit\(\)", r"Wh_ModBeforeUninit" + self.name + "()", content, flags=re.DOTALL)
        content = re.sub(r"Wh_ModUninit\(\)", r"Wh_ModUninit" + self.name + "()", content, flags=re.DOTALL)
        content = re.sub(r"Wh_ModSettingsChanged\(\)", r"Wh_ModSettingsChanged" + self.name + "()", content, flags=re.DOTALL)
        content = re.sub(r"^\s+//\s.*?$", "\n", content, flags=re.DOTALL|re.MULTILINE)
        content = re.sub("\n+", "\n", content, flags=re.DOTALL)
        content = generate_slash_block(self.name) + content

        filename = self.url.split("/")[-1] or "output.mod"
        self.save(content, filename)


######################################################################


class TaskbarIconSizeMod(URLProcessor):
    def __init__(self):
        url = "https://raw.githubusercontent.com/ramensoftware/windhawk-mods/refs/heads/main/mods/taskbar-icon-size.wh.cpp"
        super().__init__(url, "TBIconSize")

    def format_content(self, content):
        content = re.sub(r'Wh_GetIntSetting\(L\"IconSize\"\)', 'Wh_GetIntSetting(L"TaskbarIconSize")', content, flags=re.DOTALL)
        content = re.sub(r'Wh_GetIntSetting\(L\"TaskbarButtonWidth\"\)', 'Wh_GetIntSetting(L"TaskbarButtonSize")', content, flags=re.DOTALL)
        content = re.sub(r' = Wh_GetIntSetting\(L\"TaskbarHeight\"\);', ' = Wh_GetIntSetting(L"TaskbarHeight") + (abs(Wh_GetIntSetting(L"TaskbarOffsetY"))*2);', content, flags=re.DOTALL)
        return content


######################################################################


class StartButtonPosition(URLProcessor):
    def __init__(self):
        url = "https://raw.githubusercontent.com/ramensoftware/windhawk-mods/refs/heads/main/mods/taskbar-start-button-position.wh.cpp"
        super().__init__(url, "StartButtonPosition")

    def format_content(self, content):
        content = re.sub(r"std::atomic<bool> g_taskbarViewDllLoaded;", "", content, flags=re.DOTALL)
        content = re.sub(r"std::atomic<bool> g_unloading;", "", content, flags=re.DOTALL)
        content = re.sub(r"typedef enum MONITOR_DPI_TYPE {.*?} MONITOR_DPI_TYPE;", "", content, flags=re.DOTALL)

        content = re.sub(r"HRESULT WINAPI IUIElement_Arrange_Hook", "HRESULT WINAPI IUIElement_Arrange_Hook_"+self.name, content, flags= re.DOTALL)

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
        content = re.sub(r"element\.Dispatcher\(\)\.TryRunAsync\(\s+winrt::Windows::UI::Core::CoreDispatcherPriority::High,\s+\[element\]\(\) {", "element.Dispatcher().TryRunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::High,[element]() {\\n ApplySettingsFromTaskbarThread(); \\n", content, flags=re.MULTILINE | re.DOTALL)
        content = re.sub(r"Wh_GetIntSetting\(L\"startMenuOnTheLeft\"\);", "0;", content, flags=re.MULTILINE | re.DOTALL)
        content = re.sub(r"Wh_GetIntSetting\(L\"startMenuWidth\"\);", "0;", content, flags=re.MULTILINE | re.DOTALL)

        content = re.sub(r"margin\.Right = 0;", "", content, flags=re.DOTALL|re.MULTILINE )
        content = re.sub(r"margin\.Right = -width;", "", content, flags=re.DOTALL)
        content = re.sub(r"return IUIElement_Arrange_Original\(pThis, &newRect\);", "return original();", content, flags=re.DOTALL)

        content = "bool ApplyStyle(XamlRoot xamlRoot);\n" + content
        return content


def generate_mod_art():
    print(generate_slash_block("Dock-like"))

def process_all_mods():
    generate_mod_art()
    processors = [
        TaskbarIconSizeMod(),
        StartButtonPosition()
    ]

    for processor in processors:
        processor.process()

if __name__ == "__main__":
    process_all_mods()
