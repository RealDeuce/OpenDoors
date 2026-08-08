# `od_draw_box()`

Draws a single-line box using the configured OpenDoors box characters.

## Synopsis

```c
BOOL od_draw_box(BYTE btLeft, BYTE btTop, BYTE btRight, BYTE btBottom);
```

Coordinates are one-based and inclusive. The current attribute is used for the
border. ANSI or AVATAR support is required. Invalid dimensions set
`ERR_PARAMETER`, and lack of graphics support sets `ERR_NOGRAPHICS`.

The function returns true after the box has been displayed.

## See also

[`od_window_create()`](od_window_create.md),
[`od_set_attrib()`](od_set_attrib.md)
