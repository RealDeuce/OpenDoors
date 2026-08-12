# OpenDoors TODO

## Defects found during unit-test implementation

- [ ] Report no byte for a nonblocking socket receive that would block. In
  `ODComGetByte()`, `recv()` returning `SOCKET_ERROR` with
  `WSAEWOULDBLOCK` leaves the retry loop when `bWait` is false, then falls
  through as `kODRCSuccess` without storing a byte. Preserve this existing
  behavior until a test-driven fix establishes the intended result.

- [ ] Do not write stdio output after its readiness wait expires. In
  `ODComSendByte()`, ten consecutive `select()` timeouts currently leave
  `retval` at -1, exit the bounded loop, and proceed to `fwrite()` even though
  stdout was never reported writable. Preserve this existing behavior until a
  test-driven fix defines and verifies the intended timeout result.

- [ ] Initialize the complete `struct tm` in `DOSToCTime()`. The non-Turbo
  implementation assigns the six encoded calendar fields but leaves
  `tm_isdst` indeterminate before calling `mktime()`, so daylight-saving
  interpretation can depend on uninitialized automatic storage.

- [ ] Return an OpenDoors result code from Unix `ODFileDelete()`. The Unix
  branch currently returns `unlink()` directly, producing `-1` on failure
  instead of the documented `kODRCGeneralFailure` used by the other platform
  implementations.

- [x] Correct the pthread `ODThreadSleep()` interruption test. POSIX
  `nanosleep()` reports interruption by returning `-1` and setting `errno` to
  `EINTR`; the current loop compares the return value itself with `EINTR`, so
  an interrupted sleep returns early instead of resuming with the remaining
  interval.

- [x] Place `static` before `const` in `ODCom.c`'s
  `cp437_unicode_table` declaration. MinGW diagnoses the current
  `const static` order with `-Wold-style-declaration`, and because the table
  is file-scope state in every isolated `ODCom.c` unit, `-Werror` prevents
  every such Windows unit from compiling before its selected function runs.

- [x] Resolve the unit runner's default build path before creating it.
  [`unit/tools/run.py`](unit/tools/run.py) currently calls `mkdir()` on the
  optional `--build` value before replacing an omitted value with the
  documented platform-specific default, so every documented invocation that
  omits `--build` fails before compiling a test.

- [x] Split exact Turbo unit execution around a host WASM assembly pass.
  DOS JWasm 2.20 could write an object and then run indefinitely under
  DOSBox for otherwise valid generated assembly. Turbo C now emits and
  normalizes every selected assembly file in one DOSBox pass, host Open
  Watcom WASM creates the OMF objects, and a second DOSBox pass links and
  executes them with Turbo C.

- [x] Avoid function-local `static const` arrays in Turbo-compatible tests.
  Turbo C 2.01 generated references relative to its BSS base while emitting
  the initializer bytes in the data segment, so every element read as zero.
  The affected kernel switch test now spells out its eight inputs while
  preserving the same branch coverage.

- [x] Narrow the repeated initialized-state term in deferred timer dispatch.
  On the pthread path, `ODKrnlDispatchPending()` already returns when
  OpenDoors is not initialized and makes no intervening call capable of
  clearing that state before timer dispatch, so the duplicate term cannot
  independently affect the decision. The Windows frame-update callbacks can
  end the session and therefore still require the later guard.

- [ ] Preserve a received UART byte while lowering RTS in
  `ODComInternalISR()`. The receive-data path reused its byte temporary to
  read the modem-control register at the high-water mark, then enqueued that
  modem-control value instead of the byte received from the UART. The received
  byte needs separate storage from subsequent register operations.

- [ ] Store the interrupt-vector segment returned by DOS in
  `ODComGetVect()`. The Borland inline-assembly path stored `BX` into both
  words of its far function pointer instead of storing the `ES:BX` returned
  by DOS interrupt 21h function 35h. The exact Turbo C regression test
  installs, obtains, compares, and restores a disposable interrupt vector.

- [x] Analyze Turbo C unit tests with the exact Turbo preprocessor path. The
  Turbo runner currently recompiles translation units generated from a Watcom
  16-bit analysis. A function containing `__WATCOMC__` alternatives can
  therefore be instrumented as the Watcom implementation but compiled as the
  Borland implementation, invalidating both its mocks and coverage model. The
  Turbo path must generate from Turbo-compatible definitions and route any
  selected inline assembly through the configured WASM/TASM-compatible step.

- [x] Analyze Windows unit tests with Windows platform definitions. The unit
  runner previously generated isolated translation units using a host-UNIX
  Clang AST and then compiled them with MinGW, so Windows-only functions and
  dependencies could remain active but unisolated. The analyzer now uses a
  GNU Windows target, with a regression requiring the corresponding target
  flag before either MinGW architecture is compiled.

- [x] Do not replay cross-target coverage through the host Unix preprocessor.
  A Windows-modeled unit retained platform guards and target-only coverage
  probes, but the independent LLVM executable was then compiled and run as
  Unix. It executed different, uninstrumented branches and could neither test
  nor satisfy the Windows coverage model. LLVM coverage is now limited to the
  host-compatible Unix configurations; Windows and DOS retain strict branch
  and MC/DC gating through portable probes executed by their target compiler.

- [x] Preserve DOS unit-test assertion output on failure. The Watcom runner
  previously left stdout on the emulated display and retained only a `.BAD`
  marker, so a failed CI test identified the function but not its assertion.
  Each DOS test now writes a separate `.OUT` file and the runner includes its
  contents in the aggregated failure report.

- [x] Keep Turbo unit configuration out of its 126-byte DOS command tail.
  Repeating several manifest `-D`/`-U` options after generating an already
  configured translation unit truncated the TCC command before its filename,
  yielding the misleading error “No file names given.” Configuration defines
  and undefines are now reasserted in the generated source; only a Turbo-shard
  selector, when needed, remains on the compiler command line.

- [x] Preserve and require LLVM MC/DC records for instrumented nested boolean
  expressions. The portable condition probes currently turn expressions such
  as `active && !(inside && owner)` into a form for which Clang declines MC/DC
  mapping, and the LLVM coverage gate does not reject a missing MC/DC record.
  Portable MC/DC remains green, but the intended independent LLVM oracle is
  absent at those sites.
  The LLVM gate now compares mapped records with every modeled compound
  decision and reports, waives, or fails absent records explicitly.

- [ ] Discard every fragment of an overlong `FILES.BBS` line in
  `od_list_files()`. When `ODStringNormalizeLine()` reports the first truncated
  fragment as incomplete, [`ODList.c`](ODList.c) marks later fragments for
  suppression but continues parsing and displaying the first fragment. This
  can display a fabricated entry and perform a filesystem lookup using an
  incomplete filename token.

- [ ] Preserve the first eight characters of an overlength undotted filename
  in `ODListFilenameSplit()`. [`ODList.c`](ODList.c) currently reports the
  filename component as present but copies zero characters when the base name
  exceeds eight characters and has no extension. The equivalent dotted case
  retains the first eight characters, so an undotted entry can resolve to an
  empty filename instead of receiving the same eight-character truncation.

## Defects found during DOS32 serial acceptance testing

- [x] Give the DOS find-first/find-next transfer area its required packed
  layout on DOS32. Open Watcom's normal flat-model alignment inserted two
  bytes before the file-size field, so the runtime interpreted the first two
  filename characters as part of the size and returned a filename beginning
  at its third character. The private structure now has an explicit 43-byte
  DOS layout and a compile-time size check.

## Defects found during Windows acceptance testing

- [x] Replace forced Windows thread termination and suspension with
  cooperative synchronization. Remote-input, carrier-detection, and timer
  workers now observe a shared stop request, are woken through a semaphore,
  and are joined before their handles and session state are released. Chat and
  shutdown work is dispatched by the session owner; no path uses
  `TerminateThread()` or `SuspendThread()`.

- [x] Synchronize Windows worker access to shared OpenDoors state. A
  writer-preferring session-state lock, kernel-state mutex, and synchronized
  input queue now cover their separate ownership domains. Frame commands and
  worker notifications are queued for the session owner, and the screen
  presenter takes shared access while reading the virtual local screen.

- [x] Give every Windows thread handle an explicit owner and complete
  lifecycle. Kernel initialization owns the three worker handles and kernel
  shutdown joins and closes each one. Frame shutdown owns and closes the frame
  and screen handles after their cooperative message-loop stop; chat no longer
  creates a separate thread.

- [x] Propagate and unwind partial Windows worker initialization.
  `ODInit()` ignores the results from both `ODFrameStart()` and
  `ODKrnlInitialize()`. Failures while allocating the activation semaphore,
  creating a later worker, or assigning its support objects return without
  stopping earlier workers or releasing handles. `ODFrameCreateWindow()` also
  ignores failure to start its screen worker, and `ODScrnStartWindow()` leaks
  its startup record when thread creation fails. A subsequent initialization
  can overwrite the only references to the surviving resources. Initialization
  now reports frame and kernel startup failure, unwinds workers in reverse
  order, frees failed screen startup records, and stops the kernel when frame
  startup fails.

- [x] Synchronize publication and shutdown of the frame and screen threads.
  The frame window, screen-thread handle, thread IDs, and programmatic-shutdown
  flag are shared between threads through plain or `volatile` objects.
  `volatile` does not establish ordering or atomic ownership, and the current
  shutdown path polls those objects indefinitely while retrying
  `PostThreadMessage()`. Replace the polling protocol with explicit startup
  and shutdown events and define a bounded failure path. Startup events now
  publish message queues, window handles, and results. A startup deadline
  reports failure; cooperative cleanup still joins the affected thread so it
  cannot outlive the session state. Shutdown posts a cooperative message and
  joins without volatile polling.

- [x] Close Win32 semaphore handles with `CloseHandle()`.
  `ODSemaphoreFree()` currently passes a kernel semaphore handle to the GDI
  `DeleteObject()` function. `ODSemaphoreFree()` now closes the kernel handle
  with `CloseHandle()`.

- [x] Make forced shutdown safe when `od_noexit` is enabled. A kernel worker
  sets `bKernelActive` before calling `od_exit()`, causing `ODKrnlShutdown()`
  to return without stopping any kernel threads. Ordinarily the process exits
  immediately and masks the incomplete teardown; with `od_noexit`, execution
  can continue after queues, communications, and screen resources have been
  released while workers still reference them. Workers now schedule shutdown
  for the owner, and `od_exit()` cooperatively joins kernel and UI threads
  before releasing queues, communications, or screen state and returning.

- [x] Application callbacks invoked by Windows worker threads can access the
  OpenDoors API and `od_control` concurrently with the session-owner thread.
  Workers no longer invoke application callbacks. Time-message and chat
  callbacks are dispatched on the session-owner thread; the established Windows help
  and configuration callbacks remain on the frame thread and are documented
  as unable to access the OpenDoors API or ABI.

- [x] Stop the Windows kernel timer cooperatively before tearing down the
  session. It was previously killed with `TerminateThread()` without a join,
  allowing `od_exit()` to free input and screen state while the timer still
  used it. The timer now waits on a shutdown event and its handle is joined
  and closed before teardown continues.

- [x] Allow Windows `od_spawn()` to launch executable paths longer than 79
  characters. The program-name split used a fixed 80-byte local buffer, so a
  valid longer path was truncated before `_spawnvpe()` attempted to find it.
  The split now allocates exactly enough internal storage without changing
  the established first-space parsing behavior.

- [x] Balance explicit `od_kernel()` calls in multithreaded builds. The
  multithreaded conditional omitted the API-exit operation and reentrancy-flag
  reset, so the first explicit call left the API boundary permanently active
  and made later calls return immediately. Both operations now occur on every
  platform, with Windows and pthread regressions covering repeated calls.

- [x] Give asynchronous Windows message-window text a defined owner and
  ordering. `ODScrnShowMessage()` posted a borrowed string pointer, while
  `ODScrnRemoveMessage()` sent its command synchronously and could overtake the
  earlier post. The show request now owns a text copy, show and removal use the
  same posted-message order, and the frame thread releases the copy after the
  modal window closes.

- [x] Size the Windows main-status snapshot for every public source field.
  The fixed 160-byte buffer could not hold maximum-length caller name,
  location, chat reason, and surrounding text together. Its size is now
  derived from those `od_control` arrays with room for the fixed decoration.

- [x] Keep communications internals out of the public API boundary. Socket
  retry paths in `ODComGetByte()` called `od_sleep()`, so the remote-input
  worker could enter the owner-only API while handling a transient
  `WSAEWOULDBLOCK`; modem-control pauses could also dispatch after
  `od_spawnvpe()` had deliberately released its API boundary. Multithreaded
  communications waits now use an internal thread sleep which performs no
  owner dispatch; blocking owner operations also stop promptly if a
  checkpoint completed session teardown.

- [x] Release the session-state lock around blocking owner-thread operations.
  Blocking input, sleep, communications output, modem-response, process,
  hangup, output-drain, shutdown, and Windows paint paths previously retained
  the API writer or presentation reader while waiting. Owner operations now
  save and restore their complete nested API level around the blocking call;
  only established input and sleep checkpoints dispatch pending work. The
  Windows presenter draws from a coherent screen snapshot after releasing its
  reader, and internal assertions reject raw waits entered with the owner
  writer held.

## Defects found during the documentation audit

- [x] Correct the popup-menu level bounds check in `od_popup_menu()`.
  `MenuLevelInfo` has eleven elements (indices 0 through 10). Level 11 is now
  rejected before it can access one element beyond the array.

- [x] Report failure of the popup menu-item allocation as `ERR_MEMORY`.
  [`ODPopup.c`](ODPopup.c) now reports failure to allocate the `tMenuItem`
  array consistently with failure of the later saved-screen allocation.

- [x] Release popup menu-item storage when menu creation fails.
  [`ODPopup.c`](ODPopup.c) now retains ownership locally until menu creation
  succeeds and releases both menu-item and saved-screen storage on failure.

- [x] Ensure `od_printf()` restores its color-parser state on every return.
  [`ODPrntf.c`](ODPrntf.c) now clears the internal `chColorCheck` value through
  a common exit path, including when formatted text ends with a delimited or
  single-byte color sequence.

- [x] Handle a blank line safely in `od_list_files()`.
  [`ODList.c`](ODList.c) now removes LF and optional CR terminators with
  guarded length checks and safely reports the normalized line length.

- [x] Treat `ODDirOpen()` attribute masks as inclusion masks on UNIX.
  [`ODPlat.c`](ODPlat.c) now applies the DOS find-first rules through one
  matcher on UNIX and Windows: normal files are included, hidden/system/label/
  directory entries are opt-in, and archive/read-only attributes are ignored.

- [x] Release UNIX directory-search storage when `glob()` setup fails.
  [`ODPlat.c`](ODPlat.c) now releases both `glob()` storage and `tODDirInfo`
  when path expansion fails or produces no paths.

- [x] Close the Windows directory-search handle when no entry matches.
  [`ODPlat.c`](ODPlat.c) now calls `FindClose()` before releasing a search
  whose initial attribute-filter pass finds no acceptable entry.

- [x] Stop Windows directory enumeration when no later entry matches.
  [`ODPlat.c`](ODPlat.c) now stops filtering immediately when `FindNextFile()`
  reaches the end of the search, so stale `WIN32_FIND_DATA` cannot keep the
  loop running. A Windows-only regression covers a rejected final entry and
  has a short timeout so the former failure cannot hang CI indefinitely.

- [x] Copy UNIX directory-entry basenames safely.
  [`ODPlat.c`](ODPlat.c) now selects the text after the final directory
  separator and copies it with the bounded `ODStringCopy()` helper. A UNIX-only
  nested-path regression verifies that `szFileName` contains only the entry
  basename.

- [x] Handle UNIX `stat()` failures in `ODDirRead()`.
  [`ODPlat.c`](ODPlat.c) now advances past and skips entries whose metadata
  cannot be read, returning end-of-file when no later entry remains. A
  UNIX-only regression removes a captured match before it is read and verifies
  that end-of-file remains stable.

- [x] Classify UNIX directory entries with the POSIX file-type predicates.
  [`ODPlat.c`](ODPlat.c) now uses `S_ISDIR()` so only actual directories receive
  `DIR_ATTRIB_DIREC`. A UNIX-domain-socket regression covers a non-directory
  file type that the former bit test could misclassify.

- [x] Bound the file specification copied by `od_list_files()`.
  [`ODList.c`](ODList.c) now accepts file specifications through 99 characters
  using bounded copies and rejects longer values with `ERR_LIMIT`. Boundary
  regressions distinguish this limit from an ordinary file-open failure.

- [x] Bound the filename token read from each FILES.BBS entry.
  `ODListGetFirstWord()` in [`ODList.c`](ODList.c) now enforces the 80-byte
  destination capacity. Tokens through 79 characters are accepted; longer
  tokens stop the listing with `ERR_LIMIT` rather than being truncated into a
  different path.

- [x] Bound the components produced by `ODListFilenameSplit()`.
  [`ODList.c`](ODList.c) now receives every destination capacity, rejects an
  oversized directory component with `ERR_LIMIT`, and terminates the extension
  at the number of extension characters actually copied. Regressions cover the
  directory limit and a valid eight-plus-three filename.

- [x] Treat drive prefixes as a DOS and Windows path feature.
  `ODListFilenameSplit()` in [`ODList.c`](ODList.c) now leaves the drive
  component empty on UNIX and treats a colon as an ordinary filename
  character. An integration regression verifies that a colon-named file is
  resolved relative to its explicit index file.

- [x] Insert the UNIX directory separator in `ODMakeFilename()`.
  [`ODUtil.c`](ODUtil.c) now inserts `/` between a nonempty UNIX path and its
  basename when required. Direct and `od_list_files()` integration regressions
  cover helper output and a directory argument without a trailing separator.

- [x] Check for an empty path before inspecting its final character.
  `ODMakeFilename()` in [`ODUtil.c`](ODUtil.c) now uses one shared condition
  which verifies that the path is nonempty before reading its final character.
  A cross-platform regression verifies that an empty path returns the basename
  unchanged.

- [x] Reserve separator space in `ODMakeFilename()` only when required.
  [`ODUtil.c`](ODUtil.c) now uses checked arithmetic for the path, basename,
  optional separator, and terminator, and compares the exact required size
  before modifying the output. Exact-fit, aliasing, and undersized regressions
  cover each boundary.

- [x] Bound paths assembled while processing FILES.BBS entries.
  [`ODList.c`](ODList.c) now assembles each search path in a separate bounded
  buffer, checks every path operation, and retains the original directory for
  all entries. Regressions cover consecutive relative entries and a resolved
  path exceeding 99 characters.

- [x] Return immediately when the multithreaded chat thread cannot be started.
  The separate chat worker has now been removed: both API-requested and local
  operator chat execute on the session-owner thread. The Windows kernel
  regression covers queued chat toggles, cooperative worker lifetime, and
  owner-thread timer callbacks.

- [x] Repair Unix `od_spawnvpe()` process handling in
  [`ODSpawn.c`](ODSpawn.c). The Unix launcher now detects setup, `fork()`,
  `execve()`, and `waitpid()` failures; retries interrupted waits; and uses
  `_exit()` after a failed child launch. `P_NOWAIT` uses a double fork and
  reaps the intermediate process without changing the application's `SIGCHLD`
  disposition. Bare executable names now search the current directory and
  `PATH`, and a null environment inherits the current environment as required
  by the public contract. A Unix-only regression covers waited exit status,
  path lookup, environment inheritance, synchronous launch-error reporting,
  asynchronous execution, and signal-disposition preservation.

- [x] Make Unix `od_spawn()` interpret the result of `system()` correctly.
  [`ODSpawn.c`](ODSpawn.c) now decodes the wait status and reports failure when
  `system()` returns `-1` or the command shell exits with status 127. Other
  nonzero child statuses remain successful launch results, matching the
  function contract and the DOS and Windows behavior. Unix regressions cover
  statuses 0, 7, and 127.

- [x] Preserve the caller's Unix signal mask around `od_spawn()`.
  [`ODSpawn.c`](ODSpawn.c) now saves the complete caller mask while blocking
  `SIGALRM`, restores that exact mask after `system()` returns, and reports
  mask setup or restoration failures. Unix regressions cover callers entering
  with `SIGALRM` blocked and unblocked.

- [x] Prevent repeated `od_log_open()` calls from abandoning an open stream.
  [`ODLog.c`](ODLog.c) now treats an already-open logging session as success,
  retaining its stream without writing a second separator or startup entry. A
  cross-platform regression verifies that repeated calls produce only one
  session header and entry.

- [x] Close an active log even if logging was subsequently disabled.
  `ODLogClose()` in [`ODLog.c`](ODLog.c) now suppresses the final entry when
  logging is disabled but still closes the active stream and clears both log
  hooks. The cross-platform logging regression covers disabling an open
  session before shutdown.

- [x] Detect log write failures. `od_log_open()` and `od_log_write()` in
  [`ODLog.c`](ODLog.c) now check both formatted output and the flush which
  commits buffered data. A failed initial record closes and clears the new log
  session, while standard-message hooks propagate entry failures. Regressions
  force file-size-limit failures during both session opening and an active
  write.

- [x] Validate time conversion before formatting log entries.
  `od_log_open()` and `od_log_write()` in [`ODLog.c`](ODLog.c) now reject the
  `(time_t)-1` failure result from `time()` and a null result from `localtime()`
  before accessing the time record. Opening remains transactional, and a
  white-box regression covers both failure sentinels and a valid record.

- [x] Bound log messages formatted into the global work buffer.
  [`ODLog.c`](ODLog.c) now expands configurable startup, chat-reason, and exit
  templates through the C89-compatible bounded formatter, using no additional
  large automatic or heap buffer. Oversized output sets `ERR_LIMIT`; startup
  remains transactional, chat-message failure propagates, and exit cleanup
  continues without the oversized entry. Regressions cover all three paths.

- [x] Account for failures while closing the activity log.
  The private `ODLogClose()` hook in [`ODLog.c`](ODLog.c) now returns a Boolean
  result, checks final formatting, writing, flushing, and `fclose()`, preserves
  the first runtime error, and always clears the stream and hooks. It records
  `ERR_GENERALFAILURE` unless the formatter supplied `ERR_LIMIT`; public API,
  ABI, and the process error level requested through `od_exit()` are unchanged.
  Regressions cover successful, disabled, oversized, and forced-failure closes.

- [x] Check text drop-file opens and writes during `od_exit()`.
  The `DOOR.SYS`, DoorWay-style `DOOR.SYS`, `SFDOORS.DAT`, `CHAIN.TXT`, and
  `TRIBBS.SYS` rewrite paths in [`ODInEx2.c`](ODInEx2.c) now check the open,
  retain the first output failure, suppress subsequent writes to a failed
  stream, and check the final flush and close. Open failure records
  `ERR_FILEOPEN`; output or close failure records `ERR_GENERALFAILURE` without
  changing the requested process exit status or interrupting later shutdown.

- [x] Check binary `EXITINFO.BBS` writeback results during `od_exit()`.
  [`ODInEx2.c`](ODInEx2.c) now reports failure to reopen the record as
  `ERR_FILEOPEN`, checks the RemoteAccess 2.x, primitive, RemoteAccess 1.x
  extension, and QuickBBS writes, suppresses later blocks after the first
  output failure, and checks the final flush and close. Output and close
  failures record `ERR_GENERALFAILURE`; all loaded record buffers are released
  and the remaining shutdown work continues without changing the requested
  process exit status.

- [x] Reject reversed rectangles in `od_gettext()` before calling the
  low-level screen interface. [`ODBlock.c`](ODBlock.c) now rejects a left edge
  following the right edge or a top edge following the bottom edge with
  `ERR_PARAMETER`, before graphics-mode validation or either screen backend.
  Regressions cover both orderings through the virtual and legacy local-screen
  paths and verify that the caller's destination buffer remains unchanged.

- [x] Correct restoration of a legacy screen snapshot into a shorter output
  window. [`ODBlock.c`](ODBlock.c) now computes a positive count of skipped
  rows, selects the final rows which fit, and maps the saved cursor into the
  retained portion for both graphics and plain-ASCII restoration. The
  calculations use small `INT` values and retain the legacy buffer format.
  Guarded-buffer regressions cover final-row selection and cursor placement in
  both display modes.

- [x] Advance the source row after restoring a full nonblank line in the
  plain-ASCII path of `od_restore_screen()`. [`ODBlock.c`](ODBlock.c) now
  advances the fixed 160-byte snapshot stride after every completed row while
  tracking destination-margin output separately. The first printable
  character of the next row settles either immediate or delayed terminal wrap;
  a column-1 cursor uses space and backspace instead. Wider virtual and remote
  destinations receive an explicit line-feed/carriage-return pair after cell
  80. Regressions cover an 80-column local display, a column-1 cursor, and a
  132-column virtual screen.

- [x] Balance the API-entry state when `od_set_cursor()` rejects a coordinate.
  The `nRow < 1` or `nColumn < 1` branch in [`ODGraph.c`](ODGraph.c) now calls
  `OD_API_EXIT()` before returning, so invalid coordinates cannot leave API
  serialization state acquired.

- [x] Validate the upper bounds passed to `od_set_cursor()`. The function now
  rejects positions beyond the active virtual or local window before changing
  or transmitting the cursor position. AVATAR positions are additionally
  limited to the one-byte coordinate range, while ANSI can address valid rows
  above 255 in a taller virtual window.

- [x] Propagate communications-method failures from `od_carrier()` and
  `od_set_dtr()`. [`ODCore.c`](ODCore.c) now initializes the carrier state and
  checks both communications results. A failed query returns `FALSE`, and
  failed or unsupported operations set `ERR_GENERALFAILURE`; local mode
  continues to set `ERR_NOREMOTE`.

- [x] Decide how `od_clear_keybuffer()` should report an unsupported or failed
  communications-buffer purge. No public error is appropriate: the function
  clears OpenDoors' logical key buffer, and a transport without a distinct
  purgeable inbound buffer has no additional buffer to clear. The private
  communications result remains intentionally outside this `void` API's
  contract.

- [x] Make a direct pre-initialization request for zero-based port 0 survive
  drop-file parsing. The new [`od_set_port()`](docs/reference/api/od_set_port.md)
  records an explicit selection without changing `tODControl`; values from 0
  through 255 survive drop-file parsing. The `-P` and `-PORT` parser now uses
  the same private forced-port state, correcting its former failure to retain
  zero despite setting `PRESET_PORT`.

- [x] Parse `-HANDLE` and `-SOCKET` values without narrowing them through
  `atoi()`. On Windows and Unix-like systems, [`ODCmdLn.c`](ODCmdLn.c) now
  validates the complete unsigned decimal token and accumulates it directly in
  `DWORD_PTR`, rejecting malformed or overflowing values transactionally. The
  unused legacy DOS path is unchanged.

- [x] Obtain both input and output terminal speeds in the Unix forced-local
  initialization path. [`ODInEx1.c`](ODInEx1.c) now checks `tcgetattr()`,
  prefers a recognized input speed, falls back to the output speed, translates
  POSIX `speed_t` constants to numeric BPS, and retains the nominal 19,200
  value when terminal speed information is unavailable.

- [x] Handle failure of `getpwuid()` during Unix forced-local initialization.
  [`ODInEx1.c`](ODInEx1.c) now applies account names only when the account
  record and respective source string are available. Missing information
  retains the caller identity already selected during initialization.

- [x] Keep custom-hot-key callbacks paired with their keys when a personality
  removes a key. [`ODStat.c`](ODStat.c) now moves the final active key and its
  callback together, then clears both elements of the vacated slot. The
  existing first-match and unordered-removal behavior is preserved.

- [x] Handle configuration keywords without an option string safely.
  [`ODCFile.c`](ODCFile.c) now checks the option length before indexing its
  final character while trimming trailing whitespace. A custom keyword with
  no option text reaches the application callback with a valid empty string.

- [x] Handle an empty `DoorDir` configuration option without indexing before
  its string. [`ODCFile.c`](ODCFile.c) now checks the option length first and
  treats an empty value as an omitted setting, without allocating restore-path
  storage or scheduling a directory change.

- [x] Preserve delivery of recognized built-in configuration keywords to the
  custom configuration callback. Despite the old manual describing the
  callback as an unknown-keyword handler, established doors use it to consume
  settings which OpenDoors also recognizes. The reference now documents the
  actual all-lines behavior, and the configuration callback test covers a
  built-in keyword as well as application-specific keywords.

- [x] Bound the configuration paths copied from `BBSDir`, `DoorDir`, and
  `LogFileName`. [`ODCFile.c`](ODCFile.c) now uses each destination's actual
  size when copying the setting, preserving the historical fixed-size fields
  while guaranteeing null termination. The configuration guide and field
  reference document the resulting path limits.

- [x] Do not change to `DoorDir` unless the original directory was saved.
  [`ODCFile.c`](ODCFile.c) now schedules the change only after allocating and
  populating the restoration path. Repeated settings reuse that saved path and
  retain the existing last-setting-wins behavior without leaking allocations.

- [x] Reject a `PortAddress` configuration option which contains no
  hexadecimal digits. `ODCfgGetWordHex()` in [`ODCFile.c`](ODCFile.c) now
  reports whether conversion succeeded and returns the value through an output
  parameter, so invalid text leaves the previous port address unchanged.

- [x] Strip line endings safely from empty custom drop-file lines.
  [`ODCFile.c`](ODCFile.c) now uses the shared line-normalization helper, which
  checks the remaining length before removing LF and CR. Empty LF and CRLF
  lines remain individual physical lines in a custom format. [`ODUtil.h`](ODUtil.h)
  now includes the standard header required by its existing `FILE` declaration
  instead of depending on caller include order.

- [x] Bound the configuration filename copied into the parser's fallback
  buffer. [`ODCFile.c`](ODCFile.c) now retries a basename only when it fits and
  copies it using the destination's actual size. The unused copy made after an
  ordinary filename failed has been removed.

- [x] Handle an explicitly empty configuration filename without forming a
  pointer before its string. No code change is required: the fallback branch
  is entered only when `strchr()` has already found a directory separator or
  drive colon, so an empty string cannot reach the pointer subtraction and the
  backward scan must find a delimiter before reaching the start of the string.

- [x] Reset all per-run configuration parser state before reinitialization.
  [`ODCFile.c`](ODCFile.c) now clears its pending paging, inactivity, page
  duration, directory, forced-name, and personality settings at the start of
  each run, and nulls closed configuration and custom drop-file handles. A
  two-session test verifies that a second configuration is read independently.

- [x] Make the default configuration filename work on case-sensitive file
  systems. The distributed sample is now named lowercase `door.cfg`, matching
  the single deterministic filename used by [`ODCFile.c`](ODCFile.c), and the
  current documentation states that its case is significant where applicable.

- [x] Bound multiword command-line values correctly. `ODGetNextArgName()` in
  [`ODCmdLn.c`](ODCmdLn.c) passes `strlen(destination) - destination_size - 1`
  to `strncat()`. The subtraction underflows, so a long username, location,
  BBS name, or custom-option string can overflow its destination before the
  function writes a terminating null at the nominal final byte.

- [x] Keep the standard status line's remaining-time field within its allotted
  columns. [`ODStand.c`](ODStand.c) writes `od_time_left` at column 24 and the
  next field at column 35, but the default `"%d mins   "` occupies twelve
  columns when `user_timelimit` is 1,000 through 1,440. The final character
  overlaps the following field.

- [x] Write the complete RemoteAccess traffic-log arrays back to standard
  `EXITINFO.BBS`. `ODWriteExitInfoPrimitive()` in [`ODInEx2.c`](ODInEx2.c)
  copies only 31 bytes beginning at `busyperhour`, although the 24 hourly
  `WORD` values and seven daily `WORD` values occupy 62 bytes and the reader
  copies all 62. Half of the hourly value at the boundary and every daily
  value are left stale in the rewritten record.

- [x] Store the SFDOORS.DAT login time in hour:minute order.
  [`ODInEx1.c`](ODInEx1.c) derives the hour and minute portions of line 15, but
  passes the minute portion first to `sprintf("%02u:%02u", ...)`, producing
  `MM:HH` in `user_logintime`.

- [x] Reject reversed or degenerate rectangles in `od_draw_box()`.
  [`ODDrBox.c`](ODDrBox.c) checks only whether coordinates exceed the fixed
  80-by-25 bounds. If the right edge does not follow the left edge, or the
  bottom edge does not follow the top edge, byte-sized width and height
  calculations wrap and can produce very large repeated output or AVATAR
  clear-area dimensions.

- [x] Handle titles safely in very narrow windows. [`ODWin.c`](ODWin.c)
  permits a three-, four-, or five-column window, then computes the maximum title
  length as `btBetweenSize - 4`. That expression is negative for these valid
  widths and is converted to an unsigned size for comparison, so a nonempty
  title is not truncated to fit and subsequent byte-sized spacing calculations
  wrap.

- [x] Preserve non-waiting behavior after `od_get_key(FALSE)` discards a line
  feed. [`ODCore.c`](ODCore.c) checks the queue only before entering its
  do/while loop. If the queued event is a line feed, the loop discards it and
  calls `ODInQueueGetNextEvent()` with an infinite timeout without checking
  `bWait` or the queue again, so an operation requested as a poll can block.

- [x] Compare the full unsigned byte range correctly in `od_input_str()`.
  [`ODCore.c`](ODCore.c) stores the received byte in plain `char` before
  comparing it with the unsigned `chMin` and `chMax` parameters. On targets
  where `char` is signed, input bytes 128 through 255 become negative and can
  never satisfy a range intended to accept them.

- [x] Release multiline-editor bookkeeping allocations on every exit path.
  [`ODEdit.c`](ODEdit.c) allocates `pRememberBuffer` and dynamically grows
  `papchStartOfLine`, but `od_multiline_edit()` never frees either allocation
  after the editor finishes or after a later setup/indexing failure. Every
  invocation therefore leaks internal memory independently of the caller's
  text-buffer ownership.

- [x] Report the grown multiline-editor buffer capacity correctly.
  [`ODEdit.c`](ODEdit.c) updates the editor instance's `unBufferSize` after a
  successful `pfBufferRealloc` call, but writes the original function argument
  to `tODEditOptions.unFinalBufferSize` on return. The returned final pointer
  and reported capacity can consequently describe different allocations.

- [x] Validate custom multiline-editor rectangles before deriving unsigned
  dimensions. [`ODEdit.c`](ODEdit.c) replaces zero coordinates with defaults
  but does not reject negative, reversed, off-screen, or otherwise unusable
  values. The width, height, scrolling, allocation, and redraw calculations
  can then underflow or address positions outside the supported screen.

- [x] Parse display-file extensions from the final path component.
  [`ODEmu.c`](ODEmu.c) selects automatic mode only when the entire path has no
  period. A period in a directory component therefore disables automatic
  `.rip`/`.avt`/`.ans`/`.asc` variant selection for a base name without an
  extension.

- [x] Recognize explicitly named RIP display files from their final extension.
  `od_send_file()` and `od_send_file_section()` uppercase the explicit path,
  then search it for the lowercase substring `.rip`. Consequently no explicit
  RIP filename is recognized by that test; a case-corrected substring search
  would also misclassify extensions such as `.ripple`.

- [x] Bound display-file path construction. Automatic variant selection in
  [`ODEmu.c`](ODEmu.c) constructs candidates in the fixed global work buffer,
  and the RIP status-message path concatenates developer and caller strings in
  that buffer before truncating the result to the smaller message destination.
  Either operation can overflow before the attempted file open or truncation.

- [x] Bound the `od_send_file_section()` marker construction.
  [`ODEmu.c`](ODEmu.c) initializes a 256-byte array with the two-character
  marker `@#`, then permits `strncat()` to append 254 section-name bytes plus a
  null terminator. A section name of 254 bytes or more writes one byte beyond
  the array.

- [x] Correct the RemoteAccess `^FQ` through `^FT` numeric substitutions.
  [`ODEmu.c`](ODEmu.c) uses the format string `%ul` for `DWORD` upload and
  download counters. It supplies the wrong conversion type where `unsigned
  int` and `unsigned long` differ and emits a literal `l` after the result on
  every platform.

- [x] Correct the remaining RemoteAccess numeric varargs mismatches.
  [`ODEmu.c`](ODEmu.c) passes `DWORD` values to the `^FL`, `^FN`, `^FP`,
  `^F9`, and `^F:` substitutions without matching casts, and prints the signed
  `INT32` value used by `^KA` with an unsigned conversion. These calls have
  undefined behavior on platforms where the typedef and format conversion use
  different promoted argument types.

- [x] Validate the complete `od_edit_str()` display rectangle.
  [`ODEdStr.c`](ODEdStr.c) rejects only row and column values below one. It
  accepts as many as 80 represented field positions and normally draws an
  additional cursor cell, without checking the active screen dimensions or
  whether `nColumn + field_length` overflows the row. A field reaching the
  lower-right screen cell can wrap and scroll while it is being drawn.

- [x] Reconcile the `W` filename-format character in `od_edit_str()` with its
  documented purpose. [`ODEdStr.c`](ODEdStr.c) accepts letters, separators,
  period, and wildcards for `W`, but rejects all digits and ordinary filename
  punctuation which the related `F` class accepts. Valid wildcard filenames
  such as `FILE2.*` therefore cannot be entered with that format class.

- [x] Correct the success test in `ex_vote.c`'s `WriteCurrentUser()` example.
  It entered the “unable to update” path when `fwrite()` returned one, which is
  the successful one-record result, while an actual short write was treated as
  success. The error branch already returned after closing the stream; the
  separate descriptor-ownership problem is recorded below.

- [x] Close the multi-node Vote example's shared file descriptor exactly once.
  `ExclusiveFileOpen()` passes the descriptor returned by `sopen()` to
  `fdopen()`, so `fclose()` in `ExclusiveFileClose()` already closes it. The
  following `close(hHandle)` attempts to close the same descriptor again and
  can affect an unrelated file if the descriptor number has been reused.

- [x] Format the signed 32-bit `system_calls` field with a matching argument
  type. The RemoteAccess `^KA` substitution in [`ODEmu.c`](ODEmu.c) was
  corrected with the other numeric substitutions; the DOS status personality
  in [`ODRA.c`](ODRA.c) required the same explicit `unsigned long` conversion
  for its established `%lu` presentation.

- [x] Release partially read `EXITINFO.BBS` allocations during initialization.
  `ODReadExitInfoPrimitive()` in [`ODInEx2.c`](ODInEx2.c) now releases and
  clears `pExitInfoRecord` after a short read, and the RemoteAccess 1.x path in
  [`ODInEx1.c`](ODInEx1.c) does the same for `pExtendedExitInfo`. The
  RemoteAccess 2.x failure path also clears `pRA2ExitInfoRecord` after its
  existing release so that no failed record read leaves an incomplete or
  dangling global pointer.

- [x] Validate `time()` before adjusting primitive `EXITINFO.BBS` time limits.
  `ODWriteExitInfoPrimitive()` in [`ODInEx2.c`](ODInEx2.c) now applies its
  elapsed-time compensation only when both the startup and current timestamps
  are valid. If either clock query failed, it retains the time limit read from
  the BBS while continuing to update the other fields in the record.

- [x] Validate session timestamps before calculating used time at exit.
  [`od_exit()`](ODInEx2.c) now updates `od_control.user_time_used` only when
  the startup and shutdown timestamps form a valid, nondecreasing interval.
  The primitive `EXITINFO.BBS` time-limit calculation uses the same private
  elapsed-minutes helper, so clock failure and backward movement have the same
  retain-existing-value fallback in both paths.

- [ ] Accept an empty string in `ODStringToName()` without reading before it.
  [`ODCore.c`](ODCore.c) indexes `strlen(pszToConvert) - 1` while checking for
  trailing newline and carriage-return characters. For an empty custom
  drop-file field, that expression reads one byte before the supplied buffer.

- [ ] Handle a failed local-time conversion while deciding whether to page.
  [`od_page()`](ODCore.c) dereferences the result of `localtime()` without
  checking it. If the current time cannot be converted, a nonblank page reason
  crashes before `od_okaytopage` can allow or reject the request.

- [ ] Prevent `od_edit_str()` from inserting Control-Y into unrestricted fields.
  The Control-Y handler clears the current value by jumping into the shared
  whole-line deletion block, but then falls through to ordinary character
  insertion with the original Control-Y byte. The unrestricted `?` format
  accepts that byte, so the documented erase-field command leaves a one-byte
  control character instead of an empty value.
- [ ] Ensure `ODInitReadSFDoorsDAT()` closes `SFDOORS.DAT` when a mandatory
  line is missing. Each of the first 32 failed `fgets()` paths currently
  returns without closing the successfully opened stream.

- [x] Invalidate only the rectangle changed by `ODScrnPutText()` on Windows.
  [`ODScrn.c`](ODScrn.c) adds the requested zero-based right and bottom
  coordinates to the active window's right and bottom boundaries. The left
  and top coordinates are instead added to their corresponding starting
  boundaries, so a write to a subrectangle can invalidate an unrelated and
  potentially out-of-window area.

- [ ] Validate complete, ordered source and destination rectangles in
  `ODScrnCopyText()`. [`ODScrn.c`](ODScrn.c) checks each supplied coordinate
  against the active window independently, but does not require left to be no
  greater than right or top to be no greater than bottom. It also checks only
  the destination origin, although the function contract requires the whole
  copied rectangle to fit. Invalid input can therefore underflow the temporary
  allocation size or pass out-of-window coordinates to `ODScrnPutText()`.

- [ ] Handle one-row boundaries in `ODScrnScrollUpOneLine()`.
  [`ODScrn.c`](ODScrn.c) calculates the number of rows to move as bottom minus
  top and executes that copy with a `do` loop. A valid one-row boundary makes
  the byte count zero, but the loop still runs and then underflows it to 255,
  copying well beyond the boundary instead of merely clearing its only row.

- [ ] Advance past silent bells and tab characters in `ODScrnDisplayBuffer()`.
  [`ODScrn.c`](ODScrn.c) advances its input pointer for an audible bell but not
  for a bell suppressed by silent mode, and never advances it for a tab. The
  loop count still decreases, so every remaining iteration processes the same
  control byte and any following buffered characters are discarded.

- [ ] Use the tab-expanded column when `ODScrnDisplayBuffer()` positions its
  destination pointer. After calculating the next tab stop in
  `btCurrentColumn`, [`ODScrn.c`](ODScrn.c) rebuilds `pDest` with the unchanged
  `btCursorColumn`. Once input advancement is corrected, the next printable
  character would therefore be stored at the pre-tab position while cursor
  and invalidation accounting report the expanded position.

- [x] Repair or remove the dormant `USE_KERNEL_SIGNAL` no-carrier handler.
  [`sig_no_carrier()`](ODKrnl.c) contains the incomplete expression
  `od_control.baud != 0 &&`, so enabling this Unix-only configuration cannot
  compile and its intended additional condition is not recoverable from the
  implementation alone.

- [x] Use an array-compatible type for key-sequence table indices.
  [`ODLongestFullCode()`](ODGetIn.c), [`ODHaveStartOfSequence()`](ODGetIn.c),
  and [`ODGetCodeIfLongest()`](ODGetIn.c) compare a signed `int` index with the
  unsigned result of `DIM(aKeySequences)`, producing strict modern-compiler
  warnings. Any correction must retain values and arithmetic suitable for
  16-bit DOS compilers.

- [x] Use an array-compatible type for the Windows key-table index in
  `ODScrnWindowProc()`. [`ODScrn.c`](ODScrn.c) declares `nKeyTableIndex` as a
  signed `int` and compares it with the unsigned result of
  `DIM(aWinKeyToODKey)`, producing a strict MinGW warning. Any correction must
  retain a type and loop form accepted by the supported legacy compilers.

- [x] Explicitly discard the reserved `od_window_create()` argument.
  In non-debug builds, [`od_window_create()`](ODWin.c) assigns zero to
  `nReserved` but never reads it, producing a strict modern-compiler warning.
  Preserve the existing public signature and behavior while expressing that
  the reserved argument is intentionally unused.

- [x] Remove unreachable configuration-token overflow handling.
  [`ODConfigInit()`](ODCFile.c) increments `wCurrent` only while it is below
  32, so the following `wCurrent <= 32` test is always true and its overflow
  branch cannot run. Preserve the established 32-character truncation while
  making the bound and termination behavior explicit.

- [x] Remove the invariant configuration-scan completion test.
  [`ODConfigInit()`](ODCFile.c) always lets its `wCurrent < TEXT_SIZE` keyword
  loop terminate normally, including after a built-in match, so the later
  `wCurrent >= TEXT_SIZE` callback condition is always true. Preserve the
  established behavior in which the developer callback receives every
  configuration line.

- [x] Give each unit-test platform a distinct generated build path.
  Concurrent selector runs for the same source and function currently write
  the same generated source, object, and executable names. Running Unix and
  pthread coverage together can therefore corrupt a compiler input while it
  is being parsed; Clang 19 crashed when the two `ODComInbound` runs collided.
  Default output paths now include the platform, DOS Watcom convention, and
  Windows architecture; explicit `--build` paths remain supported.

- [x] Allow enough DOSBox runtime for the complete unit suite.
  The Watcom runner uses a fixed 120-second default and killed the DOS16 batch
  after 252 cases, causing every remaining coverage record to be reported as
  missing. Use a full-suite allowance consistent with the Turbo C runner while
  retaining the explicit timeout override.

- [ ] Test the Windows communications event mask instead of its union.
  [`ODComWaitEvent()`](ODCom.c) uses bitwise OR when deciding whether a
  completed wait includes `EV_RLSD`. Since that expression is always nonzero,
  an unrelated event causes a carrier-status query and can make the function
  return before the requested carrier-loss event occurs.

- [x] Remove the dominated unknown-attribute check from ANSI brightness output.
  The brightness comparison in [`od_set_attrib()`](ODCore.c) is inside the
  `else` of a test that handles `od_control.od_cur_attrib == -1`, so its own
  repeated `od_cur_attrib == -1` term cannot be true. The original
  implementation and its behavioral unit suite must be retained as the
  pre-refactor baseline before simplifying this decision.

- [x] Remove the invariant `bNormal` edit-loop dispatch.
  [`od_edit_str()`](ODEdStr.c) initializes `bNormal` to `TRUE`, never changes
  it, and then conditionally jumps to `keep_going` when it is true. The false
  path is unreachable, so the variable and conditional obscure the actual
  unconditional initial control flow.

- [x] Express the chat input-color transition as the Boolean state change it
  tests. [`ODKrnlChatMode()`](ODKrnl.c) repeated both the input-source and
  current-color terms to spell out their two differing combinations. The
  equivalent inequality would retain the established behavior while allowing
  each decision condition to be tested independently.

- [x] Resolve the unused signal-number warnings in the legacy Unix kernel
  handlers. The handlers existed only in the uncompilable dormant
  `USE_KERNEL_SIGNAL` implementation and were removed with that implementation,
  so there are no remaining handler arguments to discard.

- [x] Remove dominated section-found checks from the section-file scanners.
  Both loops in [`od_send_file_section()`](ODEmu.c) repeated
  `bSectionFound` in an `else if` reached only after the two not-found paths
  had failed. Removing the locally guaranteed term retains the established
  section boundary behavior and makes the actual marker decision independently
  testable.

- [x] Remove dominated ANSI erase-command parameter-count checks.
  In [`ODEmulateFromBuffer()`](ODEmu.c), the final `J` branch and the latter
  two `K` branches can only be reached after the zero-parameter cases have
  failed. Their repeated `btNumParams >= 1` terms were therefore guaranteed;
  retaining only the parameter-value tests preserves terminal behavior and
  permits independent MC/DC evidence.

- [x] Use a compiler-recognized fallthrough annotation in the emulator.
  The escape-character branch in [`ODEmulateFromBuffer()`](ODEmu.c) intentionally
  reaches the ordinary AVATAR dispatcher when an AVATAR sequence is active.
  Its historical prose comment did not suppress GCC's strict fallthrough
  warning. Use a form recognized by supported modern compilers without
  changing the established control flow or breaking legacy compilers.

- [ ] Preserve a detected DESQview or Windows multitasker in the Turbo C
  `ODPlatInit()` path. Unlike the Watcom implementation, the inline-assembly
  implementation falls through after assigning `kMultitaskerDV` and
  `kMultitaskerWin`, then unconditionally assigns `kMultitaskerNone`. As a
  result, only its OS/2 branch can currently retain a detected multitasker.

- [x] Use an unsigned path index in the Unix `ODDirRead()` implementation.
  [`ODPlat.c`](ODPlat.c) stores the current `glob()` position in an `int` but
  compares it with `glob_t.gl_pathc`, whose type is `size_t`. This produces a
  signed/unsigned comparison warning and can represent only part of the
  theoretical `glob()` result range where `int` is narrower than `size_t`.

- [x] Replace the Unix `ODDirRead()` EOF-controlled loop with an unconditional
  search loop. Every branch that sets `bEOF` also returns from the function,
  so the false outcome of `while(!pDirInfo->bEOF)` cannot be reached. Expressing
  the loop as unconditional would preserve behavior and remove the misleading
  dead exit condition.

- [x] Initialize every field of `ODEditOptionsDefault`.
  [`ODEdit.c`](ODEdit.c) predates the `pszFinalBuffer` member now present at
  the end of `tODEditOptions`, so its positional initializer stops after
  `wFlags`. Static storage still zero-initializes the omitted pointer, but
  strict modern compilers report the incomplete initializer.

- [x] Compile the `ODEditTryToGrow()` size-width check only when `size_t` is
  wider than `UINT`. The `nGrownSize > (UINT)-1` defense is meaningful on
  64-bit targets, but is invariantly false on Windows x86 where both types are
  32-bit, leaving an unreachable MC/DC condition in that configuration.

- [x] Remove or justify the unreachable unfinished-range fallback in
  `ODEditDetermineChanged()`. Once a difference sets `bFoundStart`, each inner
  line scan continues until equal characters or one of the two line endings
  sets `bFoundFinish`; the scan cannot leave its line while that flag remains
  false. Consequently, the final `if(!bFoundFinish)` body cannot execute.

- [ ] Preserve the width-boundary character when hard-wrapping editor text.
  `ODEditBufferFormatAndIndex()` breaks with `pch` pointing at the ordinary
  character that reached the width limit, then follows the same path used for
  an explicit EOL and increments `pch`. Word wrapping deliberately consumes
  the selected space, but hard wrapping currently drops a real character from
  the indexed display.

- [x] Remove the contradictory NUL path from the paired-EOL test in
  `ODEditEnterText()`. `IS_EOL_CHAR(pch[1])` treats NUL as an EOL character,
  but the same condition immediately requires `pch[1] != '\0'`. The behavior
  is correct, but the expanded expression contains conditions that cannot vary
  independently for MC/DC.

- [ ] Destroy the time-edit control when toolbar construction fails after
  creating it. `ODFrameCreateToolbar()` currently calls
  `DestroyWindow(hwndTimeUpDown)` in both cleanup branches; when creation of
  the up-down control fails, that value is `NULL`, the successfully created
  edit control is leaked, and its subclass remains installed until its parent
  is destroyed.

- [ ] Unwind DOS screen state and allocations when `od_spawnvpe()` loses its
  initialized session while draining output. The DOS path allocates its screen
  and directory buffers, saves and changes the screen boundary, attribute, and
  cursor state, then returns immediately if `ODWaitDrain()` leaves
  `bODInitialized` false. That return currently leaks both allocations and
  leaves the locally modified screen state in place instead of using the normal
  restoration path.

- [x] Diagnose the Windows `ODLog` isolated-test null-page fault.
  The generated `od_log_open()` unit executable faults while reading address
  zero under both 32-bit and 64-bit Wine. The following `od_log_write()` and
  `ODLogClose()` cases fail similarly, so their shared fixture or production
  state must be inspected in a debugger before changing expectations or code.
  Address-to-line and instruction inspection identified MinGW's `_errno()`
  accessor as the null return. The test-only errno fixture now supplies that
  Windows CRT accessor, and strict x86 and x64 `ODLog.c` suites pass.
