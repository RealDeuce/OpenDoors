# Constants and flags

Include [`OpenDoor.h`](api/index.md) rather than copying these numeric values
into an application. The names below are grouped by their intended use.

## Errors

| Name | Meaning |
| --- | --- |
| `ERR_NONE` | No error has been reported. |
| `ERR_MEMORY` | Required memory could not be allocated. |
| `ERR_NOGRAPHICS` | ANSI, AVATAR, or RIP support is required. |
| `ERR_PARAMETER` | An argument or saved object is invalid. |
| `ERR_FILEOPEN`, `ERR_FILEREAD` | A file could not be opened or read. |
| `ERR_LIMIT` | A platform or representation limit was exceeded. |
| `ERR_NOREMOTE` | The operation requires a remote session. |
| `ERR_GENERALFAILURE` | The operation failed without a more specific cause. |
| `ERR_NOTHINGWAITING` | No requested input was ready before the deadline. |
| `ERR_NOMATCH` | A requested named object was not found. |
| `ERR_UNSUPPORTED` | The operation is not implemented on this target. |

The most recent API error is stored in
[`od_control.od_error`](control/runtime.md).

## Colors

`D_BLACK` through `D_GREY` select dark colors; `L_BLACK` through `L_WHITE`
select their bright forms. The equivalent `B_*` names are retained for source
compatibility. Pass these names to [`od_set_color()`](api/od_set_color.md) or
combine foreground and background into the traditional IBM-PC attribute used by
[`od_set_attrib()`](api/od_set_attrib.md).

## Input

- `GETIN_NORMAL`, `GETIN_RAW`, and `GETIN_RAWCTRL` control translation in
  [`od_get_input()`](api/od_get_input.md).
- `OD_KEY_F1` through `OD_KEY_F12`, arrows, Insert, Delete, Home, End, Page Up,
  Page Down, and Shift-Tab identify translated extended keys.
- `EDIT_FLAG_*` values control redraw, field mode, strict input, password
  display, cancellation, automatic entry/deletion, blanks, literals, and size
  display in [`od_edit_str()`](api/od_edit_str.md).
- `EDIT_RETURN_*` values describe how single-line editing ended.

## Menus, screen, and status

- `MENU_ALLOW_CANCEL`, `MENU_PULLDOWN`, `MENU_KEEP`, and `MENU_DESTROY` modify
  popup behavior; `POPUP_*` values report special results.
- `SCROLL_NO_CLEAR` preserves lines exposed by [`od_scroll()`](api/od_scroll.md).
- `STATUS_NORMAL`, `STATUS_NONE`, and `STATUS_ALTERNATE_1` through
  `STATUS_ALTERNATE_7` select the local status display.
- `BOX_*` indices identify characters in [`od_control.od_box_chars`](control/customization.md).

## Session configuration

`DORINFO1` through `DOOR32SYS`, `CUSTOM`, and `NO_DOOR_FILE` identify the
normalized door-information source. `COM_FOSSIL`, `COM_INTERNAL`, `COM_WIN32`,
`COM_DOOR32`, `COM_SOCKET`, and `COM_STDIO` select communication methods.
`COM_DEFAULT_FLOW`, `COM_RTSCTS_FLOW`, and `COM_NO_FLOW` select flow control.

Use `PAGE_DISABLE`, `PAGE_ENABLE`, or `PAGE_USE_HOURS` for paging policy.
`DIS_*` bits disable individual automatic behaviors. `ERRORLEVEL_*` indices
select configured process exit codes.

## Components and personalities

Use `INCLUDE_CONFIG_FILE`, `INCLUDE_LOGFILE`, and `INCLUDE_MPS` to enable the
built-in optional components, or their `NO_*` counterparts to disable them.
`PER_OPENDOORS`, `PER_PCBOARD`, `PER_RA`, and `PER_WILDCAT` identify built-in
personalities. Custom personality callbacks receive `PEROP_DISPLAY*`,
`PEROP_UPDATE*`, `PEROP_INITIALIZE`, `PEROP_CUSTOMKEY`, and `PEROP_DEINITIALIZE`
operations.
