# OpenDoors issues

This file records behavioral and design problems found while exercising the
public interface. Acceptance tests continue to describe the current behavior;
an issue is not treated as a contract change until it is resolved deliberately.

## Initialization and terminal state

- [x] Preserve explicitly configured virtual-screen dimensions during
  initialization. `od_init()` now applies the 80-column and 23-row defaults
  independently only when the corresponding field is zero. Drop-file formats
  which provide dimensions may still replace those initial values.

- [x] Clear the communications and input-queue handles when `od_exit()` frees
  them. Shutdown now clears both handles before entering the permanent terminal
  state, so neither can be reused by a later path.

- [x] Define and enforce a one-session post-exit contract. Public entry now
  rejects pending, finalizing, and completed sessions, and `od_noexit` only
  permits the host to continue with non-OpenDoors work. The erroneous
  two-session test and documentation guarantee were removed.

## Kernel dispatch and shutdown

- [x] Dispatch Windows UI pending work at outermost API entry as well as exit.
  `ODSyncAPIEntry()` now dispatches only for the outermost call after acquiring
  the session writer. Exit dispatch remains in place for work queued while the
  operation runs.

- [x] Do not resume a kernel or its containing API after a returning forced
  shutdown. Nested no-exit shutdown now becomes pending, cooperative waits
  unwind, and the outermost API-exit boundary performs teardown exactly once.

- [x] Prevent input-event loss when the common queue is full. Remote input now
  reserves capacity before reading from the communications source, allowing
  its existing backpressure to retain unread bytes. Local overflow rejects an
  entire ordinary or extended key and rings the local bell. The default ring
  now allocates 256 entries, providing the documented usable capacity of 255
  events after its reserved slot.

## Door-information files

- [x] Give every binary `EXITINFO.BBS` record an explicitly defined,
  compiler-independent byte layout. The three on-disk records now use the
  byte-packed DOS layout on every compiler, without changing the alignment of
  unrelated internal structures, and all multibyte fields are read and written
  as little-endian values. Acceptance fixtures use fixed historical byte counts
  and offsets rather than reproducing the host compiler's structure layout.

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

- [x] Handle failure to deliver cooperative UI shutdown messages before
  waiting for the target thread. A failed asynchronous window post now falls
  back to a bounded synchronous send and then a checked thread-queue post.
  `ODFrameShutdown()` only joins after one of those delivery paths succeeds;
  if every path fails, it preserves the live thread and its handles instead of
  entering an impossible infinite wait.

- [x] Remove the obsolete Windows control-lock contention concern. The session
  reader/writer control lock, including its event and writer-preference
  machinery, was removed when control data became application-flow-owned and the Windows
  UI worker was changed to consume a cache and deferred update queue. There is
  consequently no longer a real control lock whose reader/writer behavior
  needs the proposed integration test.

- [x] Replace permanent Windows owner-thread affinity with caller
  serialization. OpenDoors no longer records the thread which initialized the
  session; a serialized application may hand subsequent calls to another
  thread. Applications must use one lock for every API call, public global,
  and returned pointer. The Windows frame thread now uses explicit request
  queues, while overlapping application calls remain an unsupported contract
  violation rather than a partially enforced special case.

## Resolved during acceptance-suite development

- [x] Export `od_set_port()` from each Windows DLL ABI. The function was
  declared with `ODAPIDEF`, documented as public, and present in static
  libraries, but was absent from all three Windows module-definition files.
  An installed shared-library consumer therefore failed to link it. The new
  exports were appended without changing any established ordinal.
