# `od_window_create()`

Creates a popup window and saves the screen contents beneath it.

## Synopsis

```c
void *od_window_create(INT nLeft, INT nTop, INT nRight, INT nBottom,
    char *pszTitle, BYTE btBorderCol, BYTE btTitleCol,
    BYTE btInsideCol, INT nReserved);
```

Coordinates are one-based and inclusive. `pszTitle` may be null or empty.
The three attributes select the border, title, and interior colors.
`nReserved` must be zero.

The function requires ANSI or AVATAR graphics. On success it returns an opaque
window handle which must eventually be passed to `od_window_remove()`. On
failure it returns `NULL` and sets `od_control.od_error`, commonly to
`ERR_PARAMETER`, `ERR_NOGRAPHICS`, or `ERR_MEMORY`.

The window interface retains its legacy screen-size limits. Use block and
extended snapshot APIs when manipulating screens beyond those limits.

## See also

[`od_window_remove()`](od_window_remove.md), [`od_draw_box()`](od_draw_box.md)
