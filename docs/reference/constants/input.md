# Input and editors

## Input events and waits

| Name | Meaning |
| --- | --- |
| `EVENT_CHARACTER` | [`tODInputEvent`](../types.md) contains an ordinary character. |
| `EVENT_EXTENDED_KEY` | [`tODInputEvent`](../types.md) contains one translated `OD_KEY_*` code. |
| `OD_NO_TIMEOUT` | Wait indefinitely for input. See [General and platform definitions](general.md#logical-values-and-timeouts). |
| `GETIN_NORMAL` | Translate supported terminal key sequences and control-key alternatives. |
| `GETIN_RAW` | Return each received byte separately without extended-key translation. |
| `GETIN_RAWCTRL` | Translate terminal key sequences but do not treat control-key alternatives as extended keys. |

The `GETIN_*` values are flags passed to
[`od_get_input()`](../api/od_get_input.md). Flags may be combined with bitwise
OR; `GETIN_RAW` disables all sequence translation regardless of the other
bits.

## Extended keys

| Name | Key |
| --- | --- |
| `OD_KEY_F1` | Function key F1. |
| `OD_KEY_F2` | Function key F2. |
| `OD_KEY_F3` | Function key F3. |
| `OD_KEY_F4` | Function key F4. |
| `OD_KEY_F5` | Function key F5. |
| `OD_KEY_F6` | Function key F6. |
| `OD_KEY_F7` | Function key F7. |
| `OD_KEY_F8` | Function key F8. |
| `OD_KEY_F9` | Function key F9. |
| `OD_KEY_F10` | Function key F10. |
| `OD_KEY_F11` | Function key F11. |
| `OD_KEY_F12` | Function key F12. |
| `OD_KEY_UP`, `OD_KEY_DOWN` | Up and down arrows. |
| `OD_KEY_LEFT`, `OD_KEY_RIGHT` | Left and right arrows. |
| `OD_KEY_INSERT`, `OD_KEY_DELETE` | Insert and Delete. |
| `OD_KEY_HOME`, `OD_KEY_END` | Home and End. |
| `OD_KEY_PGUP`, `OD_KEY_PGDN` | Page Up and Page Down. |
| `OD_KEY_SHIFTTAB` | Shift-Tab. |

These codes are reported in
[`tODInputEvent.chKeyPress`](../types.md#input-events) when the event type is
`EVENT_EXTENDED_KEY`.

## Single-line editor flags

Combine flags with bitwise OR and pass them to
[`od_edit_str()`](../api/od_edit_str.md).

| Name | Effect |
| --- | --- |
| `EDIT_FLAG_NORMAL` | Default behavior with no optional flags. |
| `EDIT_FLAG_NO_REDRAW` | Do not draw or erase the field on entry and exit. |
| `EDIT_FLAG_FIELD_MODE` | Allow previous-field and next-field keys to finish editing. |
| `EDIT_FLAG_EDIT_STRING` | Edit the initialized input string instead of starting empty. |
| `EDIT_FLAG_STRICT_INPUT` | Prevent edits which would make characters violate their format positions. |
| `EDIT_FLAG_PASSWORD_MODE` | Display the field character instead of each typed character. |
| `EDIT_FLAG_ALLOW_CANCEL` | Allow Escape to restore the original value and cancel. |
| `EDIT_FLAG_FILL_STRING` | Accept only when every input position is filled. |
| `EDIT_FLAG_AUTO_ENTER` | Accept automatically when the field becomes full. |
| `EDIT_FLAG_AUTO_DELETE` | When editing an existing value, replace it if the first key is ordinary input. |
| `EDIT_FLAG_KEEP_BLANK` | Leave unused field-background characters visible after editing. |
| `EDIT_FLAG_PERMALITERAL` | Display format literals permanently and prevent their removal. |
| `EDIT_FLAG_LEAVE_BLANK` | Return an empty string when no non-literal character was entered. |
| `EDIT_FLAG_SHOW_SIZE` | Display exactly the permitted input width without the usual extra cursor cell. |

## Single-line editor results

| Name | Meaning |
| --- | --- |
| `EDIT_RETURN_ERROR` | Editing failed. |
| `EDIT_RETURN_CANCEL` | The user cancelled and the original value was restored. |
| `EDIT_RETURN_ACCEPT` | The user accepted the value. |
| `EDIT_RETURN_PREVIOUS` | Field mode ended with a previous-field key. |
| `EDIT_RETURN_NEXT` | Field mode ended with a next-field key. |

## Multi-line editor values

| Name | Meaning |
| --- | --- |
| `FORMAT_PARAGRAPH_BREAKS` | Preserve paragraphs while allowing display-line wrapping. |
| `FORMAT_LINE_BREAKS` | Preserve explicit line breaks. |
| `FORMAT_FTSC_MESSAGE` | Use FTSC-style message line conventions. |
| `FORMAT_NO_WORDWRAP` | Do not perform automatic word wrapping. |
| `EDIT_MENU_DO_NOTHING` | The menu callback requests no editor action. |
| `EDIT_MENU_EXIT_EDITOR` | The menu callback requests that editing finish. |
| `EFLAG_NORMAL` | Default [`tODEditOptions.dwEditFlags`](../types.md#multi-line-editor-options) value. No other multi-line flags are currently defined. |
| `OD_MULTIEDIT_ERROR` | [`od_multiline_edit()`](../api/od_multiline_edit.md) failed. |
| `OD_MULTIEDIT_SUCCESS` | [`od_multiline_edit()`](../api/od_multiline_edit.md) completed successfully. |
