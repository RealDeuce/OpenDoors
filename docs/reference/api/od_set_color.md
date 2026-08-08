# `od_set_color()`

Selects the foreground and background colors for subsequent output.

## Synopsis

```c
void od_set_color(INT nForeground, INT nBackground);
```

Both arguments use the OpenDoors color constants. The selected values are
combined into an IBM-PC display attribute and sent to terminals which support
color. They also become the attribute used by the virtual and local screens.
The function returns no value.

## See also

[`od_set_attrib()`](od_set_attrib.md), [`od_printf()`](od_printf.md),
[Constants and flags](../constants.md)
