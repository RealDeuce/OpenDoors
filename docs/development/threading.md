# Threading and session ownership

This document describes the internal threading design. Application authors
should use the public rules in
[Windows programming](../guides/windows.md#threads-and-api-ownership).

## Compatibility requirements

The public API has one process-wide [`od_control`](../reference/control/index.md)
object and one active door session. The thread which calls
[`od_init()`](../reference/api/od_init.md) owns every public function, public
global, returned public pointer, and owner callback. The implementation must
retain the `tODControl` layout and established calling conventions, including
support for legacy Win32 and 16-bit DOS targets.

## Thread roles

All platforms use the same cooperative kernel flow. The session owner polls
communications, detects carrier loss, interprets input, updates time and
status, runs chat mode, mutates terminal state, invokes owner callbacks, and
performs initialization and shutdown.

Windows retains one library UI thread. It owns the top-level window, screen
child, and their message queue; converts operator commands to pending
owner-thread operations; and paints published screen generations. Startup and
shutdown use events and a cooperative message-loop stop followed by a join.

`OD_THREAD_SUPPORT` is an internal build macro selecting the thread primitives
needed by the Windows UI. It does not select a separate kernel implementation.

## Synchronization domains

The owner is the sole accessor of `od_control`. The Windows UI reads a cache
containing only its required fields. At an outer API entry, exit, or blocking
checkpoint, the owner drains the pending UI FIFO into `od_control` in order and
then refreshes that cache. The cache and FIFO head and tail are protected by
the kernel-state mutex.

Windows screen state has two complete buffers. The owner mutates the
application buffer and records one dirty bit. At outer API exit, or before a
blocking checkpoint, it exchanges buffers under the presentation mutex and
invalidates the screen child. `WM_PAINT` holds only that mutex while reading an
immutable display generation.

The input queue mutex protects its indices, event storage, last-activity time,
and last list-control key. Its semaphore counts available events.
Communications serialization remains inside the communications module.

## Owner dispatch and callbacks

Frame requests are nodes in an ordered linked FIFO: chat, keyboard,
sysop-next, inactivity, time-limit, lockout, or shutdown. Dispatch detaches the
FIFO under the kernel-state mutex, then applies and frees nodes without holding
the mutex. Requests are neither coalesced nor reordered.

Time processing and normal application callbacks run synchronously on the
owner. Windows help and configuration callbacks retain their established
frame-thread context. They block the frame message loop while running and must
not access any OpenDoors API, ABI object, or returned pointer. They may notify
application-owned synchronization or queue work for the owner.

No callback runs while the kernel-state or input-queue mutex is held.

## Blocking calls and shutdown

Blocking input and modem waits use short bounded waits. The owner temporarily
drops its API nesting level, waits, restores that level, dispatches pending UI
work, and runs the cooperative kernel. Process creation uses the same nesting
release and restore around child execution.

[`od_exit()`](../reference/api/od_exit.md) stops and joins the Windows UI before
destroying the UI FIFO mutex, input queue, communications object, or virtual
screen.

## Review checklist

1. Cooperative work stays in the shared owner-thread kernel.
2. Cross-thread work is represented as data and dispatched by the owner.
3. The UI thread never invokes owner-thread API or owner callbacks.
4. No kernel-state or input-queue mutex is held while waiting, painting,
   communicating, joining a thread, or invoking application code.
5. New platform-guarded code remains compilable by every applicable legacy
   toolchain.
