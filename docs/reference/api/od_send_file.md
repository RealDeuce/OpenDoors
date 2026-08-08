# `od_send_file()`

Sends an ASCII/ANSI/AVATAR/RIP file from disk, using terminal emulation.

## Synopsis

```c
BOOL od_send_file(const char *pszFileName);
```

## Return value

TRUE if the file was successfully sent FALSE if OpenDoors was unable to send the file

## Description

This powerful function will display any ASCII, ANSI, AVATAR or RIP file. The `od_send_file()` function can be used to display existing BBS text files, such as a "logoff screen", before your door hangs up on the user. You can also make use of the `od_send_file()` function to build many of your door screens as external files. This will allow you to easily create these screens in an ANSI editor program, such as "TheDraw". It will could also optionally allow sysops to customize your door for use on their own BBS.

The `od_send_file()` function is called with the full path and filename of the file you wish to have displayed. Thus, if you wished to send the ANSI file MAINMENU.SCR, you would simply call:

```text
od_send_file("MAINMENU.SCR");
```

In many cases, instead of having just one file that you want displayed in particular, you will have several different files, and will want a different one displayed according to the user's graphics mode. For example, you might have the four files, MAINMENU.ASC, MAINMENU.ANS, MAINMENU.AVT and MAINMENU.RIP; the .ASC file containing no special control codes, the .ANS file containing ANSI control codes, the .AVT file containing AVATAR control codes, and the .RIP file containing RIP graphics control codes. In this case, you can have the `od_send_file()` function automatically select the appropriate file according to the user's current display mode, by omitting the extension altogether. Thus, a call to:

```text
od_send_file("MAINMENU");
```

would cause OpenDoors to automatically send the appropriate file, according to the user's graphics mode settings. When the `od_send_file()` function is used in this "automatic mode" (where you do not specify a filename extension), it will look for one of the four filename extensions listed below.

```text
+----------------------------------------------------------+
| Extension| File type                                     |
+----------+-----------------------------------------------|
|   .ASC   | Does not require any graphics mode to display |
|   .ANS   | Requires ANSI graphics mode to display        |
|   .AVT   | Requires AVATAR graphics mode to display      |
|   .RIP   | Requires RIP graphics mode to be displayed    |
+----------------------------------------------------------+
```

If the user has RIP graphics enabled, `od_send_file()` will first search for the .RIP file. If no file exists with the specified filename and a .RIP extension, `od_send_file()` will then search for .AVT, then .ANS, and if not found .ASC. If the user has only ANSI graphics enabled, `od_send_file()` will attempt first to display the .ANS file, and if not found will search for .ASC. In the case that the user is using plain-ASCII mode, this function will attempt only to display the .ASC file.

When displaying a .RIP file to the remote system, OpenDoors will attempt to locate and display a corresponding .AVT/.ANS/.ASC file on the local system. If no such file can be found, a window will be displayed, indicating the name of the .RIP file that is being sent to the remote system. When a .RIP file is being displayed, page pausing is disabled.

When displaying .AVT/.ANS/.ASC files, `od_send_file()` will send any ANSI or AVATAR codes in the file directly to the remote terminal, and interpret them to display on the local screen (regardless of the actual filename extension). This interpretation is accomplished by OpenDoor's built in terminal emulator. The terminal emulator fully supports all ANSI and AVATAR level 0 and level 0+ control codes. The terminal emulator will also translate Remote Access/QuickBBS style control codes, if enabled by setting od_control.od_no_ra_codes to FALSE. The control codes supported by OpenDoors are listed in the chart on the following pages. When these control codes are inserted into the file, OpenDoors will replace them with various pieces of user or system information.

```text
  +-----------------------------------------------------+
  | CONTROL | ASCII |                                   |
  |  CODE   | VALUE | DESCRIPTION                       |
  +---------+-------+-----------------------------------|
  |   ^FA   | 06,65 | Displays the user's full name     |
  |   ^FB   | 06,66 | Location the user is calling from |
  |   ^FC   | 06,67 | Displays the user's password      |
  |   ^FD   | 06,68 | Business/data phone number        |
  |   ^FE   | 06,69 | Home/voice phone number           |
  |   ^FF   | 06,70 | Date of the user's last call      |
  |   ^FG   | 06,71 | Time of day of the last call      |
  |   ^FH   | 06,72 | The user's `A' flags settings     |
  |   ^FI   | 06,73 | The user's `B' flags settings     |
  |   ^FJ   | 06,74 | The user's `C' flags settings     |
  |   ^FK   | 06,75 | The user's `D' flags settings     |
  |   ^FL   | 06,76 | User's remaining netmail credit   |
  |   ^FM   | 06,77 | Number of messages posted by user |
  |   ^FN   | 06,78 | Last read message number by user  |
  |   ^FO   | 06,79 | Displays security level of user   |
  |   ^FP   | 06,80 | Number of times user has called   |
  |   ^FQ   | 06,81 | Total # of uploads by user        |
  |   ^FR   | 06,82 | Total KBytes uploaded by user     |
  |   ^FS   | 06,83 | Total # of downloads by user      |
  |   ^FT   | 06,84 | Total Kbytes downloaded by user   |
  |   ^FU   | 06,85 | # of minute user has used today   |
  |   ^FV   | 06,86 | User's screen length setting      |
  |   ^FW   | 06,87 | User's first name only            |
  |   ^FX   | 06,88 | User's ANSI setting               |
  |   ^FY   | 06,89 | User's "continue?" prompt setting |
  |   ^FZ   | 06,90 | Does user have screen clearing on |
  |   ^F0   | 06,48 | User's Full-screen editor setting |
  |   ^F1   | 06,49 | User's Quiet mode setting         |
  |   ^F2   | 06,50 | User's hot-keys setting           |
  |   ^F3   | 06,51 | Displays the user's alias         |
  |   ^F4   | 06,52 | The date of the User's first call |
  |   ^F5   | 06,53 | The user's date of birth          |
  |   ^F6   | 06,54 | User's subscription expiry date   |
  |   ^F7   | 06,55 | Number of days until expiry       |
  |   ^F8   | 06,56 | User's AVATAR setting             |
  |   ^F9   | 06,57 | The user's upload:download ratio  |
  |   ^F:   | 06,58 | User's Upload K:download K ratio  |
  +-----------------------------------------------------+
```

```text
  +-----------------------------------------------------+
  | CONTROL | ASCII |                                   |
  |  CODE   | VALUE | DESCRIPTION                       |
  +---------+-------+-----------------------------------|
  |   ^F;   | 06,59 | Full-screen message reader        |
  |   ^KA   | 11,65 | Total # of calls BBS has received |
  |   ^KB   | 11,66 | Name of the last caller to BBS    |
  |   ^KC   | 11,67 | Total # of active messages on BBS |
  |   ^KD   | 11,68 | Displays # of the first message   |
  |   ^KE   | 11,69 | Displays # of the last message    |
  |   ^KF   | 11,70 | # of times user has paged sysop   |
  |   ^KG   | 11,71 | Full name of the current weekday  |
  |   ^KH   | 11,72 | Displays total number of users    |
  |   ^KI   | 11,73 | Displays the current time         |
  |   ^KJ   | 11,74 | Displays the current date         |
  |   ^KK   | 11,75 | Minutes the user has been online  |
  |   ^KL   | 11,76 | Seconds the user has been online  |
  |   ^KM   | 11,77 | Minutes the user has used today   |
  |   ^KN   | 11,78 | Seconds the user has used today   |
  |   ^KO   | 11,79 | Minutes remaining for user today  |
  |   ^KP   | 11,80 | Seconds remaining for user today  |
  |   ^KQ   | 11,81 | The user's daily time limit       |
  |   ^KR   | 11,82 | Displays the current baud rate    |
  |   ^KS   | 11,83 | The current weekday in short-form |
  |   ^KT   | 11,84 | The user's daily download limit   |
  |   ^KU   | 11,85 | # of minutes until the next event |
  |   ^KV   | 11,86 | Time of the next system event     |
  |   ^KW   | 11,87 | # of node user is currently on    |
  |   ^KX   | 11,88 | Disconnects the user              |
  +-----------------------------------------------------+
```

## Examples

For an example of the use of the `od_send_file()` function in displaying a custom door menu, see the EX_VOTE.C example program.

## Additional details

If `pszFileName` has no extension, OpenDoors searches for a compatible variant
according to the user's display capabilities. File contents are interpreted for
the local presentation while being transmitted to the remote user. Page pausing
and user interruption follow the current [`od_control`](../control/index.md)
settings.

The function returns false with [`ERR_FILEOPEN`](../constants/errors.md) when no
suitable file can be opened and with [`ERR_PARAMETER`](../constants/errors.md)
for a null name. The implementation reads display files with `fgets()` and
treats a failed read like end of file; it does not set `ERR_FILEREAD`.

## See also

[`od_disp_emu()`](od_disp_emu.md), [`od_list_files()`](od_list_files.md), [`od_hotkey_menu()`](od_hotkey_menu.md)

[`od_send_file_section()`](od_send_file_section.md),
[`od_hotkey_menu()`](od_hotkey_menu.md), [`od_disp_emu()`](od_disp_emu.md)
