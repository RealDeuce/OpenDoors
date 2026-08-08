# `od_disp_str()`

Displays a nul-terminated string on the remote and local displays.

## Synopsis

```c
void od_disp_str(const char *pszToDisplay);
```

`pszToDisplay` points to the string to display. It must not be `NULL`.

## Description

Unlike `od_printf()`, this function performs no C format substitution. It does
interpret the configured OpenDoors color codes. Ordinary control characters
such as carriage return, line feed, tab, backspace, and bell are handled by the
screen display layer.

Use this function when the text is already complete. In particular, it is the
safe choice for text which may contain percent characters supplied by a user.
The function returns no value.

## See also

[`od_printf()`](od_printf.md), [`od_disp()`](od_disp.md),
[`od_disp_emu()`](od_disp_emu.md)
