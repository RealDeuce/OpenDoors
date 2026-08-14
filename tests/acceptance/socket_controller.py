#!/usr/bin/env python3
# SPDX-License-Identifier: LGPL-2.0-or-later
"""Drive the public socket transport as an external BBS peer."""

from __future__ import annotations

import argparse
import os
import select
import socket
import subprocess
import time
from pathlib import Path

FIXTURES = {
    "ODFILE.ASC": b"FILE-CONTENT\r\n",
    "ODSECT.ASC": b"@#FIRST\r\nWRONG-SECTION\r\n@#SECOND\r\nSECTION-CONTENT\r\n",
    "ODMENU.ASC": b"MENU-CONTENT\r\n",
    "FILES.BBS": b"ITEM.TXT Acceptance listed file\r\n",
    "ITEM.TXT": b"fixture\n",
}


def receive_until(
    peer: socket.socket, marker: bytes, transcript: bytearray, timeout: int = 10
) -> None:
    deadline = time.monotonic() + timeout
    while marker not in transcript:
        remaining = deadline - time.monotonic()
        if remaining <= 0:
            raise RuntimeError(f"timed out waiting for {marker!r}; got {bytes(transcript)!r}")
        readable, _, _ = select.select([peer], [], [], remaining)
        if not readable:
            continue
        chunk = peer.recv(4096)
        if not chunk:
            raise RuntimeError(
                f"connection closed waiting for {marker!r}; "
                f"got {bytes(transcript)!r}"
            )
        transcript.extend(chunk)


def create_fixtures(directory: Path) -> None:
    for name, contents in FIXTURES.items():
        (directory / name).write_bytes(contents)


def remove_fixtures(directory: Path) -> None:
    for name in FIXTURES:
        (directory / name).unlink(missing_ok=True)


def drive_session(peer: socket.socket, timeout: int = 10) -> bytearray:
    transcript = bytearray()
    receive_until(peer, b"READY", transcript, timeout)
    peer.sendall(b"Q")
    receive_until(peer, b"ANSWER", transcript, timeout)
    peer.sendall(b"xY")
    receive_until(peer, b"STRING", transcript, timeout)
    peer.sendall(b"socket\r")
    receive_until(peer, b"KEY", transcript, timeout)
    peer.sendall(b"ABCDE")
    receive_until(peer, b"WAITKEY", transcript, timeout)
    peer.sendall(b"W")
    receive_until(peer, b"CLEAR", transcript, timeout)
    peer.sendall(b"C")
    receive_until(peer, b"AUTODETECT", transcript, timeout)
    receive_until(peer, b"\x1b[6n", transcript, timeout)
    peer.sendall(b"\x1b[")
    receive_until(peer, b"\x1b[!", transcript, timeout)
    peer.sendall(b"RIP12345678901")
    receive_until(peer, b"HOTMENU", transcript, timeout)
    peer.sendall(b"2")
    receive_until(peer, b"Why would you like to chat?", transcript, timeout)
    peer.sendall(b"\r")
    receive_until(peer, b"DONE", transcript, timeout)
    return transcript


def validate_transcript(transcript: bytearray) -> None:
    for expected in (
        b"READY",
        b"ANSWER",
        b"STRING",
        b"FILE-CONTENT",
        b"SECTION-CONTENT",
        b"ITEM.TXT",
        b"RESULT socket 7",
        b"EMU",
        b"DONE",
    ):
        if expected not in transcript:
            raise RuntimeError(f"missing {expected!r} in {bytes(transcript)!r}")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--launcher", action="append", default=[])
    parser.add_argument("--tcp", action="store_true")
    parser.add_argument("door")
    args = parser.parse_args()
    fixture_directory = Path.cwd()
    create_fixtures(fixture_directory)

    kwargs: dict[str, object] = {"close_fds": True}
    listener = None
    child_socket = None
    if args.tcp:
        listener = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        listener.bind(("127.0.0.1", 0))
        listener.listen(1)
        listener.settimeout(10)
        child_argument = str(listener.getsockname()[1])
    else:
        peer, child_socket = socket.socketpair()
        child_socket.set_inheritable(True)
        child_argument = str(child_socket.fileno())
        if os.name == "posix":
            kwargs["pass_fds"] = (child_socket.fileno(),)
        else:
            kwargs["close_fds"] = False
    process = subprocess.Popen(
        [*args.launcher, args.door, child_argument], **kwargs
    )
    if child_socket is not None:
        child_socket.close()
    if listener is not None:
        peer, _ = listener.accept()
        listener.close()
        listener = None
    try:
        transcript = drive_session(peer)
        return_code = process.wait(timeout=10)
        if return_code != 0:
            raise RuntimeError(f"socket door exited with {return_code}")
        validate_transcript(transcript)
    finally:
        if listener is not None:
            listener.close()
        peer.close()
        if process.poll() is None:
            process.kill()
            process.wait()
        remove_fixtures(fixture_directory)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
