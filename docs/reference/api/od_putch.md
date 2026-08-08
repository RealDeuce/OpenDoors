# `od_putch()`

Displays a single character on the remote and local displays.

## Synopsis

```c
void od_putch(char chToDisplay);
```

The character is displayed using the current attribute. Carriage return, line
feed, tab, backspace, and bell receive their normal OpenDoors screen behavior.
The cursor advances or the output window scrolls as necessary. The function
returns no value.

## See also

[`od_repeat()`](od_repeat.md), [`od_disp_str()`](od_disp_str.md),
[`od_set_attrib()`](od_set_attrib.md)
