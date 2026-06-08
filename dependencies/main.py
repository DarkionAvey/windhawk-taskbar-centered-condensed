from __future__ import annotations

if __package__ in (None, ""):
    import sys
    from pathlib import Path

    sys.path.insert(0, str(Path(__file__).resolve().parent.parent))

    from dependencies.common import generate_slash_block
    from dependencies.start_button_position import StartButtonPosition
    from dependencies.taskbar_icon_size import TaskbarIconSizeMod
    from dependencies.taskbar_styler import TaskbarStylerMod
else:
    from .common import generate_slash_block
    from .start_button_position import StartButtonPosition
    from .taskbar_icon_size import TaskbarIconSizeMod
    from .taskbar_styler import TaskbarStylerMod


def generate_mod_art() -> None:
    print(generate_slash_block("TAI"))


def process_all_mods() -> None:
    generate_mod_art()
    processors = [
        TaskbarIconSizeMod(),
        StartButtonPosition(),
        # TaskbarStylerMod(),
    ]

    for processor in processors:
        processor.process()


if __name__ == "__main__":
    process_all_mods()
