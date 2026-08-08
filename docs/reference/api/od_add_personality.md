# `od_add_personality()`

Installs a custom status line / sysop function key personality into OpenDoors.

## Synopsis

```c
BOOL od_add_personality(const char *pszName, BYTE btOutputTop,
    BYTE btOutputBottom, OD_PERSONALITY_PROC *pfPerFunc);
```

## Return value

TRUE on success FALSE on failure

## Description

If used, this function should be called before any other OpenDoors API functions. This function installs a new personality into OpenDoors. The first parameter specifies the string that will be used to identify the personality. This is the string that the user will be able to supply in the configuration file to select this personality, and is also the string that can be passed to [`od_set_personality()`](od_set_personality.md) to manually switch to this personality. The second and third parameters specify the 1-based to and bottom line numbers of the output window to be used with this personality. For instance, a top value of 1 and bottom value of 23 would cause all door output to be displayed on the first 23 lines of the screen, leaving the bottom two lines for use by the personality's status line. The last parameter is a pointer to the personality function, which OpenDoors will call to perform various operations with that involve the personality. For more information on personalities and the OpenDoors Multiple Personality System, see the section.

This function only has an effect under the DOS version of OpenDoors.

## Additional details

Call this function before any operation which initializes OpenDoors. `pszName`
identifies the personality, the two line numbers define its local door-output
area, and `pfPerFunc` receives
[`PEROP_*`](../constants/components.md#personality-callback-operations)
operations.

The function returns true when the personality is registered. An exhausted
personality table returns false with [`ERR_LIMIT`](../constants/errors.md). Custom
personalities affect the text-mode local interface; targets without that
interface return false with [`ERR_UNSUPPORTED`](../constants/errors.md).

OpenDoors copies and uppercases at most 32 characters of `pszName`, so the name
string need not remain allocated after the call. It retains `pfPerFunc` and may
call it later. The function does not validate a null name, a null callback, or
the ordering and range of the output bounds; the caller must supply all four
arguments correctly.

## See also

[`od_set_personality()`](od_set_personality.md), [`od_set_statusline()`](od_set_statusline.md)

[`od_set_personality()`](od_set_personality.md),
[`od_set_statusline()`](od_set_statusline.md), [Types and
callbacks](../types.md)
