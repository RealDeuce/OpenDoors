# `od_get_cursor()`

Returns OpenDoors' current estimate of the remote cursor position.

## Synopsis

```c
void od_get_cursor(INT *pnRow, INT *pnColumn);
```

Each non-`NULL` pointer receives a one-based coordinate. At least one pointer
must be supplied; passing two null pointers sets
[`od_control.od_error`](../control/runtime.md) to
[`ERR_PARAMETER`](../constants.md).

The position comes from the virtual remote screen in a remote session and from
the local screen in local mode. The function does not query the terminal.

## See also

[`od_set_cursor()`](od_set_cursor.md), [Terminal and screen
model](../../guides/terminal-screen.md)
