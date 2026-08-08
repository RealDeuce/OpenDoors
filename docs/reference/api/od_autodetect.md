# `od_autodetect()`

Attempts to automatically determine the terminal capabilities of the remote system.

## Synopsis

```c
void od_autodetect(INT nFlags);
```

## Return value

N/A

## Description

This function can be used to determine whether or not the remote terminal supports ANSI and/or RIP (Remote Imaging Protocol) graphics modes. This information is usually supplied to the door by the BBS software, through the door information file. For this reason, most door programs do not need to make used of this function. However, if your door will be running under any BBS software that does not report the ANSI or RIP capabilities of the remote system, you may wish to use this function. [`od_autodetect()`](od_autodetect.md) will set either of the following OpenDoors control structure variables to TRUE if the corresponding graphics mode is detected:

od_control.user_ansi     - TRUE if ANSI mode is available od_control.user_rip      - TRUE if RIP mode is available

However, if either of these variables have previously been set to TRUE (either explicitly by your program, or due to the corresponding modes being enabled in the door information file), and [`od_autodetect()`](od_autodetect.md) does not detect the corresponding graphics mode, they will not be set to FALSE. Not all terminal software that supports ANSI or RIP graphics mode will necessarily have the ability to report their graphics mode capabilities to the door. For this reason, failure to detect either of these modes does not necessarily indicate that they are not available. However, if these modes are detected by [`od_autodetect()`](od_autodetect.md), it is safe to assume that the remote system does support the detected mode.

The current implementation ignores `nFlags`. Pass [`DETECT_NORMAL`](../constants/display.md#detect_normal), the only
defined value, so the call remains compatible if a later version assigns
meaning to additional bits.

This function cannot auto-detect AVATAR mode, because there is no standard means of determining whether a remote system supports AVATAR mode.

## Examples

Below is an example of using [`od_autodetect()`](od_autodetect.md) in determining the remote terminal's graphics capabilities. Since not all terminal software supports auto-detection, this example will also prompt the user to determine their software's capabilities if [`od_autodetect()`](od_autodetect.md) fails to detect ANSI mode. This code assumes that if the terminal software supports the autodetection of ANSI mode, that it will also support the autodetection of RIP mode. OpenDoors assumes that ANSI mode is always available in conjunction with RIP mode.

/* Call the automatic terminal detection function */ [`od_autodetect()`](od_autodetect.md);

```c
/* If ANSI mode was not detected, ask the user about
if(!od_control.user_ansi)
{
   /* Prompt the user for ANSI capabilities */
   od_clr_scr();
   od_printf("Does your system support ANSI graphics?");
   od_printf(" (Y/N)");
```

```c
/* If the user chooses [Y]es */
if(od_get_answer("YN") == 'Y')
{
   /* Turn on ANSI mode */
   od_control.user_ansi = TRUE;
```

```c
/* Since ANSI mode is present, RIP mode may also */
/* be available. Prompt the user for RIP. */
od_printf("\r\n\n");
od_printf("Does your system support RIP graphics?");
od_printf(" (Y/N)");
```

```c
/* If the user chooses [Y]es */
if(od_get_answer("YN") == 'Y')
   /* Turn on RIP mode */
   od_control.user_rip = TRUE;
```

```c
/* Since ANSI mode is present, AVATAR mode may  */
/* also be available. Prompt the user for AVATAR. */
od_printf("\r\n\n");
od_printf("Does your system support AVATAR ");
od_printf("graphics? (Y/N)");
```

```c
   /* If the user chooses [Y]es */
   if(od_get_answer("YN") == 'Y')
      /* Turn on AVATAR mode */
      od_control.user_avatar = TRUE;
}
```

```c
   od_printf("\r\n\n");
}
```

## Additional details

`nFlags` is reserved and is currently ignored; pass
[`DETECT_NORMAL`](../constants/display.md). OpenDoors sends the ANSI and RIP
queries, waits for replies, and enables the corresponding
[`od_control.user_ansi`](../control/caller.md) or
[`user_rip`](../control/caller.md) state when detected. In local mode it enables
ANSI without probing.

Capabilities already known to be present are not cleared merely because a
terminal does not answer a probe. Most BBS door-information formats already
report these values, so explicit detection is normally needed only when that
information is unavailable. The function returns no value.

## See also



[Terminal and screen model](../../guides/terminal-screen.md), [Menus and
screen](../constants/display.md)
