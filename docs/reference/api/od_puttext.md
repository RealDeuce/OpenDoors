# `od_puttext()`

Displays a rectangular character-and-attribute block.

## Synopsis

```c
BOOL od_puttext(INT nLeft, INT nTop, INT nRight, INT nBottom,
    void *pBlock);
```

Coordinates are one-based and inclusive. `pBlock` supplies character and
attribute bytes in the same row-major format produced by `od_gettext()`.

OpenDoors compares the new block with its current remote screen and sends the
changes using ANSI or AVATAR operations. The complete block is recorded even
where it falls outside the clipped local presentation. Invalid arguments set
`ERR_PARAMETER`; unsupported graphics mode sets `ERR_NOGRAPHICS`; temporary
allocation failure sets `ERR_MEMORY`. AVATAR coordinates above 255 set
`ERR_LIMIT`.

## See also

[`od_gettext()`](od_gettext.md), [`od_scroll()`](od_scroll.md)
