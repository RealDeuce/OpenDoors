# `od_window_create()`

Creates a popup window of the specified size and color, storing the contents of the screen "under" the window. The window can later be removed from the screen, restoring the original contents of the screen "under" the window, using the [`od_window_remove()`](od_window_remove.md) function. Requires ANSI, AVATAR or RIP mode.

## Synopsis

```c
void *od_window_create(INT nLeft, INT nTop, INT nRight, INT nBottom,
    char *pszTitle, BYTE btBorderCol, BYTE btTitleCol,
    BYTE btInsideCol, INT nReserved);
```

## Return value

Pointer to newly allocated window structure on success NULL on failure

## Description

This function creates a pop-up window on the remote and local screens. The contents of the screen beneath the window are stored, to allow the window to later be removed from the screen using the [`od_window_remove()`](od_window_remove.md) function. The window is drawn using the boarder characters defined in the already existing od_control.od_box_chars[] array. The boarder is displayed using the color attribute specified in btBorderCol. The working area of the window is created in the color specified in btInsideCol. A title may optionally be displayed on the window by specifying a string in pszTitle. This title will be displayed in the color specified by btTitleCol. If you do not wish a title to be displayed, pass an empty string or NULL pointer in pszTitle. On success, `od_window_create()` will return a pointer to a buffer which was allocated to store information on the window and the contents of the screen "under" the window. This pointer should at some point be passed in a call to [`od_window_remove()`](od_window_remove.md).

This function requires ANSI, AVATAR or RIP graphics mode. If AVATAR mode is active, this function will take advantage of special AVATAR control sequences to display the window much faster than is possible in ANSI mode. In ANSI mode, window display will be slightly faster if btBorderCol and btTitleCol are equal. Note that the nReserved parameter of this function is not currently used. To preserve compatibility with future versions of OpenDoors, this parameter should always be set to 0. Currently, the size of the buffer allocated to store the window information will be (length*width*2) + 4 bytes in size.

If `od_window_create()` fails, it returns `NULL` and records the reason in
[`od_control.od_error`](../control/runtime.md#od_error).

## Examples

For an example of the use of the `od_window_create()` function, see the included ex_chat.c example program.

## Additional details

Coordinates are one-based and inclusive. `pszTitle` may be null or empty. The
three attributes select the border, title, and interior colors. `nReserved` is
ignored by the current implementation; pass zero for compatibility with the
documented interface.

The function requires ANSI or AVATAR graphics. On success it returns an opaque
window handle which must eventually be passed to
[`od_window_remove()`](od_window_remove.md). On failure it returns `NULL` and
sets [`od_control.od_error`](../control/runtime.md), commonly to
[`ERR_PARAMETER`](../constants/errors.md), [`ERR_NOGRAPHICS`](../constants/errors.md), or
[`ERR_MEMORY`](../constants/errors.md).

The window must fit within columns 1 through 80 and rows 1 through 25 and must
have room for a border and at least one interior row and column. These limits
remain even when the current virtual screen is larger.

## See also

[`od_window_remove()`](od_window_remove.md), [`od_draw_box()`](od_draw_box.md), [`od_gettext()`](od_gettext.md), [`od_puttext()`](od_puttext.md), [`od_save_screen()`](od_save_screen.md), [`od_restore_screen()`](od_restore_screen.md), [`od_scroll()`](od_scroll.md)

[`od_window_remove()`](od_window_remove.md), [`od_draw_box()`](od_draw_box.md)
