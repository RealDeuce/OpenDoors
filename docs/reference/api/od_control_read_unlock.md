# `od_control_read_unlock()`

Releases one level of shared read access to the OpenDoors control structure.

## Synopsis

```c
void od_control_read_unlock(void);
```

## Description

Call this function once for each successful
[`od_control_read_lock()`](od_control_read_lock.md) call. Nested read locks
remain in effect until the matching number of unlock operations has been
performed. A read lock obtained while a write lock is held is likewise counted
and must be released.

Calling this function without a corresponding read lock is invalid. In a
diagnostic build, OpenDoors reports the mismatch through its internal
assertion mechanism. The function does not initialize OpenDoors and does not
change [`od_control.od_error`](../control/runtime.md#od_error).

This function must be called on the session-owner thread. It does not permit a
background thread to access [`od_control`](../control/index.md) or any other
part of the OpenDoors API or ABI.

## See also

[`od_control_read_lock()`](od_control_read_lock.md),
[`od_control_write_lock()`](od_control_write_lock.md),
[`od_control_write_unlock()`](od_control_write_unlock.md)
