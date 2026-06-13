from __future__ import annotations

import re
from abc import ABC, abstractmethod

import requests

from .common import MODIFIED_DEPENDENCIES_DIR, generate_slash_block
from .cpp_patcher import CppPatcher


class URLProcessor(ABC):
    def __init__(self, url: str, name: str, injection_order: str):
        self.url = url
        self.name = name
        self.injection_order = injection_order
        self.output_folder = MODIFIED_DEPENDENCIES_DIR
        self.output_folder.mkdir(parents=True, exist_ok=True)

    @abstractmethod
    def format_content(self, content: str) -> str:
        pass

    def download(self) -> str:
        response = requests.get(self.url, timeout=30)
        response.raise_for_status()
        return response.text

    def save(self, content: str, filename: str) -> None:
        filename = f"{self.injection_order}_{filename}"
        filepath = self.output_folder / filename
        filepath.write_text(content, encoding="utf-8")
        print(f"Saved: {filepath}")

    def process(self) -> None:
        filename = self.url.split("/")[-1] or "output.mod"
        content = self.download()

        if match := re.search(r"^\s*//\s*@compilerOptions\s+(.*)$", content, re.MULTILINE):
            compiler_dump_dir = self.output_folder / "compiler-options-dump"
            compiler_dump_dir.mkdir(parents=True, exist_ok=True)
            (compiler_dump_dir / filename.replace(".cpp", ".txt")).write_text(
                match.group(1).strip(),
                encoding="utf-8",
            )

        patch = CppPatcher(content, source_name=filename)
        content = (
            patch.strip()
            .remove_regex(
                r"//\s+==WindhawkMod==.*?WindhawkModSettings==\s+(?=#include)",
                label="remove Windhawk metadata block",
            )
            .replace_literal("g_settings", f"g_settings_{self.name.lower()}", label="namespace g_settings")
            .remove_literal('Wh_Log(L">");', label="remove placeholder Wh_Log")
            .replace_literal("LoadSettings", f"LoadSettings{self.name}", label="namespace LoadSettings")
            .replace_literal("ApplySettings(", f"ApplySettings{self.name}(", label="namespace ApplySettings")
            .replace_literal(
                "HookTaskbarDllSymbols",
                f"HookTaskbarDllSymbols{self.name}",
                label="namespace HookTaskbarDllSymbols",
            )
            .replace_literal("Wh_ModInit", f"Wh_ModInit{self.name}", label="namespace Wh_ModInit")
            .replace_literal("Wh_ModAfterInit", f"Wh_ModAfterInit{self.name}", label="namespace Wh_ModAfterInit")
            .replace_literal(
                "Wh_ModBeforeUninit",
                f"Wh_ModBeforeUninit{self.name}",
                label="namespace Wh_ModBeforeUninit",
            )
            .replace_literal("Wh_ModUninit", f"Wh_ModUninit{self.name}", label="namespace Wh_ModUninit")
            .replace_literal(
                "Wh_ModSettingsChanged",
                f"Wh_ModSettingsChanged{self.name}",
                label="namespace Wh_ModSettingsChanged",
            )
            .replace_literal(
                "g_taskbarViewDllLoaded",
                f"g_taskbarViewDllLoaded{self.name}",
                label="namespace g_taskbarViewDllLoaded",
            )
            .text()
        )

        content = self.format_content(content)

        patch = CppPatcher(content, source_name=filename)

        content = (
            patch
            .collapse_blank_lines(required=True)
            .prepend(generate_slash_block(self.name))
            .remove_whitespace_only_lines(required=False)
            .collapse_blank_lines(required=False)
            .strip_optional()
            .text()
        )
        self.save(content, filename)
