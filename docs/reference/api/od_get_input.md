# `od_get_input()`

Retrieves one translated input event, including extended keys.

## Synopsis

```c
BOOL od_get_input(tODInputEvent *pInputEvent,
    tODMilliSec TimeToWait, WORD wFlags);
```

`pInputEvent` receives the event type, source, and character or extended-key
code. `TimeToWait` is the maximum wait in milliseconds; use
[`OD_NO_TIMEOUT`](../types.md) to wait indefinitely. `wFlags` combines the
[`GETIN_*`](../constants.md) translation options.

The function returns true when an event was obtained. A timeout or immediate
empty-queue check returns false without manufacturing an input event; a null
destination sets [`ERR_PARAMETER`](../constants.md). Extended terminal key
sequences are combined into one [`EVENT_EXTENDED_KEY`](../types.md) event when
translation is enabled.

## See also

[`od_get_key()`](od_get_key.md), [`od_key_pending()`](od_key_pending.md), [Types
and callbacks](../types.md)
