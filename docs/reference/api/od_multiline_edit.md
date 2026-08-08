# `od_multiline_edit()`

Provides a multiple line text editor which can be used for entering editing any text that spans more than one line, such as messages or text files.

## Synopsis

```c
INT od_multiline_edit(char *pszBufferToEdit,
    UINT unBufferSize, tODEditOptions *pEditOptions);
```

## Return value

OD_MULTIEDIT_SUCCESS on success, or OD_MULTIEDIT_ERROR on failure

## Description

This function provides a text editor with optional word wrap capabilities. This editor can be used for entering or editing text files, messages or other information that spans multiple lines. The editor can be configured to operate in full-screen mode, or to occupy any smaller area of the screen that you specify. It provides the navigation (home / end / page up / arrow keys) features and editing features (insert / overwrite mode, Ctrl-Y to delete a line, etc.) that you would expect.

The [`od_multiline_edit()`](od_multiline_edit.md) function is designed to be both easy to use and very flexible. To that end, the function only takes three parameters. The first two parameters are required, and the third parameter is an optional options structure. The first parameter, pszBufferToEdit, is a pointer to the buffer of text to edit. This buffer must always be a '\0'-terminated string. This buffer must be initialized before calling [`od_multiline_edit()`](od_multiline_edit.md). The second parameter, unBufferSize, indicates the size of the buffer that is passed in pszBufferToEdit. Note that this should be the total amount of space that is available in the buffer for text entered by the user, not the length of data that is actually initially in the buffer. If you do not wish to customize any of the [`od_multiline_edit()`](od_multiline_edit.md) options, then you may simply set the third parameter to 0. Hence, a simple example of how to use [`od_multiline_edit()`](od_multiline_edit.md) is:

char szMyEditBuffer[4000] = ""; od_multiline_edit(szMyEditBuffer, sizeof(szMyEditBuffer), NULL);

If you wish to customize [`od_multiline_edit()`](od_multiline_edit.md), you should pass a pointer to a tODEditOptions structure as the third parameter. You should initialize this entire structure to zeros before attempting to use it. You can then set any values of this structure which you wish to change from their default. Any values that are left at 0 will automatically revert to their defaults. For example, if you wanted to specify a text format other than the default, you could create, initialize and pass in a tODEditOptions structure as follows:

char szMyEditBuffer[4000] = ""; tODEditOptions MyEditOptions; memset(&MyEditOptions, 0, sizeof(MyEditOptions)); MyEditOptions.TextFormat = FORMAT_LINE_BREAKS; od_multiline_edit(szMyEditBuffer, sizeof(szMyEditBuffer), &MyEditOptions);

The definition of the tODEditOptions structure is as follows:

```text
typedef struct
{
   INT nAreaLeft;
   INT nAreaTop;
   INT nAreaRight;
   INT nAreaBottom;
   tODEditTextFormat TextFormat;
   tODEditMenuResult (*pfMenuCallback)(void *pUnused);
   void * (*pfBufferRealloc)(void *pOriginalBuffer,
      UINT unNewSize);
   DWORD dwEditFlags;
   char *pszFinalBuffer;
   UINT unFinalBufferSize;
} tODEditOptions;
```

nAreaLeft, nAreaTop, nAreaRight, nAreaBottom allows you to specify the portion of the screen that the text editor should use. This defaults to 1, 1 - 80, 23.

TextFormat allows you to specify what format the text should be stored in the buffer using. The default is FORMAT_PARAGRAPH_BREAKS, which specifies that a line break only appears at the end of each paragraph, and that the contents of a paragraph are word wrapped. FORMAT_LINE_BREAKS specifies that a line break appears at the end of each line of text on the screen, and that newly entered text is word wrapped. FORMAT_NO_WORDWRAP is equivalent to FORMAT_LINE_BREAKS, except that newly entered text is not word wrapped. Instead, lines may be arbitrarily long. For each of these text formats, [`od_multiline_edit()`](od_multiline_edit.md) automatically decides whether line breaks should take the form of a carriage return ('\r'), line feed ('\n'), or some combination of these, based on what it sees in the buffer that you supply. If no line breaks are found in the buffer, then the default is to use just a line feed ('\n') character. FORMAT_FTSC_MESSAGE specifies a FTSC- compliant message, such as is used in a *.MSG message file. Among other things, this specifies that carriage returns ('\r') end paragraphs, and that line feeds ('\n') should be ignored.

pfMenuCallback allows you to provide a callback function that will be called when the user presses the escape (or control-Z) key. This allows you to provide a menu that can be accessed from within the text editor. This function should return EDIT_MENU_DO_NOTHING if the editor should continue normally, or EDIT_MENU_EXIT_EDITOR if the [`od_multiline_edit()`](od_multiline_edit.md) should return. If no menu callback function is provided, then [`od_multiline_edit()`](od_multiline_edit.md) always returns when the escape or control-z key is pressed.

pfBufferRealloc allows you to provide a function which will attempt to reallocate a larger buffer if the user enters more text than will fit in the originally supplied buffer. You should only do this if you have dynamically allocated the buffer that you initially passed into [`od_multiline_edit()`](od_multiline_edit.md). If you allocated the buffer using malloc() or calloc(), then pfBufferRealloc can be set to point to the realloc() function. If you allocated the buffer using the C++ new operator, then you must write a your own reallocation function which obeys the same semantics as the C realloc() function. If no buffer reallocation function is provided, then [`od_multiline_edit()`](od_multiline_edit.md) will never allow the user to enter more text than will fit in the buffer that you initially supply. If you are using the buffer reallocation option, you can obtain a pointer to the final buffer, and the size of the final buffer, from the pszFinalBuffer and unFinalBufferSize members.

## Additional details

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
