# `od_save_screen_ex()`

Saves the complete, dynamically sized remote screen into a versioned snapshot.

## Synopsis

```c
BOOL od_save_screen_ex(void *pBuffer, DWORD dwBufferSize);
```

Call `od_save_screen_size()` first, allocate the returned number of bytes, and
pass that allocation and size here. The snapshot contains screen dimensions,
output boundaries, cursor, attribute, scrolling state, and all
character/attribute cells.

The function returns false with `ERR_PARAMETER` for a null or undersized
buffer. Allocation or platform size limits discovered while establishing the
screen are reported as `ERR_MEMORY` or `ERR_LIMIT`. The snapshot is opaque;
applications should not inspect or modify it.

## See also

[`od_save_screen_size()`](od_save_screen_size.md),
[`od_restore_screen_ex()`](od_restore_screen_ex.md)
