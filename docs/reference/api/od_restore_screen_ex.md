# `od_restore_screen_ex()`

Restores a complete snapshot created by
[`od_save_screen_ex()`](od_save_screen_ex.md).

## Synopsis

```c
BOOL od_restore_screen_ex(const void *pBuffer, DWORD dwBufferSize);
```

The buffer must contain a valid, unmodified extended snapshot and the supplied
size must include that complete snapshot. Its width and height must exactly
match the current remote screen; snapshots are not cropped or resized during
restore.

On success OpenDoors restores the remote contents and state, transmits the
appropriate representation to the remote user, and refreshes the visible local
portion. Malformed, truncated, version-incompatible, or dimension-mismatched
buffers fail with [`ERR_PARAMETER`](../constants.md).

## See also

[`od_save_screen_size()`](od_save_screen_size.md),
[`od_save_screen_ex()`](od_save_screen_ex.md)
