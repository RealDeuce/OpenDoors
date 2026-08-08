# `od_kernel()`

Performs the periodic housekeeping required by an active OpenDoors session.

## Synopsis

```c
void od_kernel(void);
```

The kernel checks carrier state, time limits, inactivity, local sysop keys,
status updates, and configured callbacks. API functions invoke it frequently,
so ordinary doors seldom need to call it after every operation. Call it during
long computations or wait loops which otherwise make no OpenDoors calls.

The function returns no value. It may terminate the session when connection or
time-limit policy requires it.

## See also

[`od_sleep()`](od_sleep.md), [`od_carrier()`](od_carrier.md),
[Session lifecycle](../../guides/session-lifecycle.md)
