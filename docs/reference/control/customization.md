# Customization and callbacks

OpenDoors exposes its built-in configuration, text, colors, local controls, and
lifecycle hooks through [`od_control`](index.md).

## Program and components

Set `od_prog_name`, `od_prog_version`, and `od_prog_copyright` before
initialization. `od_config_file`, `od_logfile`, and `od_mps` select optional
components; their filenames, callbacks, message tables, and component-specific
settings are stored in the adjacent `od_config_*`, `od_logfile_*`, and
personality fields. `od_reg_name` and `od_reg_key` remain solely for historical
source compatibility.

The exact component members are `od_config_filename`, `od_config_function`,
`od_config_callback`, `od_logfile_disable`, `od_logfile_name`,
`od_logfile_messages`, and the component selector pointers themselves.

## Command-line and lifecycle hooks

`od_cmd_line_flag_handler`, `od_cmd_line_handler`, `od_cmd_line_help_func`, and
`od_cmd_line_help` extend the standard parser. Lifecycle hooks include
`od_before_exit`, before/after chat and shell callbacks, configuration and help
callbacks, `od_ker_exec`, `od_local_input`, `od_time_msg_func`, and
`od_no_file_func`. Each callback executes synchronously; avoid blocking the
kernel indefinitely.

The chat and shell hook member names are `od_cbefore_chat`, `od_cafter_chat`,
`od_cbefore_shell`, and `od_cafter_shell`; the remaining general callbacks
include `od_help_callback` and `od_config_callback`.

## Local keys and personalities

`key_chat`, `key_dosshell`, `key_drop2bbs`, `key_hangup`, keyboard, time,
lockout, status, and sysop-next members select local function keys.
`od_hot_key`, `od_hot_function`, and `od_num_keys` add application-defined local
keys. `od_default_personality` chooses the initial local interface.

The complete built-in key fields are `key_keyboardoff`, `key_lesstime`,
`key_lockout`, `key_moretime`, `key_status`, and `key_sysopnext` in addition to
the four named above.

## Text, colors, and formatting

`od_cfg_text`, `od_cfg_lines`, `od_color_names`, `od_color_char`, and
`od_color_delimiter` control configuration parsing and inline color names.
`od_box_chars` supplies window borders. `od_full_color`, `od_full_put`,
`od_no_ra_codes`, and `od_default_rip_win` tune terminal output.

Prompt pointers such as `od_press_key`, `od_no_time`, `od_paging`, status text,
day/month names, chat text, and timeout messages may be replaced before they are
used. Color fields for chat, lists, windows, menus, and continuation prompts use
IBM-PC attributes.

The complete prompt set is `od_after_chat`, `od_after_shell`, `od_before_chat`,
`od_before_shell`, `od_chat_reason`, `od_continue`, `od_hanging_up`,
`od_exiting`, `od_help_text`, `od_help_text2`, `od_inactivity_timeout`,
`od_inactivity_warning`, `od_no_keyboard`, `od_no_sysop`, `od_no_response`,
`od_no_time`, `od_offline`, `od_paging`, `od_press_key`, `od_sending_rip`,
`od_status_line`, `od_sysop_next`, `od_time_left`, `od_time_warning`, and
`od_want_chat`. The `od_day` and `od_month` arrays supply localized names;
`od_continue_yes`, `od_continue_no`, and `od_continue_nonstop` supply response
keys.

Color members are `od_chat_color1`, `od_chat_color2`, `od_list_comment_col`,
`od_list_name_col`, `od_list_offline_col`, `od_list_size_col`,
`od_list_title_col`, `od_local_win_col`, `od_continue_col`, `od_menu_title_col`,
`od_menu_border_col`, `od_menu_text_col`, `od_menu_key_col`,
`od_menu_highlight_col`, and `od_menu_highkey_col`.

On Windows, `od_app_icon` and `od_cmd_show` customize the native local window.

## Detailed reference

### Opendoors Customization

The OpenDoors control structure provides many variables which allow you to customize OpenDoor's behavior and appearance. These customization variables fit into one of the following categories:

```c
General Behavior Customization Variables
Sysop Function Keys Customization Variables
Color Customization Variables
Language-Specific Prompts Customization Variables
```

This section deals with those variables that fit into the first category, "General Behavior Customization Variables". The other categories are dealt with in the following sections of this chapter.

Below is a brief overview of the variables grouped into this section of the OpenDoors control structure. Following the overview is a detailed description of each of these variables.

od_app_icon              Program icon for Win32 version.

od_box_chars             Array of characters used by the [`od_draw_box()`](../api/od_draw_box.md) function.

od_before_exit           Function to call prior to exiting.

od_cafter_chat           Function to call after sysop chat.

od_cafter_shell          Function to call after DOS shell.

od_cbefore_chat          Function to call prior to sysop chat.

od_cbefore_shell         Function to call prior to DOS shell.

od_cfg_lines             Sets the configuration file's custom door information file line keywords.

od_cfg_text              Sets the built-in configuration file keywords that OpenDoors will recognize.

od_chat_active           Controls whether or not sysop chat mode is active.

od_clear_on_exit         Controls whether the screen is cleared upon door exit.

od_color_delimiter       Indicates what character should delimit imbedded color codes for the [`od_printf()`](../api/od_printf.md) function.

od_color_names           Strings which OpenDoors recognizes as the names of various text colors.

od_config_file           Used to enable or disable the OpenDoors configuration file system.

od_config_filename       Sets the filename that will be read by the configuration file system.

od_config_function       The callback function that OpenDoors will call to allow your program to process custom configuration file entries.

od_default_personality   Sets the default personality to be used with the OpenDoors Multiple Personality System, and also sets the personality to use when the MPS is not active.

od_default_rip_win       Whether OpenDoors should use the default 43-line RIP window for ANSI text (TRUE), or a 23-line window (FALSE).

od_disable               Disable OpenDoors activities such as reading door information file and monitoring carrier detect / remaining time.

od_disable_dtr           Specifies the string that will be sent to the modem to prevent the modem from hanging up when DTR is lowered.

od_emu_simluate_modem    Simulates modem display speed for emulation functions such as [`od_send_file()`](../api/od_send_file.md), [`od_disp_emu()`](../api/od_disp_emu.md) and [`od_hotkey_menu()`](../api/od_hotkey_menu.md).

od_errorlevel            Sets the errorlevel OpenDoors exits with under various conditions.

od_force_local           Forces door to operate in local mode, ignoring any door information file and using default user settings.

od_help_callback         Allows you to provide a help menu item under the Win32 version of OpenDoors

od_in_buf_size           Sets size of OpenDoor's internal local/remote inbound buffer.

od_inactive_warning      Number of seconds before hanging up that OpenDoors displays the inactivity timeout warning.

od_inactivity            Controls user inactivity timeout.

od_ker_exec              Is called whenever [`od_kernel()`](../api/od_kernel.md) executes.

od_last_input            Indicates whether the last input came from the remote user (==0) or the local sysop (==1).

od_list_pause            Controls whether or not the user may pause display within the [`od_list_files()`](../api/od_list_files.md) and [`od_send_file()`](../api/od_send_file.md) functions by using the [P] key.

od_list_stop             Controls whether or not the user may terminate display within the [`od_list_files()`](../api/od_list_files.md) and [`od_send_file()`](../api/od_send_file.md) functions using [S], [CTRL]-[K], etc.

od_logfile               Enables or disables the OpenDoors log file system.

od_logfile_disable       Prevents the logfile from being opened, even if the logfile is enabled by od_logfile.

od_logfile_messages      Array of message strings that OpenDoors will use when writing log file entries.

od_logfile_name          Contains the filename and possibly path of the logfile.

od_maxtime               Indicates the maximum length of time any user is permitted to use the door.

od_maxtime_deduction     Indicates the amount of time that has temporarily been taken away from the user's remaining time, as a result of the maximum door time setting.

od_mps                   Enables or disables the OpenDoors Multiple Personality System.

od_no_file_func          Called when no door information file can be read. od_no_ra_codes           Disables translation of RA/QBBS control codes.

od_nocopyright           Prevents OpenDoors from displaying it's name and version number when a door program begins execution.

od_noexit                Prevents OpenDoors from exiting when the [`od_exit()`](../api/od_exit.md) function is called.

od_page_len              Controls length of the sysop page beep.

od_page_pausing          Enables or disables page pausing in [`od_send_file()`](../api/od_send_file.md), [`od_hotkey_menu()`](../api/od_hotkey_menu.md) and [`od_list_files()`](../api/od_list_files.md) functions.

od_page_startmin         Indicates the time of day at which sysop paging is first enabled.

od_page_statusline       Which status line (if any) is activated when the user pages the sysop.

od_page_endmin           Indicates the time of day at which sysop paging is disabled.

od_prog_name             Stores the name of your program.

od_prog_version          Stores the version number of your program.

od_prog_copyright        Place your copyright information here.

od_reg_key               Retained compatibility storage; unused by the current implementation.

od_reg_name              Retained compatibility storage; unused by the current implementation.

od_spawn_freeze_time     Indicates whether the user's time remaining continues to be decreased during the execution of the od_spawn...() functions (FALSE), or if the timer should be "frozen" (TRUE).

od_swapping_disable      Disables swapping during DOS shell and od_spawn...() functions.

od_swapping_noems        Prevents swapping form being done to EMS expanded memory.

od_swapping_path         Location where disk swap file should be created.

od_status_on             Controls whether the status line sub- system is active.

od_time_msg_func         Called instead of displaying time limit warning messages.

### Command-line extensions

The command-line extension fields are read only by
[`od_parse_cmd_line()`](../api/od_parse_cmd_line.md). Because that function is
normally called before OpenDoors initialization, these fields must be assigned
before the call.

#### `od_cmd_line_flag_handler`

```c
BOOL (*od_control.od_cmd_line_flag_handler)(const char *keyword);
```

This callback receives each command-line argument which is not one of the
built-in OpenDoors options. The argument is passed exactly as it appears in the
argument vector, including any leading `-` or `/`. Return [`TRUE`](../constants/general.md#true) to accept the
argument as a complete flag. Return [`FALSE`](../constants/general.md#false) to let
[`od_cmd_line_handler`](#od_cmd_line_handler) process it instead. No following
arguments are consumed when this callback returns [`TRUE`](../constants/general.md#true).

The pointer is initially `NULL`. OpenDoors calls it synchronously and never
assigns it.

#### `od_cmd_line_handler`

```c
void (*od_control.od_cmd_line_handler)(char *keyword, char *options);
```

This compatibility callback handles an unknown command-line keyword not
accepted by [`od_cmd_line_flag_handler`](#od_cmd_line_flag_handler). `keyword`
is the original argument. `options` contains the following non-option
arguments, joined with spaces, up to the next argument beginning with `-` or
`/`, a recognized OpenDoors option, or the end of the command line. Those
arguments are consumed by the parser. An unknown keyword without following
option text produces an empty string when another command-line option follows.

Both pointers refer to temporary parser storage and must not be retained after
the callback returns. The callback is synchronous, its pointer is initially
`NULL`, and OpenDoors never assigns it.

#### `od_cmd_line_help_func`

```c
void (*od_control.od_cmd_line_help_func)(void);
```

When the parser encounters `-?`, `-H`, or `-HELP`, it invokes this callback in
place of all built-in command-line help and then terminates the process with
exit status zero. The callback cannot return control to command-line parsing.
The pointer is initially `NULL` and is read only by OpenDoors.

#### `od_cmd_line_help`

```c
const char *od_control.od_cmd_line_help;
```

If no [`od_cmd_line_help_func`](#od_cmd_line_help_func) is installed, this
field may point to replacement help text. OpenDoors displays it on standard
output on non-Windows platforms or in a message box on Win32, then terminates
the process. A `NULL` pointer selects the built-in help text. OpenDoors reads
the pointer but does not copy, free, or assign the string.

### General callbacks

#### `od_config_function`

```c
void (*od_control.od_config_function)(char *keyword, char *options);
```

This callback extends the optional OpenDoors configuration-file component. It
is invoked for each nonblank configuration line whose keyword does not match
an entry in [`od_cfg_text`](#od_cfg_text). `keyword` contains the uppercased
keyword, truncated to 32 characters; `options` contains the remainder of the
line after leading and trailing spaces and tabs have been removed. Comments
begin with a semicolon and are removed before parsing.

The component saves the callback pointer when configuration processing begins,
so it must be assigned before initialization. Both arguments point into
temporary mutable parser buffers and must not be retained. The pointer is
initially `NULL`; OpenDoors reads it but never assigns it.

#### `od_config_callback`

```c
void (*od_control.od_config_callback)(void);
```

On Win32, a non-`NULL` pointer retains the application's Config command in the
local window's Door menu. OpenDoors invokes the callback synchronously when the
operator chooses that command. If the pointer is `NULL` when the frame window
is created, OpenDoors removes the menu item; assigning it later does not add
the item again. Other platforms do not read this field. The pointer is
initially `NULL` and is never assigned by OpenDoors.

#### `od_help_callback`

```c
void (*od_control.od_help_callback)(void);
```

On Win32, a non-`NULL` pointer retains the Contents command in the local
window's Help menu. OpenDoors invokes the callback synchronously when the
operator chooses that command. If the pointer is `NULL` when the frame window
is created, OpenDoors removes the menu item; assigning it later does not add
the item again. Other platforms do not read this field. The pointer is
initially `NULL` and is never assigned by OpenDoors.

#### `od_ker_exec`

```c
void (*od_control.od_ker_exec)(void);
```

OpenDoors invokes this callback near the beginning of every non-recursive
[`od_kernel()`](../api/od_kernel.md) execution. It provides a regular service
point for application work in both single-threaded and multithreaded builds.
The callback executes synchronously while the kernel is marked active; a
recursive [`od_kernel()`](../api/od_kernel.md) call returns without invoking it again. The pointer is
initially `NULL` and is read only by OpenDoors.

#### `od_local_input`

```c
void (*od_control.od_local_input)(INT16 key);
```

This callback receives a local operator keystroke which was not consumed by a
built-in command or an entry in [`od_hot_key`](#od_hot_key). The key uses the
IBM scan-code/ASCII representation described under [function
keys](#function-keys). OpenDoors calls the function before placing that same
key in the common local/remote input queue. Up and Down Arrow are also passed
to it while an API operation has reserved those keys for ordinary input.

Remote input never invokes this callback. The pointer is initially `NULL` and
is read only by OpenDoors.

#### `od_no_file_func`

```c
void (*od_control.od_no_file_func)(void);
```

OpenDoors invokes this callback when it cannot read any supported
door-information file. Before the call,
[`od_info_type`](connection.md#od_info_type) is [`NO_DOOR_FILE`](../constants/session.md#no_door_file). The callback
may provide a local-login path by setting [`od_force_local`](#od_force_local)
to [`TRUE`](../constants/general.md#true), or may read another
format, populate the required control fields, and set [`od_info_type`](connection.md#od_info_type) to
[`CUSTOM`](../constants/session.md#custom). If neither action supplies sufficient startup information,
initialization reports that no door-information file could be read and exits.

The callback executes synchronously during initialization. Its pointer is
initially `NULL` and is read only by OpenDoors.

#### `od_time_msg_func`

```c
void (*od_control.od_time_msg_func)(char *message);
```

This callback replaces OpenDoors' normal display of four time-related
messages: the inactivity warning, inactivity timeout, warning at three, two,
or one minute remaining, and expiration of the caller's session time. The
argument is the exact string OpenDoors would otherwise pass to
[`od_disp_str()`](../api/od_disp_str.md); the remaining-time warning has already
been formatted with the number of minutes. The storage may be a prompt string
or an internal work buffer and must not be modified or retained.

The callback changes only how the message is delivered. OpenDoors still
performs the warning bookkeeping and timeout shutdown. The pointer is initially
`NULL` and is read only by OpenDoors.

#### `od_app_icon`

```c
HICON od_control.od_app_icon;
```

Normally, the Win32 version of OpenDoors displays its own icon on the application title bar, on the Windows taskbar, and in the help|about dialog box. You can supply your own icon by setting this variable to point to the handle of the icon, as returned by LoadIcon();

#### `od_box_chars`

```c
char od_control.od_box_chars[8];
```

This variable allows you to specify which character the [`od_draw_box()`](../api/od_draw_box.md) function uses in drawing the boarder of a window. The elements of this array are as follows:

od_box_chars[BOX_UPPERLEFT]  - Upper left corner of box od_box_chars[BOX_TOP]        - Top horizontal line od_box_chars[BOX_UPPERRIGHT] - Upper right corner of box od_box_chars[BOX_LEFT]       - Left Vertical line od_box_chars[BOX_LOWERLEFT]  - Lower left corner of box od_box_chars[BOX_LOWERRIGHT] - Lower right corner of box od_box_chars[BOX_BOTTOM]     - Bottom horizontal line od_box_chars[BOX_RIGHT]      - Right horizontal line

#### `od_before_exit`

```c
void (*od_control.od_before_exit)(void);
```

This callback is invoked during [`od_exit()`](../api/od_exit.md), after the
maximum-door-time deduction has been restored and elapsed time has been
accounted for, but before OpenDoors displays its shutdown message, closes the
connection, or rewrites the door-information file. It can therefore perform
application cleanup and make final changes to fields which must be written
back to the BBS. The pointer is initially `NULL`; OpenDoors reads it but never
assigns it.

#### `od_cafter_chat`

```c
void (*od_control.od_cafter_chat)(void);
```

This callback is invoked while chat cleanup is in progress, after
[`od_after_chat`](#od_after_chat) has been displayed, but before the original
display attribute is restored and before
[`od_chat_active`](runtime.md#od_chat_active) is reset to [`FALSE`](../constants/general.md#false). It can
restore screen contents saved by [`od_cbefore_chat`](#od_cbefore_chat). The
pointer is initially `NULL` and is read only by OpenDoors.

#### `od_cafter_shell`

```c
void (*od_control.od_cafter_shell)(void);
```

This callback is invoked after the command interpreter returns and after
OpenDoors displays [`od_after_shell`](#od_after_shell). It can restore screen
state saved by [`od_cbefore_shell`](#od_cbefore_shell). The pointer is initially
`NULL` and is read only by OpenDoors.

#### `od_cbefore_chat`

```c
void (*od_control.od_cbefore_chat)(void);
```

This callback is invoked after OpenDoors has set
[`od_chat_active`](runtime.md#od_chat_active) to [`TRUE`](../constants/general.md#true), but before it displays
[`od_before_chat`](#od_before_chat) or enters its chat input loop. It is
commonly used to save or rearrange the application's screen. The callback may
set [`od_chat_active`](runtime.md#od_chat_active) to [`FALSE`](../constants/general.md#false) to suppress the built-in chat loop; OpenDoors
then proceeds through its normal chat cleanup. The pointer is initially `NULL`
and is read only by OpenDoors.

#### `od_cbefore_shell`

```c
void (*od_control.od_cbefore_shell)(void);
```

This callback is invoked immediately before OpenDoors displays
[`od_before_shell`](#od_before_shell) and runs the local command interpreter.
It can save screen state or prepare application resources for the shell. The
pointer is initially `NULL` and is read only by OpenDoors.

The chat and shell callbacks execute synchronously inside OpenDoors. A chat or
shell operation requested recursively from one of these callbacks is deferred
or suppressed until the current operation has completed.

#### `od_cfg_lines`

```c
char od_control.od_cfg_lines[25][33];
```

This array contains the 25 keywords used in a custom door-information file
definition. Each element has room for a keyword of up to 32 characters plus
its terminating null byte. Matching is case-insensitive. The effective
defaults, in array order, are:

```text
Ignore           ComPort          FossilPort       ModemBPS
LocalMode        UserName         UserFirstName    UserLastName
Alias            HoursLeft        MinutesLeft      SecondsLeft
ANSI             AVATAR           PagePausing      ScreenLength
ScreenClearing   Security         City             Node
SysopName        SysopFirstName   SysopLastName    SystemName
RIP
```

OpenDoors supplies a default only when an element's first byte is zero, then
uppercases all elements when the configuration component begins reading a
file. An application which changes these keywords must do so before
initialization. OpenDoors reads them while interpreting `CustomFileLine`
configuration entries; it does not modify them after the initial defaulting
and uppercasing step.

#### `od_cfg_text`

```c
char od_control.od_cfg_text[49][33];
```

This array contains the 49 built-in keywords recognized by the optional
configuration-file component. Each element has room for a keyword of up to 32
characters plus its terminating null byte. Matching is case-insensitive. The
effective defaults, in array order, are:

```text
Node                       BBSDir                     DoorDir
LogFileName                DisableLogging             SundayPagingHours
MondayPagingHours          TuesdayPagingHours         WednesdayPagingHours
ThursdayPagingHours        FridayPagingHours          SaturdayPagingHours
MaximumDoorTime            SysopName                  SystemName
SwappingDisable            SwappingDir                SwappingNoEMS
LockedBPS                  SerialPort                 CustomFileName
CustomFileLine             InactivityTimeout          PageDuration
ChatUserColour             ChatSysopColour            FileListTitleColour
FileListNameColour         FileListSizeColour         FileListDescriptionColour
FileListOfflineColour      Personality                NoFossil
PortAddress                PortIRQ                    ReceiveBuffer
TransmitBuffer             PagePromptColour           LocalMode
PopupMenuTitleColour       PopupMenuBorderColour      PopupMenuTextColour
PopupMenuKeyColour         PopupMenuHighlightColour   PopupMenuHighKeyColour
NoFIFO                     FIFOTriggerSize             DisableDTR
NoDTRDisable
```

The spelling shown above is the spelling installed by the library; matching is
case-insensitive but does not otherwise normalize alternate spellings.
OpenDoors supplies a default only when an element's first byte is zero, then
uppercases the complete array when the configuration component begins reading
a file. An application which replaces a keyword must do so before
initialization. OpenDoors subsequently reads these strings to identify built-in
options and does not restore their original case.

#### `od_clear_on_exit`

```c
char od_control.od_clear_on_exit;
```

This variable contains a Boolean value, which indicates whether or not you wish OpenDoors to clear the screen prior to exiting. This variable defaults to a value of TRUE, which causes the screen to be cleared when a door program exits. However, you may wish to set this variable to a value of FALSE, which will cause the contents of the screen to remain unchanged when the door exits. While setting this variable to FALSE will probably result in a messy display if the door is to return control to a batch file, if the door returns directly to the BBS, it will result in a smoother transition from the door back to the BBS (as the sysop is not left with a blank screen). If your door has a configuration file or configuration program, you may wish to have an option which will allow the individual sysop to determine whether or not the screen should be cleared when the door exits.

#### `od_color_delimiter`

```c
char od_control.od_color_delimiter;
```

This variable sets the character that is used to delimit color codes in the [`od_printf()`](../api/od_printf.md) function, and defaults to the back- quote (`) character. If you wish to be able to display the back- quote (`) character using the [`od_printf()`](../api/od_printf.md) function, and thus wish to use a different character to delimit color codes in the [`od_printf()`](../api/od_printf.md) function, simply set this variable to the alternative character you wish to use. If you wish to disable the imbedded color codes feature of the [`od_printf()`](../api/od_printf.md) function, simply set this variable to a value of zero. For more information on [`od_printf()`](../api/od_printf.md) imbedded color codes, see the description of the [`od_printf()`](../api/od_printf.md) function, which begins on page 110.

#### `od_color_names`

```c
char od_control.od_color_names[12][33];
```

This array sets the strings that OpenDoors will recognize as color description keywords. These are the keywords that can be imbedded in [`od_printf()`](../api/od_printf.md) format strings, and are also the keywords that can be used to change color settings in the OpenDoors configuration file. If you wish to change these keywords, you will normally do so before calling any OpenDoors functions. These keywords should always be supplied in upper- case characters. The defaults values for this array are as follows:

[0]  "BLACK" [1]  "BLUE" [2]  "GREEN" [3]  "CYAN" [4]  "RED" [5]  "MAGENTA" [6]  "YELLOW" [7]  "WHITE" [8]  "BROWN" [9]  "GREY" [10] "BRIGHT" [11] "FLASHING"

#### `od_config_file`

```c
void (*od_control.od_config_file)(void);
```

Set this variable to INCLUDE_CONFIG_FILE to enable the OpenDoors configuration file system, or set it to NO_CONFIG_FILE to disable the configuration file system. This variable should only be set prior to your first call to an OpenDoors function. For more information on the OpenDoors configuration file system, see page 224.

#### `od_config_filename`

```c
char *od_control.od_config_filename;
```

If set, this variable should point to a string containing the filename that you wish the OpenDoors configuration file system to read. If this variable has its default value of NULL, the filename DOOR.CFG will be used by default.

#### `od_default_personality`

```c
void (*od_control.od_default_personality)(BYTE operation);
```

On DOS and DOS32, this field selects the status-line personality used when the
multiple-personality component is absent or does not select a requested
personality. It may point to an application-supplied personality procedure or
to one of the built-in procedures named by [`PER_OPENDOORS`](../constants/components.md#per_opendoors), [`PER_PCBOARD`](../constants/components.md#per_pcboard),
[`PER_RA`](../constants/components.md#per_ra), and [`PER_WILDCAT`](../constants/components.md#per_wildcat).

The pointer is initially `NULL`, which selects the standard OpenDoors
personality. OpenDoors reads it during initialization and then calls the
selected procedure with [`PEROP_INITIALIZE`](../constants/components.md#perop_initialize); the personality supplies function
key mappings, status-line behavior, and its paging status-line default. Assign
the pointer before the first OpenDoors API call. The personality system is not
implemented on the other platforms, where this field is retained for
structure compatibility but is not read.

#### `od_default_rip_win`

```c
char od_control.od_default_rip_win;
```

This variable defaults to FALSE. When set to FALSE, OpenDoors resets the RIP text window to a 23-line window that is most appropriate for doors that support both RIP-graphics and non-RIP mode. When this variable is set to TRUE, OpenDoors will use the default sized text output window, 43 lines in size.

#### `od_disable`

```c
     unsigned int od_control.od_disable;
```

This variable is a bit-mapped flag which can be used to disable certain OpenDoors features which are normally active, in order to allow for maximum customization of OpenDoors. Each bit of this variable represents a different feature that can be disabled. To DISABLE a feature, you set the bit that corresponds to the particular feature. To ENABLE the feature, the bit is reset. Each bit is represented by a keyword, as follows:

DIS_INFOFILE - Setting the DIS_INFOFILE bit of the [`od_control.od_disable`](#od_disable) variable allows you to prevent OpenDoors from reading or re-writing a door information file. If you wish to disable OpenDoors' reading of the door information file, you must  do so prior to calling [`od_init()`](../api/od_init.md) or any other OpenDoors door-driver functions. At the same time, you must also manually set any required variables that are normally set by the information obtained from the door information file, such as the comm port number, baud rate, user name, and so on. You may wish to disable reading of the door information file in a number of cases. For example, you may wish to manually read another format of door information file not supported by OpenDoors, or to obtain the necessary door information from your program's command line. Also, if you are using OpenDoors to write a non-door communications program, such as a terminal program, you want to prevent OpenDoors from attempting to read a door information file on startup.

DIS_CARRIERDETECT - Setting this bit allows you to prevent OpenDoors from exiting when it the carrier detect signal from the modem disappears. This bit may be set or rest at any time. If you use this bit to disable OpenDoors' carrier detection, you will probably want to monitor the state of the carrier detect signal yourself, using the [`od_carrier()`](../api/od_carrier.md) function, which is described on page 51.

DIS_TIMEOUT - This flag allows you to prevent OpenDoors from exiting when the user runs out of time. As with the DIS_CARRIERDETECT flag, you may set or reset this bit at any time. You will most often want to use this setting when writing a non-door program, which you would not want to have exit after a particular amount of time has elapsed. Be sure that you do not confuse this flag with the user's inactivity timeout. To disable the inactivity timeout, set the do_control.od_inactivity variable to 0.

DIS_LOCAL_OVERRIDE - This setting affects OpenDoors' behavior when a locked BPS rate is specified in the configuration file, and another BPS rate is specified in the door information file. By default, OpenDoors will initialize the modem at the BPS rate specified in the configuration file, unless the BPS rate specified in the door information file is 0. In this case, the 0 BPS rate is used to indicate that the door is operating in local mode, and will override the BPS rate specified in the configuration file. Setting this flag disables the local mode override, causing the modem to always be initialized at the locked BPS rate, even when the door information file specifies that local mode should be used.

DIS_BPS_SETTING - When used with a FOSSIL driver, OpenDoors normally changes the BPS rate to that passed from the BBS (if the BBS passes a valid FOSSIL BPS rate). Setting the DIS_BPS_SETTING flag disables this BPS rate setting.

DIS_LOCAL_INPUT -  The local keyboard may be disabled by setting this bit. This only affects the sysop's input in circumstances that input is also accepted from the remote user; this setting has no effect on the sysop function keys.

DIS_SYSOP_KEYS - This setting also disables the local keyboard. However, unlike the DIS_LOCAL_INPUT, this function disables both sysop function keys and door input from the local keyboard.

DIS_DTR_DISABLE - This setting prevents OpenDoors from disabiling DTR response from the modem. Even if not specified, OpenDoors only disables DTR response in the when exiting under the Win32 version if an open serial port handle was not provided to OpenDoors at startup.

DIS_NAME_PROMPT - Prevents OpenDoors from prompting for a user name when operating in automatic local mode (by setting od_force_local to TRUE or specifying -local on the command line).

Note that in order to disable the OpenDoors status line, the [`od_control.od_status_on`](#od_status_on) variable is used, instead of the od_disable variable. You may also disable the user's inactivity timeout by setting the [`od_control.od_inactivity`](#od_inactivity) variable to 0. The [`od_control.od_status_on`](#od_status_on) variable is described later in this section.

#### `od_disable_dtr`

```c
char od_control.od_disable_dtr[40];
```

Unles the DIS_DTR_DISABLE od_disable flag is set, the Win32 version of OpenDoors will attempt to disable DTR response by the modem when closing the serial port, if the serial port was opened by OpenDoors. This is done by sending a series of commands to the modem, and possibly waiting for responses to the command. The string format specifies each command, followed by the required response. The command and response is separated by a single space character. If no response is required between two commands, then those commands may be separated by two space characters. A '|' character is translated into a carriage return, and a '~' character is translated into a one second pause. The default value of this string is "~+++~  AT&D0  ATO".

#### `od_emu_simulate_modem`

```c
BOOL od_control.od_emu_simulate_modem;
```

When this flag is set to its default value of FALSE, the OpenDoors terminal emulator displays text at full speed. When this flag is set to TRUE, the emulation functions will display text at approximately the same speed as it would be displayed when sent over the modem, based on the current connect speed. In local mode, an average modem speed of 9600bps is assumed. This allows animations to be displayed locally at the same speed as they would appear on the remote system. This switch affects the following functions: [`od_disp_emu()`](../api/od_disp_emu.md) [`od_send_file()`](../api/od_send_file.md) [`od_hotkey_menu()`](../api/od_hotkey_menu.md)

#### `od_errorlevel`

```c
unsigned char od_control.od_errorlevel[8];
```

Allows you to configure the errorlevel (program exit code) which OpenDoors exits with under various circumstances. The elements of this array are as follows:

[ERRORLEVEL_ENABLE]     Enables or disables custom errorlevels [ERRORLEVEL_CRITICAL]   Critical error errorlevel [ERRORLEVEL_NOCARRIER]  Carrier lost errorlevel [ERRORLEVEL_HANGUP]     Sysop manually terminated call [ERRORLEVEL_TIMEOUT]    User time expired errorlevel [ERRORLEVEL_INACTIVITY] Keyboard inactivity timeout errorlevel [ERRORLEVEL_DROPTOBBS]  Sysop returned user to BBS errorlevel [ERRORLEVEL_NORMAL]     Door has exited normally

If you wish to override the default errorlevels used by OpenDoors, you should set element [ERRORLEVEL_ENABLE] of this array to TRUE, and set the remaining array elements to the appropriate errorlevels. Note that the settings in this array only affect the errorlevels which OpenDoors uses when it causes the door to exit for one of the reasons listed above. This setting has no effect on the errorlevel returned when your program explicitly exits by calling the [`od_exit()`](../api/od_exit.md) function, or your program returns by calling exit() or returning from the main() function.

#### `od_force_local`

```c
char od_control.od_force_local;
```

This variable defaults to FALSE, which causes OpenDoors to behave normally. When this variable is set to TRUE prior to calling [`od_init()`](../api/od_init.md) or any other OpenDoors functions, OpenDoors will operate in local mode. In this case, no door information file will be read. Also, the user name will be used if [`od_control.user_name`](caller.md#user_name) has not been set prior to calling [`od_init()`](../api/od_init.md) or the first OpenDoors function.

The default OpenDoors settings when [`od_control.od_force_local`](#od_force_local) is set are as follows:

\- ANSI mode is on - Time limit is 60 minutes - User's location is the name of the BBS, or "Unknown Location" otherwise if BBS name is not known. - User name is set to sysop's name ("Sysop" if no sysop name is specified in the configuration file).

You may wish to add a "-local" type parameter to your program's command line, which will permit the sysop to easily operate the door in local mode, as an interface to the [`od_control.od_force_local`](#od_force_local) setting.

#### `od_in_buf_size`

```c
unsigned int od_control.od_in_buf_size;
```

Specifies the size, in characters, of the OpenDoor's internal local/remote inbound buffer size. Two bytes of storage are required for each character in this buffer. This variable should only be changed prior to calling [`od_init()`](../api/od_init.md) or the first OpenDoors function. If not set, this variable defaults to a value of 256.

The buffer corresponding to this variable should not be confused with the FOSSIL or internal communications receive buffer (which is set by [`od_control.od_com_rx_buf`](connection.md#od_com_rx_buf)). Unlike the serial I/O receive buffer, which is used only for characters received from the remote system, this buffer serves as a queue for input from both the remote system and the local keyboard. If you find that characters are lost when information is being set to your door from the user, you may wish to increase the size of this buffer.

#### `od_inactivity`

```c
unsigned int od_control.od_inactivity;
```

OpenDoors has a built in user-inactivity timeout facility, which will automatically disconnect a user who appears .to be sleeping at the keyboard. If the user has not pressed any keys on their keyboard for to great a length of time, they will be warned that they are about to be disconnected due to inactivity. If they still do not respond after another few seconds, OpenDoors will automatically disconnect the user and return control to the BBS software. The [`od_control.od_inactivity`](#od_inactivity) variable allows you to set the maximum length of time, in seconds, after which the user will be disconnected for inactivity. This variable defaults to a value of 200 seconds. You may disable OpenDoors' inactivity timeout altogether, by setting the [`od_control.od_inactivity`](#od_inactivity) variable to a value of 0.

od_inactive    int [`od_control.od_inactive_warning`](runtime.md#od_inactive_warning). _warning This variable sets the number of seconds prior to hanging up that OpenDoors displays the inactivity timeout warning. This variable should only be changed after your first call to an OpenDoors API function. If not explicitly set by your program, this setting defaults to 10 seconds.

#### `od_list_pause`

```c
char od_control.od_list_pause;
```

This variable contains a Boolean value, which allows you to control whether or not the user may pause displaying within the [`od_list_files()`](../api/od_list_files.md) and [`od_send_file()`](../api/od_send_file.md) function. When this variable is set to its default value of TRUE, the user will be able to pause the display by pressing the [P] key, and resume display by pressing any other key. However, the pause feature may be disabled by setting this variable to FALSE.

#### `od_list_stop`

```c
char od_control.od_list_stop;
```

This variable contains a Boolean value, which allows you to control whether or not the user may abort displaying within the [`od_list_files()`](../api/od_list_files.md) and [`od_send_file()`](../api/od_send_file.md) function. When this variable is set to its default value of TRUE, the user will be able to pause the display by pressing the [S], [CTRL]-[K] or [CTRL]-[C] keys. However, the stop feature may be disabled by setting this variable to FALSE.

#### `od_logfile`

```c
void *(od_control.od_logfile)(void);
```

To make the OpenDoors log file system available in your program, set this variable to INCLUDE_LOGFILE, prior to calling any OpenDoors functions. If not set, or if set to NO_LOGFILE, the OpenDoors log file system will not automatically be enabled.

#### `od_logfile_disable`

```c
char od_control.od_logfile_disable;
```

This variable defaults to the value of FALSE, unless the "LogfileDisable" option is specified in the configuration file, in which case the variable will be set to TRUE. If this variable is set to TRUE, OpenDoors will not write to a logfile, even if the logfile system is enabled using [`od_control.od_logfile`](#od_logfile).

#### `od_logfile_messages`

```c
char *od_control.od_logfile_messages[14];
```

This array of pointers to strings contains the messages that OpenDoors will automatically write to the log file, if the log file system is enabled. If you wish to change the settings of this array, you should do so before calling any OpenDoors functions. The default strings for this array are as follows:

[0] "Carrier lost, exiting door" [1] "System operator terminating call, exiting door" [2] "User's time limit expired, exiting door" [3] "User keyboard inactivity time limit exceeded, exiting door" [4] "System operator returning user to BBS, exiting door" [5] "Exiting door with errorlevel %d, [6] "Invoking operating system shell" [7] "Returning from operating system shell" [8] "User paging system operator" [9] "Entering sysop chat mode" [10] "Terminating sysop chat mode" [11] "%s entering door" [12] "Reason for chat: %s" [13] "Exiting door"

#### `od_logfile_name`

```c
char od_control.od_logfile_name[80];
```

This variable specifies the filename, and optionally the full path of the logfile where OpenDoors should perform logging. This variable only has an effect when set prior to calling any OpenDoors functions. If the log file name is specified in the configuration file, that name will be stored in this variable. If you do not set this variable, and the log file name is not specified in the configuration file, the default name "DOOR.LOG" will be used. If you wish to set this variable, you should do so prior to calling [`od_init()`](../api/od_init.md) or any OpenDoors function.

#### `od_maxtime`

```c
unsigned int od_control.od_maxtime;
```

This variable specifies the maximum length of time that any user is permitted to use the door, and is normally set from a configuration file option. If upon entering the door, the user's time remaining online is greater than the od_maxtime setting, their time remaining is temporarily decreased to the maximum value. Then upon exit of the door, the number of subtracted minutes is added back onto the user's remaining time. If the user's remaining time is less than this value, then the setting has no effect. A value of 0 disables the maximum time setting altogether.

#### `od_mps`

```c
void (*od_control.od_mps)(void);
```

To make the OpenDoors Multiple Personality system available in your program, set this variable to INCLUDE_MPS before calling any OpenDoors functions. If this variable is not set, or is set to NO_MPS, the Multiple Personality System will be disabled. For more information on the OpenDoors Multiple Personality System, see page 233.

#### `od_no_ra_codes`

```c
char od_control.od_no_ra_codes;
```

This variable defaults to FALSE. When set to TRUE, the translation of the RemoteAccess/QuickBBS control codes by the functions [`od_send_file()`](../api/od_send_file.md), [`od_hotkey_menu()`](../api/od_hotkey_menu.md) and [`od_disp_emu()`](../api/od_disp_emu.md) is disabled.

#### `od_nocopyright`

```c
char od_control.od_nocopyright;
```

This member is retained in `tODControl` for source and binary compatibility.
The current OpenDoors implementation does not read it, so setting it to either
[`TRUE`](../constants/general.md#true) or [`FALSE`](../constants/general.md#false) does not change startup output, colors, or any other behavior.
Applications must not use the field to determine whether startup identification
was displayed.

#### `od_noexit`

```c
char od_control.od_noexit;
```

This variable contains a Boolean value, which allows you to prevent OpenDoors from exiting when shutting down. This may be useful when you want to have your program to do more processing after you have called the [`od_exit()`](../api/od_exit.md) function, or if you do not wish to have your program exit automatically when the user drops carrier. Normally, this variable will default to a value of FALSE, indicating that OpenDoors will exit normally when the [`od_exit()`](../api/od_exit.md) function is called. However, you may optionally set this variable to TRUE after [`od_init()`](../api/od_init.md) or some OpenDoors function has been called. In this case, when the [`od_exit()`](../api/od_exit.md) function is called, either by your program manually, or automatically by OpenDoors in response to the user dropping carrier, etc., OpenDoors will not exit. However, the normal operations of closing the serial port and re-writing the door information file will be carried out. If you set the od_noexit variable to TRUE, you will probably have to provide some mechanism to allow your program to detect when OpenDoors shutdowns due to the loss of carrier, etc. The best way of doing this is to provide a function which is to be called at the beginning of the [`od_exit()`](../api/od_exit.md) function, by setting the od_control.od_before_exit pointer, described above.

#### `od_page_len`

```c
char od_control.od_page_len;
```

This variable allows you to control the length, in seconds, of the sysop page beep produced when the user pages the sysop via the [`od_page()`](../api/od_page.md) function.

#### `od_prog_copyright`

```c
char od_control.od_prog_copyright[40];
```

This variable should contain your program's copyright notice, such as "(C) Copyright 1996 by Your Name". This information is used in the Help|about dialog box under the Win32 version of OpenDoors, and may be used in other places in future versions of OpenDoors.

#### `od_prog_name`

```c
char od_control.od_prog_name[40];
```

This variable should contain the full name of your program, up to 39 characters. If not set, OpenDoors will use the string "OpenDoors" in place of this variable. If used, this variable should be set prior to calling any OpenDoors functions, and should not include your program's version number. This information is used to write your program's name in the log file and to indicate your program's name on various windows, among other places.

#### `od_prog_version`

```c
     char od_control.od_prog_version[40];
```

This variable should contain the version information of your program. If used, this variable should be set prior to calling any OpenDoors functions. This information is used in the Help|About dialog box under the Win32 version of OpenDoors, among other places.

#### `od_reg_key`

```c
DWORD od_control.od_reg_key;
```

This member is retained for source and binary compatibility. The current
implementation neither reads nor validates it. No current OpenDoors feature,
API result, startup path, or license check depends on its value. Leave it zero
unless an application itself uses the storage while maintaining compatibility
with an established `tODControl` layout.

#### `od_reg_name`

```c
char od_control.od_reg_name[36];
```

This member is retained for source and binary compatibility. The current
implementation does not read or display it and does not associate it with a
license or registration mechanism. Leave the first byte zero unless application
code itself has an established reason to use the storage; any such use is an
application convention rather than OpenDoors behavior.

#### `od_spawn_freeze_time`

```c
char od_control.od_spawn_freeze_time;
```

This variable is a Boolean value which indicates whether or not the user's time remaining is frozen during the execution of one of the od_spawn...() functions. If this variable is set to TRUE, the user's time remaining will not decrease during the time that the od_spawn...() function is executing. However, if this variable is set to FALSE, the user's time remaining will continue to be subtracted during the execution of the od_spawn...() function. The default value of this variable is FALSE.

#### `od_swapping_disable`

```c
char od_control.od_swapping_disable;
```

This variable is a Boolean value which specifies whether or not OpenDoors will attempt to swap itself and your entire door upon DOS shell or a call to one of the od_spawn...() functions. This variable defaults to FALSE. If set to TRUE, OpenDoors will not attempt to perform swapping activities.

#### `od_swapping_noems`

```c
char od_control.od_swapping_noems;
```

This variable is a Boolean value which can be used to prevent OpenDoors from swapping to EMS memory. This variable defaults to the value FALSE. If set to TRUE, OpenDoors will not attempt to use EMS memory for swapping, and will only swap to disk.

#### `od_swapping_path`

```c
char od_control.od_swapping_path;
```

This variable specifies the drive and directory where OpenDoors should create its disk swapping file, if applicable. More than one path can be specified, by separating the paths with a semi- colon (;) character.

#### `od_status_on`

```c
char od_control.od_status_on;
```

This variable is a Boolean value which allows your program to completely disable the OpenDoors status line. The variable defaults to a value of TRUE, which causes the OpenDoors status line to be controllable by function keys, displayed and updated as it would normally be. However, if this variable is set to FALSE, then OpenDoors will not update the status line, nor will it allow the status line to be re-displayed as a result of one of the status line ([F1] through [F10]) keys being pressed. When you change the value of this variable from FALSE to TRUE, OpenDoors will automatically redisplay the status line. Note, however, that the status line isn't automatically removed when the value of this variable is changed from TRUE to FALSE. In order to erase the status line after resetting the value of this variable, you should reset the output window to the full screen, by calling the function window(1,1,25,80). Then manually erase the old status line either by clearing the bottom two lines of the screen, or by clearing the entire screen.

It is important that you do not confuse the use of this variable with the [`od_set_statusline()`](../api/od_set_statusline.md) function, which is described on page 137. When the status line is enabled, the sysop can change which status line, if any, is being displayed, using the function keys [F1] through [F10]. The [`od_set_statusline()`](../api/od_set_statusline.md) function allows your program to make the same changes to the status line setting which the sysop can make by pressing one of the function keys. The status line can be removed from the screen, allowing a full 25 lines of text to be displayed, by pressing the [F10] key, or by making the appropriate call to the [`od_set_statusline()`](../api/od_set_statusline.md) function. Note, however, than when this is done, the status line is still enabled, and can be turned on by pressing any of the other function keys. On the other hand, if the status line is turned off using this variable ([`od_control.od_status_on`](#od_status_on)), the status line sub-system will be disabled, and pressing function keys will not "bring it back". So, if you were writing a program where a status line would be undesirable - such as a non-door communications program, you would use the [`od_control.od_status_on`](#od_status_on) variable. On the other hand, if you only wanted to temporarily remove the status line - say in order that all 25 lines of a door program's output could be viewed - while still allowing the status line to be turned on with the sysop function keys, you would use the [`od_set_statusline()`](../api/od_set_statusline.md) function. For more information on the [`od_set_statusline()`](../api/od_set_statusline.md) function, see page 137.

### Output and diagnostic controls

#### `od_always_clear`

```c
BOOL od_control.od_always_clear;
```

This field determines how [`od_clr_scr()`](../api/od_clr_scr.md) treats the
caller's screen-clearing preference. Initialization unconditionally sets it to
[`TRUE`](../constants/general.md#true), which makes every call clear the screen. Set it to [`FALSE`](../constants/general.md#false) after
initialization to honor bit `0x02` of
[`user_attribute`](caller.md#user_attribute) when an extended
`EXITINFO.BBS` record or a [`CUSTOM`](../constants/session.md#custom) door-information reader supplies that
field: OpenDoors clears when the bit is set and leaves the screen unchanged
when it is clear.

For other door-information formats, OpenDoors has no screen-clearing
preference to honor and clears the screen even when this field is [`FALSE`](../constants/general.md#false).
The field affects both local and remote clearing as one operation; it does not
cause either screen to be cleared independently.

#### `od_color_char`

```c
char od_control.od_color_char;
```

This field enables the compact binary color form accepted by
[`od_printf()`](../api/od_printf.md). When the configured marker is encountered
in formatted output, the immediately following byte is passed as an IBM-PC
attribute to [`od_set_attrib()`](../api/od_set_attrib.md); neither byte is
displayed. For example, if the marker is `1`, the byte sequence `1, 0x0e`
selects yellow on black.

Initialization unconditionally sets this field to zero, disabling the binary
form. Assign a nonzero marker afterward. This mechanism is independent of the
delimited color-name syntax selected by
[`od_color_delimiter`](#od_color_delimiter); setting both fields to zero
disables all inline color processing in [`od_printf()`](../api/od_printf.md).

#### `od_disable_inactivity`

```c
BOOL od_control.od_disable_inactivity;
```

When [`TRUE`](../constants/general.md#true), this field temporarily suppresses both the inactivity warning and
inactivity shutdown without changing [`od_inactivity`](#od_inactivity) or its
last-activity timestamp. It is initially [`FALSE`](../constants/general.md#false). The Win32 local User menu can
toggle it, and application code may change it at runtime. Re-enabling the timer
after its deadline has passed can cause timeout processing on the next kernel
update.

#### `od_full_color`

```c
BOOL od_control.od_full_color;
```

OpenDoors normally compares a requested attribute with
[`od_cur_attrib`](runtime.md#od_cur_attrib) and transmits only the ANSI or
AVATAR changes which are required. Setting this initially false field to
[`TRUE`](../constants/general.md#true) makes each [`od_set_attrib()`](../api/od_set_attrib.md) call transmit a
complete color selection even when OpenDoors believes the attribute is already
active. This can resynchronize a terminal whose state was changed outside the
OpenDoors output interfaces, at the cost of additional output.

#### `od_full_put`

```c
BOOL od_control.od_full_put;
```

When [`od_puttext()`](../api/od_puttext.md) updates a remote screen, OpenDoors
normally compares the new block with the saved virtual screen and skips runs
whose characters and effective backgrounds are already identical. Setting
this initially false field to [`TRUE`](../constants/general.md#true) disables that optimization and transmits
the complete requested rectangle. Local virtual-screen storage is updated in
either mode.

#### `od_internal_debug`

```c
BOOL od_control.od_internal_debug;
```

This field is read only by builds compiled with the private `OD_DIAGNOSTICS`
instrumentation. In the diagnostic Win32 build, setting it to [`TRUE`](../constants/general.md#true) enables
modal progress and modem-command message boxes during shutdown. It is
initially [`FALSE`](../constants/general.md#false); ordinary distributed builds compile out these checks, so
assigning it has no effect there. The field remains public for structure and
diagnostic-build compatibility.

#### `od_cmd_show`

```c
int od_control.od_cmd_show;
```

This field exists only in the Win32 structure layout. A GUI application should
copy the `nCmdShow` value received by `WinMain()` into it before initialization.
If the value is `SW_MINIMIZE`, `SW_SHOWMINIMIZED`, or `SW_SHOWMINNOACTIVE`,
OpenDoors initially shows its local frame minimized without activating it; all
other values cause the frame to be restored. The static value is zero, which
therefore follows the restore path. OpenDoors reads but never writes the field.

### Function keys

The local system operator can enter chat mode, invoke a command shell, change
the caller's remaining time, and perform the other operations described below
without those keystrokes being passed to the caller. The fields in this section
use the IBM BIOS key representation: the high byte is the keyboard scan code,
and the low byte is the ASCII character when the key produces one. Thus
`0x2e00` identifies Alt-C and `0x4800` identifies Up Arrow.

The DOS and DOS32 personalities assign the built-in mappings during
initialization. Consequently, an application which replaces one of these
mappings must do so after [`od_init()`](../api/od_init.md), or after the first
API call which causes initialization. A value of zero leaves an operation
without an ordinary keyboard mapping. On platforms which do not provide the
DOS personality system, these fields retain their zero-initialized values
unless the application assigns them.

OpenDoors tests built-in operations before application-defined hot keys. A
custom hot key which duplicates a built-in mapping therefore does not receive
the keystroke.

#### `key_chat`

```c
WORD od_control.key_chat;
```

This field selects the key which enters or leaves sysop chat. The standard and
RemoteAccess personalities use Alt-C (`0x2e00`), PCBoard uses F10 (`0x4400`),
and Wildcat uses Alt-A (`0x4100`). While chat is active, the Escape key also
ends chat independently of this setting.

#### `key_dosshell`

```c
WORD od_control.key_dosshell;
```

This field selects the key which invokes the local command shell. The standard
and RemoteAccess personalities use Alt-J (`0x2400`), PCBoard uses F5
(`0x3f00`), and Wildcat uses Alt-D (`0x2000`). OpenDoors processes the before
and after shell strings and callbacks around the shell operation.

#### `key_drop2bbs`

```c
WORD od_control.key_drop2bbs;
```

This field selects the key which terminates the door and returns the connected
caller to the BBS without dropping the connection. The standard and
RemoteAccess personalities use Alt-D (`0x2000`), PCBoard uses Alt-X
(`0x2d00`), and Wildcat uses F10 (`0x4400`). OpenDoors performs the shutdown;
the application may change the mapping but does not otherwise read this field.

#### `key_hangup`

```c
WORD od_control.key_hangup;
```

This field selects the key which terminates the caller's connection and shuts
down the door with the configured hangup error level. The standard and
RemoteAccess personalities use Alt-H (`0x2300`), PCBoard uses F8 (`0x4200`),
and Wildcat leaves the operation unassigned.

#### `key_keyboardoff`

```c
WORD od_control.key_keyboardoff;
```

This field selects the key which toggles
[`od_user_keyboard_on`](runtime.md#od_user_keyboard_on), thereby enabling or
disabling input from the remote caller. All four built-in personalities use
Alt-K (`0x2500`). Local sysop keys remain active; to disable local keyboard
processing, use the appropriate [`od_disable`](#od_disable) flag.

#### `key_lesstime`

```c
WORD od_control.key_lesstime;
```

This field selects the key which subtracts one minute from
[`user_timelimit`](caller.md#user_timelimit). The value is never reduced below
zero. The standard and RemoteAccess personalities use Down Arrow (`0x5000`);
PCBoard and Wildcat leave this built-in operation unassigned. The Wildcat
personality separately installs Down Arrow as a personality hot key and uses it
to subtract five minutes.

#### `key_lockout`

```c
WORD od_control.key_lockout;
```

This field selects the key which sets
[`user_security`](caller.md#user_security) to zero, terminates the connection,
and shuts down the door with the hangup error level. The standard and
RemoteAccess personalities use Alt-L (`0x2600`), PCBoard uses F2 (`0x3c00`),
and Wildcat uses Alt-0 (`0x8100`).

#### `key_moretime`

```c
WORD od_control.key_moretime;
```

This field selects the key which adds one minute to
[`user_timelimit`](caller.md#user_timelimit), up to 1,440 minutes. The standard
and RemoteAccess personalities use Up Arrow (`0x4800`); PCBoard and Wildcat
leave this built-in operation unassigned. The Wildcat personality separately
installs Up Arrow as a personality hot key and uses it to add five minutes.

#### `key_status`

```c
WORD od_control.key_status[9];
```

Each element selects the key which activates the status-line number having the
same array index. There are nine elements, for lines 0 through 8. The mapping
has no visible effect while [`od_status_on`](#od_status_on) is [`FALSE`](../constants/general.md#false), and
pressing the key for the already active line does not redraw it.

The RemoteAccess personality maps F1 through F7 to elements 0 through 6, F9 to
element 7, and F10 to element 8. The standard personality maps F1, F9, and F10
to elements 0, 7, and 8 and leaves the others unassigned. PCBoard maps Alt-H
to element 1 and may exchange the element 0 and element 1 mappings as its
display changes. Wildcat leaves all nine elements unassigned. The compatibility
macros [`key_nohelp`](../compatibility.md) and [`key_help`](../compatibility.md) name elements 0 and 6 respectively.

#### `key_sysopnext`

```c
WORD od_control.key_sysopnext;
```

This field selects the key which toggles
[`sysop_next`](runtime.md#sysop_next). The standard, RemoteAccess, and PCBoard
personalities use Alt-N (`0x3100`); Wildcat uses F1 (`0x3b00`).

#### `od_num_keys`

```c
BYTE od_control.od_num_keys;
```

This field is the number of active entries in [`od_hot_key`](#od_hot_key) and
[`od_hot_function`](#od_hot_function). It is initially zero, but a personality
may add its own entries during initialization. The maximum is 16; assigning a
larger value causes OpenDoors to read beyond both arrays.

An application which installs custom local keys should do so after
initialization, preserve any entries already installed by the personality, and
never increment this field beyond 16.

#### `od_hot_key`

```c
INT16 od_control.od_hot_key[16];
```

The first [`od_num_keys`](#od_num_keys) elements contain application- or
personality-defined local key codes. OpenDoors compares each otherwise
unhandled local keystroke with these entries. A match is not placed in the
normal local/remote input queue and is not passed to
[`od_local_input`](#od_local_input).

The array is zero-initialized. OpenDoors does not assign application entries;
the DOS personalities may append private entries through the personality SDK.

#### `od_hot_function`

```c
void (*od_control.od_hot_function[16])(void);
```

Each element is the optional callback for the corresponding
[`od_hot_key`](#od_hot_key) entry. After a matching key is stored in
[`od_last_hot`](runtime.md#od_last_hot), OpenDoors first notifies the active
personality with [`PEROP_CUSTOMKEY`](../constants/components.md#perop_customkey), then calls this function if the pointer is
not `NULL`. The callback is synchronous and receives no arguments. It may use
the array index known when it was installed or examine
[`od_last_hot`](runtime.md#od_last_hot); a personality is permitted to clear
that field before the callback runs.

All pointers are initially `NULL`. OpenDoors reads but never writes
application callback pointers.

For example, the following adds Page Up as an application-defined local key,
provided fewer than 16 keys are already installed:

```c
static void add_points(void)
{
    current_user->points += 10;
}

if (od_control.od_num_keys < 16) {
    unsigned int key = od_control.od_num_keys++;
    od_control.od_hot_key[key] = 0x4900;
    od_control.od_hot_function[key] = add_points;
}
```

### Color customization

These fields contain IBM-PC text attributes in the format accepted by
[`od_set_attrib()`](../api/od_set_attrib.md): the low four bits select the
foreground, bits 4 through 6 select the background, and bit 7 selects blinking
when that mode is supported. The optional configuration component can set most
of them through the corresponding `Colour` keyword.

During initialization, OpenDoors replaces every zero-valued color field with
the default given below. Consequently, a nonzero application override may be
assigned before initialization, but attribute zero (black on black) must be
assigned afterward.

#### `od_chat_color1`

```c
BYTE od_control.od_chat_color1;
```

This is the attribute used for text entered by the local sysop in built-in chat
mode. It is also used for the before- and after-chat messages and as the common
chat attribute outside typed text. Its default is `0x0c` (light red on black).
The `ChatSysopColour` configuration setting can replace it. OpenDoors reads
the field whenever it selects the sysop chat color.

#### `od_chat_color2`

```c
BYTE od_control.od_chat_color2;
```

This is the attribute used for text entered by the remote caller in built-in
chat mode. Its default is `0x0f` (bright white on black). The
`ChatUserColour` configuration setting can replace it.

#### `od_list_title_col`

```c
BYTE od_control.od_list_title_col;
```

[`od_list_files()`](../api/od_list_files.md) uses this attribute for blank
lines and lines beginning with a space, which serve as headings or comments in
a `FILES.BBS` listing. Its default is `0x0f`. The `FileListTitleColour`
configuration setting can replace it.

#### `od_list_name_col`

```c
BYTE od_control.od_list_name_col;
```

This is the filename attribute in a `FILES.BBS` listing, including entries
whose referenced file is unavailable. Its default is `0x0e`. The
`FileListNameColour` configuration setting can replace it.

#### `od_list_size_col`

```c
BYTE od_control.od_list_size_col;
```

This is the file-size attribute for available files in a `FILES.BBS` listing.
Its default is `0x0d`. The `FileListSizeColour` configuration setting can
replace it.

#### `od_list_comment_col`

```c
BYTE od_control.od_list_comment_col;
```

This is the file-description attribute in a `FILES.BBS` listing. Its default
is `0x03`. The `FileListDescriptionColour` configuration setting can replace
it.

#### `od_list_offline_col`

```c
BYTE od_control.od_list_offline_col;
```

This is the attribute used for [`od_offline`](#od_offline) when a
`FILES.BBS` entry names a file which cannot be found. Its default is `0x0c`.
The `FileListOfflineColour` configuration setting can replace it.

#### `od_continue_col`

```c
BYTE od_control.od_continue_col;
```

This is the attribute used for the [`od_continue`](#od_continue) page prompt.
OpenDoors restores the previous attribute before reading the response or
erasing the prompt. Its default is `0x0f`. The `PagePromptColour`
configuration setting can replace it.

#### `od_local_win_col`

```c
BYTE od_control.od_local_win_col;
```

This attribute supplies both the text and border colors for local message
windows, including the startup information-file message and the shutdown
message. Its default is `0x19`. No built-in configuration-file keyword changes
it; application code may assign it.

#### `od_menu_title_col`

```c
BYTE od_control.od_menu_title_col;
```

This is the title attribute used by
[`od_popup_menu()`](../api/od_popup_menu.md). Its default is `0x74`. The
`PopupMenuTitleColour` configuration setting can replace it.

#### `od_menu_border_col`

```c
BYTE od_control.od_menu_border_col;
```

This is the popup-menu border and separator attribute. Its default is `0x70`.
The `PopupMenuBorderColour` configuration setting can replace it.

#### `od_menu_text_col`

```c
BYTE od_control.od_menu_text_col;
```

This is the ordinary text attribute for an unselected popup-menu item. Its
default is `0x70`. The `PopupMenuTextColour` configuration setting can replace
it.

#### `od_menu_key_col`

```c
BYTE od_control.od_menu_key_col;
```

This is the shortcut-key attribute within an unselected popup-menu item. Its
default is `0x7f`. The `PopupMenuKeyColour` configuration setting can replace
it.

#### `od_menu_highlight_col`

```c
BYTE od_control.od_menu_highlight_col;
```

This is the text attribute for the selected popup-menu item. Its default is
`0x07`. The `PopupMenuHighlightColour` configuration setting can replace it.

#### `od_menu_highkey_col`

```c
BYTE od_control.od_menu_highkey_col;
```

This is the shortcut-key attribute within the selected popup-menu item. Its
default is `0x0f`. The `PopupMenuHighKeyColour` configuration setting can
replace it.

### Text customization

The prompts in this section allow the application's local interface and caller
messages to be translated or otherwise customized. Initialization assigns all
of their defaults unconditionally, so replacements must be installed after
[`od_init()`](../api/od_init.md), or after another API call has caused
initialization. For example:

```c
od_control.od_before_shell=(char *)"\n\rJust a moment...\n\r";
```

The detailed entries below give the current default from the implementation,
the field's exact consumer, any required formatting conversions, and the cases
in which a fixed display width or `NULL` value has special meaning.


#### `od_after_chat`

```c
char *od_control.od_after_chat;
```

This string is displayed when built-in chat cleanup begins, before
[`od_cafter_chat`](#od_cafter_chat) is called. Its default is
`"\n\rChat mode ended.\n\r\n\r"`. Assign `NULL` after initialization to suppress
the message. OpenDoors reads the string but does not modify or free it.

#### `od_after_shell`

```c
char *od_control.od_after_shell;
```

This string is displayed after the local command interpreter returns and
before [`od_cafter_shell`](#od_cafter_shell) is called. Its default is
`"\n\r...Thanks for waiting\n\r\n\r"`. Assign `NULL` after initialization to
suppress the message.

#### `od_before_chat`

```c
char *od_control.od_before_chat;
```

This string is displayed after [`od_cbefore_chat`](#od_cbefore_chat) returns
and immediately before the built-in chat input loop. Its default is
`"\n\rThe system operator has placed you in chat mode to talk with you:\n\r\n\r"`.
Assign `NULL` after initialization to suppress the message.

#### `od_before_shell`

```c
char *od_control.od_before_shell;
```

This string is displayed after [`od_cbefore_shell`](#od_cbefore_shell) returns
and immediately before OpenDoors invokes the local command interpreter. Its
default is `"\n\rPlease wait a moment...\n\r"`. Assign `NULL` after
initialization to suppress the message.

#### `od_chat_reason`

```c
char *od_control.od_chat_reason;
```

[`od_page()`](../api/od_page.md) displays this prompt before reading the
caller's reason for requesting chat. The default is
`"               Why would you like to chat? (Blank line to cancel)\n\r"`.
The fifteen leading spaces center the prompt over the 79-column input frame.
This pointer must not be `NULL` when paging is used.

#### `od_continue`

```c
char *od_control.od_continue;
```

This is the page-boundary prompt used by display-file and file-listing
operations. The default is `"Continue? [Y/n/=]"`. OpenDoors uses its byte
length to erase the prompt after a valid response, so embedded terminal
control sequences or characters whose displayed width differs from one column
will leave the erasure width incorrect. The pointer must not be `NULL` while
page pausing is enabled.

#### `od_continue_yes`

```c
char od_control.od_continue_yes;
```

This is the affirmative response to [`od_continue`](#od_continue). Its default
is lowercase `y`; OpenDoors accepts both cases. Enter and Space are always
accepted as affirmative responses independently of this field.

#### `od_continue_no`

```c
char od_control.od_continue_no;
```

This is the response which stops the current paged display. Its default is
lowercase `n`; OpenDoors accepts both cases. Escape, Ctrl-C, Ctrl-K, and `s` in
either case also stop the display independently of this field.

#### `od_continue_nonstop`

```c
char od_control.od_continue_nonstop;
```

This response continues the current display and disables further page prompts
for that operation. Its default is `=`; OpenDoors applies the usual
case-insensitive comparison even though the default has no case.

#### `od_day`

```c
char *od_control.od_day[7];
```

These strings contain abbreviated weekday names in Sunday-through-Saturday
order. The defaults are `"Sun"`, `"Mon"`, `"Tue"`, `"Wed"`, `"Thu"`, `"Fri"`,
and `"Sat"`. The logfile component uses them in each session-opening date.
OpenDoors does not require a three-byte value, but longer strings change the
log layout.

#### `od_month`

```c
char *od_control.od_month[12];
```

These strings contain abbreviated month names in January-through-December
order. The defaults are `"Jan"` through `"Dec"`. The logfile component uses
them in session-opening dates, and the RemoteAccess personality uses them in a
local status line. Longer strings therefore alter the log layout and can
overwrite adjacent status-line fields.

#### `od_hanging_up`

```c
char *od_control.od_hanging_up;
```

This is the local message-window text shown while OpenDoors is terminating the
call. Its default is `"Ending call..."`. A `NULL` pointer suppresses this
window on the terminating-call path. The string is not sent to the remote
caller.

#### `od_exiting`

```c
char *od_control.od_exiting;
```

This is the local message-window text shown during a normal return to the BBS.
Its default is `"Program is exiting..."`. Unlike
[`od_hanging_up`](#od_hanging_up), this pointer is not checked for `NULL` before
it is passed to the local window implementation.

#### `od_help_text`

```c
char *od_control.od_help_text;
```

The standard DOS and DOS32 personality displays this string on row 24 of its
help status screen. The 80-column default is
`"  Alt: [C]hat [H]angup [L]ockout [J]Dos [K]eyboard-Off [D]rop to BBS            "`.
Replacement text should occupy exactly 80 screen columns; shorter text can
leave prior contents visible and longer text can wrap into the following row.
Other built-in personalities do not use this field.

#### `od_help_text2`

```c
char *od_control.od_help_text2;
```

The standard DOS and DOS32 personality displays this string on row 25 of its
help status screen. It defaults to the library's `OD_VER_STATUSLINE` text:
`"  OpenDoors 6.30 - (C) Copyright 1991-2001 by Brian Pirie                      "`.
Replacement text must fit the 80-column row; output beyond its final cell can
wrap and scroll the local screen. Other built-in personalities do not use this
field.

#### `od_inactivity_timeout`

```c
char *od_control.od_inactivity_timeout;
```

This message is delivered when the caller's inactivity limit expires, before
OpenDoors shuts down the connection. Its default is
`"\n\rMaximum user inactivity time has elapsed, please call again.\n\r\n\r"`.
If [`od_time_msg_func`](#od_time_msg_func) is non-`NULL`, the string is passed
to that callback instead of being displayed. It contains no formatting
conversion and must not be `NULL` while inactivity checking is enabled.

#### `od_inactivity_warning`

```c
char *od_control.od_inactivity_warning;
```

This message is delivered once when the inactivity warning interval begins.
Its default is
`"\n\rWARNING: Inactivity timeout in 10 seconds, press a key now to remain online.\n\r\n\r"`.
The number in this literal is not generated from
[`od_inactive_warning`](runtime.md#od_inactive_warning); an application which
changes that interval should also replace this text. The message is redirected
through [`od_time_msg_func`](#od_time_msg_func) when that callback is present.

#### `od_no_keyboard`

```c
char *od_control.od_no_keyboard;
```

The standard DOS and DOS32 personality displays this indicator on row 24 when
remote caller input is disabled. Its default is `"[Keyboard]"`, exactly ten
columns. Replacement text should remain ten columns so that status updates
which erase ten spaces neither leave old characters nor erase adjacent fields.

#### `od_no_sysop`

```c
char *od_control.od_no_sysop;
```

[`od_page()`](../api/od_page.md) displays this message when paging is disabled
or the current time is outside the allowed interval. Its default is
`"\n\rSorry, the system operator is not available at this time.\n\r"`. It is
followed immediately by [`od_press_key`](#od_press_key).

#### `od_no_response`

```c
char *od_control.od_no_response;
```

[`od_page()`](../api/od_page.md) displays this message if the configured page
duration expires without the sysop entering chat. Its default is
`" No response.\n\r\n\r"`. It is followed immediately by
[`od_press_key`](#od_press_key).

#### `od_no_time`

```c
char *od_control.od_no_time;
```

This message is delivered when the caller's remaining session time reaches
zero, immediately before OpenDoors initiates timeout shutdown. Its default is
`"\n\rSorry, you have used up all of your time for this session.\n\r\n\r"`.
[`od_time_msg_func`](#od_time_msg_func) receives it instead when installed.

#### `od_offline`

```c
char *od_control.od_offline;
```

[`od_list_files()`](../api/od_list_files.md) displays this marker when a
`FILES.BBS` entry names a file which cannot be found. Its default is
`"[OFFLINE] "`, exactly ten columns. A different length changes the alignment
and available width of the following description field.

#### `od_paging`

```c
char *od_control.od_paging;
```

[`od_page()`](../api/od_page.md) displays this text when it begins sounding the
sysop page. Its default is `"\n\rPaging system operator for chat"`. One period
and one bell are then emitted for each second of
[`od_page_len`](#od_page_len), until the sysop answers or the interval expires.

#### `od_press_key`

```c
char *od_control.od_press_key;
```

This prompt follows [`od_no_sysop`](#od_no_sysop) and
[`od_no_response`](#od_no_response). Its default is
`"Press [Enter] to continue"`. Despite the wording, the paging paths accept
Enter or Line Feed through [`od_get_answer()`](../api/od_get_answer.md).

#### `od_sending_rip`

```c
char *od_control.od_sending_rip;
```

When a `.RIP` file is sent without local echo,
[`od_send_file()`](../api/od_send_file.md) and
[`od_send_file_section()`](../api/od_send_file_section.md) prepend this text to
the filename in a local progress window. Its default is
`"Sending RIP file: "`. It is not transmitted to the caller.

#### `od_status_line`

```c
char *od_control.od_status_line[3];
```

The standard DOS and DOS32 personality uses these strings to construct its
primary two-row local status display:

| Element | Default and use |
| --- | --- |
| `0` | `"                                                                     [Node:     "`; the 80-column row-24 background. The personality overwrites the beginning with element 1 and writes the node at column 77. |
| `1` | `"%s of %s at %lu BPS"`; formatted with [`user_name`](caller.md#user_name), [`user_location`](caller.md#user_location), and [`od_connect_speed`](connection.md#od_connect_speed), beginning at column 1 of row 24. |
| `2` | `"Security:        Time:                                               [F9]=Help "`; the 79-column row-25 background. The personality separately fills security, time, and state indicators, then writes the final cell directly. |

Element 1 must retain, in order, two string conversions and one `unsigned
long` conversion compatible with those arguments. Elements 0 and 2 should
remain exactly 80 and 79 screen columns respectively. Longer rendered output
can wrap or overwrite the next status row; shorter background strings can
leave stale characters visible.

#### `od_sysop_next`

```c
char *od_control.od_sysop_next;
```

The standard DOS and DOS32 personality displays this indicator on row 25 while
[`sysop_next`](runtime.md#sysop_next) is true. Its default is `"[SN] "`, exactly
five columns. Replacement text should remain five columns because the update
path clears exactly five spaces when the state becomes false.

#### `od_time_left`

```c
char *od_control.od_time_left;
```

The standard DOS and DOS32 personality formats this string with
[`user_timelimit`](caller.md#user_timelimit) at column 24 of row 25. Its default
is `"%d mins   "`. The replacement must contain one integer conversion
compatible with an `int`. The default occupies ten columns for a two-digit
value, eleven for a three-digit value, and twelve for a four-digit value; the
next status field begins at column 35.

#### `od_time_warning`

```c
char *od_control.od_time_warning;
```

This format is used when three, two, or one minute remains in the caller's
session. Its default is
`"\n\rWARNING: You only have %d minute(s) remaining for this session.\n\r\n\r"`.
It must contain one integer conversion compatible with an `int`. OpenDoors
formats the result before passing it to [`od_time_msg_func`](#od_time_msg_func)
or displaying it.

#### `od_want_chat`

```c
char *od_control.od_want_chat;
```

The standard DOS and DOS32 personality displays this indicator on row 25 while
[`user_wantchat`](caller.md#user_wantchat) is true. Its default is
`"[Want-Chat]"`, exactly eleven columns. Replacement text should remain eleven
columns because the update path clears exactly eleven spaces when the request
is removed.

Initialization assigns every prompt pointer and response character in this
section unconditionally. Applications must therefore replace them after
[`od_init()`](../api/od_init.md), or after another API call has caused
initialization. Except for the explicitly nullable chat, shell, and hangup
messages, prompt pointers must refer to valid strings whenever the associated
facility can use them. OpenDoors reads these application replacements but does
not copy, modify, or free them.
