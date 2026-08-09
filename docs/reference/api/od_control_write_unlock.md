# `od_control_write_unlock()`

Releases one level of exclusive write access to the OpenDoors control
structure.

## Synopsis

```c
void od_control_write_unlock(void);
```

## Description

Call this function once for each successful
[`od_control_write_lock()`](od_control_write_lock.md) call. The exclusive lock
remains in effect while another nested write lock is outstanding. If a read
lock was nested inside the write lock and remains outstanding, releasing the
last write level changes the held access to shared read access.

Calling this function without a corresponding write lock is invalid. In a
diagnostic build, OpenDoors reports the mismatch through its internal
assertion mechanism. The function does not initialize OpenDoors and does not
change [`od_control.od_error`](../control/runtime.md#od_error).

This function must be called on the session-owner thread.

## See also

[`od_control_write_lock()`](od_control_write_lock.md),
[`od_control_read_lock()`](od_control_read_lock.md),
[`od_control_read_unlock()`](od_control_read_unlock.md)
