# `od_key_pending()`

Reports whether an input event is waiting in the OpenDoors queue.

## Synopsis

```c
BOOL od_key_pending(void);
```

The function services required OpenDoors housekeeping and then returns true if
a local or remote key can be read without waiting. It does not remove the
event. A false result simply means that nothing is presently queued.

## See also

[`od_get_key()`](od_get_key.md), [`od_get_input()`](od_get_input.md),
[`od_clear_keybuffer()`](od_clear_keybuffer.md)
