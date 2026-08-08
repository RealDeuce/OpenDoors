# `od_window_remove()`

Removes a window previously created using [`od_window_create()`](od_window_create.md), restoring the original screen background.

## Synopsis

```c
BOOL od_window_remove(void *pWinInfo);
```

## Return value

TRUE on success FALSE on failure

## Description

The [`od_window_remove()`](od_window_remove.md) function removes a window from the screen which was previously created by [`od_window_create()`](od_window_create.md), and deallocates the memory which was allocated to store the window information. The contents of the screen beneath the window is restored to appear as it did prior to the call to [`od_window_create()`](od_window_create.md). pWinInfo must point to the value returned by [`od_window_create()`](od_window_create.md).

Note that overlapping windows must be removed in the reverse order from which they were created for proper display results. The last window to be created must be the first window to be removed.

If [`od_window_remove()`](od_window_remove.md) fails, it returns
[`FALSE`](../constants/general.md#false) and records the reason in
[`od_control.od_error`](../control/runtime.md#od_error).

## Examples

For an example of the use of the [`od_window_remove()`](od_window_remove.md) function, see the included ex_chat.c example program.

## Additional details

`pWinInfo` must be the still-owned opaque handle returned by a successful
[`od_window_create()`](od_window_create.md) call. The handle is consumed and
must not be reused.

The function returns true when the saved screen has been restored. A null
pointer fails with [`ERR_PARAMETER`](../constants/errors.md). If restoring the
saved rectangle fails, the function preserves the error from
[`od_puttext()`](od_puttext.md). It cannot determine whether a non-null pointer
is stale or came from some other allocation; passing anything other than a
currently owned window handle has undefined behavior.

## See also

[`od_window_create()`](od_window_create.md), [`od_draw_box()`](od_draw_box.md), [`od_gettext()`](od_gettext.md), [`od_puttext()`](od_puttext.md), [`od_save_screen()`](od_save_screen.md), [`od_restore_screen()`](od_restore_screen.md), [`od_scroll()`](od_scroll.md)

[`od_window_create()`](od_window_create.md),
[`od_popup_menu()`](od_popup_menu.md)
