# Runtime state and policy

These members control or report the active session.

| Members | Description |
| --- | --- |
| `od_always_clear`, `od_clear_on_exit` | Screen-clearing policy. |
| `od_force_local`, `od_silent_mode` | Local-session and local-display behavior. |
| `od_chat_active`, `od_current_statusline`, `od_status_on` | Current chat and status-display state. |
| `od_error` | Most recent [`ERR_*`](../constants/errors.md) API error. |
| `od_last_input`, `od_last_hot` | Most recent ordinary and custom hotkey input. |
| `od_maxtime`, `od_maxtime_deduction` | Door-specific time cap and adjustment. |
| `od_okaytopage`, `od_pagestartmin`, `od_pageendmin` | Paging availability and hours. |
| `od_page_pausing`, `od_page_len`, `od_page_statusline` | Display-file paging behavior. |
| `od_user_keyboard_on` | Whether local operator keystrokes enter the door input queue. |
| `od_update_status_now`, `od_cur_attrib` | Pending status refresh and current text attribute. |
| `od_disable`, `od_disable_dtr`, `od_disable_inactivity` | Masks or conditions which disable automatic behaviors. |
| `od_inactivity`, `od_inactive_warning` | Inactivity timeout and warning interval. |
| `od_max_key_latency` | Retained compatibility member; the current input implementation does not read it. |
| `od_noexit`, `od_nocopyright`, `od_internal_debug` | Exit, startup display, and diagnostic controls. |
| `od_spawn_freeze_time`, `od_swapping_disable`, `od_swapping_noems`, `od_swapping_path` | Child-process and legacy DOS swapping policy. |
| `event_*` | Pending BBS event status, schedule, days, and exit level. |
| `od_errorlevel` | Exit levels indexed by the [`ERRORLEVEL_*`](../constants/errors.md) constants. |
| `od_logfile_disable`, `od_logfile_name` | Runtime log enablement and selected log filename. |
| `od_emu_simulate_modem` | Enables modem-style emulation behavior where supported. |
| `od_in_buf_size` | Requested communications input-buffer size. |
| `od_list_pause`, `od_list_stop` | Keys used to pause and stop file listings. |
| `event_status`, `event_starttime`, `event_errorlevel` | Current BBS event state, time, and exit level. |
| `event_days`, `event_force`, `event_last_run` | Event schedule, force flag, and last-run date. |

Many runtime members may be adjusted after initialization. Members used to
construct components, buffers, or communication state must be set beforehand.

## Compatibility-only runtime member

### `od_max_key_latency`

```c
tODMilliSec od_control.od_max_key_latency;
```

This member is retained in `tODControl` for source and binary compatibility,
but the current implementation never reads it. Assigning a value does not
change terminal key-sequence timing. `ODGetIn.c` currently waits the fixed
internal value `MAX_CHARACTER_LATENCY`, 250 milliseconds, between bytes while
resolving a remote control sequence.

The field has no effective default or supported operational range in this
version. Use the timeout parameter to
[`od_get_input()`](../api/od_get_input.md) to limit how long that call waits for
its first input event; that timeout is separate from the fixed inter-byte wait.

## Detailed reference

### Door Settings

This section deals with those variables in the OpenDoors control structure which reflect the current door settings. These variables are as follows:

od_cur_attrib            The current display attribute, or -1 if unknown.

od_okaytopage            Controls whether the user is currently permitted to page the sysop.

od_pageendmin            End of valid paging hours.

od_pagestartmin          Start of valid paging hours.

od_silent_mode           Turns off local user interface.

od_user_keyboard_on      Controls whether OpenDoors will currently accept input from the remote user's keyboard.

od_update_status_now     Forces immediate update of the status line.

sysop_next               Indicates whether or not the sysop has reserved use of the system after the current calls.

#### `od_cur_attrib`

```c
int od_control.od_cur_attrib;
```

This read-only values stores the current display color attribute, or the value -1 if the current display color is unknown (such as when the door first begins execution).

#### `od_okaytopage`

```c
char od_control.od_okaytopage;
```

This variable allows you to control whether or not the user is currently permitted to page the sysop via the [`od_page()`](../api/od_page.md) function. A value of PAGE_ENABLE indicates that paging is currently permitted, regardless of the sysop page hours setting. A value of PAGE_DISABLE indicates that paging is not current permitted. A value of PAGE_USE_HOURS indicates that the [`od_page()`](../api/od_page.md) function should check the values of the

od_pagestartmin and od_pageendmin variables in order to determine whether or not paging should be permitted. The od_okaytopage variable should only be set after you call [`od_init()`](../api/od_init.md) or some other OpenDoors function. The default value is PAGE_USE_HOURS. For more information on the [`od_page()`](../api/od_page.md) function itself, see page 101.

#### `od_pageendmin`

```c
unsigned int od_control.od_pageendmin;
```

This variable can be used to set the beginning of valid sysop paging hours within the [`od_page()`](../api/od_page.md) function. If the [`od_control.od_okaytopage`](#od_okaytopage) variable (which is described above) is set to MAYBE, then OpenDoors will check the value of this variable prior to paging the sysop via the [`od_page()`](../api/od_page.md) function. This variable should contain the time at which the valid sysop paging hours end, represented as the a number of minutes since midnight. For more information on the [`od_page()`](../api/od_page.md) function itself, see page 101.

#### `od_pagestartmin`

```c
unsigned int od_control.od_pagestartmin;
```

This variable can be used to set the beginning of valid sysop paging hours within the [`od_page()`](../api/od_page.md) function. If the [`od_control.od_okaytopage`](#od_okaytopage) variable (which is described above) is set to MAYBE, then OpenDoors will check the value of this variable prior to paging the sysop via the [`od_page()`](../api/od_page.md) function. This variable should contain the time at which the valid sysop paging hours begin, represented as the a number of minutes since midnight. For more information on the [`od_page()`](../api/od_page.md) function itself, see page 101.

#### `od_silent_mode`

```c
BOOL od_control.od_silent_mode;
```

If this variable is set to TRUE prior to the first call to any OpenDoors function, OpenDoors will operate in silent mode, where the local display and sysop commands are not used. Silent mode is automatically disabled if the program is running in local mode.

#### `od_update_status_now`

```c
char od_control.od_update_status_now;
```

Setting this variable to TRUE forces OpenDoors to update the status line during the next [`od_kernel()`](../api/od_kernel.md) execution. When the status line is updated, this variable is reset to its default value of FALSE.

#### `od_user_keyboard_on`

```c
char od_control.od_user_keyboard_on;
```

This variable is a Boolean value, indicating whether OpenDoors will currently accept input from a remote user. OpenDoors provides a function key (usually [ALT]-[K], unless you have changed the default), which will allow the sysop to temporarily prevent the user from having any control over the door. When the sysop activates this feature, a flashing [Keyboard-Off] indicator will appear on the status line, and this variable will be set to FALSE. When the sysop presses the [ALT]-[K] combination a second time, to toggle the user's keyboard back on, the flashing indicator will disappear, and this variable will be set back to TRUE.

#### `sysop_next`

```c
char od_control.sysop_next;
```

This variable is a Boolean value, indicating whether or not the "sysop next" feature has been activated. The "sysop next" feature, which reserves the system for the sysop after the call has ended, can be toggled on and off within OpenDoors by use of a function key (Alt-N by default). Also, when the "sysop next" feature has been activated, an indicator will appear on the OpenDoors status line. This variable is only available under systems that produce an SFDOORS.DAT or RA 1.00 and later style extended EXITINFO.BBS door information file. For more information on testing the type of door information file available, please see page 158.

### Diagnostics

To help in diagnosing problems in your OpenDoors programs, OpenDoors stores information on the most recent error which occurred. When any of the OpenDoors functions return an "error" or "failure" state, the reason for this failure is recorded.

The following OpenDoors control structure variable provides diagnostics information:

od_error                 Stores a "reason code" for the last failed OpenDoors API function call.

#### `od_error`

```c
int od_control.od_error;
```

When any of the OpenDoors API functions return an "error" or "failure" state (usually denoted by either of the values FALSE or NULL), the reason for the failure is recorded in this variable. Since successful function calls do not alter the value of the [`od_control.od_error`](#od_error) variable, you must be careful not only to check the value of the [`od_control.od_error`](#od_error) variable, but also to check the OpenDoors function return codes, in order to determine which function failed.

This variable will always store the reason for the most recent function call failure, or ERR_NONE if no functions have failed. od_error may take on any of the following values:

ERR_NONE            Indicates that no error has occurred yet.

ERR_MEMORY          Function was unable to allocate required memory. This usually indicates that there is not enough available memory. This failure may also be due to memory corruption caused by your program inadvertently overwriting heap structures. If your program has been compiled in either the small or the medium memory model, try recompiling it in the compact, large, or huge memory models. If your program is already compiled in the compact, large, or huge memory models, try making more system memory available to your program.

ERR_NOGRAPHICS      This setting indicates that the function called requires ANSI, AVATAR or RIP graphics mode, but none of these modes are active.

ERR_PARAMETER       An invalid parameter was passed to an OpenDoors functions. Check the function's description in chapter four, to determine the required values for each function parameter.

ERR_FILEOPEN        OpenDoors was unable to open a file. This can be due to the specified filename not existing, due to the file being locked for exclusive access by another process, or due to a hardware failure.

ERR_FILEREAD        OpenDoors was able to open the specified file, but unable to read the required data from the file. This error may be due to an invalid file format, due to a portion of the file being locked by another process, or due to a hardware failure.

ERR_LIMIT           An internal function limit has been exceeded. Refer to the function's description in chapter four for information on the function's limitations.

ERR_NOREMOTE        Indicates that a function has been called which is not valid in local mode, such as [`od_carrier()`](../api/od_carrier.md) or [`od_set_dtr()`](../api/od_set_dtr.md).
