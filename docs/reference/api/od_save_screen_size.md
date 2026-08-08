# `od_save_screen_size()`

Returns the buffer size required for a complete extended screen snapshot.

## Synopsis

```c
DWORD od_save_screen_size(void);
```

The size includes the versioned snapshot header and every character/attribute
cell in the current remote screen. Allocate at least this many bytes and pass
the same size to `od_save_screen_ex()` and `od_restore_screen_ex()`.

A return value of zero indicates that a representable snapshot cannot be
created. `od_control.od_error` is then `ERR_MEMORY` or `ERR_LIMIT` as
appropriate. DOS snapshots are limited to one object smaller than 64 KiB.

## See also

[`od_save_screen_ex()`](od_save_screen_ex.md),
[`od_restore_screen_ex()`](od_restore_screen_ex.md)
