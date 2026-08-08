# `od_gettext()`

Copies a rectangular area of the remote screen into a caller-supplied buffer.

## Synopsis

```c
BOOL od_gettext(INT nLeft, INT nTop, INT nRight, INT nBottom,
    void *pBlock);
```

Coordinates are one-based and inclusive. `pBlock` must hold two bytes per
cell: character followed by IBM-PC attribute, in row-major order.

The rectangle is checked against the current virtual remote screen, which may
be larger than the local display. Invalid coordinates or a null buffer produce
`ERR_PARAMETER`; a terminal without ANSI or AVATAR graphics produces
`ERR_NOGRAPHICS`. The function returns true on success.

## See also

[`od_puttext()`](od_puttext.md), [`od_scroll()`](od_scroll.md),
[Terminal and screen model](../../guides/terminal-screen.md)
