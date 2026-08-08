# `od_init()`

Initializes OpenDoors and begins the door session.

## Synopsis

```c
void od_init(void);
```

The function reads configuration and door information, establishes local and
remote I/O, initializes enabled components, creates the session screen, and
starts normal OpenDoors housekeeping. It is safe to call when OpenDoors has
already been initialized; normal API functions also call it automatically when
needed.

Program identity, component selection, callbacks, and other initialization
settings in [`od_control`](../control/index.md) must be assigned before this
call. Command-line parsing must also occur first. Fatal initialization errors
are handled through the configured OpenDoors exit behavior.

## See also

[`od_parse_cmd_line()`](od_parse_cmd_line.md), [`od_kernel()`](od_kernel.md),
[`od_exit()`](od_exit.md), [Session
lifecycle](../../guides/session-lifecycle.md)
