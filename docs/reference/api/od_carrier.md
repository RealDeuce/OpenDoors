# `od_carrier()`

Reports whether the remote connection is still present.

## Synopsis

```c
BOOL od_carrier(void);
```

The result reflects the current serial, socket, Door32, or standard-I/O remote
session. Local mode returns false and sets `od_control.od_error` to
`ERR_NOREMOTE`. The function is most useful when automatic carrier checking
has been disabled and the application intends to handle loss of connection
itself.

Normal API calls and `od_kernel()` perform the configured automatic checks.

## See also

[`od_kernel()`](od_kernel.md), [`od_set_dtr()`](od_set_dtr.md)
