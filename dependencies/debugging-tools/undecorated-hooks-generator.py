import re

# This file generates a Windhawk mod to monitor method calls.
# Simply paste the methods of interest inside the input_block block,
# and it will generate the mod. Useful for tracking method execution
# and identifying potential hooks.
# input_block = r"""
# Line 168: 	Line 13333: [00149450] public: virtual int __cdecl CTaskListThumbnailWnd::GetMargins(struct tagRECT *)const
# 	Line 173: 	Line 13338: [001497F0] public: virtual int __cdecl CTaskListThumbnailWnd::GetThumbBarRectFromIndex(int,struct tagRECT *)const
# 	Line 177: 	Line 13342: [00149930] public: virtual int __cdecl CTaskListThumbnailWnd::GetWindowLocation(struct tagRECT *)const
# 		Line  46: 	Line  2187: [0003F848] private: float __cdecl CTaskListThumbnailWnd::_CalcThumbnailScale(class CDPA<struct ITaskThumbnail,class CTContainer_PolicyUnOwned<struct ITaskThumbnail> > const *,struct tagRECT const *,int,int,unsigned char,int *,int *,struct tagSIZE *)const
# 	Line  51: 	Line  2193: [000403A0] public: virtual int __cdecl CTaskListThumbnailWnd::TryGetThumbShareRegionRectFromIndex(int,struct tagRECT *)const
#
# """
input_block = r"""
	Line  4177: [000729A0] public: virtual bool __cdecl WinRTIconLoadData::IsSmallIconMode(void)
	Line  4398: [000786A0] public: virtual unsigned int __cdecl WinRTIconLoadData::GetDPI(void)
	[00065FB8] private: bool __cdecl IconContainer::ShouldProcessIconLoadData(struct IIconLoadData *)
	
	Line  3754: [0006D3A0] public: virtual enum IconType __cdecl IconLoadDataBase::GetType(void)
	Line  4174: [000729A0] public: virtual bool __cdecl IconLoadDataBase::IsSmallIconMode(void)
	Line  4395: [000786A0] public: virtual unsigned int __cdecl IconLoadDataBase::GetDPI(void)
	Line  4469: [0007A330] public: virtual bool __cdecl IconLoadDataBase::IsValid(void)
	Line  5524: [00095050] public: virtual void __cdecl IconLoadDataBase::SetIcon(struct HICON__ *,int,enum IconQuality,int)
	Line  5716: [00097370] public: virtual enum IconQuality __cdecl IconLoadDataBase::GetIconQuality(void)
	Line  5797: [00098230] public: virtual long __cdecl IconLoadDataBase::SetDefaults(unsigned int,unsigned int,unsigned int,enum IconType,enum IconSizeMode,enum IconLoadPass,int,class tip2::tip_test<class tip2::details::merged_data<struct TaskbarTip::_tip_IconLoadTest,struct TaskbarTip::_tip_IconLoadTest> >)
	Line  9098: [000E2D80] public: virtual void __cdecl IconLoadDataBase::SetIconQuality(enum IconQuality)
"""
	# Line  59797: [007D7F74] public: static void __cdecl TaskbarTelemetry::StartItemPressedScaleAnimation(bool const &)


def generate_hook_code(input_block, output_filename="generated_hooks.cpp"):
    input_block = re.sub(string=input_block, pattern=r"\s+Line\s+\d+\:\s", repl="\n", flags=re.MULTILINE | re.DOTALL)
    pattern = re.compile(
        r'\[\w+\]\s+'
        r'(?P<access>public:|private:|protected:|RemoveAccessToken:)?\s+'
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

    with open(output_filename, "w", encoding="utf-8") as f:
        f.write("\n".join(output_lines))


if __name__ == '__main__':
    generate_hook_code(input_block)
