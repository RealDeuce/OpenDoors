# `od_control_write_lock()`

Obtains exclusive write access to the OpenDoors control structure.

## Synopsis

```c
tODControl *od_control_write_lock(void);
```

## Return value

On success, the function returns the address of the exported
[`od_control`](../control/index.md) structure. It returns `NULL` when called
from a non-owner thread after [`od_init()`](od_init.md), or when the caller is
attempting to promote an existing read lock to a write lock.

The function does not initialize OpenDoors and does not change
[`od_control.od_error`](../control/runtime.md#od_error).

## Description

The write lock excludes internal readers and writers while application code
examines and modifies a related set of [`od_control`](../control/index.md)
fields. Use it when a runtime change must become visible as one operation. The
pointer is the actual public control structure; it is not a copy and must not
be freed.

Write locks may be nested on the session-owner thread. A read lock may be
nested inside a write lock. Each successful lock operation requires its
matching unlock operation.

OpenDoors deliberately does not perform an implicit read-to-write promotion.
Two readers which both attempted promotion could wait for one another
indefinitely. If [`od_control_read_lock()`](od_control_read_lock.md) is already
held without a write lock, this function returns `NULL`; release all read
locks, obtain the write lock, and recheck the values on which the update
depends.

All OpenDoors API and ABI access remains confined to the thread which called
[`od_init()`](od_init.md). A background thread cannot acquire this lock as a
way to become an OpenDoors caller. Public locks held across an API call are
temporarily released and reacquired at the outer API boundary, as described
for [`od_control_read_lock()`](od_control_read_lock.md).

## Example

```c
tODControl *control = od_control_write_lock();

if(control != NULL) {
    control->od_user_keyboard_on = FALSE;
    control->od_disable |= DIS_TIMEOUT;
    od_control_write_unlock();
}
```

## See also

[`od_control_write_unlock()`](od_control_write_unlock.md),
[`od_control_read_lock()`](od_control_read_lock.md),
[`od_control_get()`](od_control_get.md),
[`od_control`](../control/index.md)
