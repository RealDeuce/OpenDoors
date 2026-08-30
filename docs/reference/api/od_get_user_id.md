# `od_get_user_id()`

Returns the best stable caller identifier available from the selected
door-information format.

## Synopsis

```c
const char *od_get_user_id(void);
```

## Return value

Returns a pointer to a null-terminated identifier owned by OpenDoors. The
caller must not modify or free it. The value remains valid until the next call
to `od_get_user_id()`, or until [`od_exit()`](od_exit.md) for a `BBSDEV.DRP`
session. An empty string means no usable identity is available or the public
API lifecycle does not permit the call.

## Description

For [`BBSDEVDRP`](../constants/session.md#bbsdevdrp), the result is the exact
opaque user key supplied by the BBS. It is not the caller alias and OpenDoors
does not interpret or normalize it.

For a legacy format which supplies a user number, the result is:

```text
<user number>:<real name or handle>
```

The real name is preferred; the handle is used when the name is empty. For a
format without a user number, the result begins with a colon:

```text
:<real name or handle>
```

The delimiter keeps a format without a number distinct from user number zero.
The legacy result is deterministic, but only the information available in the
drop file can contribute to it. BBSDEV's opaque key is preferable when
available because names and legacy record numbers may be reused independently.

This function does not initialize OpenDoors.

## See also

[`od_init()`](od_init.md),
[`od_control.user_name`](../control/caller.md#user_name),
[`od_control.user_handle`](../control/caller.md#user_handle),
[`od_control.user_num`](../control/caller.md#user_num)
