# `od_popup_menu()`

Displays a popup menu and allows the user to choose an item.

## Synopsis

```c
INT od_popup_menu(char *pszTitle, char *pszText,
    INT nLeft, INT nTop, INT nLevel, WORD uFlags);
```

`pszTitle` supplies the optional title and `pszText` describes the menu text.
`nLeft` and `nTop` position the menu; `nLevel` selects the saved menu level and
must be within the implementation's supported level range. `uFlags` combines
the `MENU_*` options from `OpenDoor.h`.

ANSI or AVATAR support is required. A positive return is the one-based selected
item. `POPUP_ESCAPE`, `POPUP_LEFT`, and `POPUP_RIGHT` report navigation;
`POPUP_ERROR` reports failure. Invalid input, graphics mode, or allocation
failure is reported through `od_control.od_error`.

## See also

[`od_window_create()`](od_window_create.md),
[`od_hotkey_menu()`](od_hotkey_menu.md), [Constants and flags](../constants.md)
