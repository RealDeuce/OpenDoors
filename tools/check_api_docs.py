#!/usr/bin/env python3
# SPDX-License-Identifier: LGPL-2.0-or-later
"""Check that the high-level OpenDoors API has Markdown reference coverage."""

from __future__ import annotations

import re
import sys
from collections import defaultdict
from pathlib import Path


ROOT = Path(__file__).resolve().parent.parent
HEADER = ROOT / "OpenDoor.h"
API_DIR = ROOT / "docs" / "reference" / "api"
CONTROL_DIR = ROOT / "docs" / "reference" / "control"
START_MARKER = "OpenDoors API function prototypes."
END_MARKER = "Definitions for compatibility with previous versions."
CONTROL_MARKER = "The OpenDoors control structure"
DOCS_DIR = ROOT / "docs"
CONSTANTS_PAGE = DOCS_DIR / "reference" / "constants.md"
TYPES_PAGE = DOCS_DIR / "reference" / "types.md"
COMPATIBILITY_PAGE = DOCS_DIR / "reference" / "compatibility.md"
CONTROL_INDEX = CONTROL_DIR / "index.md"


def prose_code_spans(text: str):
    """Yield unlinked inline-code spans outside headings and fenced examples."""
    in_fence = False
    for line_number, line in enumerate(text.splitlines(), 1):
        if line.lstrip().startswith("```"):
            in_fence = not in_fence
            continue
        if in_fence or line.startswith("#"):
            continue

        linked_ranges = [
            (match.start(), match.end())
            for match in re.finditer(r"\[[^\]]*`[^`]+`[^\]]*\]\([^)]+\)", line)
        ]
        for match in re.finditer(r"`([^`]+)`", line):
            if any(start <= match.start() < end for start, end in linked_ranges):
                continue
            prefix = line[: match.start()]
            if prefix.rfind("[") > prefix.rfind("]"):
                # The link label may continue onto a later Markdown line.
                continue
            yield line_number, match.group(1)


def inline_names(path: Path) -> set[str]:
    return {
        name
        for _line_number, name in prose_code_spans(path.read_text(encoding="utf-8"))
    }


def declared_functions() -> set[str]:
    text = HEADER.read_text(encoding="utf-8")
    try:
        start = text.index(START_MARKER)
        end = text.index(END_MARKER, start)
    except ValueError as error:
        raise RuntimeError("OpenDoor.h API declaration markers were not found") from error

    declaration_block = text[start:end]
    return set(
        re.findall(
            r"ODAPIDEF\b(?:(?!;).)*?\b(od_[a-z0-9_]+)\s*\(",
            declaration_block,
            flags=re.DOTALL,
        )
    )


def documented_functions() -> dict[str, list[Path]]:
    pages: dict[str, list[Path]] = defaultdict(list)
    for path in API_DIR.rglob("od_*.md"):
        pages[path.stem].append(path)
    return pages


def control_fields() -> set[str]:
    text = HEADER.read_text(encoding="utf-8")
    marker = text.index(CONTROL_MARKER)
    start = text.index("typedef struct\n{", marker)
    end = text.index("} tODControl;", start)
    fields: set[str] = set()

    for source_line in text[start:end].splitlines():
        line = source_line.split("/*", 1)[0].strip()
        if ";" not in line:
            continue
        match = re.search(r"\(\*\s*(\w+)\s*\)", line)
        if match is None:
            match = re.search(r"\b(\w+)\s*(?:\[[^;]+\])?\s*;", line)
        if match is not None:
            fields.add(match.group(1))

    return fields


def reference_symbols(
    declared: set[str], fields: set[str]
) -> tuple[dict[str, Path], dict[str, set[Path]]]:
    """Return documented prose names and the pages which define each name."""
    targets: dict[str, Path] = {}
    definitions: dict[str, set[Path]] = defaultdict(set)

    for name in declared:
        page = API_DIR / f"{name}.md"
        for spelling in (name, f"{name}()"):
            targets[spelling] = page
            definitions[spelling].add(page)

    control_pages = [
        CONTROL_DIR / "connection.md",
        CONTROL_DIR / "caller.md",
        CONTROL_DIR / "runtime.md",
        CONTROL_DIR / "customization.md",
    ]
    for field in fields:
        pages = {
            page
            for page in control_pages
            if re.search(rf"\b{re.escape(field)}\b", page.read_text(encoding="utf-8"))
        }
        if not pages:
            continue
        target = next(page for page in control_pages if page in pages)
        for spelling in (field, f"od_control.{field}"):
            targets[spelling] = target
            definitions[spelling].update(pages)

    for name in inline_names(CONSTANTS_PAGE):
        if name not in targets:
            targets[name] = CONSTANTS_PAGE
            definitions[name].add(CONSTANTS_PAGE)

    type_names = {
        "BYTE",
        "INT8",
        "WORD",
        "INT16",
        "DWORD",
        "INT32",
        "DWORD_PTR",
        "INT",
        "UINT",
        "BOOL",
        "TRUE",
        "FALSE",
        "tODMilliSec",
        "tODInputEventType",
        "tODInputEvent",
        "tODEditOptions",
        "OD_PERSONALITY_PROC",
        "OD_COMPONENT",
    }
    for name in type_names:
        targets[name] = TYPES_PAGE
        definitions[name].add(TYPES_PAGE)

    for name in {
        "ERR_*",
        "GETIN_*",
        "EDIT_*",
        "MENU_*",
        "STATUS_*",
        "PEROP_*",
        "INCLUDE_*",
        "PER_*",
        "COM_*",
    }:
        targets[name] = CONSTANTS_PAGE
        definitions[name].add(CONSTANTS_PAGE)

    compatibility_names = {
        "od_set_colour",
        "od_init_with_config",
        "od_log_open()",
        "od_emulate()",
        "ODConfigInit",
        "ODLogEnable",
        "ODMPSEnable",
    }
    for name in compatibility_names:
        targets[name] = COMPATIBILITY_PAGE
        definitions[name].add(COMPATIBILITY_PAGE)

    targets["od_control"] = CONTROL_INDEX
    definitions["od_control"] = {CONTROL_INDEX}
    targets["od_*"] = API_DIR / "index.md"
    definitions["od_*"] = {API_DIR / "index.md"}
    targets["OpenDoor.h"] = API_DIR / "index.md"
    definitions["OpenDoor.h"] = {API_DIR / "index.md"}
    targets["OD_WIN32_STATIC"] = DOCS_DIR / "guides" / "building.md"
    definitions["OD_WIN32_STATIC"].add(DOCS_DIR / "guides" / "building.md")
    return targets, definitions


def main() -> int:
    declared = declared_functions()
    documented = documented_functions()
    fields = control_fields()
    failures: list[str] = []

    for name in sorted(declared):
        if name not in documented:
            failures.append(f"missing API page for {name}()")

    for name, paths in sorted(documented.items()):
        if name not in declared:
            relative = ", ".join(str(path.relative_to(ROOT)) for path in paths)
            failures.append(f"API page has no current declaration: {name} ({relative})")
            continue

        for path in paths:
            text = path.read_text(encoding="utf-8")
            if f"# `{name}()`" not in text:
                failures.append(f"{path.relative_to(ROOT)} has no canonical heading")
            if "## Synopsis" not in text:
                failures.append(f"{path.relative_to(ROOT)} has no Synopsis section")
            if not re.search(rf"\b{re.escape(name)}\s*\(", text):
                failures.append(f"{path.relative_to(ROOT)} does not name its declaration")

    control_text = "\n".join(
        path.read_text(encoding="utf-8") for path in CONTROL_DIR.glob("*.md")
    )
    for name in sorted(fields):
        if not re.search(rf"\b{re.escape(name)}\b", control_text):
            failures.append(f"od_control field is not documented: {name}")

    targets, definitions = reference_symbols(declared, fields)
    for path in sorted(DOCS_DIR.rglob("*.md")):
        for line_number, name in prose_code_spans(path.read_text(encoding="utf-8")):
            if name not in targets or path in definitions[name]:
                continue
            failures.append(
                f"{path.relative_to(ROOT)}:{line_number} has an unlinked "
                f"API reference: `{name}`"
            )

    if failures:
        for failure in failures:
            print(f"error: {failure}", file=sys.stderr)
        return 1

    print(
        f"API documentation covers all {len(declared)} high-level functions "
        f"with {sum(len(paths) for paths in documented.values())} page(s); "
        f"all {len(fields)} od_control fields are named; prose API references "
        "are linked."
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
