# OpenDoors issues

This file records behavioral and design problems found while exercising the
public interface. Acceptance tests continue to describe the current behavior;
an issue is not treated as a contract change until it is resolved deliberately.

## Initialization and terminal state

- [ ] Preserve explicitly configured virtual-screen dimensions during
  initialization. For every information type other than `CUSTOM`, `od_init()`
  assigns 80 columns and 23 rows before reading or bypassing the drop file.
  Consequently, a caller using local operation or `DIS_INFOFILE` cannot select
  a wider or taller virtual screen through `user_screenwidth` and
  `user_screen_length`; the values are reset even when the caller assigned
  them before initialization. Drop-file formats which provide dimensions may
  subsequently replace the defaults, making the behavior depend on how the
  session was initialized.

- [ ] Clear the communications handle when `od_exit()` frees it. A subsequent
  initialization failure may call `exit()`, which invokes
  the registered OpenDoors exit handler while `bODInitialized` is true. The
  handler then passes the stale communications handle left by the preceding
  session to `ODComClose()`. AddressSanitizer reports a heap use-after-free in
  this path. `od_exit()` sets the initialized flag to false before returning
  when `od_noexit` is enabled, so a later public API can currently enter this
  sequence.

- [ ] Define and enforce a one-session post-exit contract. Every ordinary
  public API currently calls `od_init()` when `bODInitialized` is false, so an
  API call made after a returning `od_exit()` implicitly starts another
  session. The historical `od_noexit` documentation promises only that the
  host application may continue after shutdown; it does not promise that
  OpenDoors can be initialized or used again. The recent API reference and
  `config_reinit_test` added a two-session guarantee which the older lifecycle
  was not designed to satisfy and which exposes retained and stale per-session
  state. Resolve this deliberately by rejecting every OpenDoors call after
  completed teardown, retaining `od_noexit` only to let the host perform
  non-OpenDoors work, and reconciling the recent documentation and tests with
  that lifetime rule.

## Kernel dispatch and shutdown

- [x] Dispatch Windows UI pending work at outermost API entry as well as exit.
  `ODSyncAPIEntry()` now dispatches only for the outermost call after acquiring
  the session writer. Exit dispatch remains in place for work queued while the
  operation runs.

- [ ] Do not resume a single-threaded kernel or its containing API after a
  returning forced shutdown. Carrier, timeout, inactivity, and local sysop
  handling call `od_exit()` synchronously from `od_kernel()`. When `od_noexit`
  is true, `od_exit()` frees the communications object, input queue, and screen
  state and returns, after which `od_kernel()` can continue draining input or
  call `ODKrnlTimeUpdate()` with the freed queue, and the outer API can continue
  as well. This is an internal violation of the rule that no OpenDoors
  operation may continue after teardown, even when the application made no
  post-exit API call.

- [ ] Report or prevent input-event loss when the common queue is full.
  `ODInQueueAddEvent()` returns `kODRCNoMemory` when the ring has no free slot,
  but `ODKrnlHandleReceivedChar()` ignores that result, silently dropping the
  local or remote character. A caller waiting for a terminator or complete
  response can consequently wait indefinitely. Also reconcile capacity: the
  implementation allocates 128 entries by default and the ring reserves one
  slot, while the current reference says that zero selects a capacity of 256
  events.

## Door-information files

- [ ] Give every binary `EXITINFO.BBS` record an explicitly defined,
  compiler-independent byte layout. `ODInEx.h` requests one-byte structure
  packing for MSVC and newer Turbo C, but not for GCC, Clang, MinGW, or
  Open Watcom DOS32. The reader nevertheless uses fixed historical byte counts
  such as 476, 1493, and 2363. Tests which create a fixture with the same
  private C structure can reproduce the compiler's layout and conceal the
  incompatibility; acceptance fixtures must instead be assembled from the
  documented on-disk offsets.

## Child processes

- [ ] Provide a platform-neutral way to pass child-process arguments.
  `od_spawnvpe()` retains its documented compatibility with the target C
  runtime's `spawnvpe()` interface. On Windows, that runtime serializes the
  array into a single command line without automatically protecting spaces;
  an element containing `argument one` therefore reaches a conventional C
  child as two arguments unless the caller includes the required quotation
  marks. Unix-like targets pass the elements directly as `argv`. Automatically
  quoting the established Windows interface would reinterpret strings from
  existing callers, so portable normalized argument handling would require a
  new API or an explicit opt-in rather than a silent behavior change.

## Windows thread lifecycle

- [x] Review `CreateThread()` use by workers which call the C runtime.
  The former frame, screen, kernel, and chat workers used runtime facilities
  after being created by `CreateThread()`. Windows worker creation was changed
  to `_beginthreadex()` with return through its runtime-managed wrapper. The
  later owner-flow and UI consolidation leaves only the frame worker, which
  retains that wrapper.

- [ ] Handle failure to deliver cooperative UI shutdown messages before
  waiting indefinitely for the target thread. `ODFrameShutdown()` ignores the
  results of `PostMessage(WM_OD_SHUTDOWN)` and its fallback
  `PostThreadMessage(WM_QUIT)`, then performs an infinite thread join. A stale
  window handle, missing message queue, or failed post can therefore turn a
  recoverable shutdown-delivery failure into a permanent process hang.

- [ ] Exercise the real Windows control lock under contention. The isolated
  Windows unit cases mock `WaitForSingleObject()` and manually change the lock
  predicate, while `windows_chat_failure_test` does not create competing
  control-lock readers or writers. Add repeated multi-reader wakeup, queued
  writer preference, entry/exit dispatch, and shutdown contention coverage
  using the actual Windows event and critical section so missed wakes,
  starvation, and lifecycle races are observable.

- [ ] Enforce owner-thread API entry in non-asserting Windows builds. Public
  API entry currently relies on `ASSERT(ODSyncIsOwnerThread())`, but
  `nAPILevel` and the physical-writer bookkeeping are process-global. A
  non-owner application thread entering concurrently in a release build can
  mistake the owner's active call for recursion, skip physical exclusion, and
  release or reacquire the owner's writer from a blocking path. The documented
  single-owner requirement should fail deterministically rather than corrupt
  synchronization state.

## Resolved during acceptance-suite development

- [x] Export `od_set_port()` from each Windows DLL ABI. The function was
  declared with `ODAPIDEF`, documented as public, and present in static
  libraries, but was absent from all three Windows module-definition files.
  An installed shared-library consumer therefore failed to link it. The new
  exports were appended without changing any established ordinal.
