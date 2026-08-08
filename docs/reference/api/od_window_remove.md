# `od_window_remove()`

Removes a popup window and restores the contents saved beneath it.

## Synopsis

```c
BOOL od_window_remove(void *pWinInfo);
```

`pWinInfo` must be the still-owned opaque handle returned by a successful
`od_window_create()` call. The handle is consumed and must not be reused.

The function returns true when the saved screen has been restored. A null or
invalid handle, or an incompatible display state, causes failure and sets
`od_control.od_error`.

## See also

[`od_window_create()`](od_window_create.md),
[`od_popup_menu()`](od_popup_menu.md)
