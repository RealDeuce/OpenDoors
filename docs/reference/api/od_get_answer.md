# `od_get_answer()`

Function to allow the user to respond to a prompt using only certain keys.

## Synopsis

```c
char od_get_answer(const char *pszOptions);
```

## Return value

Character that user entered

## Description

This function can be used to get a response from the user, when only particular responses should be accepted. The parameter to the `od_get_answer()` function is simply a string listing the valid responses. The function will wait until the user selects one of the valid responses, and then return that response. The function is case insensitive, and will return the character in the same case that was supplied to it in the string.

## Examples

od_get_answer("YN"); - If the user presses 'y', will return 'Y'.

od_get_answer("yn"); - If the user presses 'y', will return 'y'.

od_get_answer("ABC 123\n\rZ"); - Valid responses will be: [A], [B], [C], [SPACE], [1], [2], [3], [ENTER], [Z]

## Additional details

`pszOptions` is a nul-terminated string containing the allowed keys. Matching
is case-insensitive; the return value uses the form represented in the options
string. Input from both the remote user and enabled local keyboard is accepted.

The function continues processing OpenDoors housekeeping while it waits.
`pszOptions` must be non-null and contain at least one character; the legacy
interface does not diagnose an unusable option set.

## See also

[`od_get_key()`](od_get_key.md), [`od_hotkey_menu()`](od_hotkey_menu.md)
