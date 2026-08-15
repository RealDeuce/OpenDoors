#!/usr/bin/env python3
# SPDX-License-Identifier: LGPL-2.0-or-later
"""Drive the interactive acceptance door through DOSBox COM1 emulation."""

from __future__ import annotations

import argparse
import socket
import subprocess
import sys
import tempfile
from pathlib import Path

from transport_scenarios import SCENARIOS, create_fixtures, drive_scenario
from transport_scenarios import remove_fixtures


def print_log_tail(log_path: Path) -> None:
    if not log_path.exists():
        return
    contents = log_path.read_bytes()[-16384:].decode(errors="replace")
    if contents:
        print("DOSBox output (last 16 KiB):", file=sys.stderr)
        print(contents, file=sys.stderr)


def main() -> int:
    parser = argparse.ArgumentParser(
        description="drive an OpenDoors acceptance door through DOSBox COM1"
    )
    parser.add_argument("dosbox", type=Path)
    parser.add_argument("door", type=Path)
    parser.add_argument(
        "--pre-command",
        action="append",
        default=[],
        help="DOS command to run before the acceptance door (repeatable)",
    )
    parser.add_argument(
        "--scenario",
        action="append",
        choices=SCENARIOS,
        default=[],
        help="scenario to run (repeatable; defaults to the complete suite)",
    )
    args = parser.parse_args()

    dosbox = args.dosbox.resolve()
    door = args.door.resolve()
    scenarios = args.scenario or list(SCENARIOS)
    if not dosbox.is_file():
        raise RuntimeError(f"DOSBox executable not found: {dosbox}")
    if not door.is_file():
        raise RuntimeError(f"DOS acceptance door not found: {door}")

    build_directory = door.parent
    log_path = build_directory / f"dos-serial-{door.stem.lower()}.log"
    failure_path = build_directory / "ODFAIL.TXT"
    failure_path.unlink(missing_ok=True)
    create_fixtures(build_directory)

    listener = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    listener.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    listener.bind(("127.0.0.1", 0))
    listener.listen(1)
    port = listener.getsockname()[1]

    process: subprocess.Popen[bytes] | None = None
    peer: socket.socket | None = None
    try:
        with tempfile.TemporaryDirectory(prefix="opendoors-dos-serial-") as temp:
            config = Path(temp) / "dosbox.conf"
            config.write_text(
                "[serial]\n"
                f"serial1=nullmodem server:127.0.0.1 port:{port} "
                "usedtr:1\n",
                encoding="ascii",
            )
            with log_path.open("wb") as log:
                commands = [
                    str(dosbox),
                    "-noconsole",
                    "-conf",
                    str(config),
                    "-c",
                    f'mount c "{build_directory}"',
                    "-c",
                    "c:",
                ]
                for command in args.pre_command:
                    commands.extend(("-c", command))
                for scenario in scenarios:
                    commands.extend(("-c", f"{door.name} {scenario}"))
                commands.extend(("-c", "exit"))
                process = subprocess.Popen(
                    commands,
                    stdout=log,
                    stderr=subprocess.STDOUT,
                )
                listener.settimeout(30)
                peer, _ = listener.accept()
                for scenario in scenarios:
                    drive_scenario(peer, scenario, timeout=30)
                    if scenario == "session":
                        peer.close()
                        peer = None
                return_code = process.wait(timeout=30)
                if return_code != 0:
                    raise RuntimeError(f"DOSBox exited with {return_code}")
    except Exception:
        if failure_path.exists():
            print(failure_path.read_text(errors="replace"), file=sys.stderr)
        print_log_tail(log_path)
        raise
    finally:
        listener.close()
        if peer is not None:
            peer.close()
        if process is not None and process.poll() is None:
            process.terminate()
            try:
                process.wait(timeout=5)
            except subprocess.TimeoutExpired:
                process.kill()
                process.wait()
        remove_fixtures(build_directory)

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
