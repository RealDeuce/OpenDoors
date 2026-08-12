# `od_control_read_lock()`

Obtains shared read access to the OpenDoors control structure.

## Synopsis

```c
const tODControl *od_control_read_lock(void);
```

## Return value

On success, the function returns a pointer to the same
[`tODControl`](../control/index.md) object exported as
[`od_control`](../control/index.md). After [`od_init()`](od_init.md), a call
from any thread other than the session-owner thread returns `NULL`.

The function does not initialize OpenDoors and does not change
[`od_control.od_error`](../control/runtime.md#od_error).

## Description

This function obtains shared access to [`od_control`](../control/index.md).
It is intended for application code which must inspect several fields as one
consistent set while the Windows screen presenter copies terminal state.
Internal readers may continue to run while the read lock is held; an internal
writer waits until the application releases it.

The thread which calls [`od_init()`](od_init.md) owns the OpenDoors session.
All OpenDoors API calls and all access to the public API and ABI, including
[`od_control`](../control/index.md), must occur on that thread. The lock does
not transfer session ownership and is not a means for a background thread to
call OpenDoors safely.

Read locks may be nested. Each successful call must be paired with one
[`od_control_read_unlock()`](od_control_read_unlock.md) call. A read lock may
also be obtained while the same thread holds the write lock. Attempting to
obtain a write lock while holding only a read lock is rejected; release the
read lock before calling
[`od_control_write_lock()`](od_control_write_lock.md).

When an OpenDoors API function is called while a public lock is held, the
library temporarily releases that public lock at the outer API boundary and
reacquires it before returning. This permits internal work and owner-thread
callbacks to proceed without deadlock. Consequently, an API call made inside a
locked region is a synchronization point: values must be checked again after
the call if another internal operation could have changed them.

On builds without an internal threading kernel, the synchronization operation
is a no-op, but ownership, nesting, and matching-unlock rules remain the same.

## Example

```c
const tODControl *control = od_control_read_lock();

if(control != NULL) {
    unsigned user_number = control->user_num;
    int minutes_left = control->user_timelimit;

    od_control_read_unlock();
    printf("User %u has %d minutes left\n", user_number, minutes_left);
}
```

## See also

[`od_control_read_unlock()`](od_control_read_unlock.md),
[`od_control_write_lock()`](od_control_write_lock.md),
[`od_control_get()`](od_control_get.md),
[`od_control`](../control/index.md)
