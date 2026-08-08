# `od_set_statusline()`

Temporarily changes whether the local OpenDoors status line is displayed.

## Synopsis

```c
void od_set_statusline(INT nSetting);
```

Use the [`STATUS_*`](../constants.md) values defined by [`OpenDoor.h`](index.md)
to select the normal setting, force the status line on, or force it off. The
selected personality redraws its local area as necessary. This setting affects
the operator display only and does not alter remote output. The function returns
no value.

## See also

[`od_set_personality()`](od_set_personality.md), [Constants and
flags](../constants.md)
