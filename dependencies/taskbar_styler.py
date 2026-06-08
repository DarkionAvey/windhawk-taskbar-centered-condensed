from __future__ import annotations

from .cpp_patcher import CppPatcher
from .url_processor import URLProcessor


class TaskbarStylerMod(URLProcessor):
    def __init__(self):
        url = "https://raw.githubusercontent.com/ramensoftware/windhawk-mods/refs/heads/main/mods/windows-11-taskbar-styler.wh.cpp"
        super().__init__(url, "BlurBrush", "c")

    def format_content(self, content: str) -> str:
        patch = CppPatcher(content, source_name=self.name)
        return (
            patch.keep_from_literal("#include <initguid.h>")
            .keep_until_literal("void SetOrClearValue")
            .strip_optional()
            .assert_startswith("#include", label="Taskbar styler should start with includes")
            .assert_endswith(
                "////////////////////////////////////////////////////////////////////////////////",
                label="Taskbar styler should end at separator",
            )
            .text()
        )
