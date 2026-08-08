# `od_edit_str()`

Allows you to perform formatted input with full line editing features, etc., in ANSI/AVATAR/RIP graphics mode.

## Synopsis

```c
WORD od_edit_str(char *pszInput, char *pszFormat, INT nRow,
    INT nColumn, BYTE btNormalColour, BYTE btHighlightColour,
    char chBlank, WORD nFlags);
```

## Return value

This function will return one of the following values:

EDIT_RETURN_ERROR        Indicates that an error has occurred, and the edit function was unable to run. This will occur if there is an error in one of the parameters, or if ANSI/AVATAR/RIP graphics is not available

EDIT_RETURN_CANCEL       Indicates that the user pressed the cancel key [ESC], and that the string was left unaltered.

EDIT_RETURN_ACCEPT       Indicates that the user pressed the accept key [Enter], or that the auto- enter feature was activated.

EDIT_RETURN_PREVIOUS     Indicates that the user wishes to move to the previous field, by pressing [UP ARROW], [SHIFT]-[TAB], etc.

EDIT_RETURN_NEXT         Indicates that the user wishes to move to the next field, by pressing [DOWN ARROW], [TAB], etc.

## Description

To perform string input within OpenDoors, one of two functions can be used, [`od_input_str()`](od_input_str.md) and `od_edit_str()`. The first function, [`od_input_str()`](od_input_str.md), allows simple line input and editing, and can be used in ASCII, ANSI, AVATAR and RIP modes. The second function, `od_edit_str()`, allows many formatted input options, advanced line editing, and other features, but requires the use of ANSI, AVATAR or RIP terminal modes.

As mentioned above, the `od_edit_str()` function allows for advanced line editing, such as inputting and deleting text from the middle of the string (whereas the [`od_input_str()`](od_input_str.md) function only allows editing from the end of the string, such as backspacing to erase a mistake). The edit functions available from the `od_edit_str()` are listed below. Note that some of these functions may or may not be available, depending upon the capabilities of the user's terminal program. While there is no single standard used for the transmission of special edit keys such as the arrow keys, the `od_edit_str()` function makes as much effort as possible to make all of the edit features available to most terminal programs. Many of the edit functions can be accesses using either [CONTROL]-key combinations or special keys such as the arrow keys, delete key, and so on. OpenDoors will recognize most of these special control keys when sent as either an ANSI control sequence (which is sent by most terminal programs), or as a DoorWay style scan code / ASCII code sequence (which is also available from many terminal programs, but is not usually required). The `od_edit_str()` edit functions are as follows. Note that all edit functions are always available from the local keyboard.

HOME - Moves the cursor to the beginning of the line being edited. Press the [HOME] key, either in DoorWay mode or from the local keyboard.

END - Moves the cursor to the end of the line being edited. Press the [END] key, either in DoorWay mode or from the local keyboard.

DELETE CHARACTER - Deletes the character under the cursor. Press [DELete] on the local keyboard, in DoorWay mode, and under many terminal programs without DoorWay mode. Alternatively, press [CONTROL]-[G].

BACKSPACE - Deletes the character left of the cursor. Press [BACKSPACE] or [CONTROL]-[H].

TOGGLE INSERT MODE - Switches the `od_edit_str()` function between insert mode and overwrite mode. Press [INSert], either in DoorWay mode, or from the local keyboard. Alternatively, press [CONTROL]-[V].

CURSOR LEFT - Moves the cursor left one character. Press [LEFT ARROW] on the local keyboard, in DoorWay mode, and under many terminal programs without DoorWay mode. Alternatively, press [CONTROL]-[S].

CURSOR RIGHT - Moves the cursor right one character. Press [RIGHT ARROW] on the local keyboard, in DoorWay mode, and under many terminal programs without DoorWay mode. Alternatively, press [CONTROL]-[D].

ERASE ENTIRE LINE - Press [CONTROL]-[Y].

ACCEPT INPUT - Press the [ENTER] / [RETURN] line to accept the input. Alternatively, press [CONTROL]-[Z]. Note that this key will only work when the current input is "valid" (ie, it conforms to the format string, which is described below)

CANCEL INPUT - Only available if specifically enabled on the `od_edit_str()` command line. Press [ESCape].

NEXT FIELD - If enabled, allows the user to move to the next field in a dialog box / form. Press [DOWN ARROW] in DoorWay mode and under many terminal programs without DoorWay mode. Alternatively, press [TAB]. Note that the [DOWN ARROW] key is NOT usually available from the local keyboard, as it is usually used to adjust the user's remaining time.

PREVIOUS FIELD - If enabled, allows the user to move to the previous field in a dialog box / form. Press [UP ARROW] in DoorWay mode and under many terminal programs without DoorWay mode. Alternatively, press [SHIFT]-[TAB] on the local keyboard or in DoorWay mode. Again, note that the [UP ARROW] key is NOT usually available from the local keyboard, as it is usually used to adjust the user's remaining time.

Let us now look at the parameters which the `od_edit_str()` function accepts. The first parameter, pszInput, is a pointer to the string where the user's input should be stored. It is important that this string be long enough to accommodate the longest input your format string will permit, including the '\0' C string terminator (ie, the string should be one character greater than the length of the format string, not including the format string's ' and " characters).

The second parameter, pszFormat, is a pointer to a string which specifies the format and maximum length of the input the `od_edit_str()` function should accept. Using the format string, not only do you specify the length of the input field, but you can also force the user's input into certain formats. For example, if you wished to input a North American style phone number, you could use a format string of "###-###-####". Then regardless of whether the user typed any dash character or not, their input would be converted, as they type, to the format of the phone number 613-599-5554. You could also specify a format string such of "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMM", which would permit the user to enter a name of up to 30 characters. Note that since the cursor can be moved to the position immediately following the last character, a the input field for a 30 character string will occupy 31 columns on the screen. The `od_edit_str()` function would then automatically capitalize the name, so that the first character of each word is capitalized, and the remain characters of the word is in lower case. Even if the user were to move the cursor to the middle of the string they had entered, and add or delete a space (and thus either make one work two or two words one), `od_edit_str()` would re- format the string to reflect the change. The valid characters for the format sting, along with their meanings, are listed below. Note that the format string is NOT case sensitive (except for literal strings delimited by the '' or "" characters), and space characters can be added at any point to increase legibility.

\#    Indicates that numeric characters from '0' to '9' are valid for this position

%    Indicates that numeric characters from '0' to '9', and the space character (' ') are valid for this position.

9    Indicates that numeric characters from '0' to '9', along with '.', '-' and '+' are valid for this position. This format style is intended for floating-point numeric input.

?    Indicates that any character is valid for this position.

\*    Indicates that any printable character, from ASCII 32 to ASCII 127, is valid for this position.

A    Indicates that alphabetical characters 'A' to 'Z', 'a' to 'z' and space (' ') are valid for this position.

C    Indicates that city name characters are valid for this position. As with the 'M' format character, words are automatically capitalized so that the first letter is in upper case, and all subsequent letters are in lower case. In addition to permitting alphabetical characters and the space (' ') character, the ',' and '.' characters are also accepted in this position.

D    Indicates that date characters '0' to '9', '-' and '/' are valid for this position.

F    Indicates that MS-DOS filename characters are valid for this position.

H    Indicates that hexidecimal character '0' to '9', 'A' to 'F' and 'a' to 'f' are valid for this position.

L    Indicates that only lower case alphabetical characters 'a' to 'z', and the space (' ') character is valid for this position. However, if the user attempts to enter an upper case alphabetical character in this position, it will automatically be converted to the lower case equivalent.

M    Indicates that name characters are valid for this position. These characters are the alphabetical characters 'A' to 'Z', 'a' to 'z', and the space character (' '). A character's case is converted such that the first character of a word is in upper case, and all other letters are in lower case.

T    Indicates that telephone number character '0' to '9', '(', ')', '-' and ' ' are valid for this position.

U    Indicates that only upper case alphabetical characters 'A' to 'Z', and the space (' ') character is valid for this position. However, if the user attempts to enter a lower case alphabetical character in this position, it will automatically be converted to the upper case equivalent.

W    Indicates that MS-DOS filename characters are permitted in this position, including the '*' and '?' wildcard characters.

X    Indicates that alphanumeric characters 'A' to 'Z', 'a' to 'z', '0' to '9' and ' ' are valid for this position.

Y    Indicates that yes/no characters 'Y', 'N', 'y', 'n' are valid for this position. The characters are automatically converted to upper case.

'/"  Single or double quotes can be used to specify sequences of characters that should appear at the same location in the input string (referred to elsewhere as "literal strings"). When the user is entering the string, these characters are automatically supplied, and the user is not required to type them. Literal strings must begin and end with the same quote character. Remember that the double quote (") character must be imbedded in C strings by preceding the quote character with a \ (backslash) character.

The third and fourth parameters, nRow and nColumn specify the location on the screen where the first (left most) character of the input field should be located. These parameters are identical to the nRow and nColumn parameters passed to the [`od_set_cursor()`](od_set_cursor.md) function. In other words, nRow specifies the line number on the screen, where 1 is the first line, and nColumn specifies the column across the screen, where 1 is the first column.

The fifth and sixth parameters, btNormalColor and btHighlightColor, allow you to specify the color of the input field. The fifth parameter, btNormalColor, specifies the color of the input field when input is not taking place and the sixth parameter, btHighlightColor, specifies the color of the field while input is taking place. Thus, if you had several input fields on the screen at one time, you would be able to make is easier for the user to identify the currently active field by having the field currently accepting input highlighted in a color distinct from the other fields. When the `od_edit_str()` function begins, it will change the current color of the field from the normal color to the highlighted color. Then, when the `od_edit_str()` function exits, it will change the current color of the field back to its normal color. If you do not wish to have the field highlighted, you can set both of these parameters to the same value, and disable field re-drawing by using the eighth parameter, flags.

The seventh parameter accepted by the `od_edit_str()` function, chBlank, will serve one of two purposes. Normally, this parameter will specify a background character to display in the unfilled portion at the end of the input field. This can be set to a character, such as the ASCII 177 grey block character, to produce a visual background to the field. Doing this will show the user visually how long the field is, and how many character they will be permitted to type into the field. Normally, this field will be displayed during input, and removed when the `od_edit_str()` function exits. However, you may cause the background to remain in place using the eighth parameter, flags. If you do not wish to have this "background" visual field effect, simply set the character parameter to a space (ASCII 32). In password input mode, this parameter will instead specify the character to display in place of characters typed by the user. In this case, the background display character defaults to the space (ASCII 32) character.

The eighth, and last, parameter accepted by the `od_edit_str()` function is the nFlags parameter. This parameter is a bit-mapped flags variable which allows you to control special features of the `od_edit_str()` function. More than one of these settings may be specified by listing a chain of the values, separated by the bitwise-or (|) operator. If you do not wish to turn on any of these modes, simply pass the EDIT_FLAG_NORMAL value as the flags parameter.

EDIT_FLAG_NORMAL - Default setting, use this value of none of the other flags below are active.

EDIT_FLAG_NO_REDRAW - When set, prevents the `od_edit_str()` function from re-drawing the input string and field when it starts up and exits. If you set this flag, the normal color and highlight color should contain the same value. If background character (the character parameter) is not a space (ASCII 32) character, you must draw the field background prior to calling `od_edit_str()`. Also, if you are calling `od_edit_str()` with the EDIT_FLAG_EDIT_STRING flag set, you must display the existing string in the field prior to calling `od_edit_str()`.

EDIT_FLAG_FIELD_MODE - Setting this flag specifies that `od_edit_str()` should operate in field input mode. In field input mode, the user may finish entering their input by pressing the previous field or next field button (arrow keys, tab keys, etc.), as described above. If the user chooses to finish and accept their input by pressing one of these keys, the `od_edit_str()` return value will reflect which choice they made. This will allow you to make it possible for the user to move between a number of input fields in a form / dialog box, as demonstrated in the example accompanying the [`od_draw_box()`](od_draw_box.md) function.

EDIT_FLAG_EDIT_STRING - Setting this flag specifies that `od_edit_str()` should edit a pre-existing string, instead of starting with a blank string. In this case, the input_string parameter MUST point to an initialized string. This string may either contain some text, or be empty, but `od_edit_str()` will expect to find a string terminator ('\0') character, and will begin editing the contents of the string prior to that character. If you do not set the EDIT_FLAG_EDIT_STRING flag, the previous contents of the input_string parameter is not significant, as `od_edit_str()` will automatically start with a blank string.

EDIT_FLAG_STRICT_INPUT - Setting this flag causes the `od_edit_str()` function to operate in "strict" input mode, which may be desirable if your input format contains more than one type of input. Normally, if you were inputting such a string, the user would be able to move to the middle of the string, and insert any text. Doing so would cause the rest of the input line to shift right. However, in cases where your format string specifies different types of character to be permitted in different positions, this can cause the input to be changed so that it no longer conforms to the format string. In this case, the user's input will no longer be valid, and the user will not be able to exit the function by pressing [ENTER] (although [ESCAPE] will still be available, if you activated it) until they change their input. However, when strict input mode is turned on, `od_edit_str()` will restrict the ways in which the user is permitted to edit the string, to prevent just such a case from occurring.

EDIT_FLAG_PASSWORD_MODE - Setting this flag causes the `od_edit_str()` function to operate in "password" mode. In password mode, the characters typed by the user will be hidden, displayed instead as the blank character specified in the "character" parameter.

EDIT_FLAG_ALLOW_CANCEL - When this flag is set, the user will be able to cancel their current input and abort the editing process by pressing their [ESCAPE] key. When they do so, any changes they have made to the input field will be canceled, and replaced by the original contents of the string. The `od_edit_str()` function will then exit, indicating that the user has canceled their input.

EDIT_FLAG_FILL_STRING - When set, this flag will force the user to enter a string that fills the entire length of the format string. Normally, the user will be able to enter a string of any length up to the maximum length specified by the format string. However in some cases, such as when inputting a date, you will want to have the input field filled. (Otherwise, the user would be able to enter only the first part of the date.)

EDIT_FLAG_AUTO_ENTER - When set, this flag will cause the `od_edit_str()` function to automatically simulate pressing of the [ENTER] key when the string is filled. This can be used to cause the `od_edit_str()` function to finish inputting as soon as a valid string is entered, instead of having to wait for the user to press [ENTER] / [RETURN].

EDIT_FLAG_AUTO_DELETE - When set, along with the EDIT_FLAG_EDIT_STRING flag, this flag will activate the auto-delete feature of the `od_edit_str()` function. When auto-delete is active, if the first key pressed by the user is not an edit control key, the existing text will automatically be deleted, and a totally new string accepted from the user. This could be useful when you are allowing the user to go back to edit a previous input. If the user wishes to only change part of the old string, they can move the cursor to the location where they wish to make the change, and perform their editing. However, if the user wishes to completely replace the old string with a new one, they can simply begin to type, and the old string will automatically be deleted, and the new string accepted.

EDIT_FLAG_KEEP_BLANK - Normally, OpenDoors will only display the input field background (as passed in the "character" parameter) while the user is editing the string, and will remove it when the `od_edit_str()` function exits. However, you may wish to continue having this field displayed after input has taken place, and the `od_edit_str()` function has exited. In this case, setting this flag will cause the background characters to remain visible after input has finished.

EDIT_FLAG_PERMALITERAL - When the format string contains literal characters (such as forcing a ':' character to be added to a time input by using the format string "##':'##':'##"), the `od_edit_str()` function can operate in one of two modes. In the default mode, the literal characters will only be displayed when they have been automatically added to the string. For instance, if you were inputting the current time using the above format string, this mode would result in the input field initially being blank. When the user types the first digit of the time, that number would appear. When the user types the second digit of the time, that number will appear, and then the colon character will automatically be added by OpenDoors. However, you can also set the `od_edit_str()` function to operate in "PermaLiteral" mode, by setting this flag. When the EDIT_FLAG_PERMALITERAL flag is set, the input field will initially contain the literal characters (ie, the colons in our example), with the cursor still located at the leftmost position in the input field. In this mode, the literal character become a permanent part of the input field, and can not be moved or deleted by the user - instead the cursor simply skips over the literal character's position.

EDIT_FLAG_LEAVE_BLANK - This flag applies to the special case where the first character or characters of the format string are literals. By default, the `od_edit_str()` function will always return a string containing at least these first literal characters. However, you can alter this behaviors by setting this flag. When set, if no non-literal characters have been entered in the string, `od_edit_str()` will return an empty string.

EDIT_FLAG_SHOW_SIZE - Normally, `od_edit()` adds an extra blank to the end of the input field, to give the cursor a space to move into when the field is full. However, you may prefer to have the input field be shown as exactly the maximum size of input that is permitted. Setting EDIT_FLAG_SHOW_SIZE does just this. In this case, the cursor will be positioned immediately past the end of the input field when the maximum number of characters have been entered.

## Examples

Below are several examples of typical uses of the `od_edit_str()` function. For the sake of simplicity, all of these examples perform their input beginning at the top, left hand corner of the screen, and store the user's input in the string variable named "string". For an example of the user of the `od_edit_str()` function in a dialog-box / form entry application, see the example accompanying the [`od_draw_box()`](od_draw_box.md) function.

To input a name with a maximum of 25 characters, having the first letter of each word automatically capitalized:

```c
od_edit_str(string, "MMMMMMMMMMMMMMMMMMMMMMMMM", 1, 1,
            0x03, 0x21, 176, EDIT_FLAG_NORMAL);
```

To input a North American style phone number, requiring that all digits be filled, and running in "strict input" mode:

```c
od_edit_str(string, "###'-'###'-'####",
            1, 1, 0x03, 0x21, 176,
            EDIT_FLAG_FILL_STRING|
            EDIT_FLAG_STRICT_INPUT);
```

To allow the user to edit a previously entered 20 character string, with auto-delete mode on. Any characters will be permitted in the string. Remember that when the EDIT_FLAG_EDIT_STRING flag is set, the string must be initialized prior to calling the `od_edit_str()` function.

```c
od_edit_str(string, "????????????????????",
            1, 1, 0x03, 0x21, 176,
            EDIT_FLAG_EDIT_STRING|
            EDIT_FLAG_AUTO_DELETE);
```

To input a password of up to 16 characters from the user. Here, the password will only be permitted to contain upper case characters, and the `od_edit_str()` password mode is used, with a small block displayed in place of any characters typed:

```c
od_edit_str(string, "UUUUUUUUUUUUUUUU",
            1, 1, 0x03, 0x21, 254,
            EDIT_FLAG_PASSWORD_MODE);
```

To input a two-digit number from the user, requiring that both digits be filled, and automatically accepting the input after the two digits have been entered (not requiring the user to press [ENTER]):

```c
od_edit_str(string, "##", 1, 1, 0x03, 0x21, 176,
            EDIT_FLAG_FILL_STRING|
            EDIT_FLAG_AUTO_ENTER);
```

To input a filename to download, as a field in a dialog box. Here, the filename will be permitted to contain valid filename characters, and the [`od_input_str()`](od_input_str.md) function will operate in field mode, with the cancel [ESCape] key enabled. Also, string edit mode will be enabled, allowing the user to edit a previously entered line, and the EDIT_FLAG_KEEP_BLANK flag will be set, causing the field background to remain displayed after the user exits. This time, however, auto-delete mode will not be used. Note that this combination of parameters expects that the field and it's contents will have already been displayed, prior to calling the `od_edit_str()` function.

```c
od_edit_str(string, "WWWWWWWWWWWW",
            1, 1, 0x03, 0x21, 176,
            EDIT_FLAG_EDIT_STRING|
            EDIT_FLAG_FIELD_MODE|
            EDIT_FLAG_ALLOW_CANCEL|
            EDIT_FLAG_KEEP_BLANK);
```

To input a string without the field background and line redrawing before and after input takes place:

```c
od_edit_str(string, "******************************",
            1, 1, 0x07, 0x07, ' ',
            EDIT_FLAG_NO_REDRAW);
```

To input a date, using PermaLiteral mode. Here, the month is entered by a three digit short form ("JAN", "FEB", etc.), and the literal characters such as the '-' and the "19" are a permanent part of the input field:

```c
od_edit_str(string,"UUU'-'##'-19'##",
            1, 1, 0x03, 0x21, 176,
            EDIT_FLAG_PERMALITERAL|
            EDIT_FLAG_FILL_STRING);
```

## Additional details

`pszInput` receives the edited result. It supplies the starting value only when
`EDIT_FLAG_EDIT_STRING` is present; without that flag the function begins with
an empty string. `pszFormat` defines editable positions, literals, and character
classes. At most 80 editable or literal positions are accepted. The row,
column, colors, blank character, and [`EDIT_*`](../constants/input.md) flags
control presentation and behavior.

The return value identifies the key or editing condition which ended input. A
null input or format pointer, a row or column below 1, an empty format, or a
format representing more than 80 positions returns `EDIT_RETURN_ERROR` and sets
[`ERR_PARAMETER`](../constants/errors.md). The function is designed for a
cursor-addressable graphics mode, but it does not perform its own ANSI/AVATAR
capability check and does not define `ERR_NOGRAPHICS` as a return condition.

## See also

[`od_input_str()`](od_input_str.md), `od_get_char()`, [`od_clear_keybuffer()`](od_clear_keybuffer.md)

[`od_input_str()`](od_input_str.md),
[`od_multiline_edit()`](od_multiline_edit.md), [Input and
editors](../constants/input.md)
