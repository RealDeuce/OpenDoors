#!/usr/bin/env python3
"""Check that every OpenDoors version representation agrees with VERSION."""

import argparse
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def read(path):
    return (ROOT / path).read_text(encoding="utf-8")


def require(pattern, path, description):
    if re.search(pattern, read(path), re.MULTILINE) is None:
        raise SystemExit(f"{path}: missing or incorrect {description}")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--tag", help="also validate a release tag")
    args = parser.parse_args()

    version = read("VERSION").strip()
    match = re.fullmatch(r"(\d+)\.(\d+)\.(\d+)", version)
    if match is None:
        raise SystemExit("VERSION must contain major.minor.patch")

    major, minor, patch = (int(value) for value in match.groups())
    if any(value > 15 for value in (major, minor, patch)):
        raise SystemExit("OD_VERSION only supports single hexadecimal digits")

    compact = f"{major}.{minor}{patch}"
    abi = f"{major}{minor}"
    encoded = f"0x{major:X}{minor:X}{patch:X}"

    require(rf"^#define OD_VERSION\s+{re.escape(encoded)}$", "OpenDoor.h",
            "OD_VERSION")
    require(rf'^#define OD_DLL_NAME\s+"ODOORS{abi}"$', "ODGen.h",
            "OD_DLL_NAME")
    require(rf"OpenDoors {re.escape(compact)}", "ODGen.h",
            "compact version text")
    require(rf"OpenDoors {re.escape(compact)}", "ODRes.rc",
            "resource version text")
    require(rf"^OpenDoors {re.escape(compact)}\b", "FILE_ID.DIZ",
            "FILE_ID.DIZ version")

    for definition in ("OpenDoor.def", "OpenDoor64.def", "OpenDoorMinGW.def"):
        require(rf"^LIBRARY ODOORS{abi}$", definition, "DLL ABI name")
        require(rf"^VERSION {major}\.{minor}$", definition, "DLL version")

    if args.tag is not None:
        tag_pattern = rf"v{re.escape(version)}(?:-rc[1-9][0-9]*)?"
        if re.fullmatch(tag_pattern, args.tag) is None:
            raise SystemExit(
                f"tag {args.tag!r} does not match {version} or an rc tag")

    print(
        f"OpenDoors {version}: display {compact}, OD_VERSION {encoded}, "
        f"ABI ODOORS{abi}")


if __name__ == "__main__":
    main()
