from __future__ import annotations

import os
from pathlib import Path

from art import text2art

DEPENDENCIES_DIR = Path(__file__).resolve().parent
REPO_ROOT = DEPENDENCIES_DIR.parent
MOD_PARTS_DIR = REPO_ROOT / "mod-parts"
HOOKS_DIR = MOD_PARTS_DIR / "hooks"
PATCHES_DIR = DEPENDENCIES_DIR / "patches"
MODIFIED_DEPENDENCIES_DIR = DEPENDENCIES_DIR / "modified-dependencies"


def read_file(path: str | os.PathLike[str]) -> str:
    return Path(path).read_text(encoding="utf-8")


def read_patch(filename: str) -> str:
    return read_file(PATCHES_DIR / filename)


def generate_slash_block(name: str) -> str:
    art = text2art(name, font="starwars").rstrip("\n")
    art_lines = [f"////  {line}  ////" for line in art.splitlines()]
    width = max(len(line) for line in art_lines) + 8
    line = "/" * width
    return "\n".join([line, line, line, *art_lines, line, line, line]) + "\n"
