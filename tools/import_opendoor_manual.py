#!/usr/bin/env python3
# SPDX-License-Identifier: LGPL-2.0-or-later
"""Import the substantive OpenDoors 6.00 manual into the Markdown reference.

The historic manual is the authoritative source for the detailed API prose.
This importer deliberately performs a conservative reformat: page furniture
and fixed-width line wrapping are removed, but paragraphs are otherwise kept
intact.  Current signatures and implementation notes remain sourced from the
existing Markdown pages.
"""

from __future__ import annotations

import re
from pathlib import Path


ROOT = Path(__file__).resolve().parent.parent
SOURCE = ROOT / "historic" / "OPENDOOR.TXT"
API_DIR = ROOT / "docs" / "reference" / "api"
CONTROL_DIR = ROOT / "docs" / "reference" / "control"
CONTROL_FIELD_PAGES: dict[str, str] = {}

LABELS = {
    "PURPOSE",
    "FORMAT",
    "RETURNS",
    "DESCRIPTION",
    "SEE ALSO",
    "EXAMPLE",
    "EXAMPLES",
}


def read_pages() -> dict[int, str]:
    text = SOURCE.read_bytes().decode("cp437")
    pages: dict[int, str] = {}
    for record in text.split("\f"):
        match = re.search(r"End of Page\s+(\d+)", record)
        if match is None:
            continue
        number = int(match.group(1))
        lines = record.splitlines()
        while lines and not lines[-1].strip():
            lines.pop()
        if lines and re.search(r"End of Page\s+\d+", lines[-1]):
            lines.pop()
        if lines and set(lines[-1].strip()) == {"="}:
            lines.pop()
        # Preserve blank lines immediately before the printed footer.  They
        # distinguish a paragraph which ends at a page boundary from one which
        # continues on the following page.
        pages[number] = "\n".join(lines).lstrip("\n")
    return pages


def function_ranges(pages: dict[int, str]) -> list[tuple[str, int, int]]:
    starts: list[tuple[str, int]] = []
    for number in range(47, 148):
        for match in re.finditer(r"(?m)^(OD_[A-Z0-9_]+)\(\)\s*$", pages[number]):
            starts.append((match.group(1).lower(), number))
    ranges: list[tuple[str, int, int]] = []
    for index, (name, start) in enumerate(starts):
        end = starts[index + 1][1] - 1 if index + 1 < len(starts) else 147
        ranges.append((name, start, end))
    return ranges


def source_for_range(pages: dict[int, str], start: int, end: int) -> str:
    # A form feed is only page furniture.  It must not create a paragraph
    # boundary because sentences and examples routinely continue on the next
    # printed page.
    return "\n".join(pages[number] for number in range(start, end + 1))


def parse_labelled_function(text: str, name: str) -> dict[str, list[str]]:
    title = re.search(rf"(?m)^{re.escape(name.upper())}\(\)\s*$", text)
    if title is None:
        raise RuntimeError(f"missing title for {name}()")
    lines = text[title.end() :].splitlines()
    if lines and set(lines[0].strip()) == {"-"}:
        lines.pop(0)

    sections: dict[str, list[str]] = {}
    current: str | None = None
    for line in lines:
        match = re.match(r"^([A-Z ]+?):?\s{2,}(.*)$", line)
        label = match.group(1).strip() if match else ""
        if label in LABELS:
            current = "EXAMPLE" if label == "EXAMPLES" else label
            sections.setdefault(current, []).append(match.group(2))
            continue
        if current is None:
            continue
        if line.startswith(" " * 15):
            line = line[15:]
        sections[current].append(line)
    return sections


def paragraphs(lines: list[str]) -> list[list[str]]:
    result: list[list[str]] = []
    current: list[str] = []
    for line in lines:
        line = line.rstrip()
        if not line.strip():
            if current:
                result.append(current)
                current = []
            continue
        current.append(line)
    if current:
        result.append(current)
    return result


def link_functions(
    text: str, current_name: str, api_prefix: str = ""
) -> str:
    available = {path.stem for path in API_DIR.glob("od_*.md")}

    def replace(match: re.Match[str]) -> str:
        name = match.group(1)
        if name == current_name:
            return f"`{name}()`"
        if name in available:
            return f"[`{name}()`]({api_prefix}{name}.md)"
        return f"`{name}()`"

    return re.sub(r"\b(od_[a-z0-9_]+)\(\)", replace, text, flags=re.I)


def render_block(
    lines: list[str],
    current_name: str,
    code_language: str = "text",
    api_prefix: str = "",
) -> str:
    output: list[str] = []
    for block in paragraphs(lines):
        nonblank = [line for line in block if line.strip()]
        indents = [len(line) - len(line.lstrip()) for line in nonblank]
        is_table = any(
            re.match(r"\s*[+|].*[+|]\s*$", line) or set(line.strip()) == {"-"}
            for line in nonblank
        )
        is_declaration = bool(
            re.search(r"\bod_control\.[A-Za-z0-9_]+", " ".join(nonblank))
            and any(";" in line for line in nonblank)
        )
        stripped_first = nonblank[0].lstrip() if nonblank else ""
        is_c_source = stripped_first.startswith("#include") or (
            any(";" in line for line in nonblank)
            and any("{" in line or "}" in line for line in nonblank)
        )
        is_code = (
            (bool(indents) and min(indents) >= 8)
            or is_declaration
            or is_c_source
        )
        if is_table or is_code:
            amount = min(indents) if is_code else 0
            cleaned = "\n".join(line[amount:].rstrip() for line in block)
            output.extend([f"```{code_language}", cleaned, "```", ""])
            continue
        joined = " ".join(line.strip() for line in block)
        if re.match(r"^[#*+-]\s", joined):
            joined = "\\" + joined
        output.extend([link_functions(joined, current_name, api_prefix), ""])
    return "\n".join(output).rstrip()


def existing_synopsis(path: Path) -> str:
    text = path.read_text(encoding="utf-8")
    match = re.search(r"## Synopsis\s*\n\n```c\n(.*?)\n```", text, re.S)
    if match is None:
        raise RuntimeError(f"missing Synopsis C block in {path}")
    return match.group(1).rstrip()


def existing_notes(path: Path) -> str:
    text = path.read_text(encoding="utf-8")
    marked = re.search(
        r"<!-- current-notes:start -->\n(.*?)\n<!-- current-notes:end -->", text, re.S
    )
    if marked is not None:
        return marked.group(1).strip()
    synopsis = re.search(r"## Synopsis\s*\n\n```c\n.*?\n```\n", text, re.S)
    if synopsis is None:
        return ""
    tail = text[synopsis.end() :]
    tail = re.split(r"\n## See also\s*\n", tail, maxsplit=1)[0]
    return tail.strip()


def existing_see_also(path: Path) -> str:
    text = path.read_text(encoding="utf-8")
    marked = re.search(
        r"<!-- current-see-also:start -->\n(.*?)\n<!-- current-see-also:end -->",
        text,
        re.S,
    )
    if marked is not None:
        return marked.group(1).strip()
    match = re.search(r"\n## See also\s*\n\n(.*?)(?=\n## |\Z)", text, re.S)
    if match is None:
        return ""
    body = re.sub(r"\n*_Detailed reference adapted.*?Manual\._\s*", "", match.group(1))
    chunks = [chunk.strip() for chunk in re.split(r"\n\s*\n", body) if chunk.strip()]
    return chunks[-1] if chunks else ""


def write_function_page(
    name: str, source_page: int, sections: dict[str, list[str]]
) -> None:
    path = API_DIR / f"{name}.md"
    synopsis = existing_synopsis(path)
    notes = existing_notes(path)
    current_see_also = existing_see_also(path)
    purpose = render_block(sections.get("PURPOSE", []), name)

    output = [
        f"# `{name}()`",
        "",
        purpose,
        "",
        "## Synopsis",
        "",
        "```c",
        synopsis,
        "```",
    ]
    headings = [
        ("RETURNS", "Return value"),
        ("DESCRIPTION", "Description"),
        ("EXAMPLE", "Examples"),
    ]
    for key, heading in headings:
        language = "c" if key == "EXAMPLE" else "text"
        body = render_block(sections.get(key, []), name, language)
        if body:
            output.extend(["", f"## {heading}", "", body])
    if notes:
        output.extend(
            [
                "",
                "## Current implementation notes",
                "",
                "<!-- current-notes:start -->",
                notes,
                "<!-- current-notes:end -->",
            ]
        )

    legacy_see_also = render_block(sections.get("SEE ALSO", []), name)
    see_also = "\n\n".join(
        part for part in (legacy_see_also, current_see_also) if part
    )
    if see_also:
        output.extend(
            [
                "",
                "## See also",
                "",
                legacy_see_also,
                "",
                "<!-- current-see-also:start -->",
                current_see_also,
                "<!-- current-see-also:end -->",
            ]
        )
    output.extend(
        [
            "",
            f"_Detailed reference adapted from page {source_page} of the OpenDoors 6.00 Programmer's Manual._",
            "",
        ]
    )
    path.write_text("\n".join(output), encoding="utf-8")


def without_generated_reference(text: str) -> str:
    return re.sub(
        r"(?:\n*## Detailed reference from the original manual\s*)+"
        r"<!-- manual-reference:start -->.*?<!-- manual-reference:end -->\s*",
        "\n",
        text,
        flags=re.S,
    ).rstrip()


def control_lines(lines: list[str], field_segment: bool = False) -> list[str]:
    result: list[str] = []
    for line in lines:
        if field_segment and re.fullmatch(r"[a-z_]+", line.strip()):
            # Continuation of a field name wrapped in the manual's narrow
            # left-hand margin.  The canonical name is already the heading.
            continue
        if line.startswith(" " * 15):
            line = line[15:]
        elif (
            field_segment
            and len(line) >= 15
            and line[:15].strip()
            and not line.startswith("CONTROL STRUCTURE")
        ):
            # Field names were placed in a narrow left margin and frequently
            # wrapped over several lines.  The declaration supplies the
            # canonical name, so retain the right-hand reference text.
            line = line[15:]
        result.append(line.rstrip())
    return result


def public_control_fields() -> set[str]:
    text = (ROOT / "OpenDoor.h").read_text(encoding="utf-8")
    marker = text.index("The OpenDoors control structure")
    start = text.index("typedef struct\n{", marker)
    end = text.index("} tODControl;", start)
    fields: set[str] = set()
    for line in text[start:end].splitlines():
        line = line.split("/*", 1)[0]
        match = re.search(r"\(\*\s*(\w+)\s*\)", line)
        if match is None:
            match = re.search(r"\b(\w+)\s*(?:\[[^;]+\])?\s*;", line)
        if match is not None:
            fields.add(match.group(1))
    return fields


def link_control_fields(
    text: str, current_page: str, control_prefix: str = ""
) -> str:
    fields = public_control_fields()
    output: list[str] = []
    in_fence = False
    for line in text.splitlines():
        if line.startswith("```"):
            in_fence = not in_fence
            output.append(line)
            continue
        if not in_fence:
            for field in sorted(fields, key=len, reverse=True):
                page = CONTROL_FIELD_PAGES.get(field, "index")
                anchor = field
                target = (
                    f"#{anchor}"
                    if page == current_page
                    else f"{control_prefix}{page}.md#{anchor}"
                )
                line = re.sub(
                    rf"(?<![`\[])\bod_control\.{re.escape(field)}\b",
                    f"[`od_control.{field}`]({target})",
                    line,
                )
        output.append(line)
    return "\n".join(output)


def declared_control_field(lines: list[str]) -> str | None:
    first_paragraph: list[str] = []
    started = False
    for line in lines:
        if line.strip():
            started = True
            first_paragraph.append(line)
        elif started:
            break
    joined = " ".join(first_paragraph)
    match = re.search(r"\bod_control\.([A-Za-z0-9_]+)", joined)
    if match is None or ";" not in joined:
        return None
    field = match.group(1)
    return field if field in public_control_fields() else None


def extract_control_declaration(
    lines: list[str], field: str
) -> tuple[str, list[str]]:
    start = next(
        index for index, line in enumerate(lines) if f"od_control.{field}" in line
    )
    end = start
    while end < len(lines) and ";" not in lines[end]:
        end += 1
    if end >= len(lines):
        return "", lines
    declaration_lines = control_lines(lines[start : end + 1], field_segment=True)
    declaration = "\n".join(line for line in declaration_lines if line.strip())
    remaining = lines[:start] + lines[end + 1 :]
    return declaration, remaining


def render_control_segment(lines: list[str], current_page: str) -> str:
    field = declared_control_field(lines)
    declaration = ""
    if field is not None:
        declaration, lines = extract_control_declaration(lines, field)
    lines = control_lines(lines, field_segment=field is not None)
    body = render_block(lines, "", code_language="c", api_prefix="../api/")
    if declaration:
        body = f"```c\n{declaration}\n```\n\n{body}".rstrip()
    return link_control_fields(body, current_page)


def render_control_range(
    pages: dict[int, str], start: int, end: int, current_page: str
) -> str:
    text = source_for_range(pages, start, end)
    output: list[str] = []
    segment: list[str] = []

    def flush() -> None:
        nonlocal segment
        if not any(line.strip() for line in segment):
            segment = []
            return
        field = declared_control_field(segment)
        if field is not None:
            output.extend([f"#### `{field}`", ""])
        body = render_control_segment(segment, current_page)
        if body:
            output.extend([body, ""])
        segment = []

    for line in text.splitlines():
        title = re.match(r"^CONTROL STRUCTURE - (.+?)\s*$", line)
        if title:
            flush()
            heading = title.group(1).strip().title()
            output.extend([f"### {heading}", ""])
            continue
        if line.strip() and set(line.strip()) == {"-"}:
            flush()
            continue
        segment.append(line)
    flush()
    return "\n".join(output).rstrip()


def write_control_pages(pages: dict[int, str]) -> None:
    groups = {
        "connection": (150, 157),
        "caller": (158, 181),
        "runtime": (182, 186),
        "customization": (187, 219),
    }
    CONTROL_FIELD_PAGES.clear()
    for page_name, (start, end) in groups.items():
        text = source_for_range(pages, start, end)
        segments = re.split(r"(?m)^-{70,}\s*$", text)
        for segment in segments:
            field = declared_control_field(segment.splitlines())
            if field is not None:
                CONTROL_FIELD_PAGES[field] = page_name
    for page_name in groups:
        path = CONTROL_DIR / f"{page_name}.md"
        current = without_generated_reference(path.read_text(encoding="utf-8"))
        for field in public_control_fields():
            if re.search(rf"\b{re.escape(field)}\b", current):
                CONTROL_FIELD_PAGES.setdefault(field, page_name)
    for page_name, (start, end) in groups.items():
        path = CONTROL_DIR / f"{page_name}.md"
        current = without_generated_reference(path.read_text(encoding="utf-8"))
        reference = render_control_range(pages, start, end, page_name)
        path.write_text(
            current
            + "\n\n## Detailed reference from the original manual\n\n"
            + "<!-- manual-reference:start -->\n"
            + reference
            + "\n<!-- manual-reference:end -->\n",
            encoding="utf-8",
        )

    index_path = CONTROL_DIR / "index.md"
    current = without_generated_reference(index_path.read_text(encoding="utf-8"))
    introduction = source_for_range(pages, 148, 149)
    introduction = introduction.split("INTRODUCTION TO THE CONTROL STRUCTURE", 1)[1]
    introduction = re.sub(r"^\s*-{70,}\s*", "", introduction)
    body = render_control_segment(introduction.splitlines(), "index")
    index_path.write_text(
        current
        + "\n\n## Detailed reference from the original manual\n\n"
        + "<!-- manual-reference:start -->\n"
        + body
        + "\n<!-- manual-reference:end -->\n",
        encoding="utf-8",
    )


def render_topic_range(
    pages: dict[int, str],
    start: int,
    end: int,
    first_title: str,
    api_prefix: str = "../reference/api/",
    control_prefix: str = "../reference/control/",
) -> str:
    text = source_for_range(pages, start, end)
    position = text.find(first_title)
    if position < 0:
        raise RuntimeError(f"missing manual section {first_title!r}")
    lines = text[position:].splitlines()
    output: list[str] = []
    segment: list[str] = []
    first_heading = True

    def flush() -> None:
        nonlocal segment
        if not any(line.strip() for line in segment):
            segment = []
            return
        normalized = control_lines(segment)
        body = render_block(normalized, "", code_language="c", api_prefix=api_prefix)
        body = link_control_fields(body, "", control_prefix=control_prefix)
        if body:
            output.extend([body, ""])
        segment = []

    index = 0
    while index < len(lines):
        line = lines[index]
        following_rule = (
            index + 1 < len(lines)
            and lines[index + 1].strip()
            and set(lines[index + 1].strip()) == {"-"}
        )
        if following_rule:
            flush()
            if first_heading:
                first_heading = False
            else:
                output.extend([f"### {line.strip().title()}", ""])
            index += 2
            continue
        segment.append(line)
        index += 1
    flush()
    return "\n".join(output).rstrip()


def write_topic_page(
    path: Path,
    base: str,
    reference: str,
    historical_note: str = "",
) -> None:
    if path.exists():
        current = re.sub(
            r"\n*## Detailed reference from the original manual.*?"
            r"<!-- manual-reference:end -->\s*",
            "\n",
            path.read_text(encoding="utf-8"),
            flags=re.S,
        ).rstrip()
    else:
        current = base.rstrip()
    parts = [current, "", "## Detailed reference from the original manual", ""]
    if historical_note:
        parts.extend([historical_note.strip(), ""])
    parts.extend(
        [
            "<!-- manual-reference:start -->",
            reference,
            "<!-- manual-reference:end -->",
            "",
        ]
    )
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text("\n".join(parts), encoding="utf-8")


def write_special_topics(pages: dict[int, str]) -> None:
    guides = ROOT / "docs" / "guides"
    topics = [
        (
            guides / "windows.md",
            "# Windows programming\n\n"
            "OpenDoors retains the Win32 integration described by the original "
            "manual while also supporting current 32- and 64-bit MSVC and MinGW "
            "toolchains. See [Building OpenDoors](building.md) for current build "
            "commands.",
            220,
            224,
            "ADDITIONAL INFORMATION ON THE WIN32 VERSION",
            "The compiler and operating-system names below describe the original "
            "6.00 environment. The programming and process-model details are "
            "preserved; use the current building guide for supported toolchains.",
        ),
        (
            guides / "configuration.md",
            "",
            225,
            229,
            "CONFIGURATION FILE SYSTEM",
            "",
        ),
        (
            guides / "door-information-formats.md",
            "# Custom door-information file formats\n\n"
            "The configuration component can describe line-oriented drop-file "
            "formats which are not built into OpenDoors.",
            230,
            232,
            "DEFINING CUSTOM DOOR INFORMATION FILE FORMATS",
            "",
        ),
        (
            guides / "personalities.md",
            "",
            233,
            234,
            "MULTIPLE PERSONALITY SYSTEM",
            "The personality SDK is supported by the DOS platform. Current public "
            "declarations are in `ODStat.h`; the operational description below is "
            "the complete original reference.",
        ),
        (
            guides / "logging.md",
            "# Log-file system\n\n"
            "OpenDoors can maintain a FrontDoor-format activity log and allows "
            "applications to add their own entries.",
            235,
            236,
            "LOG FILE SYSTEM",
            "",
        ),
        (
            guides / "multinode.md",
            "",
            237,
            241,
            "MAKING DOORS MULTI-NODE-AWARE",
            "The original examples use DOS-era sharing functions. Their concurrency "
            "requirements remain relevant, but applications should use the locking "
            "facilities appropriate to their current platform and runtime.",
        ),
    ]
    for path, base, start, end, title, note in topics:
        reference = render_topic_range(pages, start, end, title)
        write_topic_page(path, base, reference, note)


def write_tutorial_and_troubleshooting(pages: dict[int, str]) -> None:
    guides = ROOT / "docs" / "guides"
    tutorial = render_topic_range(
        pages, 21, 39, "ABOUT THIS MANUAL", api_prefix="../reference/api/"
    )
    write_topic_page(
        guides / "programming-tutorial.md",
        "# OpenDoors programming tutorial\n\n"
        "This tutorial preserves the complete programming walkthrough and example "
        "discussion from the OpenDoors 6.00 manual.",
        tutorial,
        "The compiler, linker, and BBS launch instructions describe the original "
        "6.00 distribution. Use [Building OpenDoors](building.md) for current "
        "toolchains; the API workflow and programming discussion remain useful.",
    )
    troubleshooting = render_topic_range(
        pages, 242, 248, "ABOUT THIS CHAPTER", api_prefix="../reference/api/"
    )
    write_topic_page(
        guides / "troubleshooting.md",
        "# Troubleshooting\n\n"
        "This page retains the diagnostic guidance from the original manual. "
        "Project support is now provided through the repository rather than the "
        "historical BBS, postal, telephone, and web addresses named below.",
        troubleshooting,
        "The contact details are retained as historical context and are not current "
        "support channels.",
    )


def write_introduction_and_glossary(pages: dict[int, str]) -> None:
    introduction = render_topic_range(
        pages,
        5,
        8,
        "WELCOME!",
        api_prefix="reference/api/",
        control_prefix="reference/control/",
    )
    write_topic_page(
        ROOT / "docs" / "introduction.md",
        "# Introduction to OpenDoors\n\n"
        "OpenDoors is a C and C++ toolkit for interactive online software and "
        "BBS doors. This page retains the original manual's complete overview.",
        introduction,
        "Statements about package contents, registration, compiler versions, and "
        "the size of the user community describe the 6.00 release. Current "
        "platform and packaging information is provided elsewhere in this site.",
    )

    text = source_for_range(pages, 256, 266)
    position = text.find("GLOSSARY")
    if position < 0:
        raise RuntimeError("missing glossary")
    glossary_lines = control_lines(text[position + len("GLOSSARY") :].splitlines())
    glossary = render_block(
        glossary_lines, "", code_language="c", api_prefix="api/"
    )
    glossary = link_control_fields(glossary, "", control_prefix="control/")
    write_topic_page(
        ROOT / "docs" / "reference" / "glossary.md",
        "# Glossary\n\n"
        "Terminology used by OpenDoors and the BBS software environment.",
        glossary,
        "Definitions are retained in the context of the original manual. In "
        "particular, operating-system and modem examples reflect that period.",
    )


def main() -> int:
    pages = read_pages()
    ranges = function_ranges(pages)
    if len(ranges) != 47:
        raise RuntimeError(f"expected 47 documented functions, found {len(ranges)}")
    for name, start, end in ranges:
        text = source_for_range(pages, start, end)
        sections = parse_labelled_function(text, name)
        missing = LABELS.intersection({"PURPOSE", "FORMAT", "RETURNS", "DESCRIPTION"}) - sections.keys()
        if missing:
            raise RuntimeError(f"{name}() is missing sections: {sorted(missing)}")
        write_function_page(name, start, sections)
    write_control_pages(pages)
    write_special_topics(pages)
    write_tutorial_and_troubleshooting(pages)
    write_introduction_and_glossary(pages)
    print(
        f"Imported detailed reference for {len(ranges)} functions and "
        "the complete control structure chapter"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
