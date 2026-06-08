from __future__ import annotations

import re
from typing import Callable, Match, Union

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
            raise ValueError(
                "Use count=-1 for unlimited literal replacements. count=0 is ambiguous for str.replace()."
            )

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
        return self._replace_span(
            open_brace,
            open_brace + 1,
            "{" + statement.strip(),
            label or f"disable function: {signature}",
        )

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

    def _find_single_cpp_block_after_signature(
        self,
        signature: str,
        *,
        regex: bool,
        label: str | None,
    ) -> tuple[int, int]:
        matches = self._find_cpp_blocks_after_signature(signature, regex=regex, label=label)

        if len(matches) > 1:
            raise PatchError(
                f"Patch matched multiple function definitions in {self.source_name}: "
                f"{label or signature}\n\n"
                "Provide a more specific full method signature so only one block is changed."
            )

        return matches[0]

    def _find_cpp_blocks_after_signature(
        self,
        signature: str,
        *,
        regex: bool,
        label: str | None,
    ) -> list[tuple[int, int]]:
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
                if ch == "R" and nxt == '"':
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
