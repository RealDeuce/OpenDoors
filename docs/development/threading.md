# Threading and session ownership

This document describes the internal threading design and the constraints
which must be preserved when OpenDoors is changed. It is intended for library
developers. Application authors should use the public rules in
[Windows programming](../guides/windows.md#threads-and-api-ownership) and the
[`od_control` lock reference](../reference/api/od_control_read_lock.md).

## Compatibility requirements

[OpenDoor.h](../reference/api/index.md) exposes one process-wide
[`od_control`](../reference/control/index.md) object and an API designed
around one active door session. The thread which calls
[`od_init()`](../reference/api/od_init.md) is the session-owner thread. Every
public function, public global, returned public pointer, and application
callback which is permitted to call OpenDoors belongs to that thread.

The implementation retains the public [`tODControl`](../reference/types.md#todcontrol)
layout, established calling conventions, entry points, data exports, and DLL
ordinals. Windows 98 and Windows XP remain valid legacy targets, so internal
threading uses facilities available in their Win32 and C runtime environments.

## Thread roles

All platforms use the same cooperative kernel flow. The session-owner thread
polls communications, detects carrier loss, interprets input, updates time and
status, runs chat mode, mutates terminal state, invokes application callbacks,
and performs initialization and shutdown. Public API boundaries and bounded
wait checkpoints call that kernel often enough to make progress without a
background kernel worker.

Windows retains two user-interface threads:

- The frame thread owns the top-level Win32 window and its message queue. It
  converts operator commands into pending owner-thread operations.
- The screen thread owns the screen child window and presents copies of the
  virtual screen. It reads the virtual image under the session-state read lock
  and releases that lock before making GDI calls.

While waiting for the screen thread to publish its child window, the frame
thread continues to dispatch messages. Child creation and resizing can
synchronously notify the parent, so a plain blocking wait on the frame thread
would deadlock that exchange. UI startup is published with events rather than
`volatile` polling. Shutdown requests both threads to stop, wakes their message
loops, joins them, and closes their handles; asynchronous thread cancellation
is not used.

`OD_THREAD_SUPPORT` is an internal build macro. It means the platform thread
and synchronization primitives needed by the Windows UI must be compiled; it
does not select a separate kernel implementation and is not a public feature
macro.

## Synchronization domains

The control lock is a writer-preferring read/write lock implemented from a
Win32 mutex and change event. An outer public API call holds its write side
while operating on shared session state. The screen presenter takes read
access only long enough to copy a coherent screen image.

The kernel-state mutex protects pending operations posted by the frame thread.
Code must not hold it while acquiring the control lock or invoking application
code. The input queue has its own mutex for indices, event storage, and
last-activity time, plus a counting semaphore for available events.
Communications-object serialization remains inside the communications module.

The public [`od_control`](../reference/control/index.md) locks are logical,
nestable owner-thread locks. A read lock may be nested under a write lock;
read-to-write promotion is rejected. On entry to an outer OpenDoors API call,
a held public lock is released and the API write lock is acquired. The public
lock is restored before the call returns.

## Owner dispatch and callbacks

The owner drains pending UI operations at the outermost API entry and exit and
at bounded blocking-call checkpoints. Pending bits represent chat, keyboard,
sysop-next, inactivity, time-limit, lockout, and shutdown requests. Dispatch
copies and clears those bits under the kernel-state mutex, then applies them
without holding that mutex. A shutdown request takes precedence over a chat
toggle from the same batch.

Time processing is cooperative and therefore already runs on the owner. Time
messages and their application callback are delivered synchronously in that
context. Owner callbacks, including chat and
[`od_ker_exec`](../reference/control/customization.md#od_ker_exec), may call the
OpenDoors API recursively. They should return promptly.

Windows help and configuration callbacks retain their established frame-thread
context. They must return promptly and must not access an OpenDoors API, ABI
object, or pointer. They may notify application-owned synchronization or queue
work for the session owner.

No application callback is invoked while the kernel-state or input-queue mutex
is held. A new callback must be assigned and documented as either an owner
callback or a restricted UI callback before implementation.

## Blocking calls and shutdown

Blocking input and modem-response waits use short bounded waits. The owner
releases the API writer before each queue or operating-system wait, reacquires
it afterward, dispatches pending UI work, and runs the cooperative kernel.
[`od_sleep()`](../reference/api/od_sleep.md) uses the same checkpoint. This
keeps remote input, carrier state, timers, and UI commands moving while a
public call waits.

Process creation similarly releases the complete nested API level while a
child executes, then restores it before touching session state. There are no
kernel workers to stop or restart around [`od_spawnvpe()`](../reference/api/od_spawnvpe.md).

[`od_exit()`](../reference/api/od_exit.md) stops and joins the Windows UI
threads before releasing the input queue, communications object, or virtual
screen. The cooperative kernel owns no per-run thread resources.

## Review checklist

When adding asynchronous activity or shared state, verify all of the following:

1. Work that can run cooperatively stays in the shared owner-thread kernel.
2. A necessary UI thread has one owner for its handle and one cooperative stop
   path; partial startup unwinds every resource already created.
3. A UI thread never invokes public API or owner-thread application code.
4. Cross-thread work is represented as data and dispatched by the owner.
5. No lock is held while sleeping, waiting for input, performing communications
   I/O, painting, or joining a thread.
6. Application callbacks never run under the kernel-state or input-queue mutex.
7. The implementation compiles with current MSVC and MinGW and with the 16-bit
   Open Watcom build wherever the code is not platform-guarded.
