import os
import re
from abc import ABC, abstractmethod
from pathlib import Path
from typing import Callable, Match, Pattern, Union

import requests
from art import text2art

mod_parts_dir = "mod-parts"
hooks_dir = os.path.join(mod_parts_dir, "hooks")

RegexReplacement = Union[str, Callable[[Match[str]], str]]
DEFAULT_RE_FLAGS = re.DOTALL | re.MULTILINE


class PatchError(ValueError):
    """Raised when an expected source patch could not be applied."""


class CppPatcher:
    """
    Small C++-oriented patching helper.

    The important rule is: patch methods fail by default when they do not change
    the source. This is intentional, because upstream Windhawk mods can change
    and silent no-op patches are dangerous.
    """

    def __init__(self, content: str, *, source_name: str = "<source>"):
        self.content = content
        self.source_name = source_name

    def text(self) -> str:
        return self.content

    # ------------------------------------------------------------------
    # Generic patch primitives
    # ------------------------------------------------------------------

    def replace_regex(
            self,
            pattern: str,
            replacement: RegexReplacement,
            *,
            count: int = 0,
            flags: int = DEFAULT_RE_FLAGS,
            label: str | None = None,
            required: bool = True,
            in_function: str | None = None,
            in_function_regex: bool = False,
    ) -> "CppPatcher":
        """Replace a regex globally, or only inside one C++ function body.

        Pass ``in_function="void SomeFunction()"`` to limit the regex
        replacement to that function's body. The function signature is matched
        using the same flexible C++ signature matcher used by remove_function()
        and replace_function(), so copy-pasted multiline signatures work.
        """
        patch_label = label or f"replace regex: {pattern!r}"
        target, span = self._get_patch_target(
            in_function,
            in_function_regex=in_function_regex,
            label=patch_label,
        )
        new_target, n = re.subn(pattern, replacement, target, count=count, flags=flags)
        return self._commit_target(
            new_target,
            n,
            self._scope_label(patch_label, in_function),
            required,
            span=span,
        )

    def remove_regex(
            self,
            pattern: str,
            *,
            count: int = 0,
            flags: int = DEFAULT_RE_FLAGS,
            label: str | None = None,
            required: bool = True,
            in_function: str | None = None,
            in_function_regex: bool = False,
    ) -> "CppPatcher":
        return self.replace_regex(
            pattern,
            "",
            count=count,
            flags=flags,
            label=label or f"remove regex: {pattern!r}",
            required=required,
            in_function=in_function,
            in_function_regex=in_function_regex,
        )

    def replace_literal(
            self,
            old: str,
            new: str,
            *,
            count: int = -1,
            label: str | None = None,
            required: bool = True,
            in_function: str | None = None,
            in_function_regex: bool = False,
    ) -> "CppPatcher":
        """Replace literal text globally, or only inside one C++ function body.

        Example:
            patch.replace_literal(
                "ApplySettingsDebounced(-1);",
                "ApplySettingsDebounced(250);",
                count=1,
                in_function="void ApplySettingsFromTaskbarThreadIfRequired()",
            )
        """
        if count == 0:
            raise ValueError("Use count=-1 for unlimited literal replacements. count=0 is ambiguous for str.replace().")

        patch_label = label or f"replace literal: {old!r}"
        target, span = self._get_patch_target(
            in_function,
            in_function_regex=in_function_regex,
            label=patch_label,
        )
        n = target.count(old) if count < 0 else min(target.count(old), count)
        scoped_label = self._scope_label(patch_label, in_function)
        if n == 0 and required:
            self._raise_no_change(scoped_label)

        new_target = target.replace(old, new, count if count >= 0 else -1)
        return self._commit_target(
            new_target,
            n,
            scoped_label,
            required,
            span=span,
        )

    def remove_literal(
            self,
            old: str,
            *,
            count: int = -1,
            label: str | None = None,
            required: bool = True,
            in_function: str | None = None,
            in_function_regex: bool = False,
    ) -> "CppPatcher":
        return self.replace_literal(
            old,
            "",
            count=count,
            label=label or f"remove literal: {old!r}",
            required=required,
            in_function=in_function,
            in_function_regex=in_function_regex,
        )

    def rename_identifier(
            self,
            old: str,
            new: str,
            *,
            label: str | None = None,
            in_function: str | None = None,
            in_function_regex: bool = False,
    ) -> "CppPatcher":
        """Rename a C/C++ identifier using identifier boundaries."""
        return self.replace_regex(
            rf"(?<![A-Za-z0-9_]){re.escape(old)}(?![A-Za-z0-9_])",
            new,
            label=label or f"rename identifier: {old} -> {new}",
            in_function=in_function,
            in_function_regex=in_function_regex,
        )

    def insert_before_regex(
            self,
            pattern: str,
            insertion: str,
            *,
            label: str | None = None,
            in_function: str | None = None,
            in_function_regex: bool = False,
    ) -> "CppPatcher":
        return self.replace_regex(
            pattern,
            lambda m: insertion + m.group(0),
            count=1,
            label=label or f"insert before: {pattern!r}",
            in_function=in_function,
            in_function_regex=in_function_regex,
        )

    def insert_after_regex(
            self,
            pattern: str,
            insertion: str,
            *,
            label: str | None = None,
            in_function: str | None = None,
            in_function_regex: bool = False,
    ) -> "CppPatcher":
        return self.replace_regex(
            pattern,
            lambda m: m.group(0) + insertion,
            count=1,
            label=label or f"insert after: {pattern!r}",
            in_function=in_function,
            in_function_regex=in_function_regex,
        )

    def insert_before_literal(
            self,
            marker: str,
            insertion: str,
            *,
            label: str | None = None,
            in_function: str | None = None,
            in_function_regex: bool = False,
    ) -> "CppPatcher":
        return self.replace_literal(
            marker,
            insertion + marker,
            count=1,
            label=label or f"insert before: {marker!r}",
            in_function=in_function,
            in_function_regex=in_function_regex,
        )

    def insert_after_literal(
            self,
            marker: str,
            insertion: str,
            *,
            label: str | None = None,
            in_function: str | None = None,
            in_function_regex: bool = False,
    ) -> "CppPatcher":
        return self.replace_literal(
            marker,
            marker + insertion,
            count=1,
            label=label or f"insert after: {marker!r}",
            in_function=in_function,
            in_function_regex=in_function_regex,
        )

    def prepend(self, insertion: str) -> "CppPatcher":
        self.content = insertion + self.content
        return self

    # ------------------------------------------------------------------
    # C++ block/function helpers
    # ------------------------------------------------------------------

    def remove_function(self, signature: str, *, regex: bool = False, label: str | None = None) -> "CppPatcher":
        """
        Remove exactly one C++ function definition by signature.

        By default, the signature is treated as literal C++ text, not as a
        regular expression. Whitespace is flexible, so both one-line signatures
        and copy-pasted formatted signatures work. Regex-special C++ characters
        such as `(`, `)`, `*`, `<`, `>`, and `::` do not need escaping.

        Examples:
            patch.remove_function("FrameworkElement EnumChildElements(")
            patch.remove_function(
                "FrameworkElement EnumChildElements(\n"
                "    FrameworkElement element,\n"
                "    std::function<bool(FrameworkElement)> enumCallback)"
            )

        If the signature matches multiple function definitions, patching fails
        and asks for a more specific signature.
        """
        start, end = self._find_single_cpp_block_after_signature(signature, regex=regex, label=label)
        return self._replace_span(start, end, "", label or f"remove function: {signature}")

    def replace_function(
            self,
            signature: str,
            replacement: str,
            *,
            regex: bool = False,
            label: str | None = None,
    ) -> "CppPatcher":
        """Replace exactly one full C++ function definition by signature."""
        start, end = self._find_single_cpp_block_after_signature(signature, regex=regex, label=label)
        return self._replace_span(
            start,
            end,
            self._trim_block_replacement(replacement, trailing_newline=True),
            label or f"replace function: {signature}",
        )

    def replace_function_body(
            self,
            signature: str,
            new_body: str,
            *,
            regex: bool = False,
            label: str | None = None,
    ) -> "CppPatcher":
        """Keep the original signature and replace only the function body."""
        start, end = self._find_single_cpp_block_after_signature(signature, regex=regex, label=label)
        open_brace = self.content.find("{", start, end)
        if open_brace == -1:
            self._raise_no_change(label or f"replace function body: {signature}")
        replacement = "{\n" + self._trim_block_replacement(new_body, trailing_newline=False) + "\n}"
        return self._replace_span(open_brace, end, replacement, label or f"replace function body: {signature}")

    def disable_function_at_start(
            self,
            signature: str,
            statement: str,
            *,
            regex: bool = False,
            label: str | None = None,
    ) -> "CppPatcher":
        """Insert a statement immediately after a function's opening brace."""
        start, end = self._find_single_cpp_block_after_signature(signature, regex=regex, label=label)
        open_brace = self.content.find("{", start, end)
        if open_brace == -1:
            self._raise_no_change(label or f"disable function: {signature}")
        return self._replace_span(open_brace, open_brace + 1, "{" + statement.strip(),
                                  label or f"disable function: {signature}")

    def remove_typedef_enum(self, enum_name: str) -> "CppPatcher":
        pattern = rf"typedef\s+enum\s+{re.escape(enum_name)}\s*\{{.*?\}}\s*{re.escape(enum_name)}\s*;"
        return self.remove_regex(pattern, label=f"remove typedef enum: {enum_name}")

    # ------------------------------------------------------------------
    # Source trimming / cleanup
    # ------------------------------------------------------------------

    def keep_from_literal(self, marker: str) -> "CppPatcher":
        idx = self.content.find(marker)
        if idx == -1:
            self._raise_no_change(f"keep from literal: {marker!r}")
        if idx == 0:
            self._raise_no_change(f"keep from literal made no change: {marker!r}")
        self.content = self.content[idx:]
        return self

    def keep_until_literal(self, marker: str) -> "CppPatcher":
        idx = self.content.find(marker)
        if idx == -1:
            self._raise_no_change(f"keep until literal: {marker!r}")
        if idx == len(self.content):
            self._raise_no_change(f"keep until literal made no change: {marker!r}")
        self.content = self.content[:idx]
        return self

    def assert_startswith(self, prefix: str, *, label: str | None = None) -> "CppPatcher":
        if not self.content.startswith(prefix):
            raise PatchError(f"Assertion failed in {self.source_name}: {label or f'must start with {prefix!r}'}")
        return self

    def assert_endswith(self, suffix: str, *, label: str | None = None) -> "CppPatcher":
        if not self.content.endswith(suffix):
            raise PatchError(f"Assertion failed in {self.source_name}: {label or f'must end with {suffix!r}'}")
        return self

    def strip(self) -> "CppPatcher":
        old = self.content
        self.content = self.content.strip()
        if self.content == old:
            self._raise_no_change("strip source")
        return self

    def strip_optional(self) -> "CppPatcher":
        self.content = self.content.strip()
        return self

    def remove_line_comments(self) -> "CppPatcher":
        return self.remove_regex(r"^\s+//\s.*?$", label="remove indented line comments")

    def collapse_blank_lines(self, *, required: bool = False) -> "CppPatcher":
        return self.replace_regex(r"\n+", "\n", label="collapse blank lines", required=required)

    def remove_whitespace_only_lines(self, *, required: bool = False) -> "CppPatcher":
        return self.replace_regex(r"[ \t]*\n", "\n", label="remove whitespace-only lines", required=required)

    # ------------------------------------------------------------------
    # Internal helpers
    # ------------------------------------------------------------------

    def _scope_label(self, label: str, in_function: str | None) -> str:
        if in_function is None:
            return label
        return f"{label} inside function: {in_function}"

    def _commit(self, new_content: str, n_changes: int, label: str, required: bool) -> "CppPatcher":
        if n_changes == 0 and required:
            self._raise_no_change(label)
        self.content = new_content
        return self

    def _commit_target(
            self,
            new_target: str,
            n_changes: int,
            label: str,
            required: bool,
            *,
            span: tuple[int, int] | None,
    ) -> "CppPatcher":
        if span is None:
            return self._commit(new_target, n_changes, label, required)

        if n_changes == 0 and required:
            self._raise_no_change(label)

        start, end = span
        self.content = self.content[:start] + new_target + self.content[end:]
        return self

    def _get_patch_target(
            self,
            in_function: str | None,
            *,
            in_function_regex: bool,
            label: str,
    ) -> tuple[str, tuple[int, int] | None]:
        if in_function is None:
            return self.content, None

        start, end = self._find_single_cpp_body_after_signature(
            in_function,
            regex=in_function_regex,
            label=f"{label} inside function: {in_function}",
        )
        return self.content[start:end], (start, end)

    def _find_single_cpp_body_after_signature(
            self,
            signature: str,
            *,
            regex: bool = False,
            label: str | None = None,
    ) -> tuple[int, int]:
        block_start, block_end = self._find_single_cpp_block_after_signature(signature, regex=regex, label=label)
        open_brace = self.content.find("{", block_start, block_end)
        if open_brace == -1:
            self._raise_no_change(label or f"find function body: {signature}")

        close_brace = self._find_matching_brace(open_brace)
        return open_brace + 1, close_brace

    def _replace_span(self, start: int, end: int, replacement: str, label: str) -> "CppPatcher":
        if start < 0 or end <= start:
            self._raise_no_change(label)
        before = self.content
        self.content = before[:start] + replacement + before[end:]
        if self.content == before:
            self._raise_no_change(label)
        return self

    def _raise_no_change(self, label: str) -> None:
        raise PatchError(
            f"Patch did not change {self.source_name}: {label}\n\n"
            "Manually assess the changed upstream source code."
        )

    def _find_single_cpp_block_after_signature(self, signature: str, *, regex: bool, label: str | None) -> tuple[
        int, int]:
        matches = self._find_cpp_blocks_after_signature(signature, regex=regex, label=label)

        if len(matches) > 1:
            raise PatchError(
                f"Patch matched multiple function definitions in {self.source_name}: "
                f"{label or signature}\n\n"
                "Provide a more specific full method signature so only one block is changed."
            )

        return matches[0]

    def _find_cpp_blocks_after_signature(self, signature: str, *, regex: bool, label: str | None) -> list[
        tuple[int, int]]:
        signature_pattern = signature if regex else self._cpp_signature_to_flexible_regex(signature)

        saw_signature = False
        blocks: list[tuple[int, int]] = []

        for match in re.finditer(signature_pattern, self.content, DEFAULT_RE_FLAGS):
            saw_signature = True
            open_brace = self.content.find("{", match.end())
            if open_brace == -1:
                continue

            # Skip forward declarations and function-pointer declarations that
            # happen to contain the same signature text.
            semicolon = self.content.find(";", match.end(), open_brace)
            if semicolon != -1:
                continue

            close_brace = self._find_matching_brace(open_brace)
            end = close_brace + 1

            # Remove one following newline for cleaner deletes/replacements.
            if end < len(self.content) and self.content[end] == "\r":
                end += 1
            if end < len(self.content) and self.content[end] == "\n":
                end += 1

            blocks.append((match.start(), end))

        if blocks:
            return blocks

        if saw_signature:
            self._raise_no_change(label or f"found signature but not a function definition: {signature}")
        self._raise_no_change(label or f"find function signature: {signature}")

    def _cpp_signature_to_flexible_regex(self, signature: str) -> str:
        """Build a literal C++ signature regex with flexible whitespace.

        This intentionally does not expose regex syntax to callers. It lets a
        patch use a copied signature such as:

            Foo Bar(
                Baz baz,
                std::function<bool(Baz)> callback)

        and still match the same signature when upstream formats it on one line.
        """
        signature = signature.strip()
        signature = re.sub(r"[;{]\s*$", "", signature).strip()

        if not signature:
            raise ValueError("Function signature cannot be empty.")

        token_pattern = r"::|->|\.\*|[A-Za-z_]\w*|\d+|[^\s]"
        tokens = re.findall(token_pattern, signature)
        if not tokens:
            raise ValueError(f"Could not tokenize C++ signature: {signature!r}")

        return r"\s*".join(re.escape(token) for token in tokens)

    def _trim_block_replacement(self, text: str, *, trailing_newline: bool) -> str:
        text = text.strip()
        if trailing_newline and text:
            return text + "\n"
        return text

    def _find_matching_brace(self, open_brace: int) -> int:
        if self.content[open_brace] != "{":
            raise ValueError("open_brace must point to '{'")

        depth = 0
        i = open_brace
        n = len(self.content)
        state = "code"
        raw_end = ""

        while i < n:
            ch = self.content[i]
            nxt = self.content[i + 1] if i + 1 < n else ""

            if state == "code":
                if ch == "/" and nxt == "/":
                    state = "line_comment"
                    i += 2
                    continue
                if ch == "/" and nxt == "*":
                    state = "block_comment"
                    i += 2
                    continue
                if ch == 'R' and nxt == '"':
                    raw_end = self._read_raw_string_end(i)
                    if raw_end:
                        state = "raw_string"
                        i += 2
                        continue
                if ch == '"':
                    state = "string"
                    i += 1
                    continue
                if ch == "'":
                    state = "char"
                    i += 1
                    continue
                if ch == "{":
                    depth += 1
                elif ch == "}":
                    depth -= 1
                    if depth == 0:
                        return i

            elif state == "line_comment":
                if ch == "\n":
                    state = "code"

            elif state == "block_comment":
                if ch == "*" and nxt == "/":
                    state = "code"
                    i += 2
                    continue

            elif state == "string":
                if ch == "\\":
                    i += 2
                    continue
                if ch == '"':
                    state = "code"

            elif state == "char":
                if ch == "\\":
                    i += 2
                    continue
                if ch == "'":
                    state = "code"

            elif state == "raw_string":
                if raw_end and self.content.startswith(raw_end, i):
                    i += len(raw_end)
                    state = "code"
                    raw_end = ""
                    continue

            i += 1

        raise PatchError(f"Could not find matching closing brace in {self.source_name}")

    def _read_raw_string_end(self, start: int) -> str:
        # C++ raw string: R"delimiter(... )delimiter"
        open_paren = self.content.find("(", start + 2, start + 40)
        if open_paren == -1:
            return ""
        delimiter = self.content[start + 2:open_paren]
        if " " in delimiter or "\t" in delimiter or "\n" in delimiter:
            return ""
        return ")" + delimiter + '"'


def read_file(path: str | os.PathLike[str]) -> str:
    return Path(path).read_text(encoding="utf-8")


def generate_slash_block(name: str) -> str:
    art = text2art(name, font="starwars").rstrip("\n")
    art_lines = [f"////  {line}  ////" for line in art.splitlines()]
    width = max(len(line) for line in art_lines) + 8
    line = "/" * width
    return "\n".join([line, line, line, *art_lines, line, line, line]) + "\n"


class URLProcessor(ABC):
    def __init__(self, url: str, name: str, injection_order: str):
        self.url = url
        self.name = name
        self.injection_order = injection_order

        if os.path.exists("main.py"):
            self.output_folder = Path("modified-dependencies")
        else:
            self.output_folder = Path("dependencies") / "modified-dependencies"

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
            (compiler_dump_dir / filename.replace(".cpp", ".txt")).write_text(match.group(1).strip(), encoding="utf-8")

        patch = CppPatcher(content, source_name=filename)
        content = (
            patch.strip()
            .remove_regex(r"//\s+==WindhawkMod==.*?WindhawkModSettings==\s+(?=#include)",
                          label="remove Windhawk metadata block")
            .replace_literal("g_settings", f"g_settings_{self.name.lower()}", label="namespace g_settings")
            .remove_literal('Wh_Log(L">");', label="remove placeholder Wh_Log")
            .replace_literal("LoadSettings", f"LoadSettings{self.name}", label="namespace LoadSettings")
            .replace_literal("ApplySettings(", f"ApplySettings{self.name}(", label="namespace ApplySettings")
            .replace_literal("HookTaskbarDllSymbols", f"HookTaskbarDllSymbols{self.name}",
                             label="namespace HookTaskbarDllSymbols")
            .replace_literal("Wh_ModInit", f"Wh_ModInit{self.name}", label="namespace Wh_ModInit")
            .replace_literal("Wh_ModAfterInit", f"Wh_ModAfterInit{self.name}", label="namespace Wh_ModAfterInit")
            .replace_literal("Wh_ModBeforeUninit", f"Wh_ModBeforeUninit{self.name}",
                             label="namespace Wh_ModBeforeUninit")
            .replace_literal("Wh_ModUninit", f"Wh_ModUninit{self.name}", label="namespace Wh_ModUninit")
            .replace_literal("Wh_ModSettingsChanged", f"Wh_ModSettingsChanged{self.name}",
                             label="namespace Wh_ModSettingsChanged")
            .replace_literal("g_taskbarViewDllLoaded", f"g_taskbarViewDllLoaded{self.name}",
                             label="namespace g_taskbarViewDllLoaded")
            .text()
        )

        content = self.format_content(content)

        patch = CppPatcher(content, source_name=filename)
        content = (
            patch.remove_line_comments()
            .collapse_blank_lines(required=True)
            .prepend(generate_slash_block(self.name))
            .remove_whitespace_only_lines(required=False)
            .collapse_blank_lines(required=False)
            .strip_optional()
            .text()
        )

        self.save(content, filename)


class TaskbarIconSizeMod(URLProcessor):
    def __init__(self):
        url = "https://raw.githubusercontent.com/ramensoftware/windhawk-mods/refs/heads/main/mods/taskbar-icon-size.wh.cpp"
        super().__init__(url, "TBIconSize", "a")

    def format_content(self, content: str) -> str:
        patch = CppPatcher(content, source_name=self.name)

        injected_dpi_prefix = f"""
        {read_file(os.path.join(mod_parts_dir, "taskbar-states.cpp"))}
        {read_file(os.path.join(mod_parts_dir, "g_settings.cpp"))}
        {read_file(os.path.join(mod_parts_dir, "top-level-variables.cpp"))}

        std::wstring GetMonitorName(HMONITOR monitor) {{
            MONITORINFOEX monitorInfo = {{}};
            monitorInfo.cbSize = sizeof(MONITORINFOEX);
            if (monitor && GetMonitorInfo(monitor, &monitorInfo)) {{
                return std::wstring(monitorInfo.szDevice);
            }}
            return L"default";
        }}
        std::wstring GetMonitorName(HWND hwnd) {{
            HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
            return GetMonitorName(monitor);
        }}
        STDAPI GetDpiForMonitor"""

        return (
            patch.replace_literal('Wh_GetIntSetting(L"IconSize")', 'Wh_GetIntSetting(L"TaskbarIconSize")')
            .insert_after_regex(
                r"ExperienceToggleButton_UpdateButtonPadding_t[\s\n]+ExperienceToggleButton_UpdateButtonPadding_Original;", """double GetEffectiveTaskbarButtonTargetWidth();
bool EnsureElementTaskbarButtonWidth(FrameworkElement const& element,
                                     double targetWidth,
                                     bool allowHardWidth);""")
            .replace_function_body("void WINAPI ExperienceToggleButton_UpdateButtonPadding_Hook(void* pThis)", """    ExperienceToggleButton_UpdateButtonPadding_Original(pThis);
    if (g_hasDynamicIconScaling && g_unloading) {
        return;
    }
    FrameworkElement toggleButtonElement = nullptr;
    ((IUnknown**)pThis)[1]->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(toggleButtonElement));
    if (!toggleButtonElement) {
        return;
    }
    auto panelElement =
        FindChildByName(toggleButtonElement, L"ExperienceToggleButtonRootPanel")
            .try_as<Controls::Grid>();
    if (!panelElement) {
        return;
    }
    auto className = winrt::get_class_name(toggleButtonElement);
    if (className != L"Taskbar.ExperienceToggleButton" &&
        className != L"Taskbar.SearchBoxButton") {
        return;
    }
    if (className == L"Taskbar.SearchBoxButton" && panelElement.Margin() != Thickness{}) {
        return;
    }

    const double targetWidth = GetEffectiveTaskbarButtonTargetWidth();
    const bool allowHardWidth =
        className != L"Taskbar.SearchBoxButton" ||
        !FindChildByName(panelElement, L"SearchBoxTextBlock");
    bool changed = EnsureElementTaskbarButtonWidth(toggleButtonElement, targetWidth, allowHardWidth);
    changed = EnsureElementTaskbarButtonWidth(panelElement, targetWidth, allowHardWidth) || changed;
    if (changed) {
        Wh_Log(L"Updating taskbar button width for %s to %f", className.c_str(), targetWidth);
        panelElement.UpdateLayout();
    }""").replace_function_body("void WINAPI SearchButtonBase_UpdateButtonPadding_Hook(void* pThis)", """SearchButtonBase_UpdateButtonPadding_Original(pThis);
    if (g_hasDynamicIconScaling && g_unloading) {
        return;
    }
    FrameworkElement toggleButtonElement = nullptr;
    ((IUnknown**)pThis)[1]->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(toggleButtonElement));
    if (!toggleButtonElement) {
        return;
    }
    auto panelElement =
        FindChildByName(toggleButtonElement, L"SearchBoxButtonRootPanel")
            .try_as<Controls::Grid>();
    if (!panelElement) {
        return;
    }
    // Expanded search boxes are allowed to be wider. Icon-only search buttons
    // can safely use the same measured target as the other taskbar buttons.
    if (FindChildByName(panelElement, L"SearchBoxTextBlock")) {
        return;
    }
    const double targetWidth = GetEffectiveTaskbarButtonTargetWidth();
    bool changed = EnsureElementTaskbarButtonWidth(toggleButtonElement, targetWidth, true);
    changed = EnsureElementTaskbarButtonWidth(panelElement, targetWidth, true) || changed;
    if (changed) {
        Wh_Log(L"Updating search button width to %f", targetWidth);
        panelElement.UpdateLayout();
    }""").replace_regex(
                r'''
    for\s*\(\s*int\s+i\s*=\s*0\s*;\s*
    i\s*<\s*100\s*;\s*
    i\+\+\s*\)\s*
    \{\s*
    if\s*\(\s*!\s*g_pendingMeasureOverride\s*\)\s*
    \{\s*
    break\s*;\s*
    \}\s*
    Sleep\s*\(\s*100\s*\)\s*;\s*
    \}
    ''', """WaitForConditionWithTimeout(
            [] { return !g_pendingMeasureOverride.load(); },
            kTaskbarMeasureOverrideTimeoutMs,
            kTaskbarMeasurePollIntervalMs);""", flags=re.VERBOSE)
            .replace_literal("ApplySettingsTBIconSize(g_originalTaskbarHeight ? g_originalTaskbarHeight : 48);",
                             "ApplySettingsTBIconSize(g_originalTaskbarHeight ? g_originalTaskbarHeight : kSystemMediumTaskbarButtonSize);",
                             in_function="void Wh_ModBeforeUninitTBIconSize()")
            .replace_regex(r'''
    while\s*\(\s*
    g_hookCallCounter\s*>\s*0
    \s*\)\s*
    \{\s*
    Sleep\s*\(\s*100\s*\)\s*;
    \s*\}
    ''', """if (!WaitForConditionWithTimeout(
            [] { return g_hookCallCounter.load() <= 0; },
            kHookDrainTimeoutMs,
            kHookDrainPollIntervalMs)) {
        Wh_Log(L"Timed out waiting for taskbar icon size hooks to drain");
    }""", in_function="void Wh_ModUninitTBIconSize()",
                           flags=re.VERBOSE)

            .insert_after_literal("bool g_taskbarButtonWidthCustomized;", """\n
constexpr int kDefaultTaskbarHeight = 74;
constexpr int kDefaultTaskbarIconSize = 42;
constexpr int kDefaultTaskbarButtonSize = 74;
constexpr int kDefaultTaskbarOffsetY = 6;
constexpr int kDefaultTrayIconSize = 15;
constexpr int kDefaultTrayButtonSize = 30;
constexpr int kSystemSmallTaskbarIconSize = 16;
constexpr int kSystemMediumTaskbarIconSize = 24;
constexpr int kSystemSmallTaskbarButtonSize = 32;
constexpr int kSystemMediumTaskbarButtonSize = 44;
constexpr int kMinTaskbarHeight = kSystemMediumTaskbarButtonSize;
constexpr int kMaxTaskbarHeight = 200;
constexpr int kMinTaskbarButtonSize = kSystemMediumTaskbarButtonSize;
constexpr int kMaxTaskbarButtonSize = 300;
constexpr int kMinTaskbarIconSize = 8;
constexpr int kMaxTaskbarIconSize = 300;
constexpr int kMinTrayIconSize = 15;
constexpr int kMinTrayButtonSize = 20;
constexpr double kLayoutToleranceDip = 0.5;
constexpr int kWorkerShutdownPollMs = 10;
constexpr int kDelayedApplyWorkerShutdownTimeoutMs = 5000;
constexpr int kAnimationFollowupWorkerShutdownTimeoutMs = 2000;
constexpr int kTaskbarMeasurePollIntervalMs = 100;
constexpr int kTaskbarMeasureOverrideTimeoutMs = 10000;
constexpr int kHookDrainPollIntervalMs = 100;
constexpr int kHookDrainTimeoutMs = 10000;
bool WaitForConditionWithTimeout(std::function<bool()> condition,
                                 int timeoutMs,
                                 int pollIntervalMs);
FrameworkElement TryQueryFrameworkElement(IUnknown* unknown,
                                          PCWSTR context = L"FrameworkElement") {
  if (!unknown) {
    return nullptr;
  }
  FrameworkElement element{nullptr};
  try {
    HRESULT hr = unknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                         winrt::put_abi(element));
    if (FAILED(hr) || !element) {
      if (context) {
        Wh_Log(L"%s QueryInterface failed: %08X", context, hr);
      }
      return nullptr;
    }
  } catch (winrt::hresult_error const& ex) {
    if (context) {
      Wh_Log(L"%s QueryInterface threw %08X: %s", context, ex.code(), ex.message().c_str());
    }
    return nullptr;
  } catch (...) {
    if (context) {
      Wh_Log(L"%s QueryInterface threw: %08X", context, winrt::to_hresult());
    }
    return nullptr;
  }
  return element;
}
int ClampInt(int value, int minValue, int maxValue) {
  return value < minValue ? minValue : (value > maxValue ? maxValue : value);
}
int ReadPositiveIntSettingOrDefault(const wchar_t* key, int defaultValue) {
  int value = Wh_GetIntSetting(key);
  return value > 0 ? value : defaultValue;
}
int GetMaxTaskbarIconSizeForLayout(int taskbarHeight, int taskbarButtonSize) {
  int maxIconSize = std::min(kMaxTaskbarIconSize, std::min(taskbarHeight, taskbarButtonSize));
  return std::max(kMinTaskbarIconSize, maxIconSize);
}\n""")
            .replace_regex(re.escape("""    auto ret =
        ResourceDictionary_Lookup_TaskbarView_Original(pThis, result, key);
    if (!*ret) {
        return ret;
    }"""), """    auto ret = ResourceDictionary_Lookup_TaskbarView_Original
        ? ResourceDictionary_Lookup_TaskbarView_Original(pThis, result, key)
        : nullptr;
    if (!ret || !*ret) {
        return ret;
    }""", in_function="""ResourceDictionary_Lookup_TaskbarView_Hook(
    void* pThis,
    void** result,
    winrt::Windows::Foundation::IInspectable* key)""")
            .replace_regex(re.escape("""    auto ret =
        ResourceDictionary_Lookup_SearchUxUi_Original(pThis, result, key);
    if (!*ret) {
        return ret;
    }"""), """    auto ret = ResourceDictionary_Lookup_SearchUxUi_Original
        ? ResourceDictionary_Lookup_SearchUxUi_Original(pThis, result, key)
        : nullptr;
    if (!ret || !*ret) {
        return ret;
    }""", in_function="""ResourceDictionary_Lookup_SearchUxUi_Hook(
    void* pThis,
    void** result,
    winrt::Windows::Foundation::IInspectable* key)""")

            .replace_regex(re.escape("""    HMODULE module =
        LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }"""), """    bool loadedTaskbarDllForHooking = false;
    HMODULE module = GetModuleHandle(L"taskbar.dll");
    if (!module) {
        module = LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        loadedTaskbarDllForHooking = module != nullptr;
    }
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }""", in_function="bool HookTaskbarDllSymbolsTBIconSize()")

            .replace_regex(re.escape("""    if (!HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }"""), """if (!HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"HookSymbols failed");
        if (loadedTaskbarDllForHooking) {
            FreeLibrary(module);
        }
        return false;
    }""", in_function="bool HookTaskbarDllSymbolsTBIconSize()")
            .replace_regex(r"if \(g_unloading\)", "if (g_unloading || !key || !value)",
                           in_function="void OverrideResourceDirectoryLookup")
            .insert_after_literal("g_inSystemTrayController_UpdateFrameSize = false;",
                                  "\nApplySettingsFromTaskbarThreadGeometryChanged();",
                                  in_function="void WINAPI SystemTrayController_UpdateFrameSize_Hook(void* pThis)")
            .replace_literal('Wh_GetIntSetting(L"TaskbarButtonWidth")', 'Wh_GetIntSetting(L"TaskbarButtonSize")')
            .replace_literal("STDAPI GetDpiForMonitor", injected_dpi_prefix, count=1,
                             label="insert shared taskbar helpers before GetDpiForMonitor")
            .replace_literal("ApplySettingsTBIconSize(g_settings_tbiconsize.taskbarHeight);",
                             'Wh_Log(L"Deferring taskbar icon size settings until delayed initial apply");',
                             in_function="void Wh_ModAfterInitTBIconSize()")
            .insert_before_literal("using SendMessageTimeoutW_t = decltype(&SendMessageTimeoutW);","static bool TryCorrectShellHookMinRectMessageTai(UINT Msg, WPARAM wParam, LPARAM lParam);")
            .insert_after_literal("""LRESULT ret = SendMessageTimeoutW_Original(hWnd, Msg, wParam, lParam,
                                               fuFlags, uTimeout, lpdwResult);""","\nTryCorrectShellHookMinRectMessageTai(Msg, wParam, lParam);",in_function="""LRESULT WINAPI SendMessageTimeoutW_Hook(HWND hWnd,
                                        UINT Msg,
                                        WPARAM wParam,
                                        LPARAM lParam,
                                        UINT fuFlags,
                                        UINT uTimeout,
                                        PDWORD_PTR lpdwResult)""", )
            .insert_before_literal(
                "#include <winrt/Windows.Foundation.h>\n#include <winrt/Windows.UI.Xaml.Automation.h>",
                "#include <initguid.h>\n", )
            .insert_after_literal("TaskbarController_OnGroupingModeChanged_InitOffsets();", """WindhawkUtils::Wh_SetFunctionHookT(
            reinterpret_cast<TaskbarController_OnGroupingModeChanged_t>(
                TaskbarController_OnGroupingModeChanged_Original),
            TaskbarController_OnGroupingModeChanged_Hook,
            &TaskbarController_OnGroupingModeChanged_Hook_Original);""",
                                  in_function="bool HookTaskbarViewDllSymbols(HMODULE module, bool hookSystemTraySymbolsInline)")
            .insert_after_literal("g_inSystemTrayController_UpdateFrameSize = false;",
                                  "ApplySettingsFromTaskbarThreadGeometryChanged();",
                                  in_function="void WINAPI SystemTraySecondaryController_UpdateFrameSize_Hook(void* pThis)")
            .insert_before_regex(
                re.escape(r"using TaskbarController_UpdateFrameHeight_t = void(WINAPI*)(void* pThis);"), """using TaskbarController_OnGroupingModeChanged_t = void(WINAPI*)(void* pThis);
TaskbarController_OnGroupingModeChanged_t
    TaskbarController_OnGroupingModeChanged_Hook_Original;
void WINAPI TaskbarController_OnGroupingModeChanged_Hook(void* pThis) {
    Wh_Log(L"TaskbarController::OnGroupingModeChanged Hook");
    TaskbarController_OnGroupingModeChanged_Hook_Original(pThis);
    ApplySettingsFromTaskbarThreadGeometryChanged();
}\n""")
            .replace_literal(
                ' = Wh_GetIntSetting(L"TaskbarHeight");',
                ' = Wh_GetIntSetting(L"TaskbarHeight") + ((Wh_GetIntSetting(L"FlatTaskbarBottomCorners") || Wh_GetIntSetting(L"FullWidthTaskbarBackground"))?0:(abs(Wh_GetIntSetting(L"TaskbarOffsetY"))*2));',
                count=1,
                label="expand taskbar height setting",
            )
            .replace_literal(
                "return g_settings_tbiconsize.iconSize;",
                "return g_settings_tbiconsize.iconSize ;",
                count=1,
                label="force icon-size return patch",
            )
            .replace_function(
                "void LoadSettingsTBIconSize()",
                r'''
void LoadSettingsTBIconSize() {
  const int requestedHeight = ReadPositiveIntSettingOrDefault(L"TaskbarHeight", kDefaultTaskbarHeight);
  int taskbarHeight = ClampInt(abs(requestedHeight), kMinTaskbarHeight, kMaxTaskbarHeight);

  const int requestedOffsetY = ReadPositiveIntSettingOrDefault(L"TaskbarOffsetY", kDefaultTaskbarOffsetY);
  const int taskbarOffsetY = abs(requestedOffsetY);
  const int heightExpansion =
      ((Wh_GetIntSetting(L"FlatTaskbarBottomCorners") ||
        Wh_GetIntSetting(L"FullWidthTaskbarBackground"))
           ? 0
           : (taskbarOffsetY * 2));
  g_settings_tbiconsize.taskbarHeight =
      ClampInt(taskbarHeight + heightExpansion, kMinTaskbarHeight, kMaxTaskbarHeight + (kDefaultTaskbarOffsetY * 2));

  const int requestedButtonSize = ReadPositiveIntSettingOrDefault(L"TaskbarButtonSize", kDefaultTaskbarButtonSize);
  const int taskbarButtonSize = ClampInt(abs(requestedButtonSize), kMinTaskbarButtonSize, kMaxTaskbarButtonSize);
  g_settings_tbiconsize.taskbarButtonWidth = taskbarButtonSize;
  g_settings_tbiconsize.taskbarButtonWidthSmall = taskbarButtonSize;

  const int requestedIconSize = ReadPositiveIntSettingOrDefault(L"TaskbarIconSize", kDefaultTaskbarIconSize);
  const int maxIconSize = GetMaxTaskbarIconSizeForLayout(g_settings_tbiconsize.taskbarHeight, taskbarButtonSize);
  g_settings_tbiconsize.iconSize = ClampInt(abs(requestedIconSize), kMinTaskbarIconSize, maxIconSize);
  g_settings_tbiconsize.iconSizeSmall = std::min(g_settings_tbiconsize.iconSize, kSystemSmallTaskbarIconSize);
}
                ''',
                label="replace LoadSettingsTBIconSize",
            )
            .insert_before_literal("LoadSettingsTBIconSize();", """\nconst int oldTaskbarButtonWidth = g_settings_tbiconsize.taskbarButtonWidth;
    const int oldSmallTaskbarButtonWidth = g_settings_tbiconsize.taskbarButtonWidthSmall;\n""",
                                   in_function="void Wh_ModSettingsChangedTBIconSize()")
            .insert_after_literal("LoadSettingsTBIconSize();", """\nif (!g_unloading &&
        ((oldTaskbarButtonWidth > 0 && oldTaskbarButtonWidth != g_settings_tbiconsize.taskbarButtonWidth) ||
         (oldSmallTaskbarButtonWidth > 0 && oldSmallTaskbarButtonWidth != g_settings_tbiconsize.taskbarButtonWidthSmall))) {
        RequestTaskbarButtonSizeRelayout();
    }\n""",
                                  in_function="void Wh_ModSettingsChangedTBIconSize()")

            .text()
        )


class StartButtonPosition(URLProcessor):
    def __init__(self):
        url = "https://raw.githubusercontent.com/ramensoftware/windhawk-mods/refs/heads/main/mods/taskbar-start-button-position.wh.cpp"
        super().__init__(url, "StartButtonPosition", "b")

    def format_content(self, content: str) -> str:
        patch = CppPatcher(content, source_name=self.name)

        self._remove_unused_upstream_code(patch)
        self._rename_and_disable_upstream_hooks(patch)
        self._patch_start_menu_logic(patch)
        self._insert_custom_hooks(patch)
        self._patch_dwm_targeting(patch)
        self._patch_reload_and_settings(patch)

        return (patch.prepend(
            "bool ApplyStyle(FrameworkElement const& element, std::wstring monitorName);\n"
            "bool InitializeDebounce();\n"
            "DispatcherTimer debounceTimer{nullptr};\n\n"
        ).replace_regex(re.escape(""" RunFromWindowThread(
        hTaskbarWnd, [](void* pParam) { ApplySettingsFromTaskbarThread(); }, 0);"""), """   if (hTaskbarWnd && IsWindow(hTaskbarWnd)) {
        RunFromWindowThread(
            hTaskbarWnd, [](void* pParam) { ApplySettingsFromTaskbarThread(); }, 0);
    }""", in_function="void ApplySettingsStartButtonPosition(HWND hTaskbarWnd)")
                .replace_regex(re.escape("""    HMODULE module =
        LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }"""),"""    bool loadedTaskbarDllForHooking = false;
    HMODULE module = GetModuleHandle(L"taskbar.dll");
    if (!module) {
        module = LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        loadedTaskbarDllForHooking = module != nullptr;
    }
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }""", in_function="bool HookTaskbarDllSymbolsStartButtonPosition()")
        .replace_regex(re.escape("""return HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));"""),""" if (!HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"HookSymbols failed");
        if (loadedTaskbarDllForHooking) {
            FreeLibrary(module);
        }
        return false;
    }
    return true;""",in_function="bool HookTaskbarDllSymbolsStartButtonPosition()")
                .text())

    def _remove_unused_upstream_code(self, patch: CppPatcher) -> None:
        patch.remove_function("FrameworkElement EnumChildElements(")
        patch.remove_function("FrameworkElement FindChildByName(")
        patch.remove_function("FrameworkElement FindChildByClassName(")
        patch.remove_function("HWND FindCurrentProcessTaskbarWnd(")
        patch.remove_function("HMODULE GetTaskbarViewModuleHandle(")
        patch.remove_literal("LoadLibraryExW_t LoadLibraryExW_Original;")
        patch.remove_regex(
            r"ExperienceToggleButton_UpdateButtonPadding_t[\s\w\d]*?ExperienceToggleButton_UpdateButtonPadding_Original;",
            label="remove ExperienceToggleButton original pointer",
        )
        patch.remove_function("void WINAPI ExperienceToggleButton_UpdateButtonPadding_Hook(")
        patch.remove_regex(
            r"AugmentedEntryPointButton_UpdateButtonPadding_t[\s\w\d]*?AugmentedEntryPointButton_UpdateButtonPadding_Original;",
            label="remove AugmentedEntryPointButton original pointer",
        )
        patch.remove_literal("std::atomic<bool> g_unloading;")
        patch.remove_literal("void ApplyStyle();")
        patch.remove_typedef_enum("MONITOR_DPI_TYPE")

    def _rename_and_disable_upstream_hooks(self, patch: CppPatcher) -> None:
        patch.replace_literal(
            "BOOL Wh_ModSettingsChangedStartButtonPosition(BOOL* bReload)",
            "BOOL Wh_ModSettingsChangedStartButtonPosition()",
            count=1,
        )
        patch.replace_literal(
            "AugmentedEntryPointButton_UpdateButtonPadding_Hook",
            f"AugmentedEntryPointButton_UpdateButtonPadding_Hook_{self.name}",
        )
        patch.replace_literal("HookTaskbarViewDllSymbols", f"HookTaskbarViewDllSymbols{self.name}")
        patch.replace_literal("LoadLibraryExW_Hook", f"LoadLibraryExW_Hook_{self.name}")
        patch.replace_literal(
            "} g_settings_startbuttonposition;",
            "    bool MoveFlyoutNotificationCenter=true;\n} g_settings_startbuttonposition;",
            count=1,
            label="add MoveFlyoutNotificationCenter setting",
        )
        patch.replace_literal("UIElement_Arrange_Hook", f"IUIElement_Arrange_Hook_{self.name}")

    def _patch_start_menu_logic(self, patch: CppPatcher) -> None:
        patch.replace_literal(
            "std::wstring processFileName = GetProcessFileName(processId);",
            "TCHAR className[256];\n"
            "    GetClassName(hwnd, className, 256);\n"
            "    std::wstring windowClassName(className);\n"
            "    std::wstring processFileName = GetProcessFileName(processId);\n"
            "    Wh_Log(L\"process: %s, windowClassName: %s\",processFileName.c_str(),windowClassName.c_str());",
            count=1,
            label="log process and window class",
        )

        # This is where the original mod moves the start button. Keep the function,
        # but disable that original behavior.
        patch.disable_function_at_start(
            "bool ApplyStyle(XamlRoot xamlRoot)",
            "if(true)return false;",
            label="disable upstream ApplyStyle(XamlRoot)",
        )

        patch.replace_literal(
            '''if (contentClassName == L"StartMenu.StartBlendedFlexFrame") {
        ApplyStyleRedesignedStartMenu(content);
    }''',
            '''if (contentClassName == L"StartMenu.StartBlendedFlexFrame") {
        ApplyStyleClassicStartMenu(content, monitor);
    }''',
            count=1,
            label="route redesigned start menu to classic position handler",
        )

        patch.replace_literal(
            r"auto original = [=] { return IUIElement_Arrange_Original(pThis, rect); };",
            "auto original = [=] { return IUIElement_Arrange_Original(pThis, rect); };\nif(true)return original();",
            count=1,
            label="disable upstream arrange hook body",
        )

        patch.replace_function(
            "void ApplyStyleClassicStartMenu(FrameworkElement content, HMONITOR monitor)",
            '''
void ApplyStyleClassicStartMenu(FrameworkElement content, HMONITOR monitor){
         if(true){ 
         ApplyStyle(content,GetMonitorName(monitor));
         return;
         }
         }
            ''',
            label="replace ApplyStyleClassicStartMenu",
        )

        patch.replace_literal(
            "frameRoot.HorizontalAlignment(HorizontalAlignment::Left);",
            "frameRoot.HorizontalAlignment(HorizontalAlignment::Center);",
            count=1,
            label="center classic start menu frame root",
        )

    def _insert_custom_hooks(self, patch: CppPatcher) -> None:
        patch.insert_after_literal(
            "WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {",
            read_file(os.path.join(hooks_dir, "taskbar.dll_sigs.cpp")),
            label="insert taskbar.dll signatures",
        )
        patch.insert_after_literal(
            "WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {",
            read_file(os.path.join(hooks_dir, "Taskbar.View.dll_sigs.cpp")),
            label="insert Taskbar.View.dll signatures",
        )
        patch.insert_before_literal(
            "bool HookTaskbarDllSymbolsStartButtonPosition() {",
            read_file(os.path.join(hooks_dir, "taskbar.dll_methods.cpp")) + "\n",
            label="insert taskbar.dll hook methods",
        )
        patch.insert_before_literal(
            "bool HookTaskbarViewDllSymbolsStartButtonPosition(HMODULE module) {",
            read_file(os.path.join(hooks_dir, "Taskbar.View.dll_methods.cpp")) + "\n",
            label="insert Taskbar.View.dll hook methods",
        )

    def _patch_dwm_targeting(self, patch: CppPatcher) -> None:
        patch.insert_after_literal(
            "enum class DwmTarget {",
            "\n        ShellExperienceHost,",
            label="add ShellExperienceHost DwmTarget",
        )
        patch.replace_regex(
            r"target = DwmTarget::SearchHost;\s+}",
            '''target = DwmTarget::SearchHost;
    }else if (_wcsicmp(processFileName.c_str(), L"ShellExperienceHost.exe") == 0) {
        target = DwmTarget::ShellExperienceHost;
    } ''',
            count=1,
            label="target ShellExperienceHost",
        )
        patch.replace_literal(
            "GetMonitorInfo(monitor, &monitorInfo);",
            '''GetMonitorInfo(monitor, &monitorInfo);
    auto monitorName = GetMonitorName(monitor);
    auto iterationTbStates = g_taskbarStates.find(monitorName);
    if (iterationTbStates == g_taskbarStates.end()) {
      return original();
    }
    TaskbarState& taskbarState = iterationTbStates->second;
''',
            count=1,
            label="load taskbar state for monitor",
        )
        patch.replace_literal(
            '''    if (_wcsicmp(processFileName.c_str(), L"SearchHost.exe") == 0) {
        target = DwmTarget::SearchHost;
    }else if (_wcsicmp(processFileName.c_str(), L"ShellExperienceHost.exe") == 0) {
        target = DwmTarget::ShellExperienceHost;
    }  else {
        return original();
    }''',
            '''    if (_wcsicmp(processFileName.c_str(), L"StartMenuExperienceHost.exe") == 0) {
        target = DwmTarget::StartMenu;
    } else if (_wcsicmp(processFileName.c_str(), L"SearchHost.exe") == 0) {
        target = DwmTarget::SearchHost;
    }else if (_wcsicmp(processFileName.c_str(), L"ShellExperienceHost.exe") == 0) {
        target = DwmTarget::ShellExperienceHost;
    }  else {
        return original();
    }
    ''',
            count=1,
            label="add StartMenuExperienceHost target branch",
        )
        patch.insert_after_literal(
            "enum class DwmTarget {",
            "\n        StartMenu,",
            label="add StartMenu DwmTarget",
        )
        patch.replace_literal("HWND g_searchMenuWnd;", "HWND g_searchMenuWnd, g_startMenuWnd;", count=1)
        patch.replace_literal("int g_searchMenuOriginalX;", "int g_searchMenuOriginalX, g_startMenuOriginalWidth;",
                              count=1)
        patch.replace_regex(
            r"if \(target == DwmTarget::SearchHost\).*?\}\s+SetWindowPos",
            read_file(os.path.join(mod_parts_dir, "start-menu-position-code.cpp")) + "SetWindowPos",
            count=1,
            label="replace SetWindowPos target branch",
        )
        patch.remove_literal("margin.Right = 0;")
        patch.remove_literal("margin.Right = -width;")

    def _patch_reload_and_settings(self, patch: CppPatcher) -> None:
        patch.disable_function_at_start(
            "void Wh_ModUninitStartButtonPosition()",
            "if(true)return;",
            label="disable Wh_ModUninitStartButtonPosition",
        )
        patch.replace_regex(
            r"if \(!ApplyStyle\(xamlRoot\)\) \{.*?\}",
            r'''
  const auto xamlRootContent = xamlRoot.Content().try_as<FrameworkElement>();
  if (!xamlRootContent) {
  Wh_Log(L"XamlRoot content is null");
  return TRUE;
  }
  auto dispatcher = xamlRootContent.Dispatcher();
  if (!dispatcher) {
  Wh_Log(L"XamlRoot content dispatcher is null");
  return TRUE;
  }
  std::wstring monitorName = GetMonitorName(hWnd);
  auto applyOnDispatcher = [xamlRootContent, monitorName]() {
  if (!ApplyStyle(xamlRootContent, monitorName)) {
   Wh_Log(L"ApplyStyles failed");
  }
 };
            if (dispatcher.HasThreadAccess()) {
                applyOnDispatcher();
            } else if (!g_unloading){
                auto priority = winrt::Windows::UI::Core::CoreDispatcherPriority::Low;
                int highPriorityPasses = g_high_priority_dispatch_passes.load();
                while (highPriorityPasses > 0) {
                    if (g_high_priority_dispatch_passes.compare_exchange_weak(
                            highPriorityPasses, highPriorityPasses - 1)) {
                        priority = winrt::Windows::UI::Core::CoreDispatcherPriority::High;
                        break;
                    }
                }
                dispatcher.TryRunAsync(priority, applyOnDispatcher);
            }

  ''',
            count=1,
            label="replace ApplyStyle reload block",
        )
        patch.replace_literal(
            'Wh_GetIntSetting(L"startMenuOnTheLeft");',
            '''Wh_GetIntSetting(L"MoveFlyoutStartMenu");'''
            '''g_settings_startbuttonposition.MoveFlyoutNotificationCenter = Wh_GetIntSetting(L"MoveFlyoutNotificationCenter");''',
            count=1,
            label="load custom flyout settings",
        )


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
            .assert_endswith("////////////////////////////////////////////////////////////////////////////////",
                             label="Taskbar styler should end at separator")
            .text()
        )


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
    mod_parts_dir = r"..\mod-parts"
    hooks_dir = os.path.join(mod_parts_dir, "hooks")
    process_all_mods()
