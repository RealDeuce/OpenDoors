# OpenDoors TODO

## Defects found during the documentation audit

- [ ] Correct the popup-menu level bounds check in `od_popup_menu()`.
  `MenuLevelInfo` has eleven elements (indices 0 through 10), but
  [`ODPopup.c`](ODPopup.c) rejects only values greater than 11. Passing level
  11 therefore accesses one element beyond the array.

- [ ] Report failure of the popup menu-item allocation as `ERR_MEMORY`.
  [`ODPopup.c`](ODPopup.c) currently assigns `ERR_PARAMETER` when allocation of
  the `tMenuItem` array fails, while failure of the later saved-screen
  allocation is correctly reported as `ERR_MEMORY`.

- [ ] Ensure `od_printf()` restores its color-parser state on every return.
  [`ODPrntf.c`](ODPrntf.c) leaves the internal `chColorCheck` value set when a
  formatted string ends immediately after a valid delimited color description
  or a single-byte color marker and attribute. A subsequent direct call to
  `od_color_config()` can then stop at that stale delimiter.

- [ ] Handle a blank line safely in `od_list_files()`. After stripping a lone
  newline, [`ODList.c`](ODList.c) tests `szLine[strlen(szLine) - 1]` for a
  carriage return. For an empty line this reads before the start of the
  buffer.

- [ ] Bound the file specification copied by `od_list_files()`.
  [`ODList.c`](ODList.c) copies `pszFileSpec` with `strcpy()` into both the
  global 1,025-byte work buffer and a 100-byte `szDirectory` buffer without
  validating its length.

- [ ] Return immediately when the multithreaded chat thread cannot be started.
  In [`ODKrnl.c`](ODKrnl.c), `od_chat()` sets `ERR_GENERALFAILURE` and executes
  `OD_API_EXIT()` after `ODKrnlStartChatThread()` fails, but then continues to
  `ODThreadWaitForExit(hChatThread)` instead of returning.

- [ ] Repair Unix `od_spawnvpe()` process handling in
  [`ODSpawn.c`](ODSpawn.c). The implementation does not handle `fork()` failure,
  ignores the result of `waitpid()`, and explicitly ignores `SIGCHLD` before
  waiting in `P_WAIT` mode. On systems which reap ignored children
  automatically, the wait fails and the uninitialized status value is
  inspected. The previous `SIGCHLD` disposition is not restored.

- [ ] Make Unix `od_spawn()` interpret the result of `system()` correctly.
  [`ODSpawn.c`](ODSpawn.c) compares the encoded wait status directly with 127,
  so a shell exit status of 127 is normally treated as success. Other nonzero
  command exit statuses are also currently reported as success.

- [ ] Prevent repeated `od_log_open()` calls from abandoning an open stream.
  [`ODLog.c`](ODLog.c) replaces its internal `FILE *` without checking whether
  logging is already active or closing the previous stream.

- [ ] Detect log write failures. `od_log_open()` and `od_log_write()` in
  [`ODLog.c`](ODLog.c) do not check the results of `fprintf()`; after a
  successful open, `od_log_write()` returns `TRUE` even when the entry could
  not be written.

- [ ] Check text drop-file opens and writes during `od_exit()`.
  [`ODInEx2.c`](ODInEx2.c) checks the `EXITINFO.BBS` update open, but the
  `DOOR.SYS`, DoorWay-style `DOOR.SYS`, `SFDOORS.DAT`, `CHAIN.TXT`, and
  `TRIBBS.SYS` rewrite paths pass the result of `fopen()` directly to
  `fprintf()` and do not check individual writes. An unavailable output file
  can therefore cause invalid stream access instead of a reported shutdown
  error.

- [ ] Reject reversed rectangles in `od_gettext()` before calling the
  low-level screen interface. [`ODBlock.c`](ODBlock.c) checks that coordinates
  are within the current bounds but does not check `nLeft > nRight` or
  `nTop > nBottom`. The virtual-screen path rejects these values, while the
  legacy local-screen path performs unsigned size calculations and can access
  memory outside the requested rectangle.

- [ ] Correct restoration of a legacy screen snapshot into a shorter output
  window. [`ODBlock.c`](ODBlock.c) describes retaining the final rows of the
  saved screen, but computes `current_height - saved_height` and adds that
  negative distance to the snapshot pointer. `od_restore_screen()` can
  therefore read before `pBuffer` when the current 80-column window is shorter
  than the saved window.

- [ ] Advance the source row after restoring a full nonblank line in the
  plain-ASCII path of `od_restore_screen()`. [`ODBlock.c`](ODBlock.c) advances
  `pchTextBuffer` only when fewer than 80 characters were emitted and an
  explicit CR/LF was required. When all 80 characters are emitted and the
  terminal wraps automatically, the next loop iteration reads the same saved
  row again instead of the following row.

- [ ] Balance the API-entry state when `od_set_cursor()` rejects a coordinate.
  [`ODGraph.c`](ODGraph.c) calls `OD_API_ENTRY()`, but its `nRow < 1` or
  `nColumn < 1` branch returns without calling `OD_API_EXIT()`. In builds where
  those macros acquire and release API serialization state, one invalid call
  can leave the library locked.

- [ ] Validate the upper bounds passed to `od_set_cursor()`. The virtual-screen
  setter silently clamps them, the legacy local-screen path narrows them to
  `BYTE`, the AVATAR command narrows them to one byte, and the ANSI command
  transmits the original integers. A single call can therefore leave the
  remote terminal, virtual screen, and local presentation at different
  positions.

- [ ] Propagate communications-method failures from `od_carrier()` and
  `od_set_dtr()`. [`ODCore.c`](ODCore.c) ignores the `tODResult` returned by
  `ODComCarrier()` and `ODComSetDTR()`. If a carrier query fails before writing
  its output parameter, `od_carrier()` returns an uninitialized Boolean;
  unsupported DTR operations and serial-driver failures are silently ignored.

- [ ] Decide how `od_clear_keybuffer()` should report an unsupported or failed
  communications-buffer purge. [`ODCore.c`](ODCore.c) empties the common input
  queue but ignores the result of `ODComClearInbound()`, so socket, Door32, and
  standard-I/O bytes which have not yet entered the queue may remain pending
  even though the function reports no limitation.
