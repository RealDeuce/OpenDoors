# `od_send_file()`

Displays the terminal-appropriate version of an ASCII, ANSI, AVATAR, or RIP
file.

## Synopsis

```c
BOOL od_send_file(const char *pszFileName);
```

If `pszFileName` has no extension, OpenDoors searches for a compatible variant
according to the user's display capabilities. File contents are interpreted
for the local presentation while being transmitted to the remote user. Page
pausing and user interruption follow the current `od_control` settings.

The function returns false with `ERR_FILEOPEN` when no suitable file can be
opened, `ERR_FILEREAD` on a read failure, or `ERR_PARAMETER` for a null name.

## See also

[`od_send_file_section()`](od_send_file_section.md),
[`od_hotkey_menu()`](od_hotkey_menu.md), [`od_disp_emu()`](od_disp_emu.md)
