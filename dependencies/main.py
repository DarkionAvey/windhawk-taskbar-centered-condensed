from abc import ABC, abstractmethod
import os
import requests
import re

from art import *

mod_parts_dir = "mod-parts"
hooks_dir = os.path.join(mod_parts_dir, "hooks")


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
        content = re.sub(r"ApplySettings\(", r"ApplySettings" + self.name + "(", content, flags=re.DOTALL)
        content = re.sub(r"HookTaskbarDllSymbols\(\)", r"HookTaskbarDllSymbols" + self.name + "()", content, flags=re.DOTALL)
        content = re.sub(r"Wh_ModInit\(\)", r"Wh_ModInit" + self.name + "()", content, flags=re.DOTALL)
        content = re.sub(r"Wh_ModAfterInit\(\)", r"Wh_ModAfterInit" + self.name + "()", content, flags=re.DOTALL)
        content = re.sub(r"Wh_ModBeforeUninit\(\)", r"Wh_ModBeforeUninit" + self.name + "()", content, flags=re.DOTALL)
        content = re.sub(r"Wh_ModUninit\(\)", r"Wh_ModUninit" + self.name + "()", content, flags=re.DOTALL)
        content = re.sub(r"Wh_ModSettingsChanged\(\)", r"Wh_ModSettingsChanged" + self.name + "()", content, flags=re.DOTALL)
        content = re.sub(r"g_taskbarViewDllLoaded", r"g_taskbarViewDllLoaded" + self.name, content, flags=re.DOTALL)
        content = self.format_content(content)
        content = re.sub(r"^\s+//\s.*?$", "\n", content, flags=re.DOTALL | re.MULTILINE)
        content = re.sub("\n+", "\n", content, flags=re.DOTALL | re.MULTILINE)
        content = generate_slash_block(self.name) + content

        content = re.sub(r'[ \t]*\n', '\n', content)  # remove whitespace-only lines
        content = re.sub(r'\n+', '\n', content).strip()

        filename = self.url.split("/")[-1] or "output.mod"

        self.save(content, filename)


######################################################################

class TaskbarIconSizeMod(URLProcessor):
    def __init__(self):
        url = "https://raw.githubusercontent.com/ramensoftware/windhawk-mods/refs/heads/main/mods/taskbar-icon-size.wh.cpp"
        super().__init__(url, "TBIconSize", "a")

    def format_content(self, content):
        content = re.sub(r'Wh_GetIntSetting\(L\"IconSize\"\)', 'Wh_GetIntSetting(L"TaskbarIconSize")', content, flags=re.DOTALL)
        content = re.sub(r'Wh_GetIntSetting\(L\"TaskbarButtonWidth\"\)', 'Wh_GetIntSetting(L"TaskbarButtonSize")', content, flags=re.DOTALL)

        cpp_code = f"""
        {read_file(os.path.join(mod_parts_dir, "taskbar-states.cpp"))}
        {read_file(os.path.join(mod_parts_dir, "g_settings.cpp"))}
        {read_file(os.path.join(mod_parts_dir, "top-level-variables.cpp"))}

        std::wstring GetMonitorName(HMONITOR monitor) {{
            MONITORINFOEX monitorInfo = {{}};
            monitorInfo.cbSize = sizeof(MONITORINFOEX);
            if (monitor && GetMonitorInfo(monitor, &monitorInfo)) {{
                return std::wstring(monitorInfo.szDevice);
            }}
            return L"default";
        }}
        std::wstring GetMonitorName(HWND hwnd) {{
            HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
            return GetMonitorName(monitor);
        }}        
        STDAPI GetDpiForMonitor"""

        content = re.sub(
            r'STDAPI GetDpiForMonitor',
            lambda _: cpp_code,
            content,
            flags=re.DOTALL | re.MULTILINE
        )

        content = re.sub(r' = Wh_GetIntSetting\(L\"TaskbarHeight\"\);', ' = Wh_GetIntSetting(L"TaskbarHeight") + ((Wh_GetIntSetting(L"FlatTaskbarBottomCorners") || Wh_GetIntSetting(L"FullWidthTaskbarBackground"))?0:(abs(Wh_GetIntSetting(L"TaskbarOffsetY"))*2));', content, flags=re.DOTALL)
        content = re.sub(r'return g_settings_tbiconsize\.iconSize;', 'return g_settings_tbiconsize.iconSize ;', content, flags=re.DOTALL)

        content = re.sub(r"void LoadSettingsTBIconSize\(\) \{.*?}", r"""
void LoadSettingsTBIconSize() {
  g_settings_tbiconsize.iconSize = Wh_GetIntSetting(L"TaskbarIconSize");
  if (g_settings_tbiconsize.iconSize <= 0) g_settings_tbiconsize.iconSize = 44;
  g_settings_tbiconsize.iconSize=g_settings_tbiconsize.iconSize;
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
        content = re.sub(r"} g_settings_startbuttonposition;", ";bool MoveFlyoutNotificationCenter=true;} g_settings_startbuttonposition;", content, flags=re.DOTALL)

        content = re.sub(r"HRESULT WINAPI IUIElement_Arrange_Hook", "HRESULT WINAPI IUIElement_Arrange_Hook_" + self.name, content, flags=re.DOTALL)
        content = re.sub(r"std::wstring processFileName = GetProcessFileName\(processId\);",
                         "TCHAR className[256];GetClassName(hwnd, className, 256);std::wstring windowClassName(className);\nstd::wstring processFileName = GetProcessFileName(processId);\nWh_Log(L\"process: %s, windowClassName: %s\",processFileName.c_str(),windowClassName.c_str());", content,
                         flags=re.MULTILINE | re.DOTALL)
        content = re.sub(r"SearchHost,", "SearchHost,ShellExperienceHost,", content, flags=re.MULTILINE | re.DOTALL)
        content = re.sub(r"target = Target::SearchHost;\s+}", """target = Target::SearchHost;
    }else if (_wcsicmp(processFileName.c_str(), L"ShellExperienceHost.exe") == 0) {
        target = Target::ShellExperienceHost;
    } """, content, flags=re.MULTILINE | re.DOTALL)

        content = re.sub(r"GetMonitorInfo\(monitor, &monitorInfo\);", """GetMonitorInfo(monitor, &monitorInfo);
    auto monitorName = GetMonitorName(monitor);
    auto iterationTbStates = g_taskbarStates.find(monitorName);
    if (iterationTbStates == g_taskbarStates.end()) {
      return original();
    }
    TaskbarState& taskbarState = iterationTbStates->second;
""", content, flags=re.MULTILINE | re.DOTALL)

        content = re.sub(r"if \(target == Target::StartMenu\).*?\}\s+SetWindowPos", """
    float dpiScale = monitorDpiX / 96.0f;
    float absStartX = taskbarState.lastStartButtonXCalculated * dpiScale;
    float absRootWidth = taskbarState.lastRootWidth * dpiScale;
    float absTargetWidth = taskbarState.lastTargetWidth * dpiScale;
    
     Wh_Log(L"original: taskbarState.lastLeftMostEdgeTray: %f, lastStartButtonXCalculated: %f g_lastRootWidth %f cx: %d, x:%d;cy: %d; y: %d; target:%d g_lastTargetWidth: %f, absStartX: %f; absRootWidth: %f; absTargetWidth: %f",
           taskbarState.lastLeftMostEdgeTray,
          taskbarState.lastStartButtonXCalculated,
          taskbarState.lastRootWidth,
          cx,
          x,
          cy, 
          y,
          target,
          taskbarState.lastTargetWidth,
          absStartX,
          absRootWidth,
          absTargetWidth);
      
    if (target == Target::StartMenu) {
    g_lastRecordedStartMenuWidth = static_cast<int>(Wh_GetIntValue(L"lastRecordedStartMenuWidth", g_lastRecordedStartMenuWidth) * dpiScale);
      if (g_settings_startbuttonposition.startMenuOnTheLeft && !g_unloading) {
        g_startMenuWnd = hwnd;
        g_startMenuOriginalWidth = cx;
        x = static_cast<int>(absRootWidth / 2.0f - absStartX - absTargetWidth+ (g_settings.userDefinedAlignFlyoutInner?g_lastRecordedStartMenuWidth/2.0f : 0.0f));
        x = std::min(0, std::max(static_cast<int>(((-absRootWidth + g_lastRecordedStartMenuWidth) / 2.0f) + (12 * dpiScale)), x));
      } else {
        if (g_startMenuOriginalWidth) {
          cx = g_startMenuOriginalWidth;
        }
        g_startMenuWnd = nullptr;
        g_startMenuOriginalWidth = 0;
        x = 0;  
      }
    
    } else if (target == Target::SearchHost) {
      if (g_settings_startbuttonposition.startMenuOnTheLeft && !g_unloading) {
        g_searchMenuWnd = hwnd;
        g_searchMenuOriginalX = x;
        x = static_cast<int>(absStartX - (g_settings.userDefinedAlignFlyoutInner? ( 12 * dpiScale) :( cx / 2.0f)));
        x = std::max(0, std::min(x, static_cast<int>(absRootWidth - cx)));
      } else {
       x = g_unloading && IsStartMenuOrbLeftAligned() ? g_searchMenuOriginalX : (absRootWidth-cx)/2;
       g_searchMenuWnd = nullptr;
       g_searchMenuOriginalX = 0;
      }

    } else if (target == Target::ShellExperienceHost) {
        int lastRecordedTrayRightMostEdgeForMonitor = taskbarState.lastRightMostEdgeTray;
        if (y != 0) {
          return original();
        }
        if (g_settings_startbuttonposition.MoveFlyoutNotificationCenter && !g_unloading) {
          x = static_cast<int>(lastRecordedTrayRightMostEdgeForMonitor * dpiScale - (g_settings.userDefinedAlignFlyoutInner ? (cx - (12 * dpiScale)) : (cx / 2.0f)));
          x = std::max(0, std::min(x, static_cast<int>(absRootWidth - cx)));
        } else {
          x = static_cast<int>(absRootWidth - cx);
        }
    }
    
     Wh_Log(L"Recalc: taskbarState.lastLeftMostEdgeTray: %f, lastStartButtonXCalculated: %f g_lastRootWidth %f cx: %d, x:%d;cy: %d; y: %d; target:%d g_lastTargetWidth: %f, absStartX: %f; absRootWidth: %f; absTargetWidth: %f",
               taskbarState.lastLeftMostEdgeTray,
              taskbarState.lastStartButtonXCalculated,
              taskbarState.lastRootWidth,
              cx,
              x,
              cy,
              y,
              target,
              taskbarState.lastTargetWidth,
              absStartX,
              absRootWidth,
              absTargetWidth);
SetWindowPos""", content, flags=re.MULTILINE | re.DOTALL)

        content = re.sub(r"FrameworkElement EnumChildElements\(.*?(?:^}$)", "", content, flags=re.MULTILINE | re.DOTALL)
        content = re.sub(r"FrameworkElement FindChildByName\(.*?(?:^}$)", "", content, flags=re.MULTILINE | re.DOTALL)
        content = re.sub(r"FrameworkElement FindChildByClassName\(.*?(?:^}$)", "", content, flags=re.MULTILINE | re.DOTALL)
        content = re.sub(r"HWND FindCurrentProcessTaskbarWnd\(.*?(?:^}$)", "", content, flags=re.MULTILINE | re.DOTALL)
        content = re.sub(r"AugmentedEntryPointButton_UpdateButtonPadding_t[\s\w\d]*?AugmentedEntryPointButton_UpdateButtonPadding_Original;", "", content, flags=re.MULTILINE | re.DOTALL)
        content = re.sub(r"AugmentedEntryPointButton_UpdateButtonPadding_Hook", "AugmentedEntryPointButton_UpdateButtonPadding_Hook_" + self.name, content, flags=re.MULTILINE | re.DOTALL)
        content = re.sub(r"HookTaskbarViewDllSymbols", "HookTaskbarViewDllSymbols" + self.name, content, flags=re.MULTILINE | re.DOTALL)
        content = re.sub(r"HMODULE GetTaskbarViewModuleHandle\(.*?(?:^}$)", "", content, flags=re.MULTILINE | re.DOTALL)
        content = re.sub(r"LoadLibraryExW_t LoadLibraryExW_Original;", "", content, flags=re.MULTILINE | re.DOTALL)
        content = re.sub(r"LoadLibraryExW_Hook", "LoadLibraryExW_Hook_" + self.name, content, flags=re.MULTILINE | re.DOTALL)
        content = re.sub(r"bool ApplyStyle\(.*?(?:^}$)", "", content, flags=re.MULTILINE | re.DOTALL)
        content = re.sub(r"element\.Dispatcher\(\)\.TryRunAsync\(\s+winrt::Windows::UI::Core::CoreDispatcherPriority::High,\s+\[element\]\(\) {", "element.Dispatcher().TryRunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::High,[element]() { \\n", content,
                         flags=re.MULTILINE | re.DOTALL)
        content = re.sub(r"if \(!xamlRoot\) \{", "if (!xamlRoot) {g_already_requested_debounce_initializing=false;\n", content, flags=re.DOTALL | re.MULTILINE)

        content = re.sub(r"margin\.Right = 0;", "", content, flags=re.DOTALL | re.MULTILINE)
        content = re.sub(r"margin\.Right = -width;", "", content, flags=re.DOTALL)
        content = re.sub(r"return IUIElement_Arrange_Original\(pThis, &newRect\);", "return original();", content, flags=re.DOTALL)
        content = re.sub(r"if \(!ApplyStyle\(xamlRoot\)\) \{.*?}",
                         r"""
const auto xamlRootContent = xamlRoot.Content().try_as<FrameworkElement>();
if (!xamlRootContent) {
g_already_requested_debounce_initializing=false;
return TRUE;
}
if (!debounceTimer) {
     xamlRootContent.Dispatcher().TryRunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::High, [xamlRootContent]() {
     InitializeDebounce(); 
  });
  return TRUE;
}
if (xamlRootContent && xamlRootContent.Dispatcher()) {
std::wstring monitorName = GetMonitorName(hWnd);
  xamlRootContent.Dispatcher().TryRunAsync(winrt::Windows::UI::Core::CoreDispatcherPriority::High, [xamlRootContent,monitorName]() {
    if (!ApplyStyle(xamlRootContent,monitorName)) {
      Wh_Log(L"ApplyStyles failed");
    }
  });
  return TRUE;
}
""",
                         content, flags=re.DOTALL | re.MULTILINE)
        content = re.sub(r"void Wh_ModUninitStartButtonPosition\(\) {", "void Wh_ModUninitStartButtonPosition() {if(true)return;", content, flags=re.MULTILINE | re.DOTALL)

        # hooks
        content = re.sub(r"WindhawkUtils::SYMBOL_HOOK taskbarDllHooks\[\] = \{", fr"WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {{{read_file(os.path.join(hooks_dir, 'taskbar.dll_sigs.cpp'))}", content, flags=re.MULTILINE | re.DOTALL)
        content = re.sub(r"WindhawkUtils::SYMBOL_HOOK symbolHooks\[\] = \{", fr"WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {{{read_file(os.path.join(hooks_dir, 'Taskbar.View.dll_sigs.cpp'))}", content, flags=re.MULTILINE | re.DOTALL)

        content = re.sub(r"bool HookTaskbarDllSymbolsStartButtonPosition\(\) \{", fr"""{read_file(os.path.join(hooks_dir, "taskbar.dll_methods.cpp"))}
bool HookTaskbarDllSymbolsStartButtonPosition() {{""", content, flags=re.MULTILINE | re.DOTALL)

        content = re.sub(r"bool HookTaskbarViewDllSymbolsStartButtonPosition\(HMODULE module\) \{", fr"""{read_file(os.path.join(hooks_dir, "Taskbar.View.dll_methods.cpp"))}
bool HookTaskbarViewDllSymbolsStartButtonPosition(HMODULE module) {{""", content, flags=re.MULTILINE | re.DOTALL)

        content = re.sub(r"Wh_GetIntSetting\(L\"startMenuOnTheLeft\"\);", """Wh_GetIntSetting(L\"MoveFlyoutStartMenu\");
g_settings_startbuttonposition.MoveFlyoutNotificationCenter = Wh_GetIntSetting(L"MoveFlyoutNotificationCenter");
""", content, flags=re.MULTILINE | re.DOTALL)
        content = re.sub(r"Wh_GetIntSetting\(L\"startMenuWidth\"\);", "660;", content, flags=re.MULTILINE | re.DOTALL)

        content = "bool ApplyStyle(FrameworkElement const& element, std::wstring monitorName);\nbool InitializeDebounce();\nDispatcherTimer debounceTimer{nullptr};\n\n" + content
        return content


######################################################################

class TaskbarStylerMod(URLProcessor):
    def __init__(self):
        url = "https://raw.githubusercontent.com/ramensoftware/windhawk-mods/refs/heads/main/mods/windows-11-taskbar-styler.wh.cpp"
        super().__init__(url, "BlurBrush", "c")

    def format_content(self, content):
        content = content.split("#include <initguid.h>")[1]
        content = "#include <initguid.h>\n" + content
        content = content.split("void SetOrClearValue")[0]
        content = content.strip()
        if not (content.startswith("#include") and content.endswith("////////////////////////////////////////////////////////////////////////////////")):
            raise ValueError("Content must start w/ include and end with '/+'")
        return content


######################################################################

def generate_mod_art():
    print(generate_slash_block("WinDock"))


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
    mod_parts_dir = r"C:\Users\bbwi\Documents\GitHub\windhawk-taskbar-centered-condensed\mod-parts"
    hooks_dir = os.path.join(mod_parts_dir, "hooks")
    process_all_mods()
