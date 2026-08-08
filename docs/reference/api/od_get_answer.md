# `od_get_answer()`

Waits for a single key from a caller-supplied set of acceptable answers.

## Synopsis

```c
char od_get_answer(const char *pszOptions);
```

`pszOptions` is a nul-terminated string containing the allowed keys. Matching
is case-insensitive; the return value uses the form represented in the options
string. Input from both the remote user and enabled local keyboard is accepted.

The function continues processing OpenDoors housekeeping while it waits.
`pszOptions` must be non-null and contain at least one character; the legacy
interface does not diagnose an unusable option set.

## See also

[`od_get_key()`](od_get_key.md), [`od_hotkey_menu()`](od_hotkey_menu.md)
