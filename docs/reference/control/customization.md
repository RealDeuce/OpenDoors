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

```c
od_before      void (*od_control.od_before_exit)();
_exit
This variable contains a pointer to a function which OpenDoors
should call prior to exiting, or NULL if you do not wish to have
any function called at exit time. For an example of the use of
this variable, see the description of the EX_VOTE.C example
program, which begins on page 38.
```

```c
od_cafter      void (*od_control.od_cafter_chat)();
_chat
The function pointed to by this variable will be called after
sysop chat mode has ended. This may be useful for allowing you
to save the user's screen contents prior to chat, and restoring
the afterwards. If this variable contains its default value of
NULL, no function will be called. To alter the string of text
which is displayed after sysop chat, see the
od_control.od_after_chat variable, which is described in the
section on the prompts customization portion of the control
structure.
```

```c
od_cafter      void (*od_control.od_cafter_shell)();
_shell
The function pointed to by this variable will be called after
the sysop has returned from a DOS shell. If this variable
contains its default value of NULL, no function will be called.
To alter the string of text which is displayed after a DOS
shell, see the od_control.od_after_shell variable, which is
described in the section on the prompts customization portion of
the control structure.
```

```c
od_cbefore     void (*od_control.od_cbefore_chat)();
_chat
The function pointed to by this variable will be called prior to
entering sysop chat mode. This may be useful for allowing you to
save the user's screen contents prior to chat, and restoring the
afterwards. If this variable contains its default value of NULL,
no function will be called. To alter the string of text which is
displayed prior to sysop chat, see the od_control.od_before_chat
variable, which is described in the section on the prompts
customization portion of the control structure. To replace the
OpenDoors sysop chat facility with your own, simply activate
your chat mode when this function is called. Your chat mode
facility should remain active until OpenDoors sets the
od_control.od_chat_active variable to FALSE. If you wish to
terminate chat mode prior to this variable being set to FALSE,
you should set this variable to FALSE yourself if you do not
wish OpenDoors to activate its own chat mode.
```

```c
od_cbefore     void (*od_control.od_cbefore_shell)();
_shell
The function pointed to by this variable will be called prior to
executing a sysop DOS shell. If this variable contains its
default value of NULL, no function will be called. To alter the
string of text which is displayed before a DOS shell, see the
od_control.od_before_shell variable, which is described in the
section on the prompts customization portion of the control
structure.
```

```c
od_cfg_lines   char od_control.cfg_lines[25][33];
```

This array contains the strings for the keywords that represent various lines in the definition of a custom door information file. Each keyword must be 32 character or less in length. These keywords are not case sensitive. See page 230 for more information on defining custom door information (drop) file formats. The default values for this array are as follows:

[0]  "Ignore" [1]  "ComPort" [2]  "FossilPort" [3]  "ModemBPS" [4]  "LocalMode" [5]  "UserName" [6]  "UserFirstName" [7]  "UserLastName" [8]  "Alias" [9]  "HoursLeft" [10] "MinutesLeft" [11] "SecondsLeft" [12] "ANSI" [13] "AVATAR" [14] "PagePausing" [15] "ScreenLength" [16] "ScreenClearing" [17] "Security" [18] "City" [19] "Node" [20] "SysopName" [21] "SysopFirstName" [22] "SysopLastName" [23] "SystemName" [24] "RIP"

If you wish to change any of these variable, you must do so before calling any OpenDoors functions.

#### `od_cfg_text`

```c
char od_control.od_cfg_text[47][33];
```

This array of strings contains the built-in configuration file keywords that are recognized by OpenDoors. These keywords may be up to 32 characters in size, and are not case sensitive. If you wish to change any of these settings, you must do so before calling any OpenDoors functions. The default values for this array are as follows:

[0]  "Node" [1]  "BBSDir" [2]  "DoorDir" [3]  "LogFileName" [4]  "DisableLogging" [5]  "SundayPagingHours" [6]  "MondayPagingHours" [7]  "TuesdayPagingHours" [8]  "WednesdayPagingHours" [9]  "ThursdayPagingHours" [10] "FridayPagingHours" [11] "SaturdayPagingHours" [12] "MaximumDoorTime" [13] "SysopName" [14] "SystemName" [15] "SwappingDisable" [16] "SwappingDir" [17] "SwappingNoEMS" [18] "LockedBPS" [19] "SerialPort" [20] "CustomFileName" [21] "CustomFileLine" [22] "InactivityTimeout" [23] "PageDuration" [24] "ChatUserColor" [25] "ChatSysopColor" [26] "FileListTitleColor" [27] "FileListNameColor" [28] "FileListSizeColor" [29] "FileListDescriptionColor" [30] "FileListOfflineColor" [31] "Personality" [32] "NoFossil" [33] "PortAddress" [34] "PortIRQ" [35] "ReceiveBuffer" [36] "TransmitBuffer" [37] "PagePromptColor" [38] "LocalMode" [39] "PopupMenuTitleColor" [40] "PopupMenuBorderColor" [41] "PopupMenuTextColor" [42] "PopupMenuKeyColor" [43] "PopupMenuHighlightColor" [44] "PopupMenuHighKeyColor" [45] "NoFIFO" [46] "FIFOTriggerSize" [47] "DiableDTR" [48] "NoDTRDisable"

#### `od_chat_active`

```c
char od_control.od_chat_active;
```

This variable is set to TRUE when sysop chat mode is active, and is set to FALSE when sysop chat mode is not active. This variable can be used to determine whether or not chat mode is active, and to force chat mode to end. When the sysop presses the chat mode key ([ALT]-[C] if the default personality is being used) while chat mode is active, this variable is set to FALSE.

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

```c
od_config      void (*od_control.od_config_function)(char *keyword, char
_function      *options);
```

If set, this variable should point to the function that OpenDoors should call when lines with unrecognized keywords are encountered in the configuration file. This allows you to add your own configuration file keywords. The first parameter to this function will be a pointer to a string containing the unrecognized keywords, and the second parameter will be a pointer to a string containing any options that were specified after the keyword. If no options were specified after the keyword, this string will have a length of 0.

```c
od_default     void (*od_control.od_default_personality)(unsigned char
_personality   operation);
```

This variable sets the default personality that OpenDoors will use if the multiple personality system is active. If the multiple personality system is not active, the personality set by this variable will be the only personality available. This variable should only be set prior to calling an OpenDoors function. This variable can be set to point to your own personality function, or it can be set to one of the manifest constants that represent one of the built-in personalities:

PER_OPENDOORS PER_PCBOARD PER_RA PER_WILDCAT

For more information on the OpenDoors Multiple Personality System, see page 230.

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

```c
od_help        void (*od_control.od_help_callback)(void);
_callback
If this variable is set to a non-NULL value, the Win32 version
of OpenDoors will provide a Contents item on the help menu, and
call the function pointed to by this variable when the user
chooses the Contents menu item.
```

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

```c
od_ker_exec    void (*od_control.od_ker_exec)(void);
```

When od_control.od_ker_exec is set to point to a function, OpenDoors will call this function whenever [`od_kernel()`](../api/od_kernel.md) executes. This provides any easy way for you to perform your own processing on a regular basis during door execution. The od_control.od_ker_exec variable defaults to NULL.

#### `od_last_input`

```c
char od_control.od_last_input;
```

Indicates whether the last key retrieved using the [`od_get_key()`](../api/od_get_key.md) function originated from the remote user, or the local sysop. If the input originated from the remote, this variable is set to 0. If the input originated from the local keyboard, this variables is set to 1.

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

```c
od_local       void (*od_control.od_local_input)(int);
_input
If set, this function is called whenever the sysop presses a
non-sysop-function key on the local keyboard. The key pressed is
passed to the function in the single int parameter that it
accepts.
```

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

#### `od_maxtime_deduction`

```c
int od_control.od_maxtime_deduction;
```

This variable store the amount of time that should be added to the user's time upon exit of the door, as a result of the maximum time deduction, described above. If the maximum time feature is not used, this variable will be given a value of 0.

#### `od_mps`

```c
void (*od_control.od_mps)(void);
```

To make the OpenDoors Multiple Personality system available in your program, set this variable to INCLUDE_MPS before calling any OpenDoors functions. If this variable is not set, or is set to NO_MPS, the Multiple Personality System will be disabled. For more information on the OpenDoors Multiple Personality System, see page 233.

```c
od_no_         void (*od_control.od_no_file_func)();
file_func
If od_no_file_func is set to point to a function, that function
will be called whenever a door information (drop) file cannot be
located or read. This provides an easy mechanism to add your own
door information file reader, or to provide a local login prompt
when no drop file is present. If you wish the door to operate in
local mode, you should set od_control.od_force_local to TRUE
prior to returning from your function. If you have successfully
read your own door information file format, you should set
od_control.od_info_type to CUSTOM. If neither of these variables
are set by the od_no_file_function, OpenDoors will report that
it is unable to find or read a door information file and will
exit immediately.
```

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
`TRUE` or `FALSE` does not change startup output, colors, or any other behavior.
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

#### `od_page_pausing`

```c
char od_control.od_page_pausing;
```

This variable contains a Boolean value that indicates whether or not page pausing is enabled in the [`od_send_file()`](../api/od_send_file.md), [`od_hotkey_menu()`](../api/od_hotkey_menu.md) and [`od_list_files()`](../api/od_list_files.md) functions. The default value of TRUE indicates that page pausing is enabled. A value of FALSE indicates that page pausing is disabled.

#### `od_pagestartmin`

```c
int od_control.od_pagestartmin;
```

```c
int od_control.od_pageendmin;
```

These variables indicate the start and end times for sysop paging, expressed as the number of minutes past midnight. Sysop paging will be available through the [`od_page()`](../api/od_page.md) function from the start time, up to but not including the end time.

#### `od_page_statusline`

```c
char od_control.od_page_statusline;
```

This variable controls which status line, if any, is activated when the user pages the system operator (via the [`od_page()`](../api/od_page.md) function). A value between 0 and 9 causes the corresponding status line to be activated. A value of -1 prevents any change from being made to the current status line setting. This variable will normally be set by personality functions (see page 233).

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

od_time        void (*od_control.od_time_msg_func)(char *string) _msg_func This variable defaults to a value of NULL. If set to point to a function, OpenDoors will call this function INSTEAD OF displaying time limit warning messages to the user. The messages redirected to this function are:

\- Inactivity timeout warning - Inactivity timeout expired - Less than 4 minutes left today - Daily time limit expired

### Function Keys

Within OpenDoors, as with most BBS software and doors, the sysop has access to a number of function keys, which permits the sysop to carry out various functions such as entering chat mode, hanging up on the user, shelling to DOS, and so on. The variables in this section allow you to customize which keys carry out the standard sysop functions, allowing you to customize your door's interface to mimic any BBS package. By default, OpenDoors emulates the function keys used by the Remote Access BBS package, but you may choose, for example, to have your door use the key combinations used by PC-Board. In addition, OpenDoors provides an interface which allows you to add your own function keys which will be accepted by the door. This could allow you to add additional features, such as giving the sysop access to a status screen which displays information about your door.

Many of the variables in this section are unsigned ints, which represent a sysop key combination such as [ALT]-[H], [F8], or [CTRL]-[P]. These values are in the same format as is returned by the Turbo C(++) / Borland C++ bioskey() function. The high- order byte represents the scan code of the key, and the low- order byte represents the ASCII value, if any, of the key combination. Note that a complete tutorial on these key codes is beyond the scope of this manual. For more information on these key codes, you should see the documentation on the bioskey() function, which accompanies your compiler. If you wish to determine the key code which corresponds to a particular keystroke, there is a simple program, listed below, which you can compile and use. This program will simply display the key code for any key pressed, until you press the [ESCape] key. So, in order to determine the code for [SHIFT]-[F8], you would simply run this program, press the [SHIFT]-[F8] key combination on your keyboard, and record the value displayed on your screen.

```c
#include <stdio.h>
#include <bios.h>
main()
{
   int nKey;
```

```c
   do
      {
      nKey = bioskey(0);
      printf("%d (from: %x, %x)\n",
         nKey, nKey>>8, nKey&0xff);
      } while((nKey & 0xff) != 27);
}
```

BUILT IN       These variable allow you to customize the sysop function keys FUNCTION       which control functions such as hanging up on the user, shelling KEYS           to DOS, and so on. All of these variable will be assigned default values, which correspond to the same function keys used by the RemoteAccess BBS package. However, you may change the values of these variables in order to customize the key combinations which carry out these functions in your own door program. Remember that if you wish to change the value of any of these variables, you must do so after having called [`od_init()`](../api/od_init.md) or some OpenDoors function. Each of these variables contain a scan- code / ASCII-code combination representing a keystroke, as is described above. These variables are as follows:

```c
+---------------------+----------------------------------------+
| VARIABLE            | CORRESPONDING FUNCTION                 |
+---------------------+----------------------------------------+
| od_control.         | Enter sysop chat mode                  |
| key_chat            | (Normally [ALT]-[C]                    |
|                     |                                        |
| od_control.         | Invoke sysop DOS shell                 |
| key_dosshell        | (Normally [ALT]-[J]                    |
|                     |                                        |
| od_control.         | Return to the BBS without hanging up   |
| key_drop2bbs        | (Normally [ALT]-[D])                   |
|                     |                                        |
| od_control.         | Hangup on the user                     |
| key_hangup          | (Normally [ALT]-[H])                   |
|                     |                                        |
| od_control.         | Turn off the user's keyboard           |
| key_keyboardoff     | (Normally [ALT]-[K])                   |
|                     |                                        |
| od_control.         | Decreases the user's remaining time    |
| key_lesstime        | (Normally [DOWN-ARROW])                |
|                     |                                        |
| od_control.         | Lock the user out of the BBS system    |
| key_lockout         | (Normally [ALT]-[L])                   |
|                     |                                        |
| od_control.         | Increases the user's remaining time    |
| key_moretime        | (Normally [UP-ARROW])                  |
|                     |                                        |
| od_control.         | Array of eight function keys to set the|
| key_status[8]       | current status line.                   |
|                     | (Normally [F1], [F2], [F3], [F4], [F5],|
|                     |  [F6], [F9], [F10])                    |
|                     |                                        |
| od_control.         | "Sysop next" toggle key                |
| key_sysopnext       | (Normally [ALT]-[N])                   |
+---------------------+----------------------------------------+
```

CUSTOM         In addition to the sysop function keys built into OpenDoors, you FUNCTION       may wish to add your own function keys to your door. For KEYS           example, you might wish to have the [ALT]-[Z] combination display a window of information about your door, or you may wish to add your own user editor to your door, accessible through the [ALT]-[E] combination. The four variables:

```c
unsigned char od_control.od_num_keys;
unsigned int od_control.od_hot_key[16];
unsigned int od_control.od_last_hot;
void (*od_control.od_hot_function[16])(void);
```

provide your program with an interface to add your own sysop function keys (not accessible by the remote user) to the door you have written.

OpenDoors allows you to define up to sixteen custom sysop function keys. The key codes (as described at the beginning of this section) are stored in the [`od_control.od_hot_key`](#od_hot_key)[] array, and the [`od_control.od_num_keys`](#od_num_keys) variable records the number of keys which have been defined. The [`od_control.od_num_keys`](#od_num_keys) variable defaults to a value of 0. So, in order to add your own function keys, simply place the key codes for these keys in the first n elements of the [`od_control.od_hot_key`](#od_hot_key)[] array, and set the [`od_control.od_num_keys`](#od_num_keys) variable to the number of keys you have defined. OpenDoors will then watch the keyboard for any of your predefined sysop function keys being pressed. If one of these keys is pressed, OpenDoors will place the key code of the pressed key in the [`od_control.od_last_hot`](runtime.md#od_last_hot) variable. Your program will then be able to respond to one of your custom function keys being pressed by checking the value of the [`od_control.od_last_hot`](runtime.md#od_last_hot) variable. At any time this variable contains a non-zero value. If this is the case, you will then be able to determine which of your function keys has been pressed by checking the key code contained in this variable. After taking the appropriate action for the key pressed, you should be sure to reset the value of the [`od_control.od_last_hot`](runtime.md#od_last_hot) variable back to zero, which will indicate to OpenDoors that your program has received and responded to the function key which was pressed.

```c
As an alternative to testing the contents of the
od_control.od_last_hot variable, you  can also have your program
respond to custom sysop function keys by providing a callback
function in the array: void
(*od_control.od_hot_function[16])(void);
```

The Nth element in this array corresponds to the Nth element in the [`od_control.od_hot_key`](#od_hot_key) array. To use this mechanism, simply set the appropriate element of this array to point to the function that you wish to have OpenDoors call when the sysop presses the corresponding function key. For instance, assume that the following function is included in your program's source code:

```c
void addPoints(void)
{
   /* add ten points to the user's score */
   currentUser->points += 10;
}
```

If you wanted to have this function called when the sysop presses the [Page Up] key, you could do the following:

```c
/* get number of new sysop function key, and increment */
/* total number of keys */
int new_key = od_control.od_num_keys++;
```

```c
/* Set next sysop hotkey to Page Up */
od_control.od_hot_key[new_key] = 0x4900;
```

```c
/* Set corresponding function to addPoints() */
od_control.od_hot_function[new_key] = addPoints;
```

### Color Customization

These variables allow you to customize the color of text displayed by OpenDoors. Each of these variables are assigned color attributes, in the format used by [`od_set_attrib()`](../api/od_set_attrib.md) (described on page 128). These variables are as follows:

```c
+---------------------+----------------------------------------+
| VARIABLE            | WHERE COLOR IS USED                    |
+---------------------+----------------------------------------+
| od_control.         | Text typed by the sysop and user in    |
| od_chat_color1 & 2  | chat mode.                             |
|                     |                                        |
| od_control.         | File description fields in FILES.BBS   |
| od_list_comment_col | listings                               |
|                     |                                        |
| od_control.         | Color of page pausing prompt that is   |
| od_continue_col     | displayed at the end of each page      |
|                     |                                        |
| od_control.         | Filename fields in FILES.BBS listings  |
| od_list_name_col    |                                        |
|                     |                                        |
| od_control.         | "Missing" string in FILES.BBS listings |
| od_list_offline_col |                                        |
|                     |                                        |
| od_control.         | File size fields in FILES.BBS listings |
| od_list_size_col    |                                        |
|                     |                                        |
| od_control.         | Title fields in FILES.BBS listings     |
| od_list_title_col   |                                        |
|                     |                                        |
| od_control.         | Color of the window title as displayed |
| od_menu_title_col   | by od_popup_menu()                     |
|                     |                                        |
| od_control.         | Color of the window border as          |
| od_menu_border_col  | displayed by od_popup_menu()           |
|                     |                                        |
| od_control.         | Color of the normal text displayed     |
| od_menu_text_col    | by od_popup_menu()                     |
|                     |                                        |
| od_control.         | Color of the shortcut keys displayed   |
| od_menu_key_col     | by od_popup_menu()                     |
|                     |                                        |
| od_control.         | Color of the selection bar as          |
| od_menu_highlight_  | displayed by od_popup_menu()           |
| col                 |                                        |
|                     |                                        |
| od_control.         | Color of the shortcut keys displayed   |
| od_menu_highkey_col | on the selected line by od_popup_menu()|
+---------------------+----------------------------------------+
```

### Text Customization

In addition to the other aspects of OpenDoors which may be customized by use of the OpenDoors control structure, all of the text displayed by OpenDoors may also be customized. This may be done either to create doors with OpenDoors that use languages other than English, or to simply give your doors a "personal touch". The variables described in this section allow you to define what text you want to have displayed by OpenDoors at any time. All of these variables are pointers to strings, and are set to default values in the [`od_init()`](../api/od_init.md) function. Thus, if you wish to change the string pointed to by any of these variables, you must do so after [`od_init()`](../api/od_init.md) or some OpenDoors API function has been called. To set any of these variables, you can simply set them to point to a string-constant in your program. For example, to set the text displayed by OpenDoors prior to a DOS shell, you could:

```c
od_control.od_before_shell=(char *)"\n\rJust a moment...\n\r";
```

The chart below lists each of the text customization variables (without the "od_control." prefix, for the sake of brevity), along with their default strings.

Note that some of these strings MUST always be the same length as their default string. You may not display longer text within these strings, and if you wish to display shorter text, you must pad the remaining space in the string with spaces, in order to preserve its length. Those string which must be of fixed length also have their length listed in the chart below. Any strings which have an asterisk (*) in their length column may be any length.

Also keep in mind that any string with "printf-style" formatting sequences, such as "%s", must retain the same sequences in the same order.

In addition, four of these pointers - od_after_chat, od_after_shell, od_before_chat and od_before_shell - can be set to a value of NULL. In this case, OpenDoors will not display any string where this variable's string is normally displayed.

```c
+-----------------------+-----+----------------------------------------------+
| VARIABLE NAME         | LEN | DEFAULT VALUE                                |
+-----------------------+-----+----------------------------------------------+
| od_after_chat         |  *  | "\n\rChat mode ended...\n\r\n\r"             |
|                       |     |                                              |
| od_after_shell        |  *  | "\n\r...Thanks for waiting\n\r\n\r"          |
|                       |     |                                              |
| od_before_chat        |  *  | "\n\rSysop breaking in for chat...\n\r\n\r"  |
|                       |     |                                              |
| od_before_shell       |  *  | "\n\rPlease wait a moment...\n\r"            |
|                       |     |                                              |
| od_chat_reason        |  *  | "                          Why would you "   |
|                       |     | "like to chat?\n\r"                          |
|                       |     |                                              |
| od_continue           |  *  | "Continue? [Y/n/=]"                          |
|                       |     |                                              |
| od_continue_no        | char| 'N'                                          |
|                       |     |                                              |
| od_continue_nonstop   | char| '='                                          |
|                       |     |                                              |
| od_continue_yes       | char| 'Y'                                          |
|                       |     |                                              |
| od_day[0]             |  3  | "Sun"                                        |
|                       |     |                                              |
| od_day[1]             |  3  | "Mon"                                        |
|                       |     |                                              |
| od_day[2]             |  3  | "Tue"                                        |
|                       |     |                                              |
| od_day[3]             |  3  | "Wed"                                        |
|                       |     |                                              |
| od_day[4]             |  3  | "Thu"                                        |
|                       |     |                                              |
| od_day[5]             |  3  | "Fri"                                        |
|                       |     |                                              |
| od_day[6]             |  3  | "Sat"                                        |
|                       |     |                                              |
| od_hanging_up         |  *  | "Terminating Call"                           |
|                       |     |                                              |
| od_help_text          |  80 | "  Alt: [C]hat [H]angup [L]ockout [J]Dos "   |
|                       |     | "[K]eyboard-Off [D]rop to BBS            "   |
|                       |     |                                              |
| od_help_text2         |  79 | "  OpenDoors 6.00 - (C)Copyright 1992, "     |
|                       |     | "Brian Pirie - Registered Version         "  |
|                       |     |                                              |
| od_inactivity_timeout |  *  | "User sleeping at keyboard, inactivity "     |
|                       |     | "timeout...\n\r\n\r"                         |
|                       |     |                                              |
| od_inactivity_warning |  *  | "Warning, only %d minute(s) remaining "      |
|                       |     | "today...\n\r\n\r"                           |
|                       |     |                                              |
| od_month[0]           |  3  | "Jan"                                        |
|                       |     |                                              |
| od_month[1]           |  3  | "Feb"                                        |
|                       |     |                                              |
| od_month[2]           |  3  | "Mar"                                        |
|                       |     |                                              |
| od_month[3]           |  3  | "Apr"                                        |
|                       |     |                                              |
| od_month[4]           |  3  | "May"                                        |
|                       |     |                                              |
| od_month[5]           |  3  | "Jun"                                        |
|                       |     |                                              |
| od_month[6]           |  3  | "Jul"                                        |
|                       |     |                                              |
| od_month[7]           |  3  | "Aug"                                        |
|                       |     |                                              |
| od_month[8]           |  3  | "Sep"                                        |
|                       |     |                                              |
| od_month[9]           |  3  | "Oct"                                        |
|                       |     |                                              |
| od_month[10]          |  3  | "Nov"                                        |
|                       |     |                                              |
| od_month[11]          |  3  | "Dec"                                        |
|                       |     |                                              |
| od_no_keyboard        |  10 | "[Keyboard]"                                 |
|                       |     |                                              |
| od_no_sysop           |  *  | "\n\rI'm afraid the sysop is not available " |
|                       |     | "at this time.\n\r"                          |
|                       |     |                                              |
| od_no_response        |  *  | " No response.\n\r\n\r"                      |
|                       |     |                                              |
| od_no_time            |  *  | "Sorry, you have used up your time for "     |
|                       |     | "today...\n\r\n\r"                           |
|                       |     |                                              |
| od_offline            |  10 | "[OFFLINE] "                                 |
|                       |     |                                              |
| od_paging             |  *  | "\n\rPaging Sysop for Chat"                  |
|                       |     |                                              |
| od_press_key          |  *  | "Press [Enter] to continue..."               |
|                       |     |                                              |
| od_sending_rip        |  *  | "\xb4 Sending RIP File \xc3"                 |
|                       |     |                                              |
| od_status_line[0]     |  80 | "                                        "   |
|                       |     | "                             [Node:     "   |
|                       |     |                                              |
| od_status_line[1]     |  *  | "%s of %s at %u BPS"                         |
|                       |     |                                              |
| od_status_line[2]     |  79 | "Security:        Time:                  "   |
|                       |     | "                             [F9]=Help "    |
|                       |     |                                              |
| od_sysop_next         |  5  | "[SN] "                                      |
|                       |     |                                              |
| od_time_left          |  10 | "%d mins   "                                 |
|                       |     |                                              |
| od_time_warning       |  *  | "Warning, only %d minute(s) remaining tod"   |
|                       |     | "ay...\n\r\n\r"                              |
|                       |     |                                              |
| od_want_chat          |  11 | "[Want-Chat]"                                |
+-----------------------+-----+----------------------------------------------+
```
