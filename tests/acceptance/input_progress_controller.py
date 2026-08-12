#!/usr/bin/env python3
"""Verify owner-side timer and socket progress during a blocking input call."""

from __future__ import annotations

import argparse
import select
import socket
import subprocess
import time


def receive_until(peer: socket.socket, marker: bytes, transcript: bytearray) -> None:
    deadline = time.monotonic() + 12
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


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--launcher", action="append", default=[])
    parser.add_argument("door")
    args = parser.parse_args()

    listener = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    listener.bind(("127.0.0.1", 0))
    listener.listen(1)
    listener.settimeout(10)
    port = listener.getsockname()[1]
    process = subprocess.Popen([*args.launcher, args.door, str(port)])
    peer = None
    try:
        peer, _ = listener.accept()
        transcript = bytearray()
        receive_until(peer, b"ARMED", transcript)
        receive_until(peer, b"TIME", transcript)
        peer.sendall(b"Z")
        receive_until(peer, b"DONE", transcript)
        return_code = process.wait(timeout=10)
        if return_code != 0:
            raise RuntimeError(f"input-progress door exited with {return_code}")
    finally:
        listener.close()
        if peer is not None:
            peer.close()
        if process.poll() is None:
            process.kill()
            process.wait()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
