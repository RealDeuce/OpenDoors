# `od_hotkey_menu()`

Displays a terminal file while watching for one of a set of hotkeys.

## Synopsis

```c
char od_hotkey_menu(char *pszFileName, char *pszHotKeys, BOOL bWait);
```

`pszFileName` uses the normal display-file selection rules. `pszHotKeys` is the
set of accepted characters. If `bWait` is true, OpenDoors waits after display
until a valid key arrives; otherwise it may return when the file finishes.

The return value is the selected key, with the same case-handling used by the
menu input code, or zero when no selection is available. File and parameter
failures are reported through `od_control.od_error`.

## See also

[`od_send_file()`](od_send_file.md), [`od_get_answer()`](od_get_answer.md),
[`od_popup_menu()`](od_popup_menu.md)
