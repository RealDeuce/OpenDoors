# `od_edit_str()`

Provides formatted, single-line string editing.

## Synopsis

```c
WORD od_edit_str(char *pszInput, char *pszFormat, INT nRow,
    INT nColumn, BYTE btNormalColour, BYTE btHighlightColour,
    char chBlank, WORD nFlags);
```

`pszInput` contains the starting value and receives the edited result.
`pszFormat` defines editable positions and character classes. The row, column,
colors, blank character, and [`EDIT_*`](../constants.md) flags control
presentation and behavior.

ANSI or AVATAR graphics are required. The return value identifies the key or
editing condition which ended input. Invalid format or position sets
[`ERR_PARAMETER`](../constants.md); unavailable graphics sets
[`ERR_NOGRAPHICS`](../constants.md).

## See also

[`od_input_str()`](od_input_str.md),
[`od_multiline_edit()`](od_multiline_edit.md), [Constants and
flags](../constants.md)
