# `od_autodetect()`

Attempts to detect capabilities reported by the remote terminal.

## Synopsis

```c
void od_autodetect(INT nFlags);
```

`nFlags` is reserved and is currently ignored; pass `DETECT_NORMAL`. OpenDoors
sends the ANSI and RIP queries, waits for replies, and enables the corresponding
[`od_control.user_ansi`](../control/caller.md) or
[`user_rip`](../control/caller.md) state when detected. In local mode it enables
ANSI without probing.

Capabilities already known to be present are not cleared merely because a
terminal does not answer a probe. Most BBS door-information formats already
report these values, so explicit detection is normally needed only when that
information is unavailable. The function returns no value.

## See also

[Terminal and screen model](../../guides/terminal-screen.md), [Constants and
flags](../constants.md)
