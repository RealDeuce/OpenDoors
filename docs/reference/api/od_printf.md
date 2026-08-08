# `od_printf()`

Performs formatted output to the remote and local displays, with support for
OpenDoors color-setting codes.

## Synopsis

```c
void od_printf(const char *pszFormat, ...);
```

`pszFormat` is a `printf`-style format string. The remaining arguments supply
the values selected by that string.

## Description

This is the usual function for displaying formatted text. It accepts the
normal C formatting conversions and also recognizes color names delimited by
`od_control.od_color_delimiter` (a backtick by default). For example:

```c
od_printf("`bright cyan`Welcome, %s!`white`\n\r", od_control.user_name);
```

Output updates the remote terminal model and is shown on the local display.
The function returns no value. Formatting which cannot be represented is
treated in the same manner as the library's formatted-output implementation.

## See also

[`od_disp_str()`](od_disp_str.md), [`od_set_color()`](od_set_color.md),
[`od_putch()`](od_putch.md)
