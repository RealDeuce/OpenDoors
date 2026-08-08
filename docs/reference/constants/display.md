# Menus and screen

## Popup menu flags

Combine these flags with bitwise OR for
[`od_popup_menu()`](../api/od_popup_menu.md).

| Name | Effect |
| --- | --- |
| `MENU_NORMAL` | Default behavior; has no effect when combined with other flags. |
| `MENU_ALLOW_CANCEL` | Let Escape close the menu and return `POPUP_ESCAPE`. |
| `MENU_PULLDOWN` | Let Left and Right close the menu and return `POPUP_LEFT` or `POPUP_RIGHT`. |
| `MENU_KEEP` | Leave the menu on screen after an ordinary selection so it can be resumed by level. |
| `MENU_DESTROY` | Remove a kept menu at the specified level instead of reading a selection. |

## Popup menu results

Positive return values identify menu entries. These values report other
outcomes:

| Name | Meaning |
| --- | --- |
| `POPUP_ERROR` | The menu could not be created or used. |
| `POPUP_ESCAPE` | The user cancelled with Escape. |
| `POPUP_LEFT` | Pulldown navigation requested the menu to the left. |
| `POPUP_RIGHT` | Pulldown navigation requested the menu to the right. |

## Scrolling

| Name | Effect |
| --- | --- |
| `SCROLL_NORMAL` | Clear lines exposed by [`od_scroll()`](../api/od_scroll.md). |
| `SCROLL_NO_CLEAR` | Leave newly exposed lines unchanged. |

## Local status line

| Name | Meaning |
| --- | --- |
| `STATUS_NORMAL` | Select the personality's normal local status display. |
| `STATUS_ALTERNATE_1` | Select personality-defined alternate display 1. |
| `STATUS_ALTERNATE_2` | Select personality-defined alternate display 2. |
| `STATUS_ALTERNATE_3` | Select personality-defined alternate display 3. |
| `STATUS_ALTERNATE_4` | Select personality-defined alternate display 4. |
| `STATUS_ALTERNATE_5` | Select personality-defined alternate display 5. |
| `STATUS_ALTERNATE_6` | Select personality-defined alternate display 6. |
| `STATUS_ALTERNATE_7` | Select personality-defined alternate display 7. |
| `STATUS_NONE` | Hide the local status display. |

Pass these values to [`od_set_statusline()`](../api/od_set_statusline.md).

## Box-character indices

The eight entries of
[`od_control.od_box_chars`](../control/customization.md) are indexed as follows:

| Index | Position |
| --- | --- |
| `BOX_UPPERLEFT` | Upper-left corner. |
| `BOX_TOP` | Top edge. |
| `BOX_UPPERRIGHT` | Upper-right corner. |
| `BOX_LEFT` | Left edge. |
| `BOX_LOWERLEFT` | Lower-left corner. |
| `BOX_LOWERRIGHT` | Lower-right corner. |
| `BOX_BOTTOM` | Bottom edge. |
| `BOX_RIGHT` | Right edge. |

## Terminal detection

`DETECT_NORMAL` is the only currently defined flag value for
[`od_autodetect()`](../api/od_autodetect.md). It requests the normal ANSI and
RIP capability probes.
