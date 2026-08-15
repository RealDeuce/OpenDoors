#!/usr/bin/env python3
# SPDX-License-Identifier: LGPL-2.0-or-later
"""Transport-neutral protocol for public OpenDoors acceptance doors."""

from __future__ import annotations

import select
import socket
import time
from pathlib import Path


SCENARIOS = ("input", "interactive", "display", "session")

FIXTURES = {
    "ODFILE.ASC": b"FILE-CONTENT\r\n",
    "ODFILE.ans": b"ANSI-CONTENT\r\n",
    "ODSECT.ASC": (
        b"@#FIRST\r\nWRONG-SECTION\r\n"
        b"@#SECOND\r\nSECTION-CONTENT\r\n"
    ),
    "ODMENU.ASC": b"MENU-CONTENT\r\n",
    "ODLONG.ASC": b"LONG-MENU-" + b"0123456789" * 80 + b"\r\n",
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
            raise RuntimeError(
                f"timed out waiting for {marker!r}; got {bytes(transcript)!r}"
            )
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


def finish(peer: socket.socket, transcript: bytearray, marker: bytes,
           timeout: int) -> None:
    receive_until(peer, marker, transcript, timeout)
    peer.sendall(b"X")


def drive_input(peer: socket.socket, timeout: int) -> bytearray:
    transcript = bytearray()
    receive_until(peer, b"INPUT-CHAR", transcript, timeout)
    peer.sendall(b"Q")
    receive_until(peer, b"INPUT-CTRL-NORMAL", transcript, timeout)
    peer.sendall(b"\x05")
    receive_until(peer, b"INPUT-CTRL-RAW", transcript, timeout)
    peer.sendall(b"\x05")
    receive_until(peer, b"INPUT-SEQUENCE", transcript, timeout)
    peer.sendall(b"\x1b[A")
    receive_until(peer, b"INPUT-RAW-SEQUENCE", transcript, timeout)
    peer.sendall(b"\x1b[A")
    receive_until(peer, b"INPUT-LINEFEED", transcript, timeout)
    peer.sendall(b"\nR")
    receive_until(peer, b"INPUT-BURST", transcript, timeout)
    peer.sendall(
        b"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-"
    )
    receive_until(peer, b"INPUT-CLEAR", transcript, timeout)
    peer.sendall(b"XYZ")
    finish(peer, transcript, b"INPUT-DONE", timeout)
    return transcript


def drive_interactive(peer: socket.socket, timeout: int) -> bytearray:
    transcript = bytearray()
    receive_until(peer, b"INTERACTIVE-ANSWER", transcript, timeout)
    peer.sendall(b"xY")
    receive_until(peer, b"INTERACTIVE-STRING", transcript, timeout)
    peer.sendall(b"ab\bCDEFG\r")
    receive_until(peer, b"INTERACTIVE-HIGH", transcript, timeout)
    peer.sendall(b"\x7f\x80\xff\r")
    receive_until(peer, b"INTERACTIVE-EDIT", transcript, timeout)
    peer.sendall(b"ab1c\r")
    receive_until(peer, b"INTERACTIVE-EDIT-CANCEL", transcript, timeout)
    peer.sendall(b"\x1b")
    receive_until(peer, b"INTERACTIVE-MULTILINE", transcript, timeout)
    peer.sendall(b"Hi\rThere\x1a")
    receive_until(peer, b"INTERACTIVE-POPUP-HOTKEY", transcript, timeout)
    peer.sendall(b"s")
    receive_until(peer, b"INTERACTIVE-POPUP-CANCEL", transcript, timeout)
    peer.sendall(b"\x1b")
    receive_until(peer, b"INTERACTIVE-POPUP-KEEP", transcript, timeout)
    peer.sendall(b"l")
    receive_until(peer, b"INTERACTIVE-PAGE-CANCEL", transcript, timeout)
    peer.sendall(b"\r")
    receive_until(peer, b"INTERACTIVE-PAGE-DISABLED", transcript, timeout)
    peer.sendall(b"reason\r\r")
    receive_until(peer, b"INTERACTIVE-CHAT", transcript, timeout)
    peer.sendall(b"hello\r")
    finish(peer, transcript, b"INTERACTIVE-DONE", timeout)
    return transcript


def drive_display(peer: socket.socket, timeout: int) -> bytearray:
    transcript = bytearray()
    receive_until(peer, b"DISPLAY-OUTPUT", transcript, timeout)
    receive_until(peer, b"DISPLAY-HOTKEY-WAIT", transcript, timeout)
    receive_until(peer, b"MENU-CONTENT", transcript, timeout)
    peer.sendall(b"2")
    receive_until(peer, b"DISPLAY-HOTKEY-EARLY", transcript, timeout)
    peer.sendall(b"AQ")
    finish(peer, transcript, b"DISPLAY-DONE", timeout)
    for expected in (
        b"DISP-STRING-RRR-PRINTF-7",
        b"-EMU-",
        b"ANSI-CONTENT",
        b"SECTION-CONTENT",
        b"ITEM.TXT",
        b"MENU-CONTENT",
    ):
        if expected not in transcript:
            raise RuntimeError(
                f"display transcript lacks {expected!r}: {bytes(transcript)!r}"
            )
    return transcript


def drive_session(peer: socket.socket, timeout: int) -> bytearray:
    transcript = bytearray()
    receive_until(peer, b"SESSION-AUTODETECT", transcript, timeout)
    receive_until(peer, b"\x1b[6n", transcript, timeout)
    peer.sendall(b"\x1b[")
    receive_until(peer, b"\x1b[!", transcript, timeout)
    peer.sendall(b"rIp12345678901")
    receive_until(peer, b"SESSION-AUTODETECT-KNOWN", transcript, timeout)
    receive_until(peer, b"SESSION-AUTODETECT-FAIL", transcript, timeout)
    receive_until(peer, b"SESSION-TIMER-ARMED", transcript, timeout)
    receive_until(peer, b"TIME-MESSAGE", transcript, timeout + 3)
    peer.sendall(b"Z")
    receive_until(peer, b"SESSION-DISCONNECT", transcript, timeout)
    peer.shutdown(socket.SHUT_RDWR)
    return transcript


DRIVERS = {
    "input": drive_input,
    "interactive": drive_interactive,
    "display": drive_display,
    "session": drive_session,
}


def drive_scenario(peer: socket.socket, scenario: str,
                   timeout: int = 10) -> bytearray:
    try:
        driver = DRIVERS[scenario]
    except KeyError as error:
        raise ValueError(f"unknown transport scenario: {scenario}") from error
    return driver(peer, timeout)
