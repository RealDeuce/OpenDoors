# `od_sleep()`

Suspends the program for a period while yielding appropriately to the host.

## Synopsis

```c
void od_sleep(tODMilliSec Milliseconds);
```

`Milliseconds` is the requested delay. The implementation uses the platform's
sleep facilities and cooperates with OpenDoors timing behavior. It is suitable
for short polling loops; a door which remains idle for a long interval should
also ensure that normal kernel processing continues. The function returns no
value.

## See also

[`od_kernel()`](od_kernel.md), [Types and callbacks](../types.md)
