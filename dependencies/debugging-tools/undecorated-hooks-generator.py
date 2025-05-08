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
	Line  1989: [0003752C] private: static int __cdecl ShellIconLoaderV2::s_GetShellIconSize(int)
	Line  4901: [00080474] private: static long __cdecl ShellIconLoaderV2::s_ForceImagePresent(int,int,struct HICON__ * *)
	Line  5534: [00095AC8] private: bool __cdecl ShellIconLoaderV2::LoadSyncWindowIcon(int,struct HICON__ * *)
	Line  5535: [00095B44] private: void __cdecl ShellIconLoaderV2::TryLoadIconFromResourceIfNeeded(void)
	Line  6637: [000AA788] private: static int __cdecl ShellIconLoaderV2::s_GetShellImageListSizeId(int)
	Line 14968: [0017B75C] private: struct winrt::fire_and_forget __cdecl ShellIconLoaderV2::ForceImagePresentAsync(int,int)
	Line 14969: [0017B7D0] public: long __cdecl ShellIconLoaderV2::InitializeLoadWindowIconOnlyV2(void)
	Line 14970: [0017B8A0] public: long __cdecl ShellIconLoaderV2::InitializeShellIconCacheIconOnlyV2(void)
	Line 14971: [0017B930] public: long __cdecl ShellIconLoaderV2::InitializeWindowAndShellIconV2(void)
	Line 14974: [0017BEF0] private: struct winrt::fire_and_forget __cdecl ShellIconLoaderV2::LoadAsyncIcon(int)
	Line 14975: [0017BF64] private: bool __cdecl ShellIconLoaderV2::LoadShellIconCacheIcon(int)
	Line 14981: [0017C510] private: bool __cdecl ShellIconLoaderV2::TryLoadAsyncIcon(int)
	Line 14985: [0017C808] public: static struct winrt::fire_and_forget __cdecl ShellIconLoaderV2::final_release(class std::unique_ptr<class ShellIconLoaderV2,struct std::default_delete<class ShellIconLoaderV2> >)
	Line 14986: [0017C88C] private: static long __cdecl ShellIconLoaderV2::s_LoadIconsFromResourceSync(unsigned short const *,unsigned short,int,struct HICON__ * *)
	Line 14987: [0017C960] private: static void __cdecl ShellIconLoaderV2::s_SetIconCallback(void *,void *,int,int)
	Line 14988: [0017CA14] private: static bool __cdecl ShellIconLoaderV2::s_ShouldLoadBetterIcon(struct _ICONINFOEXW const &,int)

"""
	# Line  59797: [007D7F74] public: static void __cdecl TaskbarTelemetry::StartItemPressedScaleAnimation(bool const &)

def remove_duplicate_lines(block):
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



def remove_static(string:str):
    return string.replace("static ", "").replace("void* pThis, ", "").replace("pThis, ", "")


def generate_hook_code(input_block, output_filename="generated_hooks.cpp"):
    input_block = re.sub(string=input_block, pattern=r"\s+Line\s+\d+\:\s", repl="\n", flags=re.MULTILINE | re.DOTALL)
    input_block=remove_duplicate_lines(input_block)
    pattern = re.compile(
        r'\[\w+\]\s+'
        r'(?P<access>public:|private:|protected:|remove:)?\s+'
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

        is_static="static" in ret_type

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
        hook_base = base_name + suffix

        ret_type_for_using=ret_type if not is_static else remove_static(ret_type)

        typedef_line = f"using {hook_base}_t = {ret_type_for_using}(WINAPI*)({', '.join(param_list)});"
        original_decl = f"{hook_base}_t {hook_base}_Original;"


        params_str = ", ".join(param_list)
        log_name = base_name

        if "static" in ret_type:
            params_str= remove_static(params_str)

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
#include <winrt/base.h>
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

    output_lines=[x.replace("remove: ","").replace("Remove::","").replace("Remove_","") for x in output_lines]

    with open(output_filename, "w", encoding="utf-8") as f:
        f.write("\n".join(output_lines))


if __name__ == '__main__':
    generate_hook_code(input_block)
