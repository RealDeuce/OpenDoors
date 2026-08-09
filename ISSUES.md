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

- [ ] Review `CreateThread()` use by workers which call the C runtime.
  Frame, screen, kernel, and chat workers allocate memory, perform formatted
  I/O, and use other runtime facilities after being created by
  `CreateThread()`. Determine whether each supported Microsoft and MinGW
  runtime requires `_beginthreadex()`/`_endthreadex()` for correct per-thread
  runtime initialization and cleanup, including the static-runtime builds.

## Resolved during acceptance-suite development

- [x] Export `od_set_port()` from each Windows DLL ABI. The function was
  declared with `ODAPIDEF`, documented as public, and present in static
  libraries, but was absent from all three Windows module-definition files.
  An installed shared-library consumer therefore failed to link it. The new
  exports were appended without changing any established ordinal.
