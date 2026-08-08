#!/usr/bin/env python3
"""Stage clean OpenDoors release trees from build and documentation output."""

import argparse
import os
import shutil
import subprocess
from importlib.metadata import distribution
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
EXAMPLE_SOURCES = (
    "ex_chat.c", "ex_diag.c", "ex_hello.c", "ex_music.c", "ex_ski.c",
    "ex_vote.c",
)
PERSONALITY_SOURCES = ("ODStand.c", "ODPCB.c", "ODRA.c", "ODWCat.c")


def version():
    return (ROOT / "VERSION").read_text(encoding="utf-8").strip()


def traditional_version():
    major, minor, patch = version().split(".")
    return f"{major}.{minor}{patch}"


def commit():
    if os.environ.get("GITHUB_SHA"):
        return os.environ["GITHUB_SHA"]
    return subprocess.check_output(
        ["git", "rev-parse", "HEAD"], cwd=ROOT, text=True).strip()


def reset_directory(path):
    if path.exists():
        shutil.rmtree(path)
    path.mkdir(parents=True)


def write_buildinfo(destination, args):
    lines = [
        f"OpenDoors release: {version()}",
        f"Traditional version: {traditional_version()}",
        f"Source commit: {commit()}",
        f"Platform: {args.platform}",
        f"Architecture: {args.architecture}",
        f"Toolchain: {args.toolchain}",
    ]
    if args.runtime:
        lines.append(f"Runtime: {args.runtime}")
    destination.write_text("\n".join(lines) + "\n", encoding="utf-8")


def metadata(args):
    destination = Path(args.prefix)
    if not destination.is_dir():
        raise SystemExit(f"install prefix does not exist: {destination}")
    shutil.copy2(ROOT / "VERSION", destination / "VERSION")
    write_buildinfo(destination / "BUILDINFO.txt", args)


def dos(args):
    destination = Path(args.destination)
    reset_directory(destination)
    (destination / "include").mkdir()
    (destination / "lib").mkdir()
    (destination / "share" / "licenses" / "OpenDoors").mkdir(parents=True)
    shutil.copy2(ROOT / "OpenDoor.h", destination / "include")
    shutil.copy2(ROOT / "ODStat.h", destination / "include")
    shutil.copy2(args.library, destination / "lib" / "ODOORL.LIB")
    shutil.copy2(ROOT / "LICENSE",
                 destination / "share" / "licenses" / "OpenDoors")
    shutil.copy2(ROOT / "VERSION", destination / "VERSION")
    write_buildinfo(destination / "BUILDINFO.txt", args)


def copy_distribution_license(name, destination):
    package = distribution(name)
    matches = [entry for entry in package.files or ()
               if str(entry).lower().endswith("dist-info/licenses/license")]
    if len(matches) != 1:
        raise SystemExit(f"could not identify the {name} license")
    source = Path(package.locate_file(matches[0]))
    shutil.copy2(source, destination)


def companion(args):
    site = Path(args.site)
    if not (site / "index.html").is_file():
        raise SystemExit(f"rendered documentation is missing: {site}")

    destination = Path(args.destination)
    reset_directory(destination)
    shutil.copytree(site, destination / "docs")
    examples = destination / "examples"
    personalities = examples / "personalities"
    personalities.mkdir(parents=True)

    for name in EXAMPLE_SOURCES:
        shutil.copy2(ROOT / name, examples / name)
    for name in PERSONALITY_SOURCES:
        shutil.copy2(ROOT / name, personalities / name)
    shutil.copy2(ROOT / "release" / "examples" / "CMakeLists.txt", examples)
    shutil.copy2(ROOT / "release" / "examples" / "README.md", examples)
    shutil.copy2(ROOT / "LICENSE", destination / "LICENSE")
    shutil.copy2(ROOT / "README.md", destination / "README.md")
    shutil.copy2(ROOT / "CHANGELOG.md", destination / "CHANGELOG.md")
    shutil.copy2(ROOT / "release" / "RELEASE_NOTES.md", destination)
    shutil.copy2(ROOT / "VERSION", destination / "VERSION")

    readme = (
        f"OpenDoors {version()} examples and documentation\n\n"
        "Open docs/index.html in a web browser for the complete reference.\n"
        "See examples/README.md for example build instructions.\n"
    )
    (destination / "README.txt").write_text(readme, encoding="utf-8")

    licenses = destination / "third-party-licenses"
    licenses.mkdir()
    copy_distribution_license("mkdocs", licenses / "MkDocs-LICENSE")
    copy_distribution_license(
        "mkdocs-material", licenses / "MkDocs-Material-LICENSE")


def add_metadata_arguments(parser):
    parser.add_argument("--platform", required=True)
    parser.add_argument("--architecture", required=True)
    parser.add_argument("--toolchain", required=True)
    parser.add_argument("--runtime", default="")


def main():
    parser = argparse.ArgumentParser()
    subparsers = parser.add_subparsers(dest="command", required=True)

    metadata_parser = subparsers.add_parser("metadata")
    metadata_parser.add_argument("--prefix", required=True)
    add_metadata_arguments(metadata_parser)
    metadata_parser.set_defaults(function=metadata)

    dos_parser = subparsers.add_parser("dos")
    dos_parser.add_argument("--library", required=True)
    dos_parser.add_argument("--destination", required=True)
    add_metadata_arguments(dos_parser)
    dos_parser.set_defaults(function=dos)

    companion_parser = subparsers.add_parser("companion")
    companion_parser.add_argument("--site", required=True)
    companion_parser.add_argument("--destination", required=True)
    companion_parser.set_defaults(function=companion)

    args = parser.parse_args()
    args.function(args)


if __name__ == "__main__":
    main()
