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


def main(version="1.1.0"):
    base_dir = os.path.dirname(os.path.abspath(__file__))
    header_path = os.path.join(base_dir, 'mod-parts', 'dock-like-mod-header.txt')
    readme_path = os.path.join(base_dir, 'README.md')
    mod_settings_path = os.path.join(base_dir, 'mod-parts', 'mod-settings.yml')
    dependencies_dir_path = os.path.join(base_dir, 'dependencies', 'modified-dependencies')
    docklike_mod_path = os.path.join(base_dir, 'mod-parts', 'docklike-mod.cpp')
    output_path = os.path.join(base_dir, 'assembled-mod.cpp')
    with open(readme_path, 'r', encoding='utf-8') as f:
        readme_contents = f.read()
    with open(mod_settings_path, 'r', encoding='utf-8') as f:
        mod_settings_contents = f.read()
    with open(header_path, 'r', encoding='utf-8') as f:
        header_contents = f.read()

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

    with open(docklike_mod_path, 'r', encoding='utf-8') as f:
        docklike_mod_contents = f.read()

    merged_contents = merged_contents + docklike_mod_contents + "\n\n"

    with open(output_path, 'w', encoding='utf-8') as f:
        f.write(merged_contents)

    print("Saved assembled mod")


if __name__ == '__main__':
    from dependencies import main as dependency_maker

    dependency_maker.process_all_mods()
    main()
