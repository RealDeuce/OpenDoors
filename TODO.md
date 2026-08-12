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

## Defects found during the documentation audit

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

- [ ] Test the Windows communications event mask instead of its union.
  [`ODComWaitEvent()`](ODCom.c) uses bitwise OR when deciding whether a
  completed wait includes `EV_RLSD`. Since that expression is always nonzero,
  an unrelated event causes a carrier-status query and can make the function
  return before the requested carrier-loss event occurs.

- [ ] Preserve a detected DESQview or Windows multitasker in the Turbo C
  `ODPlatInit()` path. Unlike the Watcom implementation, the inline-assembly
  implementation falls through after assigning `kMultitaskerDV` and
  `kMultitaskerWin`, then unconditionally assigns `kMultitaskerNone`. As a
  result, only its OS/2 branch can currently retain a detected multitasker.

- [ ] Preserve the width-boundary character when hard-wrapping editor text.
  `ODEditBufferFormatAndIndex()` breaks with `pch` pointing at the ordinary
  character that reached the width limit, then follows the same path used for
  an explicit EOL and increments `pch`. Word wrapping deliberately consumes
  the selected space, but hard wrapping currently drops a real character from
  the indexed display.

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
