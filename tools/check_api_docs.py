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


def main() -> int:
    declared = declared_functions()
    documented = documented_functions()
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
    fields = control_fields()
    for name in sorted(fields):
        if not re.search(rf"\b{re.escape(name)}\b", control_text):
            failures.append(f"od_control field is not documented: {name}")

    if failures:
        for failure in failures:
            print(f"error: {failure}", file=sys.stderr)
        return 1

    print(
        f"API documentation covers all {len(declared)} high-level functions "
        f"with {sum(len(paths) for paths in documented.values())} page(s); "
        f"all {len(fields)} od_control fields are named."
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
