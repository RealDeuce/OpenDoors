# `od_send_file_section()`

Displays one named section of a terminal display file.

## Synopsis

```c
BOOL od_send_file_section(char *pszFileName, char *pszSectionName);
```

`pszFileName` follows the same extension-selection rules as
[`od_send_file()`](od_send_file.md). `pszSectionName` contains only the section
name, without the `@#` delimiter used in the file.

OpenDoors searches for the named section and transmits its contents through the
normal terminal emulator. It returns true on success. Null arguments set
[`ERR_PARAMETER`](../constants.md); a missing or unreadable file sets
[`ERR_FILEOPEN`](../constants.md) or [`ERR_FILEREAD`](../constants.md); a
missing section is reported as failure.

## See also

[`od_send_file()`](od_send_file.md), [`od_hotkey_menu()`](od_hotkey_menu.md)
