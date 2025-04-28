import re

# This file generates a Windhawk mod to monitor method calls.
# Simply paste the methods of interest inside the input_block block,
# and it will generate the mod. Useful for tracking method execution
# and identifying potential hooks.

input_block = r"""
	Line  13: 	Line  1477: [00021384] protected: long __cdecl CTaskBand::_InsertItem(struct HWND__ *,struct ITaskItem * *,struct HWND__ *,struct HWND__ *)
	Line  16: 	Line  1491: [000220C0] protected: long __cdecl CTaskBand::_MatchApp(struct IImmersiveApplication *,struct ITaskGroup * *,struct ITaskItem * *)
	Line  19: 	Line  1494: [000228CC] protected: long __cdecl CTaskBand::_MatchApplicationView(struct IApplicationView *,struct ITaskGroup * *,struct ITaskItem * *)
	Line  33: 	Line  1543: [00024B90] protected: void __cdecl CTaskBand::_DeleteTask(struct ITaskGroup *,struct ITaskItem *,enum SWITCHER_REMOVAL_REASON)
	Line  61: 	Line  2604: [0004E5B0] public: virtual long __cdecl CTaskBand::GetIconId(struct ITaskGroup *,struct ITaskItem *,int,int *)
	Line  62: 	Line  2605: [0004E6E8] protected: void __cdecl CTaskBand::_UpdateItemIcon(struct ITaskGroup *,struct ITaskItem *)
	Line  81: 	Line  2821: [00055C6C] protected: void __cdecl CTaskBand::_SwitchToWindow(struct ITaskItem *)
	Line 106: 	Line  3295: [00065910] public: virtual long __cdecl CTaskBand::SwitchTo(struct ITaskItem *,int)
	Line 108: 	Line  3305: [000660E4] protected: void __cdecl CTaskBand::_HandleTaskActivated(struct ITaskGroup *,struct ITaskItem *)
	Line 126: 	Line  4229: [00073D60] public: virtual void __cdecl CTaskBand::RemoveIcon(struct ITaskItem *)
	Line 133: 	Line  4375: [00077D90] public: virtual void __cdecl CTaskBand::CloseItem(struct ITaskItem *)
	Line 159: 	Line  4976: [00081FBC] protected: void __cdecl CTaskBand::_AddAppTaskItem(struct ITaskItem *,struct RESOLVEDWINDOW *)
	Line 162: 	Line  5062: [00082940] protected: long __cdecl CTaskBand::_BuildTaskList(class CDPA<struct ITaskItem,class CTContainer_PolicyUnOwned<struct ITaskItem> > *)
	Line 167: 	Line  5187: [00084FE8] protected: void __cdecl CTaskBand::_HandleMonitorChanged(struct ITaskGroup *,struct ITaskItem *)
	Line 180: 	Line  5470: [0008BEB0] protected: void __cdecl CTaskBand::_HandleItemResolved(struct RESOLVEDWINDOW *,struct ITaskListUI *,struct ITaskGroup *,struct ITaskItem *)
	Line 204: 	Line  6845: [000AD8C0] public: virtual long __cdecl CTaskBand::GetTaskFromWindow(struct HWND__ *,struct ITaskGroup * *,struct ITaskItem * *)
	Line 301: 	Line 10778: [00109A90] public: virtual long __cdecl CTaskBand::HandleContextMenuCommand(struct ITaskItemFilter *,struct ITaskGroup *,struct ITaskItem *,struct HWND__ *,struct IContextMenu *,int)
	Line 380: 	Line 10944: [00111500] public: virtual long __cdecl CTaskBand::RequestContextMenu(struct ITaskGroup *,struct ITaskItem *,int,struct IContextMenu * *,struct HMENU__ * *)
	Line 430: 	Line 11031: [00118B2C] protected: long __cdecl CTaskBand::_ResolveInsertBefore(struct HWND__ *,struct ITaskGroup *,struct ITaskItem *,struct ITaskGroup * *,struct ITaskItem * *)
"""


def generate_hook_code(input_block, output_filename="generated_hooks.cpp"):
    input_block = re.sub(string=input_block, pattern=r"\s+Line\s+\d+\:\s", repl="\n", flags=re.MULTILINE | re.DOTALL)
    pattern = re.compile(
        r'\[\w+\]\s+'
        r'(?P<access>public:|private:|protected:)\s+'
        r'(?P<virtual>virtual\s+)?'
        r'(?P<return_type>[\w\s\*\d\_\:]+?)\s+'
        r'__cdecl\s+'
        r'(?P<full_method>[\w:]+)'
        r'::(?P<method>\w+)'
        r'\((?P<params>.*)\)'
    )
    hook_prefix = "HookFunction"
    hook_defs = []
    hook_array_entries = []
    hook_methods = []
    hook_names = []

    for line in input_block.strip().splitlines():
        line = line.strip()
        if not line:
            continue

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

        param_list = ["void* pThis"]
        params_clean = params_raw.strip()
        if params_clean and params_clean != "void":
            parts = [p.strip() for p in params_clean.split(",") if p.strip()]
            for idx, p in enumerate(parts, start=1):
                p_clean = p.replace("struct ", "")
                param_list.append(f"{p_clean} param{idx}")

        suffix = "_WithArgs" if len(param_list) > 1 else "_WithoutArgs"

        base_name = f"{class_name}_{method_name}"
        base_name = base_name.replace("::", "__")
        hook_base = base_name + suffix

        typedef_line = f"using {hook_base}_t = {ret_type}(WINAPI*)({', '.join(param_list)});"
        original_decl = f"{hook_base}_t {hook_base}_Original;"

        params_str = ", ".join(param_list)
        log_name = base_name

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

        hook_entry = (
            f'    {{ {{LR"({line[line.find(access):].strip()}{virtual_keyword})"}},\n'
            f'      &{hook_base}_Original,\n'
            f'      {hook_base}_Hook,\n'
            f'    }}'
        )

        hook_defs.append(typedef_line)
        hook_defs.append(original_decl)
        hook_defs.append(hook_func)
        hook_defs.append("")
        hook_array_entries.append(hook_entry)

        method_name_str = hook_base
        hook_names.append(method_name_str)
        hook_method = (
            f"bool {hook_prefix}{method_name_str}() {{\n"
            f"    HMODULE module = LoadLibrary(L\"taskbar.dll\");\n"
            f"    if (!module) {{\n"
            f"        Wh_Log(L\"Failed to load taskbar.dll for {method_name_str}\");\n"
            f"        return false;\n"
            f"    }}\n\n"
            f"    WindhawkUtils::SYMBOL_HOOK hook[] = {{ {{ {{LR\"({line[line.find(access):].strip()})\"}},\n"
            f"                                         &{hook_base}_Original,\n"
            f"                                         {hook_base}_Hook }} }};\n\n"
            f"    return WindhawkUtils::HookSymbols(module, hook, ARRAYSIZE(hook));\n"
            f"}}\n"
        )
        hook_methods.append(hook_method)

    output_lines = []
    output_lines.append(
        r"""
// ==WindhawkMod==
// @id      hooks-spy-mod
// @name    Spy for function calls
// @description     Print the names of functions being called
// @version 0.1
// @author  DarkionAvey
// @include explorer.exe
// @compilerOptions -ldwmapi -lole32 -loleaut32 -lruntimeobject -lshcore
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
    init_function.append("    if (!anyHooked) {")
    init_function.append("        return FALSE;")
    init_function.append("    }")
    init_function.append("    return TRUE;")
    init_function.append("}")
    init_function.append("")
    init_function.append("void Wh_ModUninit() {}")
    init_function.append("void Wh_ModSettingsChanged() {}")
    output_lines.extend(init_function)

    with open(output_filename, "w", encoding="utf-8") as f:
        f.write("\n".join(output_lines))


if __name__ == '__main__':
    generate_hook_code(input_block)
