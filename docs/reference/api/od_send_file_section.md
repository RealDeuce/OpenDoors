# `od_send_file_section()`

Displays one named section of an ASCII, ANSI, AVATAR, or RIP display file.

## Synopsis

```c
BOOL od_send_file_section(char *pszFileName, char *pszSectionName);
```

## Parameters

`pszFileName`
: Name of the display file. It may contain a complete filename and extension,
  or omit the extension to request automatic selection for the caller's
  terminal capabilities.

`pszSectionName`
: Name following the `@#` marker in the file. Supply only the name, without
  the `@#` characters.

## Return value

Returns [`TRUE`](../constants/general.md#true) if the requested section was found
and its display operation completed. Returns [`FALSE`](../constants/general.md#false)
if no suitable file can be opened, either parameter is null, or the section
marker is not found.

As with [`od_send_file()`](od_send_file.md), a user stop key or an active menu
hotkey ends the transmission normally rather than turning an already found
section into an error.

## Description

[`od_send_file_section()`](od_send_file_section.md) is the section-oriented counterpart to
[`od_send_file()`](od_send_file.md). It allows several screens, menu fragments,
or messages which use the same display format to reside in one file. A section
begins with a line whose first characters are `@#` followed immediately by the
section name:

```text
@#MAIN
Main menu contents go here.
@#HELP
Help text goes here.
@#GOODBYE
Logoff text goes here.
```

The marker line itself is not displayed. After finding the requested marker,
OpenDoors displays following lines until it reaches another line beginning
with `@#` or reaches the end of the file. Section names are compared exactly
as supplied with respect to character case. The comparison covers the supplied
name; for predictable results, use distinct complete names rather than names
which are prefixes of other section names.

If `pszFileName` includes an extension, OpenDoors opens that exact file. The
contents are still passed through the normal terminal-emulation path; the
extension does not prevent control sequences in the file from being
interpreted.

If no extension is present, OpenDoors selects the most capable available file
which the remote terminal can display:

| Extension | Required caller capability |
| --- | --- |
| `.RIP` | RIP |
| `.AVT` | AVATAR |
| `.ANS` | ANSI |
| `.ASC` | Plain text only |

The search starts with the caller's highest enabled capability and falls back
toward `.ASC`. Thus an ANSI caller uses an `.ANS` file when it exists and an
`.ASC` file otherwise; it does not receive an `.AVT` or `.RIP` file.

When a RIP file is selected for the caller, OpenDoors separately opens an
AVATAR, ANSI, or ASCII variant for the local presentation. If none exists, the
RIP stream is still sent remotely and a local message indicates which file is
being transmitted. Page pausing is disabled for RIP output.

The current implementation does **not** independently locate the named section
in that separate local variant. It searches the remote file, then reads the
local file from its current position as remote section lines are transmitted.
The two variants must therefore have matching line layout through the section
being displayed. If their pre-section line counts differ, the local display can
show a different part of its file even though the remote caller receives the
requested section.

For ANSI, AVATAR, and ASCII files, each displayed line passes through the local
terminal emulator while the remote representation is transmitted. ANSI and
AVATAR control sequences are interpreted, and RemoteAccess/QuickBBS text codes
are expanded unless [`od_control.od_no_ra_codes`](../control/customization.md#od_no_ra_codes)
disables that feature. The substitutions and terminal behavior are the same as
for [`od_send_file()`](od_send_file.md).

Page pausing begins with the setting in
[`od_control.od_page_pausing`](../control/runtime.md#od_page_pausing) and uses
the caller's [`user_screen_length`](../control/caller.md#user_screen_length).
The configured pause and stop keys remain active. When this function is called
while a hotkey menu is active, matching menu keys can terminate the file
display so the menu function can return the selected key.

The function initializes OpenDoors if required. It opens files in binary mode,
closes every file it opened before returning, and waits for queued remote RIP
output to drain before removing the local transmission message.

## Errors

[`od_control.od_error`](../control/runtime.md#od_error) is set to:

- [`ERR_PARAMETER`](../constants/errors.md#err_parameter) if either pointer is
  `NULL`.
- [`ERR_FILEOPEN`](../constants/errors.md#err_fileopen) if an explicitly named
  file cannot be opened or automatic selection finds no compatible file.

A missing section returns false after the file has been read but does not set a
distinct section-not-found error code. Callers which need to distinguish that
case from an earlier error should clear or record
[`od_control.od_error`](../control/runtime.md#od_error) before the call.

## Example

Given a file named `SCREENS.ANS` containing:

```text
@#WELCOME
Welcome to the example door!
@#MENU
[A] About this door
[Q] Return to the BBS
```

the following code displays only the menu section:

```c
if(!od_send_file_section("SCREENS.ANS", "MENU"))
{
    od_set_color(L_WHITE, D_RED);
    od_disp_str("The menu could not be displayed.\n\r");
}
```

To maintain `.ASC`, `.ANS`, `.AVT`, and `.RIP` variants, give every variant
the same section markers and omit the extension in the call:

```c
od_send_file_section("SCREENS", "WELCOME");
```

## See also

[`od_send_file()`](od_send_file.md), [`od_hotkey_menu()`](od_hotkey_menu.md),
[`od_disp_emu()`](od_disp_emu.md)
