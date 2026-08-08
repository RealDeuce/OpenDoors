# `od_get_key()`

Retrieves one character from the OpenDoors input queue.

## Synopsis

```c
char od_get_key(BOOL bWait);
```

If `bWait` is true, the function waits until a character is available. If it is
false and the queue is empty, the function returns zero immediately. Remote and
enabled local input are returned in arrival order.

Use [`od_get_input()`](od_get_input.md) when extended keys or the source of an
event must be distinguished.

## See also

[`od_key_pending()`](od_key_pending.md), [`od_get_input()`](od_get_input.md),
[`od_clear_keybuffer()`](od_clear_keybuffer.md)
