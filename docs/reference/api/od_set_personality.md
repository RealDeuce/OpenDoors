# `od_set_personality()`

Sets the current status line / sysop function key personality to be used.

## Synopsis

```c
BOOL od_set_personality(const char *pszName);
```

## Return value

TRUE on success FALSE on failure

## Description

This function changes the current status line / sysop function key personality. The pszName parameter should contain the string which uniquely identifies the personality to be set. This function may only be used by OpenDoors programs which include the OpenDoors "Multiple Personality System". To enable use of the MPS, include the following line before your first call to any OpenDoors function:

```text
od_control.od_mps=INCLUDE_MPS;
```

OpenDoors includes a number of built-in personalities. Additional personalities may be added using the [`od_add_personality()`](od_add_personality.md) function. The following personalities are included with this version of OpenDoors:

```text
     Name                Description
     -----------------------------------------------------------
     Standard            OpenDoors style, similar to RA 1.11
     PCBoard             Similar to PC-Board
     RemoteAccess        Similar to RemoteAccess 2.x
     Wildcat             Similar to Wildcat!
```

Personality names are not case sensitive. For more information on the OpenDoors Multiple Personality System, see the section.

This function returns [`TRUE`](../types.md#true-and-false) on success or
[`FALSE`](../types.md#true-and-false) on failure. On failure,
[`od_control.od_error`](../control/runtime.md#od_error) indicates the reason.

## Additional details

The nonempty name may identify a built-in personality or one installed with
[`od_add_personality()`](od_add_personality.md). The function returns true when
a match is selected. The text-mode implementation reports an empty name as
[`ERR_PARAMETER`](../constants/errors.md) and a missing name as
[`ERR_LIMIT`](../constants/errors.md); targets without the personality system return
false with [`ERR_UNSUPPORTED`](../constants/errors.md).

Personalities control the local status display and sysop keys; they do not
change the remote terminal protocol. The caller must not pass a null name.

## See also

[`od_add_personality()`](od_add_personality.md), [`od_set_statusline()`](od_set_statusline.md)

[`od_add_personality()`](od_add_personality.md),
[`od_set_statusline()`](od_set_statusline.md)
