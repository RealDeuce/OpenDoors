# `od_set_personality()`

Selects a previously registered local-interface personality by name.

## Synopsis

```c
BOOL od_set_personality(const char *pszName);
```

The nonempty name may identify a built-in personality or one installed with
[`od_add_personality()`](od_add_personality.md). The function returns true when
a match is selected. The text-mode implementation reports an empty name as
[`ERR_PARAMETER`](../constants.md) and a missing name as
[`ERR_LIMIT`](../constants.md); targets without the personality system return
false with [`ERR_UNSUPPORTED`](../constants.md).

Personalities control the local status display and sysop keys; they do not
change the remote terminal protocol. The caller must not pass a null name.

## See also

[`od_add_personality()`](od_add_personality.md),
[`od_set_statusline()`](od_set_statusline.md)
