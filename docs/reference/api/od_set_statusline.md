# `od_set_statusline()`

To set the currently displayed status line.

## Synopsis

```c
void od_set_statusline(INT nSetting);
```

## Return value

N/A

## Description

If you have the OpenDoors status line enabled within your door program (as is the default), the sysop will be able to control the setting of the status line using the F1 - F10 keys on the keyboard. These function keys are as follows:

```text
[F1] -  Display basic door and user information
[F2] -  Display phone numbers and important dates
[F3] -  Display security flags and up/download info
[F4] -  Display system information and current time
[F5] -  Display message info and user's settings
[F6] -  Display chat reason and sysop's comment
[F9] -  Display help information for sysop
[F10] - Turn off the status line
```

Using the [`od_set_statusline()`](od_set_statusline.md) function, you can manually set which of these status line settings is currently selected. The [`od_set_statusline()`](od_set_statusline.md) accepts a single parameter, which should be one of the values listed below, which indicates which status line you would like to have selected:

```text
+---------------+---------------+------------------------------+
|               | Corresponding |                              |
| Value         | Function Key  | Meaning                      |
+---------------+---------------+------------------------------+
| STATUS_NORMAL | [F1]          | Basic door and user info     |
| STATUS_NONE   | [F10]         | Turn off status line         |
| STATUS_HELP   | [F9]          | Displays help for the sysop  |
| STATUS_USER1  | [F2]          | Phone Numbers and dates      |
| STATUS_USER2  | [F3]          | Security flags & up/downloads|
| STATUS_USER3  | [F5]          | Message info & user settings |
| STATUS_USER4  | [F6]          | Chat reason and sysop comment|
| STATUS_SYSTEM | [F4]          | System info & current time   |
+---------------+---------------+------------------------------+
(Note that these keys may be customized using variables in the
 OpenDoors control structure.)
```

Keep in mind that the [`od_set_statusline()`](od_set_statusline.md) function only temporarily changes the current status line setting, and that the sysop will still be able to change the status line to any of the other settings using the function keys. For instance, if you wished to allow the sysop to normally see all 25 lines of text displayed by your door, but at the same time to still allow the sysop to turn on the status line at any time, you could place the line

```text
od_set_statusline(STATUS_NONE);
```

at the beginning of your program. Similarly, when the user pages the sysop, OpenDoors itself calls

```text
od_set_statusline(STATUS_USER4);
```

in order to display the status line which shows the user's reason for chat, while still allowing the sysop to switch back to any of the other status lines.

If you wish to permanently turn off the OpenDoor's status line, without allowing the sysop to be able to turn it back on using the sysop function keys, simply set the "od_control.od_status_on" variable to FALSE. This variable is described in the OpenDoors control structure section of this manual.

## Additional details

Use the [`STATUS_*`](../constants/display.md) values defined by [`OpenDoor.h`](index.md)
to select the normal line, one of seven alternate lines, or no line. Values
outside 0 through 8 are converted to [`STATUS_NORMAL`](../constants/display.md#status_normal). If status-line processing
has been disabled through [`od_status_on`](../control/customization.md#od_status_on), the call makes no change. Selecting
the already active line also makes no change unless [`od_update_status_now`](../control/runtime.md#od_update_status_now) is
set.

On DOS and DOS32, the active personality redraws its local area as necessary;
this affects the operator display and does not alter remote output. Other
targets set [`ERR_UNSUPPORTED`](../constants/errors.md). The function returns no
value.

## See also



[`od_set_personality()`](od_set_personality.md), [Menus and
screen](../constants/display.md)
