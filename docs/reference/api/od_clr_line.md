# `od_clr_line()`

Clears from the current cursor position through the end of the current line.

## Synopsis

```c
void od_clr_line(void);
```

The cleared cells use the current display attribute and the cursor remains in
place. The operation requires ANSI or AVATAR support; otherwise
[`od_control.od_error`](../control/runtime.md) is set to
[`ERR_NOGRAPHICS`](../constants.md). The function returns no value.

## See also

[`od_clr_scr()`](od_clr_scr.md), [`od_scroll()`](od_scroll.md),
[`od_set_cursor()`](od_set_cursor.md)
