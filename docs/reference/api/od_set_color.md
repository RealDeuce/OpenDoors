# `od_set_color()`

Selects the foreground and background colors for subsequent output.

## Synopsis

```c
void od_set_color(INT nForeground, INT nBackground);
```

Both arguments use the [OpenDoors color constants](../constants/colors.md).
Foreground colors may use any [`D_*`](../constants/colors.md) or
[`L_*`](../constants/colors.md) value. Background colors use the
[`D_*`](../constants/colors.md) values for steady backgrounds and the
[`B_*`](../constants/colors.md) values for blinking backgrounds. The selected
values are combined into an IBM-PC display attribute and sent to terminals
which support color. They also become the attribute used by the virtual and
local screens. The function returns no value.

## See also

[`od_set_attrib()`](od_set_attrib.md), [`od_printf()`](od_printf.md),
[Colors](../constants/colors.md)
