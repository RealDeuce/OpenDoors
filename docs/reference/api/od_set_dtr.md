# `od_set_dtr()`

Raises or lowers the serial connection's DTR signal.

## Synopsis

```c
void od_set_dtr(BOOL bHigh);
```

Passing true raises DTR; passing false lowers it and can disconnect a modem
caller. The operation is passed to the active communication method. Local mode
sets [`od_control.od_error`](../control/runtime.md) to
[`ERR_NOREMOTE`](../constants.md). The function returns no value.

## See also

[`od_carrier()`](od_carrier.md), [`od_exit()`](od_exit.md)
