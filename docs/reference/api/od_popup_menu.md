# `od_popup_menu()`

Displays a popup menu and allows the user to choose an item.

## Synopsis

```c
INT od_popup_menu(char *pszTitle, char *pszText,
    INT nLeft, INT nTop, INT nLevel, WORD uFlags);
```

`pszTitle` supplies the optional title and `pszText` describes the menu text.
`nLeft` and `nTop` position the menu; `nLevel` selects the saved menu level and
must be within the implementation's supported level range. `uFlags` combines the
[`MENU_*`](../constants/display.md) options from [`OpenDoor.h`](index.md).

ANSI or AVATAR support is required. A positive return is the one-based selected
item. [`POPUP_ESCAPE`](../constants/display.md),
[`POPUP_LEFT`](../constants/display.md), and
[`POPUP_RIGHT`](../constants/display.md) report navigation;
[`POPUP_ERROR`](../constants/display.md) reports failure. Invalid input,
graphics mode, or allocation failure is reported through
[`od_control.od_error`](../control/runtime.md).

## See also

[`od_window_create()`](od_window_create.md),
[`od_hotkey_menu()`](od_hotkey_menu.md), [Menus and
screen](../constants/display.md)
