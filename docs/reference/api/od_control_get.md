# `od_control_get()`

Returns a pointer to the current session's [`od_control`](../control/index.md)
structure.

## Synopsis

```c
tODControl *od_control_get(void);
```

The returned pointer is the same session state exposed by the global
[`od_control`](../control/index.md) declaration. It is provided for environments
where importing or binding a data symbol is inconvenient. The structure remains
owned by OpenDoors and must not be freed.

Fields may be read or assigned according to their individual descriptions.
Settings which affect initialization must still be changed before
[`od_init()`](od_init.md).

## See also

[The `od_control` structure](../control/index.md), [`od_init()`](od_init.md)
