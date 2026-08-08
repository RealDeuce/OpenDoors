# `od_get_key()`

Function to input a key from the user

## Synopsis

```c
char od_get_key(BOOL bWait);
```

## Return value

The next key waiting from the keyboard, or 0 if none.

## Description

This function retrieves the next key waiting in the OpenDoors keyboard buffer (see the description of the [`od_clear_keybuffer()`](od_clear_keybuffer.md) function, for more information on the OpenDoors keyboard buffer). The [`od_get_key()`](od_get_key.md) function allows your door to retrieve both those keystrokes pressed by the user, and the keystrokes pressed on the sysop keyboard (other than the sysop function keys), in the sequence they were pressed. Since input is accepted from both sources, it is possible for the sysop, as well as the remote user, to make selections and control the door.

Door input with OpenDoors can be accomplished with this function, with the [`od_input_str()`](od_input_str.md) function or with the [`od_edit_str()`](od_edit_str.md) function. The [`od_input_str()`](od_input_str.md) and [`od_edit_str()`](od_edit_str.md) functions is used to input an entire sequence of characters from the user (a string), and requires the user to press the [Enter] key when they are finished typing their input. On the other hand, the [`od_get_key()`](od_get_key.md) function is used to input a single keystroke (one character) from the user, and allows the user to make choices without having to press the enter key.

The [`od_get_key()`](od_get_key.md) function accepts a single parameter, which determines whether or not it should wait for the user to press a key, if they have not already done so. If you pass a FALSE value to [`od_get_key()`](od_get_key.md), then the function will not wait for a key to be pressed at the keyboard, but instead return a 0 if there are no keys waiting in the buffer. If you pass a TRUE value to [`od_get_key()`](od_get_key.md), then this function will instead wait for a key to be pressed. Also, while waiting for the user to press a key, the [`od_get_key()`](od_get_key.md) function will give up the processor to other waiting programs, if you door is running under DesqView.

If you are waiting for the user to make a choice from a menu or list of options, you will most likely pass a TRUE to the [`od_get_key()`](od_get_key.md) function, indicating that you wish for it to wait until a key is pressed. However, if you wish to continue other processing if no key is yet available from the keyboard, you should pass a FALSE to the [`od_get_key()`](od_get_key.md) function. For example, if you are displaying a screen of text, and wish to allow the user to pause or abort the display, you would simply call the [`od_get_key()`](od_get_key.md) function every few moments, passing it a value of FALSE. You would then be able to check if any control keys have been pressed, and if not, continue displaying text.

The [`od_get_key()`](od_get_key.md) function returns the ASCII value representing the keystroke that was made. If you are waiting for the user to make a particular choice, perhaps from a menu, you will most likely store the value returned by [`od_get_key()`](od_get_key.md) in a variable of type char. For example:

char key; ... key=od_get_key(TRUE);

You would then be able to determine which key the user pressed by testing the value of key, either by comparing it's numerical ASCII value, or by comparing it to a character constant. If you are testing for a non-character key, such as [ESCape], [Tab] or [Return], you may wish to use the ASCII value of that key. For example, if you wished to take some action in the case that the user presses the [Enter]/[Return] key, who's ASCII value is 13, you could do:

```text
key=od_get_key(TRUE);        /* Get keypress from user */
if(key==13)             /* If key was [Enter]/[Return] */
{
   ...                      /* Whatever you want to do */
}
```

If you wish, instead, to respond to the user pressing a character key (perhaps as a choice from a menu), you can do so by using character constants, such as 'c', '6', or 'F'. Also, when testing for an alphabetical character, you will probably want to check for the user pressing either the upper or lower- case version of the letter. For example, if you wished to have the user press the [Y] key to continue, you could test for either an upper or lower-case Y as follows:

```text
key=od_get_key(TRUE);       /* Get keypress from user */
if(key=='y' || key=='Y')        /* If key was [y]/[Y] */
{
   ...                     /* Whatever you want to do */
}
```

The charts on the following page lists the decimal value and corresponding keystroke(s) of each of the ASCII values from 0 to 127.

ASCII  KEYSTROKE                       |  ASCII   KEYSTROKE -----  ------------------------------  |  -----   ---------------------- 0    [Control]-[@]                   |   15     [Control]-[O] 1    [Control]-[A]                   |   16     [Control]-[P] 2    [Control]-[B]                   |   17     [Control]-[Q] 3    [Control]-[C]                   |   18     [Control]-[R] 4    [Control]-[D]                   |   19     [Control]-[S] 5    [Control]-[E]                   |   20     [Control]-[T] 6    [Control]-[F]                   |   21     [Control]-[U] 7    [Control]-[G]                   |   22     [Control]-[V] 8    [Control]-[H]/[Backspace]       |   23     [Control]-[W] 9    [Control]-[I]/[Tab]             |   24     [Control]-[X] 10    [Control]-[J]                   |   25     [Control]-[Y] 11    [Control]-[K]                   |   26     [Control]-[Z] 12    [Control]-[L]                   |   27     [ESCape] 13    [Control]-[M]/[Enter]/[Return]  |   32     [SpaceBar] 14    [Control]-[N]                   |

```text
ASCII  KEYSTROKE | ASCII  KEYSTROKE | ASCII  KEYSTROKE | ASCII  KEYSTROKE
-----  --------- | -----  --------- | -----  --------- | -----  ---------
 33       '!'    |  57       '9'    |  80       'P'    |  104      'h'
 34       '"'    |  58       ':'    |  81       'Q'    |  105      'i'
 35       '#'    |  59       ';'    |  82       'R'    |  106      'j'
 36       '$'    |  60       '<'    |  83       'S'    |  107      'k'
 37       '%'    |  61       '='    |  84       'T'    |  108      'l'
 38       '&'    |  62       '>'    |  85       'U'    |  109      'm'
 39    '\''  (') |  63       '?'    |  86       'V'    |  110      'n'
 40       '('    |  64       '@'    |  87       'W'    |  111      'o'
 41       ')'    |  65       'A'    |  88       'X'    |  112      'p'
 42       '*'    |  66       'B'    |  89       'Y'    |  113      'q'
 43       '+'    |  67       'C'    |  90       'Z'    |  114      'r'
 44       ','    |  68       'D'    |  91       '['    |  115      's'
 45       '-'    |  69       'E'    |  92    '\\'  (\) |  116      't'
 46       '.'    |  70       'F'    |  93       ']'    |  117      'u'
 47       '/'    |  71       'G'    |  94       '^'    |  118      'v'
 48       '0'    |  72       'H'    |  95       '_'    |  119      'w'
 49       '1'    |  73       'I'    |  96       '`'    |  120      'x'
 50       '2'    |  74       'J'    |  98       'b'    |  121      'y'
 51       '3'    |  75       'K'    |  99       'c'    |  122      'z'
 52       '4'    |  76       'L'    |  100      'd'    |  123      '{'
 53       '5'    |  77       'M'    |  101      'e'    |  124      '|'
 54       '6'    |  78       'N'    |  102      'f'    |  125      '}'
 55       '7'    |  79       'O'    |  103      'g'    |  126      '~'
 56       '8'    |                  |                  |  127    [DELete]
```

## Examples

For examples of the use of the [`od_get_key()`](od_get_key.md) function, see the examples in the description portion, above, and the examples for the [`od_exit()`](od_exit.md) and [`od_clear_keybuffer()`](od_clear_keybuffer.md) functions. For further examples of this function, see the example program EX_VOTE.C.

## Additional details

If `bWait` is true, the function waits until a character is available. If it is
false and the queue is empty, the function returns zero immediately. Remote and
enabled local input are returned in arrival order. Line-feed characters are
discarded, so a waiting call continues past them and a non-waiting call can
block after finding a queued line feed while it waits for the next character.

After returning a character, the function sets [`od_control.od_last_input`](../control/runtime.md#od_last_input) to
zero for remote input or one for local input.

Use [`od_get_input()`](od_get_input.md) when extended keys or the source of an
event must be distinguished.

## See also

[`od_get_input()`](od_get_input.md), [`od_input_str()`](od_input_str.md), [`od_edit_str()`](od_edit_str.md), [`od_clear_keybuffer()`](od_clear_keybuffer.md)

[`od_key_pending()`](od_key_pending.md), [`od_get_input()`](od_get_input.md),
[`od_clear_keybuffer()`](od_clear_keybuffer.md)
