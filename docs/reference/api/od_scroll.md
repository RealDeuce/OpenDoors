# `od_scroll()`

Scrolls a rectangular screen area up or down.

## Synopsis

```c
BOOL od_scroll(INT nLeft, INT nTop, INT nRight, INT nBottom,
    INT nDistance, WORD nFlags);
```

Coordinates are one-based and inclusive. A positive distance scrolls upward; a
negative distance scrolls downward. [`SCROLL_NO_CLEAR`](../constants.md) leaves
newly exposed lines unchanged instead of blanking them.

The operation requires ANSI or AVATAR mode. Invalid rectangles set
[`ERR_PARAMETER`](../constants.md), unsupported graphics sets
[`ERR_NOGRAPHICS`](../constants.md), and unrepresentable AVATAR coordinates or
allocation sizes set [`ERR_LIMIT`](../constants.md). The original cursor
position is restored before the function returns.

## See also

[`od_gettext()`](od_gettext.md), [`od_puttext()`](od_puttext.md), [Constants and
flags](../constants.md)
