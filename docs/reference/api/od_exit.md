# `od_exit()`

Ends the OpenDoors session and performs an orderly shutdown.

## Synopsis

```c
void od_exit(INT nErrorLevel, BOOL bTermCall);
```

`nErrorLevel` is returned to the invoking environment. If `bTermCall` is true,
OpenDoors treats the exit as termination of the current call and performs the
configured disconnect behavior.

Before returning control, OpenDoors runs the before-exit callback, updates door
information where required, closes components and communications, and restores
the local display. The function normally does not return to its caller.

## See also

[`od_init()`](od_init.md), [`od_set_dtr()`](od_set_dtr.md),
[Session lifecycle](../../guides/session-lifecycle.md)
