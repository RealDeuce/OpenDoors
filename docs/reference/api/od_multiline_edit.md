# `od_multiline_edit()`

Edits text which spans multiple display lines.

## Synopsis

```c
INT od_multiline_edit(char *pszBufferToEdit,
    UINT unBufferSize, tODEditOptions *pEditOptions);
```

`pszBufferToEdit` contains the initial text and receives the result.
`unBufferSize` is the complete destination capacity, including the terminating
nul. `pEditOptions` selects the edit rectangle, colors, callbacks, and behavior.

The function requires ANSI or AVATAR graphics. Its return value describes how
editing ended. Invalid buffers or options set
[`ERR_PARAMETER`](../constants/errors.md); lack of graphics sets
[`ERR_NOGRAPHICS`](../constants/errors.md); allocation failure sets
[`ERR_MEMORY`](../constants/errors.md).

## See also

[`od_edit_str()`](od_edit_str.md), [`od_input_str()`](od_input_str.md), [Types
and callbacks](../types.md)
