# `od_add_personality()`

Installs a custom local status-line and function-key personality.

## Synopsis

```c
BOOL od_add_personality(const char *pszName, BYTE btOutputTop,
    BYTE btOutputBottom, OD_PERSONALITY_PROC *pfPerFunc);
```

Call this function before any operation which initializes OpenDoors. `pszName`
identifies the personality, the two line numbers define its local door-output
area, and `pfPerFunc` receives [`PEROP_*`](../types.md) operations.

The function returns true when the personality is registered. An exhausted
personality table returns false with [`ERR_LIMIT`](../constants.md). Custom
personalities affect the text-mode local interface; targets without that
interface return false with [`ERR_UNSUPPORTED`](../constants.md). The name and
callback must remain valid inputs; this legacy interface does not validate null
pointers.

## See also

[`od_set_personality()`](od_set_personality.md),
[`od_set_statusline()`](od_set_statusline.md), [Types and
callbacks](../types.md)
