# `od_set_cursor()`

Moves the remote output cursor to a one-based row and column.

## Synopsis

```c
void od_set_cursor(INT nRow, INT nColumn);
```

The requested position is constrained to the current remote screen. ANSI or
AVATAR support is required; otherwise
[`od_control.od_error`](../control/runtime.md) is set to
[`ERR_NOGRAPHICS`](../constants.md). The local presentation follows the remote
cursor when that position is visible. The function returns no value.

## See also

[`od_get_cursor()`](od_get_cursor.md), [`od_clr_line()`](od_clr_line.md)
