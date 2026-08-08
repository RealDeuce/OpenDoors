# `od_input_str()`

Inputs a string from the user

## Synopsis

```c
void od_input_str(char *pszInput, INT nMaxLength,
    unsigned char chMin, unsigned char chMax);
```

## Return value

N/A

## Description

To perform string input within OpenDoors, one of two functions can be used, `od_input_str()` and [`od_edit_str()`](od_edit_str.md). The first function, `od_input_str()`, allows simple line input and editing, and can be used in ASCII, ANSI, AVATAR and RIP modes. The second function, [`od_edit_str()`](od_edit_str.md), allows many formatted input options, advanced line editing, and other features, but requires the use of ANSI, AVATAR or RIP graphics modes.

The `od_input_str()` function allows you to input a string from the user. The string will be permitted to have up to the number of characters specified by the max_len parameter, and all characters must be between the values of the min_char and max_char parameters. This function will wait until the user presses the [Enter] key to finish inputting the string.

The first parameter passed to this function should be a pointer to the string where the user's input should be stored. So, if you wanted to store a string of up to 30 characters inputted by the user, you might define this string as follows:

```text
char input_string[31];
```

Notice here than the string must be long enough to hold the thirty characters which can be entered by the user, along with the additional "null" character which is used to indicate the end of a string in C. Hence, the length of the string should always be at least one greater than the total number of characters the user is permitted to enter, passed in the nMaxLength parameter.

The second parameter passed to the `od_input_str()` function should be an integer value indicating the maximum number of characters which can be input by the user. For example, if this parameter had a value of 10, the user would be able to enter a string containing any number of characters up to and including 10 characters. If this parameter had a value of 1, the user would only be able to enter a single character. However, the user would be able to backspace, change the character, and press [Enter] when they were satisfied with their entry. Note that even if you only ask the `od_input_str()` function to input a single character, it will still expect a STRING to be passed to it, and will return a string with either zero or one character, followed by a null (string terminator) character.

The third and fourth parameters passed to this function allow you to control what characters the user will be permitted to enter as part of the string. For example, you could set the minimum character to the '0' character and the maximum character to the '9' character, permitting the user to only enter numeric characters. On the other hand, you could permit the user to enter all ASCII characters in the range from 32 to 127. The `od_input_str()` function will permit characters in the range beginning with the character passed as minchar, up to and including the character passed as maxchar.

## Examples

Below are a number of examples of the use of the `od_input_str()` function in various applications:

\- To input a two character number (only digits from 0-9):

```c
od_input_str(string, 2, '0', '9');
```

\- To input a 35 character name (characters from Space to ASCII 127):

```c
od_input_str(string, 35, 32, 127);
```

## Additional details

`pszInput` is the destination buffer. Existing contents are not displayed or
edited: input begins at position zero and replaces the buffer when the user
finishes. `nMaxLength` is the largest accepted string length, excluding its nul
terminator. `chMin` and `chMax` give the inclusive range of acceptable character
values.

The user may edit with the normal erase keys and finishes with Enter. The
destination must have room for `nMaxLength + 1` bytes. Invalid parameters are
reported through [`od_control.od_error`](../control/runtime.md). The function
returns no value.

## See also

[`od_edit_str()`](od_edit_str.md), [`od_get_key()`](od_get_key.md), [`od_clear_keybuffer()`](od_clear_keybuffer.md)

[`od_edit_str()`](od_edit_str.md), [`od_multiline_edit()`](od_multiline_edit.md)
