# API function reference

The high-level OpenDoors interface consists of the `od_*` functions declared in
`OpenDoor.h`. The functions are grouped here by the task they perform.

Most functions initialize OpenDoors automatically if necessary. Functions which
must run before initialization say so explicitly. Unless a page states
otherwise, failures place an [`ERR_*`](../constants/errors.md) value in
[`od_control.od_error`](../control/runtime.md).

## Function groups

- **Text display** sends ordinary, formatted, repeated, raw, or emulated text.
- **Color and cursor control** changes display attributes and cursor position.
- **Screen manipulation** clears or saves the complete screen.
- **Block manipulation** reads, writes, clears, or scrolls rectangular areas.
- **Windows and menus** provides boxes, popup windows, and menu selection.
- **File display** selects and sends terminal-appropriate display files.
- **Input** reads keys, events, strings, and multi-line text.
- **Door activity and command lines** handles paging, child processes, logs,
  and arguments.
- **Special control** initializes, services, configures, and ends a session.

The thread which calls [`od_init()`](od_init.md) owns the session. All public
API and ABI access must occur on that thread.
