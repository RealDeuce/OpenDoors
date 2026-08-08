# `od_restore_screen()`

Restores a screen saved by the legacy [`od_save_screen()`](od_save_screen.md)
interface.

## Synopsis

```c
BOOL od_restore_screen(void *pBuffer);
```

The buffer must be an unmodified legacy snapshot. OpenDoors restores as many
saved rows as fit the current 80-column local output window, along with the
saved cursor and attribute. ANSI or AVATAR terminals use block output; plain
text terminals receive a best-effort textual reconstruction.

The function returns true on success. A null pointer or non-80-column local
window sets [`ERR_PARAMETER`](../constants/errors.md).

## See also

[`od_save_screen()`](od_save_screen.md),
[`od_restore_screen_ex()`](od_restore_screen_ex.md)
