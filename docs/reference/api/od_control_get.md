# `od_control_get()`

Returns the address of the OpenDoors control structure.

## Synopsis

```c
tODControl *od_control_get(void);
```

## Return value

The return value is a pointer to the same [`tODControl`](../control/index.md)
object exported as the global [`od_control`](../control/index.md) variable. It
is never `NULL`.

The pointed-to structure is the writable public [`od_control`](../control/index.md) object and belongs
to OpenDoors. The application must not free it or assume that the pointer refers
to a separately allocated session object. Individual fields may be read and
written under the same rules as the global variable. Replacing the entire
structure is not a supported initialization or reset operation because it also
overwrites live runtime state and library-owned pointers.

## Description

OpenDoors keeps the information and settings for the active door session in
one [`tODControl`](../control/index.md) structure. C and C++ applications
normally access that structure through the global [`od_control`](../control/index.md)
variable:

```c
od_control.od_prog_name = "Example Door";
od_control.od_prog_version = "1.0";
```

Some foreign-function interfaces and dynamic loaders can call an exported
function more easily than they can import an exported data symbol.
[`od_control_get()`](od_control_get.md) provides function-based access for those environments:

```c
tODControl *control = od_control_get();

control->od_prog_name[0] = '\0';
strncat(control->od_prog_name, "Example Door",
    sizeof(control->od_prog_name) - 1);
```

Dereferencing the returned pointer and accessing the global variable are
exactly equivalent. A change made through one form is immediately visible
through the other. The pointer remains valid while that OpenDoors library
instance is loaded and does not change when [`od_init()`](od_init.md) or
[`od_exit()`](od_exit.md) is called.

Unlike most OpenDoors API functions, [`od_control_get()`](od_control_get.md) does not initialize
OpenDoors and does not run the OpenDoors kernel. It is therefore safe to call
while preparing settings that must be established before initialization. This
does not change the timing rules of the individual fields: a field documented
as an initialization setting must still be assigned before
[`od_init()`](od_init.md) or before another API function initializes OpenDoors
implicitly.

The function does not provide synchronization or create independent control
structures. OpenDoors has one active control structure per loaded library
instance. Use [`od_control_read_lock()`](od_control_read_lock.md) or
[`od_control_write_lock()`](od_control_write_lock.md) when access must be
synchronized with an active internal worker. All API and ABI access must still
occur on the thread which calls [`od_init()`](od_init.md); obtaining the pointer
does not transfer session ownership.

## Errors

This function has no failure return and does not change
[`od_control.od_error`](../control/runtime.md#od_error).

## Example

The following helper uses function-based access to select local mode before
initialization:

```c
static void configure_local_session(void)
{
    tODControl *control = od_control_get();

    control->od_force_local = TRUE;
    control->od_silent_mode = TRUE;
}
```

## See also

[`od_control`](../control/index.md), [`tODControl`](../control/index.md),
[`od_init()`](od_init.md),
[`od_control_read_lock()`](od_control_read_lock.md),
[`od_control_write_lock()`](od_control_write_lock.md)
