# `od_clear_keybuffer()`

Discards all currently queued local and remote key events.

## Synopsis

```c
void od_clear_keybuffer(void);
```

The serial or socket inbound queue is also cleared when a remote connection is
active. Keys which arrive after the operation remain available. The function
returns no value.

## See also

[`od_key_pending()`](od_key_pending.md), [`od_get_key()`](od_get_key.md)
