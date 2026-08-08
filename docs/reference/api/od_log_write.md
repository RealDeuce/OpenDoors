# `od_log_write()`

Appends one application message to the OpenDoors log.

## Synopsis

```c
BOOL od_log_write(const char *pszMessage);
```

`pszMessage` is written using the configured log component, filename, and
message format. The function returns true when logging succeeds. A null
message, disabled or unavailable logging component, or write failure returns
false and sets `od_control.od_error` where applicable.

## See also

[The `od_control` structure](../control/index.md),
[`od_init()`](od_init.md)
