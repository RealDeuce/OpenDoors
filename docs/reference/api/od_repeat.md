# `od_repeat()`

Displays one character repeatedly, using AVATAR compression when available.

## Synopsis

```c
void od_repeat(char chValue, BYTE btTimes);
```

`chValue` is the character and `btTimes` is the number of copies, from 0 to 255.

## Description

The resulting characters are the same as repeated calls to
[`od_putch()`](od_putch.md), but the transmitted representation may be shorter
for an AVATAR terminal. The remote screen model and local presentation are
updated for the complete expanded output. The function returns no value.

## See also

[`od_putch()`](od_putch.md), [`od_disp_str()`](od_disp_str.md)
