# Threading and session ownership

This document describes the internal threading design and the constraints
which must be preserved when OpenDoors is changed. It is intended for library
developers. Application authors should use the public rules in
[Windows programming](../guides/windows.md#threads-and-api-ownership) and the
[`od_control` lock reference](../reference/api/od_control_read_lock.md).

## Compatibility requirements

[OpenDoor.h](../reference/api/index.md) exposes one process-wide
[`od_control`](../reference/control/index.md) object and an API designed
around one active door session. The Windows implementation has historically
used helper threads, but those helpers do not turn the public interface into a
multi-caller API. The thread which calls
[`od_init()`](../reference/api/od_init.md) is the session-owner
thread. Every public function, public global, returned public pointer, and
application callback which is permitted to call OpenDoors belongs to that
thread.

The redesign retains the public
[`tODControl`](../reference/types.md#todcontrol) layout, all established calling
conventions, and all existing entry points and data exports. The four
[`od_control_read_lock()`](../reference/api/od_control_read_lock.md),
[`od_control_read_unlock()`](../reference/api/od_control_read_unlock.md),
[`od_control_write_lock()`](../reference/api/od_control_write_lock.md), and
[`od_control_write_unlock()`](../reference/api/od_control_write_unlock.md) are
appended API additions. Windows DLL
ordinals are never renumbered. Windows 98 and Windows XP remain valid legacy
targets, so implementation code is restricted to facilities available in
their Win32 and C runtime environments.

## Thread roles

The session-owner thread performs terminal emulation, input interpretation,
screen mutation, timer policy, chat mode, callbacks which may call OpenDoors,
and all initialization and shutdown work.

Windows uses three cooperative kernel workers:

- The remote-input worker polls the communications object and appends complete
  input events to the synchronized input queue.
- The carrier worker polls connection state and schedules a shutdown request.
- The timer worker schedules a time update. It does not modify
  [`od_control`](../reference/control/index.md),
  display text, call application code, or perform shutdown itself.

The frame and screen threads own their Win32 windows and message queues. They
post application-affecting commands to the owner dispatcher. The screen
thread reads the virtual local-screen image under the session-state read lock;
it never changes terminal state.

No worker is stopped with `TerminateThread()`, `SuspendThread()`, or an
equivalent asynchronous cancellation operation. `_beginthreadex()` creates
Windows threads which use the C runtime. Shutdown sets a stop request, wakes
all waiters, joins each thread, and then closes its handle. User-interface
startup is published with events rather than `volatile` polling. A startup
deadline reports failure, after which cooperative cleanup still joins the
thread rather than abandoning it with access to session state.

## Synchronization domains

The control lock is a writer-preferring read/write lock implemented from a
mutex and a change event or condition variable. An outer public API call holds
its write side while it operates on shared session state. Internal workers
take only the narrow read access required to make a decision. The Windows
screen presenter takes read access while copying cells for a paint.

The kernel-state mutex protects stop and pending-operation flags. Code must
not hold it while acquiring the control lock or invoking application code.
The input queue has its own mutex for indices, event storage, and last-activity
time, plus a counting semaphore for available events. Communications-object
serialization remains inside the communications module.

The public [`od_control`](../reference/control/index.md) locks are logical,
nestable owner-thread locks. A read
lock may be nested under a write lock. Read-to-write promotion is rejected.
On entry to an outer OpenDoors API call, a held public lock is released and the
API write lock is acquired; the public lock is restored before the call
returns. This ordering prevents a caller from deadlocking itself and makes an
API call inside a public locked region an explicit synchronization point.

## Owner dispatch and callbacks

Workers communicate through pending bits and a shutdown reason. The owner
checks pending work after an outer API call and at bounded waits used by input
and sleep functions. At an outer API exit, previously deferred application
time-message callbacks are delivered first. Operator changes captured by the
Windows frame are then applied, followed by forced shutdown, timer work, and
chat toggle. Only the owner performs those operations.

There are two callback contexts:

- Owner callbacks, including time-message, chat, and
  [`od_ker_exec`](../reference/control/customization.md#od_ker_exec) callbacks,
  execute on the owner thread and may call the OpenDoors API recursively.
  Built-in timer work
  may run at a blocking-call checkpoint, but an application time-message
  callback is queued until the active outer API call has returned through its
  API boundary.
- The Windows help and configuration callbacks retain their established frame
  thread context. They must return promptly and must not access any OpenDoors
  API, ABI object, or pointer. They may notify application-owned synchronization
  or queue work for the session-owner thread.

No application callback is invoked while the kernel-state mutex or input-queue
mutex is held. A function which adds a new callback must classify it into one
of these contexts and document the classification before implementation.

## Blocking calls and shutdown

Blocking input waits use short bounded queue waits. Between waits the owner
temporarily gives up the API writer, dispatches pending work, and reacquires
the writer before touching session state again.
[`od_sleep()`](../reference/api/od_sleep.md) uses the same
checkpoint. This permits carrier loss, time expiration, and local commands to
be handled without suspending the owner at an arbitrary instruction.

[`od_spawnvpe()`](../reference/api/od_spawnvpe.md) with
[`P_WAIT`](../reference/constants/general.md#p_wait) stops and joins the kernel
workers before closing the communications object. It releases the API writer
while the child runs,
then reacquires it and restarts the workers. Pending shutdown and chat requests
are preserved across that restart.

[`od_exit()`](../reference/api/od_exit.md) stops and joins kernel and UI threads
before it releases the input queue, communications object, or virtual screen.
With [`od_noexit`](../reference/control/customization.md#od_noexit) enabled, the
same complete teardown occurs and the caller resumes only after
`bODInitialized` has been cleared. A subsequent session does not inherit a
live worker from the preceding session.

## Optional pthread validation backend

`OPENDOORS_ENABLE_PTHREAD_KERNEL` is disabled by default. When enabled on a
Unix-like build, it compiles the same kernel-worker and synchronization paths
using only standard POSIX thread operations: thread creation and joining,
mutexes, condition variables, and thread identity comparison. It uses no
non-portable thread naming, timed-join, native-handle, semaphore, cancellation,
or scheduling extension.

The backend is a design validator rather than a claim that Unix OpenDoors
applications are generally threaded. CI exercises lock contention, rejected
non-owner access, cooperative worker shutdown, pending timer dispatch, nested
public locks, and callback execution on the owner thread. New synchronization
work should pass both the ordinary non-threaded Unix suite and this backend
before Windows acceptance tests are run.

## Review checklist

When adding worker activity or shared state, verify all of the following:

1. The worker has one owner for its handle and one cooperative stop path.
2. Partial startup unwinds every worker and support object already created.
3. The worker never invokes public API or application code.
4. Cross-thread work is represented as data and dispatched by the owner.
5. No lock is held while joining a thread. Application callbacks never run
   under the kernel-state or input-queue mutex; owner callbacks run within the
   recursive API boundary established by the control writer.
6. [`od_noexit`](../reference/control/customization.md#od_noexit), failed
   initialization, and child-process restart leave no
   worker using released session state.
7. The implementation compiles with current MSVC and MinGW, the standard
   pthread validation backend, and the 16-bit Open Watcom build where the code
   is not platform-guarded.
