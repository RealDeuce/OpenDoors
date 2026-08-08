# `od_input_str()`

Reads a line of constrained character input.

## Synopsis

```c
void od_input_str(char *pszInput, INT nMaxLength,
    unsigned char chMin, unsigned char chMax);
```

`pszInput` is both the initial text and destination buffer. `nMaxLength` is the
largest accepted string length, excluding its nul terminator. `chMin` and
`chMax` give the inclusive range of acceptable character values.

The user may edit with the normal erase keys and finishes with Enter. The
destination must have room for `nMaxLength + 1` bytes. Invalid parameters are
reported through `od_control.od_error`. The function returns no value.

## See also

[`od_edit_str()`](od_edit_str.md), [`od_multiline_edit()`](od_multiline_edit.md)
