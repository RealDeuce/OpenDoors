# `od_color_config()`

Converts a textual color description into an IBM-PC display attribute.

## Synopsis

```c
BYTE od_color_config(char *pszColorDesc);
```

`pszColorDesc` must point to a color description containing foreground and
optional background names using the configured color-name table. The keywords
for bright and flashing text are also recognized. The return value is the
combined attribute used by [`od_set_attrib()`](od_set_attrib.md) and the screen
APIs.

Unrecognized words are ignored and the unspecified portions retain their
default, producing grey on black when nothing matches. Applications which accept
configurable colors should use this function rather than maintaining a second
parser.

## See also

[`od_set_attrib()`](od_set_attrib.md), [`od_set_color()`](od_set_color.md),
[Colors](../constants/colors.md)
