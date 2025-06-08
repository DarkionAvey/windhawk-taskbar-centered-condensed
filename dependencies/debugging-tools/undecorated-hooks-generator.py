import random
import re

# This file generates a Windhawk mod to monitor method calls.
# Simply paste the methods of interest inside the input_block block,
# and it will generate the mod. Useful for tracking method execution
# and identifying potential hooks.
# input_block = r"""
#  	 [00149450] public: virtual int __cdecl CTaskListThumbnailWnd::GetMargins(struct tagRECT *)const
# 	 	 [001497F0] public: virtual int __cdecl CTaskListThumbnailWnd::GetThumbBarRectFromIndex(int,struct tagRECT *)const
# 	 	 [00149930] public: virtual int __cdecl CTaskListThumbnailWnd::GetWindowLocation(struct tagRECT *)const
# 		 	 [0003F848] private: float __cdecl CTaskListThumbnailWnd::_CalcThumbnailScale(class CDPA<struct ITaskThumbnail,class CTContainer_PolicyUnOwned<struct ITaskThumbnail> > const *,struct tagRECT const *,int,int,unsigned char,int *,int *,struct tagSIZE *)const
# 	 	 [000403A0] public: virtual int __cdecl CTaskListThumbnailWnd::TryGetThumbShareRegionRectFromIndex(int,struct tagRECT *)const
#
# """
dll_to_hook = "Taskbar.View.dll"
exe_to_hook = "explorer.exe"

input_block = r"""
	Line   7363: [00046974] public: void __cdecl TaskbarTelemetry::ViewCoordinator_ShouldTaskbarBeExpanded::StartActivity(unsigned __int64)
	Line   7367: [00046D70] protected: virtual void __cdecl TaskbarTelemetry::ViewCoordinator_ShouldTaskbarBeExpanded::StopActivity(void)
	Line   7368: [0004705C] public: __cdecl TaskbarTelemetry::ViewCoordinator_ShouldTaskbarBeExpanded::~ViewCoordinator_ShouldTaskbarBeExpanded(void)
	Line  15135: [001A29CC] public: void __cdecl TaskbarTelemetry::ViewCoordinator_ShouldTaskbarBeExpanded::ShouldTaskbarBeExpanded_HasOpenPopups<unsigned __int64 &>(unsigned __int64 &)
	Line  16616: [001D75A4] public: void __cdecl TaskbarTelemetry::ViewCoordinator_ShouldTaskbarBeExpanded::ShouldTaskbarBeExpanded_TaskbarExpandable<bool,unsigned __int64 &>(bool &&,unsigned __int64 &)
	Line  21454: [00262D5C] static  wil::details::lambda_call<`winrt::Taskbar::implementation::ViewCoordinator::ShouldTaskbarBeExpanded'::`2'::<lambda_1> >::~lambda_call<`winrt::Taskbar::implementation::ViewCoordinator::ShouldTaskbarBeExpanded'::`2'::<lambda_1> >()
	Line  28279: [00303234] public: void __cdecl TaskbarTelemetry::ViewCoordinator_ShouldTaskbarBeExpanded::IsEdgeGestureActive<bool,unsigned __int64 &>(bool &&,unsigned __int64 &)
	Line  28280: [003032AC] public: void __cdecl TaskbarTelemetry::ViewCoordinator_ShouldTaskbarBeExpanded::IsTaskbarForeground<bool,unsigned __int64 &>(bool &&,unsigned __int64 &)
	Line  36779: [0032D510] protected: virtual bool __cdecl TaskbarTelemetry::ViewCoordinator_ShouldTaskbarBeExpanded::WasAlreadyReportedToTelemetry(long)
	Line  60884: [007F3B08] public: void __cdecl TaskbarTelemetry::ViewCoordinator_ShouldTaskbarBeExpanded::ExtendedUIVisible<bool,unsigned __int64 &>(bool &&,unsigned __int64 &)
	Line  60885: [007F3B80] public: void __cdecl TaskbarTelemetry::ViewCoordinator_ShouldTaskbarBeExpanded::IgnoreForeground<bool,unsigned __int64 &>(bool &&,unsigned __int64 &)
	Line  60886: [007F3BF8] public: void __cdecl TaskbarTelemetry::ViewCoordinator_ShouldTaskbarBeExpanded::IsDragging<bool,unsigned __int64 &>(bool &&,unsigned __int64 &)
	Line  60888: [007F3CC8] public: void __cdecl TaskbarTelemetry::ViewCoordinator_ShouldTaskbarBeExpanded::IsPointerOverTaskbarFrame<bool,unsigned __int64 &>(bool &&,unsigned __int64 &)
	Line  60889: [007F3D40] public: void __cdecl TaskbarTelemetry::ViewCoordinator_ShouldTaskbarBeExpanded::IsShellViewDismissedViaTaskbarTap<bool,unsigned __int64 &>(bool &&,unsigned __int64 &)
	Line  60890: [007F3DB8] public: void __cdecl TaskbarTelemetry::ViewCoordinator_ShouldTaskbarBeExpanded::OverflowFlyoutVisible<bool,unsigned __int64 &>(bool &&,unsigned __int64 &)
	Line  60891: [007F3E30] public: void __cdecl TaskbarTelemetry::ViewCoordinator_ShouldTaskbarBeExpanded::ShellViewVisible<__int64,unsigned __int64 &>(__int64 &&,unsigned __int64 &)
	Line  60892: [007F3EA8] public: void __cdecl TaskbarTelemetry::ViewCoordinator_ShouldTaskbarBeExpanded::ShellViewVisibleCurrentState<__int64,unsigned __int64 &>(__int64 &&,unsigned __int64 &)
	Line  60893: [007F3F20] public: void __cdecl TaskbarTelemetry::ViewCoordinator_ShouldTaskbarBeExpanded::ShellViewVisibleIgnored<__int64,unsigned __int64 &>(__int64 &&,unsigned __int64 &)
	Line  60894: [007F3F98] public: void __cdecl TaskbarTelemetry::ViewCoordinator_ShouldTaskbarBeExpanded::ShellViewVisibleIgnoredBecauseOfException<__int64,__int64,unsigned __int64 &>(__int64 &&,__int64 &&,unsigned __int64 &)
	Line  60897: [007F419C] public: void __cdecl TaskbarTelemetry::ViewCoordinator_ShouldTaskbarBeExpanded::TaskbarHasFocus<bool,unsigned __int64 &>(bool &&,unsigned __int64 &)
	Line  60898: [007F4214] public: void __cdecl TaskbarTelemetry::ViewCoordinator_ShouldTaskbarBeExpanded::TaskbarShouldStayExpandedDocked<bool,unsigned __int64 &>(bool &&,unsigned __int64 &)
	Line  60944: [007F57E4] public: void __cdecl TaskbarTelemetry::ViewCoordinator_ShouldTaskbarBeExpanded::InitializeShellViewCoordinators(void)
	Line  60963: [007F669C] public: bool __cdecl winrt::Taskbar::implementation::ViewCoordinator::ShouldTaskbarBeExpanded(unsigned __int64,bool)







"""


#  [007D7F74] public: static void __cdecl TaskbarTelemetry::StartItemPressedScaleAnimation(bool const &)

def _duplicate_lines(block):
    lines = block.strip().splitlines()
    seen = set()
    unique_lines = []
    for line in lines:
        parts = line.split(']', 1)
        key = parts[1].strip() if len(parts) > 1 else line.strip()
        if key not in seen:
            seen.add(key)
            unique_lines.append(line)
    return "\n".join(unique_lines)


def _static(string: str):
    return string.replace("static ", "").replace("void* pThis, ", "").replace("pThis, ", "")


def generate_hook_code(input_block, output_filename="generated_hooks.cpp"):
    input_block = re.sub(string=input_block, pattern=r"\s+Line\s+\d+\:\s", repl="\n", flags=re.MULTILINE | re.DOTALL)
    input_block = _duplicate_lines(input_block)
    pattern = re.compile(
        r'\[\w+\]\s+'
        r'(?P<access>public:|private:|protected:|:)?\s+'
        r'(?P<virtual>virtual\s+)?'
        r'(?P<return_type>[\w\s\*\d\_\:]+?)\s+'
        r'__cdecl\s+'
        r'(?P<full_method>[\w:]+)'
        r'::(?P<method>\w+)'
        r'\((?P<params>.*)\)'
    )
    hook_prefix = "HookFunction"
    hook_defs = []
    hook_methods = []
    hook_names = []

    for line in input_block.strip().splitlines():
        line = line.strip()
        if not line:
            continue

        brackets = re.search(r'<.*>', line)
        extracted_brackets = ""
        line_original = line
        if brackets:
            extracted = brackets.group(0)
            line = line.replace(extracted, '').strip()
            extracted_brackets = extracted  #  outer < and >
        m = pattern.match(line)
        if not m:
            print(f"\033[91mUnable to parse line: {line}\033[0m")
            continue

        access = m.group('access').strip()
        virtual_keyword = m.group('virtual') or ""
        ret_type_raw = m.group('return_type').strip()
        full_method = m.group('full_method').strip()
        method_name = m.group('method').strip()
        params_raw = m.group('params').strip()

        class_name = full_method

        if ret_type_raw.lower() in ["__int64", "int64", "unsigned __int64"]:
            ret_type = "LRESULT"
        else:
            ret_type = ret_type_raw

        is_static = "static" in ret_type

        param_list = ["void* pThis"] if not is_static else []
        params_clean = params_raw.strip()
        if params_clean and params_clean != "void":
            parts = [p.strip() for p in params_clean.split(",") if p.strip()]
            for idx, p in enumerate(parts, start=1):
                p_clean = p.replace("struct ", "")
                param_list.append(f"{p_clean} param{idx}")

        suffix = "_WithArgs" if len(param_list) > (1 if not is_static else 0) else "_WithoutArgs"

        base_name = f"{class_name}_{method_name}"
        base_name = base_name.replace("::", "__")
        rand_hex = ''.join(random.choices('0123456789', k=6))
        hook_base = base_name + "_" + rand_hex + "_" + suffix

        ret_type_for_using = ret_type if not is_static else _static(ret_type)

        typedef_line = f"using {hook_base}_t = {ret_type_for_using}(WINAPI*)({', '.join(param_list)});"
        original_decl = f"{hook_base}_t {hook_base}_Original;"

        params_str = ", ".join(param_list)
        log_name = base_name

        if "static" in ret_type:
            params_str = _static(params_str)

        if ret_type == "void":
            hook_func = (
                f"""void WINAPI {hook_base}_Hook({params_str}) {{
                Wh_Log(L"Method called: {log_name}");
                {hook_base}_Original({', '.join([p.split()[-1] for p in param_list])});
            }}"""
            )
        else:
            hook_func = (
                f"""{ret_type} WINAPI {hook_base}_Hook({params_str}) {{
                Wh_Log(L"Method called: {log_name}");
                return {hook_base}_Original({', '.join([p.split()[-1] for p in param_list])});
            }}"""
            )

        hook_defs.append(typedef_line)
        hook_defs.append(original_decl)
        hook_defs.append(hook_func)
        hook_defs.append("")

        method_name_str = hook_base
        hook_names.append(method_name_str)

        hook_method = (
            f"bool {hook_prefix}{method_name_str}() {{\n"
            f"    HMODULE module = LoadLibrary(L\"{dll_to_hook}\");\n"
            f"    if (!module) {{\n"
            f"        Wh_Log(L\"Failed to load {dll_to_hook} for {method_name_str}\");\n"
            f"        return false;\n"
            f"    }}\n\n"
            f"    WindhawkUtils::SYMBOL_HOOK hook[] = {{ {{ {{LR\"({line_original[line_original.find(access):].strip()})\"}},\n"
            f"                                         &{hook_base}_Original,\n"
            f"                                         {hook_base}_Hook }} }};\n\n"
            f"    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));\n"
            f"}}\n"
        )

        hook_methods.append(hook_method)

    output_lines = []
    output_lines.append(
        fr"""
// ==WindhawkMod==
// @id      hooks-spy-mod
// @name    Spy for function calls
// @description     Print the names of functions being called
// @version 0.1
// @author  DarkionAvey
// @include {exe_to_hook}
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



""")
    output_lines.append("// Auto-generated hook definitions and functions")
    output_lines.append("")
    output_lines.extend(hook_defs)
    output_lines.append("")
    for hm in hook_methods:
        output_lines.append(hm)
        output_lines.append("")
    init_function = ["BOOL Wh_ModInit() {", "    bool anyHooked = false;"]
    for idx, name in enumerate(hook_names, start=1):
        init_function.append(f"    if (!{hook_prefix}{name}()) {{")
        init_function.append(f"        Wh_Log(L\"Method hooking failed: {name}\");")
        init_function.append("    } else {")
        init_function.append("        anyHooked = true;")
        init_function.append("    }")
        init_function.append("")
    init_function.append("    if (!anyHooked) {         Wh_Log(L\"Nothing was hooked\");")
    init_function.append("        return FALSE;")
    init_function.append("    }")
    init_function.append("    return TRUE;")
    init_function.append("}")
    init_function.append("")
    init_function.append("void Wh_ModUninit() {}")
    init_function.append("void Wh_ModSettingsChanged() {}")
    output_lines.extend(init_function)

    # output_lines = [x.replace(": ", "").replace("::", "").replace("_", "") for x in output_lines]

    with open(output_filename, "w", encoding="utf-8") as f:
        f.write("\n".join(output_lines))


if __name__ == '__main__':
    generate_hook_code(input_block)
