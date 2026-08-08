# `od_disp_emu()`

Displays a string while interpreting embedded terminal-emulation sequences.

## Synopsis

```c
void od_disp_emu(const char *pszToDisplay, BOOL bRemoteEcho);
```

`pszToDisplay` is the nul-terminated terminal stream. If `bRemoteEcho` is true,
the original output is also sent to the remote user.

## Description

The emulator recognizes the ANSI and AVATAR sequences supported by OpenDoors
and applies their effects to the virtual remote screen. This makes cursor and
block operations agree with text which arrived as an emulated stream. It is
normally used for display-file content or other already encoded output.

Passing false for `bRemoteEcho` provides local interpretation without sending
the string a second time. The function returns no value.

## See also

[`od_disp()`](od_disp.md), [`od_send_file()`](od_send_file.md),
[`od_get_cursor()`](od_get_cursor.md)
