import glob
import os
import re

TYPE_MAP = {
    "signed int": "Integer (whole number)",
    "int": "Integer (whole number)",
    "unsigned int": "Non-negative integer",
    "float": "Decimal number",
    "double": "Decimal number",
    "string": "Text",
    "bool": "Boolean (true/false)"
}


def generate_options_table(mod_settings_contents):
    blocks = [b.strip() for b in mod_settings_contents.strip().split('\n\n') if b.strip()]
    rows = []
    for block in blocks:
        lines = block.splitlines()
        first_line_match = re.match(r'-\s*(\S+):\s*(.*)', lines[0])
        if not first_line_match:
            continue
        prop = first_line_match.group(1)
        prop_code = f"`{prop}`"
        name = description = type_info = ""
        for line in lines[1:]:
            line = line.strip()
            if line.startswith('$name:'):
                name = line.split(':', 1)[1].strip()
            elif line.startswith('$description:'):
                description = line.split(':', 1)[1].strip()
            elif line.startswith('$type:'):
                type_info = line.split(':', 1)[1].strip()
        user_friendly_type = TYPE_MAP.get(type_info, type_info)
        rows.append((prop_code, name, description, user_friendly_type))

    header = "| Property | Name | Description | Accepted values |\n| --- | --- | --- | --- |"
    table_rows = "\n".join(f"| {prop} | {name} | {desc} | {type} |" for prop, name, desc, type in rows)
    table_md = f"{header}\n{table_rows}"
    return table_md


def remove_options_section(readme_contents):
    pattern = re.compile(r"(?:^|\n)#\s*Options\b.*?(?=\n##\s|\Z)", re.DOTALL)
    new_contents, _ = pattern.subn("\n", readme_contents)
    return new_contents.strip()


def read_file(path):
    with open(path, 'r', encoding='utf-8') as f:
        contents = f.read()
    return contents


def remove_cpp_comments(code: str) -> str:
    code = re.sub(r'/\*.*?\*/', '', code, flags=re.DOTALL)
    code = re.sub(r'//.*', '', code)
    code = '\n'.join(line for line in (l.strip() for l in code.splitlines()) if line)
    return code


def write_file(path, content):
    with open(path, 'w') as f:
        f.write(content)


def get_next_patch_version(version_file_path):
    if not os.path.exists(version_file_path):
        write_file(version_file_path, '0')
        return 0
    with open(version_file_path, 'r+') as f:
        try:
            patch = int(f.read().strip())
        except ValueError:
            patch = 0
        patch += 1
        f.seek(0)
        f.write(str(patch))
        f.truncate()
    return patch


def get_next_patch_version(version_file_path):
    if not os.path.exists(version_file_path):
        return 0
    with open(version_file_path, 'r') as f:
        last_version = f.read().strip()
    try:
        major_minor_file, patch_file = last_version.rsplit('.', 1)
        patch_file = int(patch_file)
    except ValueError:
        # fallback in case of malformed version file
        return 0
    return patch_file + 1


def main(major_minor="1.4"):
    base_dir = os.path.dirname(os.path.abspath(__file__))
    version_file_path = os.path.join(base_dir, 'mod-parts', 'mod-build-version.txt')

    if os.path.exists(version_file_path):
        with open(version_file_path, 'r') as f:
            current_version = f.read().strip()
        current_major_minor = '.'.join(current_version.split('.')[:2])
    else:
        current_major_minor = None

    if current_major_minor != major_minor:
        patch = 0  # reset patch
    else:
        patch = get_next_patch_version(version_file_path)

    version = f"{major_minor}.{patch}"

    with open(version_file_path, 'w') as f:
        f.write(version)

    readme_path = os.path.join(base_dir, 'README.md')
    dependencies_dir_path = os.path.join(base_dir, 'dependencies', 'modified-dependencies')
    output_path = os.path.join(base_dir, 'assembled-mod.cpp')

    readme_contents = read_file(readme_path)
    header_contents = read_file(os.path.join(base_dir, 'mod-parts', 'win-dock-mod-header.txt'))
    mod_settings_contents = read_file(os.path.join(base_dir, 'mod-parts', 'mod-settings.yml'))

    options_table = generate_options_table(mod_settings_contents)
    options_section = f"# Options\n\n{options_table}\n"

    readme_no_options = remove_options_section(readme_contents)
    new_readme_contents = f"{readme_no_options}\n\n{options_section}".strip()
    with open(readme_path, 'w', encoding='utf-8') as f:
        f.write(new_readme_contents)

    new_readme_for_header = new_readme_contents  # Use the updated README
    header_contents = header_contents.replace('{read_me_contents}', new_readme_for_header.strip())
    header_contents = header_contents.replace('{mod_settings}', re.sub(r'^\s*\$type:.*$', '', mod_settings_contents, flags=re.MULTILINE).strip())
    header_contents = header_contents.replace('{version_code}', version)

    merged_contents = header_contents + "\n\n"
    for cpp_file_path in glob.glob(os.path.join(dependencies_dir_path, '*.cpp')):
        with open(cpp_file_path, 'r', encoding='utf-8') as f:
            cpp_contents = f.read()
            merged_contents += f"{cpp_contents}\n\n"

    merged_contents = f"""
{merged_contents.strip()}

{read_file(os.path.join(base_dir, 'mod-parts', 'ascii-art-and-imports.cpp')).strip()}

{read_file(os.path.join(base_dir, 'mod-parts', 'utils-style-xml.cpp')).strip()}

{read_file(os.path.join(base_dir, 'mod-parts', 'utils-string.cpp')).strip()}

{read_file(os.path.join(base_dir, 'mod-parts', 'utils-debouncer.cpp')).strip()}

{read_file(os.path.join(base_dir, 'mod-parts', 'utils-apply-style-helpers.cpp')).strip()}

{read_file(os.path.join(base_dir, 'mod-parts', 'win-dock-mod.cpp')).strip()}
"""

    merged_contents = re.sub(r'[ \t]*\n', '\n', merged_contents)  # remove whitespace-only lines
    merged_contents = re.sub(r'\n+', '\n', merged_contents).strip()
    merged_contents = merged_contents.replace("LoadLibrary(L", "GetModuleHandle(L")
    merged_contents = merged_contents.replace(") {", ") {\n  Wh_Log(L\".\");")
    with open(output_path, 'w', encoding='utf-8') as f:
        f.write(merged_contents)

    print("Saved assembled mod")


if __name__ == '__main__':
    from dependencies import main as dependency_maker

    dependency_maker.process_all_mods()
    main()
