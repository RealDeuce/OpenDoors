# `od_save_screen()`

Saves the legacy 80-column local screen representation.

## Synopsis

```c
BOOL od_save_screen(void *pBuffer);
```

`pBuffer` must address at least 4004 bytes. The buffer receives cursor column,
cursor row, attribute, saved height, and 80 character/attribute pairs for each
saved row.

## Description

This interface retains its historical fixed-width format and local-screen
behavior for source and binary compatibility. It fails with `ERR_PARAMETER` if
the current local output window is not 80 columns wide or the pointer is null.
It returns true on success.

New programs which must preserve a remote screen wider or taller than the
local display should use the size-aware snapshot functions.

## See also

[`od_restore_screen()`](od_restore_screen.md),
[`od_save_screen_size()`](od_save_screen_size.md),
[`od_save_screen_ex()`](od_save_screen_ex.md)
