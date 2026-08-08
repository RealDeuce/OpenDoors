#!/usr/bin/env python3
# SPDX-License-Identifier: LGPL-2.0-or-later
"""Verify that imported OpenDoors 6.00 reference material was not omitted."""

from __future__ import annotations

import re
import sys
from pathlib import Path

import import_opendoor_manual as manual


ROOT = Path(__file__).resolve().parent.parent


def generated_region(path: Path) -> str:
    text = path.read_text(encoding="utf-8")
    match = re.search(
        r"<!-- manual-reference:start -->\n(.*?)\n<!-- manual-reference:end -->",
        text,
        re.S,
    )
    if match is None:
        raise RuntimeError(f"{path.relative_to(ROOT)} has no imported reference region")
    return match.group(1)


def main() -> int:
    pages = manual.read_pages()
    failures: list[str] = []
    ranges = manual.function_ranges(pages)

    for name, start, end in ranges:
        path = manual.API_DIR / f"{name}.md"
        text = path.read_text(encoding="utf-8")
        sections = manual.parse_labelled_function(
            manual.source_for_range(pages, start, end), name
        )
        for key in ("PURPOSE", "RETURNS", "DESCRIPTION", "EXAMPLE", "SEE ALSO"):
            if key not in sections:
                continue
            language = "c" if key == "EXAMPLE" else "text"
            expected = manual.render_block(sections[key], name, language)
            if expected and expected not in text:
                failures.append(f"{path.relative_to(ROOT)} omits original {key}")

    control_groups = {
        "connection": (150, 157),
        "caller": (158, 181),
        "runtime": (182, 186),
        "customization": (187, 219),
    }
    # Populate the same authoritative field-to-page mapping used by the import.
    manual.CONTROL_FIELD_PAGES.clear()
    for page_name, (start, end) in control_groups.items():
        source = manual.source_for_range(pages, start, end)
        for segment in re.split(r"(?m)^-{70,}\s*$", source):
            field = manual.declared_control_field(segment.splitlines())
            if field is not None:
                manual.CONTROL_FIELD_PAGES[field] = page_name
    for page_name in control_groups:
        current = manual.without_generated_reference(
            (manual.CONTROL_DIR / f"{page_name}.md").read_text(encoding="utf-8")
        )
        for field in manual.public_control_fields():
            if re.search(rf"\b{re.escape(field)}\b", current):
                manual.CONTROL_FIELD_PAGES.setdefault(field, page_name)

    for page_name, (start, end) in control_groups.items():
        path = manual.CONTROL_DIR / f"{page_name}.md"
        expected = manual.render_control_range(pages, start, end, page_name)
        if generated_region(path) != expected:
            failures.append(f"{path.relative_to(ROOT)} does not contain its full chapter range")

    topic_specs = [
        ("docs/guides/windows.md", 220, 224, "ADDITIONAL INFORMATION ON THE WIN32 VERSION"),
        ("docs/guides/configuration.md", 225, 229, "CONFIGURATION FILE SYSTEM"),
        ("docs/guides/door-information-formats.md", 230, 232, "DEFINING CUSTOM DOOR INFORMATION FILE FORMATS"),
        ("docs/guides/personalities.md", 233, 234, "MULTIPLE PERSONALITY SYSTEM"),
        ("docs/guides/logging.md", 235, 236, "LOG FILE SYSTEM"),
        ("docs/guides/multinode.md", 237, 241, "MAKING DOORS MULTI-NODE-AWARE"),
        ("docs/guides/programming-tutorial.md", 21, 39, "ABOUT THIS MANUAL"),
        ("docs/guides/troubleshooting.md", 242, 248, "ABOUT THIS CHAPTER"),
    ]
    for relative, start, end, title in topic_specs:
        path = ROOT / relative
        expected = manual.render_topic_range(pages, start, end, title)
        if generated_region(path) != expected:
            failures.append(f"{relative} does not contain its full manual range")

    if failures:
        for failure in failures:
            print(f"error: {failure}", file=sys.stderr)
        return 1
    print(
        f"Manual coverage includes all substantive sections for {len(ranges)} "
        "legacy API functions, all four control-structure ranges, the complete "
        "tutorial, and all special-topic ranges."
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
