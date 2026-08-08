# `od_disp()`

Sends an explicit number of bytes to the remote connection, optionally echoing
them on the local display.

## Synopsis

```c
void od_disp(const char *pachBuffer, INT nSize, BOOL bLocalEcho);
```

`pachBuffer` points to the bytes, `nSize` gives their count, and `bLocalEcho`
selects local display.

## Description

This low-level output function does not require a nul terminator and does not
perform `printf` or terminal-emulation parsing. When local echo is enabled, the
bytes also update the virtual remote screen and its clipped local presentation.
When local echo is disabled, the bytes are transmitted as raw output and are
not inferred into the screen model.

Use `od_disp_emu()` when an ANSI or AVATAR stream must be interpreted locally.
The function returns no value.

## See also

[`od_disp_str()`](od_disp_str.md), [`od_disp_emu()`](od_disp_emu.md)
