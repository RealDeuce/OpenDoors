# Caller and system information

OpenDoors normalizes several door-information formats into a common set of
fields. String arrays are fixed-size C buffers and are always subject to the
capacity declared in [`OpenDoor.h`](../api/index.md).

## System and caller identity

| Members | Description |
| --- | --- |
| `system_name`, `sysop_name` | BBS and sysop names. |
| `system_calls`, `system_last_caller`, `system_last_handle` | System call statistics and previous caller identity. |
| `user_name`, `user_handle`, `user_location` | Current caller identity. |
| `user_password`, `user_logonpassword` | Password fields supplied by formats which expose them; do not log them. |
| `user_dataphone`, `user_homephone` | Legacy telephone fields. |
| `user_org`, `user_address`, `user_callsign` | Organization and address information when available. |
| `user_comment`, `user_reasonforchat` | BBS-supplied user text. |

## Access, usage, and messages

`user_security`, `user_flags`, `user_attribute`, `user_attrib2`, and
`user_attrib3` describe access. `user_num`, `user_group`, `user_msg_group`,
`user_file_group`, `user_msg_area`, and `user_file_area` identify the caller's
record and current areas.

Call and transfer accounting is available through `user_numcalls`,
`user_uploads`, `user_downloads`, `user_upk`, `user_downk`, `user_todayk`,
`user_net_credit`, `user_pending`, `user_messages`, `user_lastread`,
`user_credit`, `user_downlimit`, `user_menu_cost`, and related fields. Their
units follow the originating BBS format; portable doors should avoid assuming
more precision than the declared type provides.

## Time and terminal

`user_lasttime`, `user_lastdate`, `user_firstcall`, `user_birthday`,
`user_subdate`, and the login date/time fields use the text formats supplied by
the BBS. Time limits and usage are represented by `user_timelimit`,
`user_time_used`, `user_deducted_time`, and `user_loginsec`.

`user_screenwidth` and `user_screen_length` establish the remote screen.
`user_ansi`, `user_avatar`, `user_rip`, and `user_rip_ver` report graphics
capabilities. `user_language`, `user_date_format`, `user_expert`,
`user_error_free`, and the EMSI fields describe other caller preferences and
transport capabilities.

Fields not supplied by the selected door-information format retain their
OpenDoors defaults. Test capability fields rather than inferring them from the
format name.

## Complete field inventory

The following less commonly used fields are retained because one or more door
formats can supply them:

| Members | Description |
| --- | --- |
| `timelog_start_date`, `timelog_busyperhour`, `timelog_busyperday` | BBS traffic-log starting date and hourly/daily statistics. |
| `user_last_pwdchange`, `user_pwd_crc`, `user_logon_pwd_crc` | Password-change and format-specific password checksum data. |
| `user_netmailentered`, `user_echomailentered` | Network and echomail entry counts. |
| `user_logintime`, `user_logindate`, `user_timeofcreation` | Login and account-creation text timestamps. |
| `user_readthru`, `user_numpages`, `user_wantchat` | Format-specific read, page, and chat status. |
| `user_menustack`, `user_menustackpointer` | BBS menu return stack. |
| `user_combinedrecord`, `user_xi_record` | Raw format-specific extension records. |
| `user_forward_to`, [`sysop_next`](runtime.md#sysop_next) | Message forwarding and sysop-next flags. |
| `user_emsi_session`, `user_emsi_crtdef`, `user_emsi_protocols` | EMSI session and terminal/protocol descriptions. |
| `user_emsi_capabilities`, `user_emsi_requests`, `user_emsi_software` | Remaining EMSI negotiation strings. |
| `user_hold_attr1`, `user_hold_attr2`, `user_hold_len` | Format-specific held-message attributes. |
| `user_protocol` | Preferred transfer-protocol character. |
| `user_last_birthday_check`, `user_sex`, `user_last_cost_menu` | Additional BBS user-record fields. |

These members preserve the source format's representation. Consult the BBS
format specification before assigning them a meaning not stated above.


## Detailed reference

### Bbs And Caller Information

As we have already described, there are two types of variables in the OpenDoors control structure. Some of the variables are simply used to allow you to customize OpenDoor's various features, such as altering colors, prompts, timeouts, etc. Other variables in the OpenDoors control structure serve to provide you with information about the user who is online and the BBS system your door is running under. This section deals with those variables that provide you with information about the BBS and the user.

The information in these variables is read from the door information file, a small file created by the BBS specifically for the purpose of communicating with door programs. Depending on what BBS system your door is running under, the type of door information file will vary. Since different door information files do not all provide the same pieces of information, some variables in this section will only be available when your door is running under particular BBS systems.  Other variables will be available with many or all BBS systems. In the description of each variable in this section, we indicate under which door information files the particular variable will be . So, if you wish to access a variable that is only under certain door information files, your program should test whether or not the required information is available under the particular door information file that was found. In order to determine which door information file your door is running under, you should use the [`od_control.od_info_type`](connection.md#od_info_type) variable. This variable is described in the section which begins on page 150. If you test the value of the [`od_control.od_info_type`](connection.md#od_info_type) variable, and find that the required information is not available, you may wish to simply use some sort of default value for the variable, or alternatively, not allow your door to run under certain BBS systems. Another possibility, if the required information is not available, is imply to obtain this information from the user yourself. For example, if you wished to know the length of the user's screen, when this information is not available from the door information file, you could simply prompt the user for their screen length the first time they use your door. This information could then be stored in your door's data files for future reference.

As an example of testing what door information file your door is running under, consider the case where you wanted to display the user's birthday. The example below will display the user's birthday if it is known, and otherwise, print the string "unknown".

```c
if(od_control.od_info_type == RA1EXITINFO
   od_control.od_info_type == RA2EXITINFO)
{
   od_disp_str(od_control.user_birthday);
}
else
{
   od_disp_str("Unknown");
}
```

The chart below lists the door information file formats that OpenDoors recognizes, along with example BBS systems that produce these files and a reference letter for each type. Thus, an OpenDoors door can run DIRECTLY under ANY BBS SYSTEM that produces one of these files formats, and under ANY OTHER BBS system when used in conjunction with a door information file conversion utility.

```c
+--------------------------+----------------------------------------+
| FILE FORMAT              | EXAMPLE BBS SYSTEMS                    |
+--------------------------+----------------------------------------+
| CHAIN.TXT                | WWIV                                   |
+--------------------------+----------------------------------------+
| DORINFO1.DEF             | RBBS-PC                                |
+--------------------------+----------------------------------------+
| DORINFO1.DEF             | QuickBBS                               |
|      &                   | Remote Access (versions 0.01-0.04)     |
| EXITINFO.BBS (Std. Ver.) |                                        |
+--------------------------+----------------------------------------+
| DOOR.SYS (DoorWay Style) | Remote Access                          |
+--------------------------+----------------------------------------+
| DOOR.SYS (PCB/GAP Style) | PC-Board                               |
|                          | GAP                                    |
+--------------------------+----------------------------------------+
| DOOR.SYS (WildCat Style) | Wildcat 3.00 and above                 |
|                          | Telegard                               |
+--------------------------+----------------------------------------+
| SFDOORS.DAT              | Spitfire                               |
|                          | TriBBS                                 |
+--------------------------+----------------------------------------+
| CALLINFO.BBS             | WildCat 2.xx                           |
+--------------------------+----------------------------------------+
| DORINFO1.DEF             | Remote Access (versions 1.00 and later)|
|      &                   |                                        |
| EXITINFO.BBS (Ext. Ver.) |                                        |
+--------------------------+----------------------------------------+
```

The chart on the following page lists all of the OpenDoors control structure variables in this section, along with a brief description of their use. The variables are then described in detail, below.

```c
+-----------------------+-----------------------------------------------+
| VARIABLE NAME         | VARIABLE CONTENTS                             |
+-----------------------+-----------------------------------------------+
| EMSI INFORMATION      | Information on current IEMSI session          |
| event_status          | The status of the next system event           |
| event_starttime       | The start time of the next system event       |
| event_errorlevel      | The errorlevel of the next system event       |
| event_days            | The days of the week to execute the event     |
| event_force           | Whether the next system event is forced       |
| event_last_run        | When the next system event was last run       |
| sysop_name            | The name of the BBS's sysop                   |
| system_calls          | Total number of calls BBS has received        |
| system_last_caller    | The name of the last caller to the BBS        |
| system_last_handle    | The handle (alias) of the last caller         |
| system_name           | The name of the BBS                           |
| TIMELOG VARIABLES     | The times at which the BBS has been most busy |
| user_ansi             | Whether the user has ANSI graphics mode on    |
| user_attribute        | User attribute bit-mapped flags               |
| user_attrib2          | Second set of user attribute bit-mapped flags |
| user_attrib3          | Third set of user attribute flags             |
| user_avatar           | Whether the user has AVATAR graphics mode on  |
| user_birthday         | The date the user was born                    |
| user_callsign         | The user's amateur radio call sign            |
| user_combinedrecord   | The user's combined message areas settings    |
| user_comment          | Sysop's comment about the user                |
| user_credit           | Amount of NetMail credit the user has         |
| user_dataphone        | The user's data phone number                  |
| user_date_format      | Format user wishes to have dates displayed in |
| user_deducted_time    | Total time that has been subtracted from user |
| user_downk            | Total Kilobytes downloaded by the user        |
| user_downlimit        | User's daily download limit                   |
| user_downloads        | Total number of files downloaded by the user  |
| user_echomailentered  | Whether or not the user has entered EchoMail  |
| user_error_free       | Whether or not connection is error-free       |
| user_file_area        | The user's current file area                  |
| user_firstcall        | Date of the user's first call to the BBS      |
| user_flags            | User's sysop-defined flag settings            |
| user_forward_to       | Name to forward user's mail to                |
| user_group            | User's group number                           |
| user_handle           | User's alias                                  |
| user_homephone        | User's home telephone number                  |
| user_language         | User's language setting                       |
| user_last_pwdchange   | Total calls since last password change        |
| user_lastdate         | Date of the user's last call                  |
| user_lastread         | Highest message number read by user           |
| user_lasttime         | Time of the user's last call                  |
| user_location         | Name of the city where the user lives         |
| user_logindate        | Date on which the current call began          |
+-----------------------+-----------------------------------------------+
```

```c
+-----------------------+-----------------------------------------------+
| VARIABLE NAME         | VARIABLE CONTENTS                             |
+-----------------------+-----------------------------------------------+
| user_loginsec         | User's security at the beginning of this call |
| user_logintime        | Time at which the current call began          |
| user_logonpassword    | User's password at the beginning of this call |
| user_menustack        | Contents of the user's current menu stack     |
| user_menustackpointer | Pointer to the top of the menu stack          |
| user_messages         | Total number of messages written by the user  |
| user_msg_area         | The user's current message area               |
| user_name             | The user's name                               |
| user_net_credit       | The user's remaining netmail credit           |
| user_netmailentered   | Whether or not the user has entered NetMail   |
| user_num              | The user's record number in the user file     |
| user_numcalls         | Number of calls the user has made to the BBS  |
| user_numpages         | Number of times the user has paged the sysop  |
| user_password         | The user's current password                   |
| user_pending          | The value of unsent NetMail written by user   |
| user_reasonforchat    | The reason the user wishes to chat with sysop |
| user_rip_ver          | RIP protocol version being used               |
| user_screen_length    | The length of the user's screen               |
| user_screenwidth      | The width of the user's screen                |
| user_security         | The user's security access level              |
| user_sex              | The user's gender                             |
| user_subdate          | The date the user's subscription expires      |
| user_timelimit        | The user's daily time limit                   |
| user_todayk           | Kilobytes downloaded by the user today        |
| user_upk              | Total Kilobytes uploaded by the user          |
| user_uploads          | Total number of files uploaded by the user    |
| user_wantchat         | Whether or not the user wishes to chat        |
| user_xi_record        | The user's record in the USERSXI.BBS file     |
+-----------------------+-----------------------------------------------+
```

```c
EMSI           char od_control.ra_emsi_session;
INFORMATION    char od_control.ra_emsi_crtdef[41];
char od_control.ra_emsi_protocols[41];
char od_control.ra_emsi_capabilities[41];
char od_control.ra_emsi_requests[41];
char od_control.ra_emsi_software[41];
char od_control.ra_hold_attr1;
char od_control.ra_hold_attr2;
char od_control.ra_hold_len;
```

These variables provide your door with information pertaining to an interactive EMSI session that has been established. Note that these variables are only available under systems that produce an RA 1.00 and later style extended EXITINFO.BBS door information file.

If an IEMSI session has been established, the Boolean variable od_control.ra_emsi_session will be TRUE, and if no session has not been established, this variable will be FALSE.

A full discussion of the IEMSI protocol is beyond the scope of this manual. Specifications for the IEMSI protocol are available from the OpenDoors support BBS.

#### `event_days`

```c
unsigned char od_control.event_days;
```

This variable is a bit-mapped flag of the days of the week on which the next system event is run. The bit-map bits are as follows:

```c
+-----+------+-----------+
| BIT | MASK | MEANING   |
+-----+------+-----------+
|  0  | 0x01 | Sunday    |
|  1  | 0x02 | Monday    |
|  2  | 0x04 | Tuesday   |
|  3  | 0x08 | Wednesday |
|  4  | 0x10 | Thursday  |
|  5  | 0x20 | Friday    |
|  6  | 0x40 | Saturday  |
|  7  | 0x80 | All Days  |
+-----+------+-----------+
```

For more information on bit-mapped flags, see the glossary item entitled "BIT-MAPPED FLAGS".

This variable is only available under systems that produce an EXITINFO.BBS door information file.

#### `event_errorlevel`

```c
unsigned char od_control.event_errorlevel;
```

This variable contains the ErrorLevel associated with the next system event. This variable is only available under systems that produce an EXITINFO.BBS door information file.

#### `event_force`

```c
char od_control.event_force;
```

This variable indicates whether the next system event should be forced to run at a particular time. If this variable contains a value of TRUE, then the user should be forced off-line in order to accommodate the event, and if this variable is false, then the event can wait until after the user logs off normally. This variable is only available under systems that produce an EXITINFO.BBS file.

#### `event_last_run`

```c
char od_control.event_last_run[9];
```

This variable contains a string representing the date on which the next system event was last run, and is in the same format as the user_lastdate variable. This variable is only available under systems that produce an EXITINFO.BBS file.

#### `event_starttime`

```c
char od_control.event_starttime[6];
```

This variable contains a string representing the time at which the next system event is scheduled to start, in the same format as the user_lasttime variable. This variable is only available under systems that produce an EXITINFO.BBS or Wildcat style DOOR.SYS door information file.

#### `event_status`

```c
unsigned char od_control.event_status;
```

This variable represents the status of the next system event, and will be equal to the value

```c
ES_ENABLED
```

if and only if the other event information contained in the control structure is valid. This variable is only available under systems that produce an EXITINFO.BBS file.

#### `sysop_name`

```c
char od_control.sysop_name[40];
```

The [`od_control.sysop_name`](#sysop_name) variable contains the name of the sysop of the BBS under which your door is running. This variable is available under any BBS system that produces a DORINFO?.DEF (including RA & QBBS which process both DORINFO1.DEF and EXITINFO.BBS files), or Wildcat style DOOR.SYS file.

#### `system_calls`

```c
long od_control.system_calls;
```

This variable contains the total number of calls that have been placed to the BBS, and is available under any BBS which produces an EXITINFO.BBS file.

#### `system_last_caller`

```c
char od_control.system_last_caller[36];
```

This string contains the name of the previous caller to the BBS, on any line, and is available under EXITINFO.BBS.

#### `system_last_handle`

```c
char od_control.system_last_handle[36];
```

This string contains the handle (alias) of the previous caller to the BBS, on any line, and is available under EXITINFO.BBS.

#### `system_name`

```c
char od_control.system_name[40];
```

The [`od_control.system_name`](#system_name) variable contains the name of the BBS under which your door is running. This variable is available under any BBS system that produces a DORINFO?.DEF (including RA & QBBS which process both DORINFO1.DEF and EXITINFO.BBS files).

#### `timelog_start_date`

```c
char od_control.timelog_start_date[9];
```

```c
VARIABLES
This string contains the date of the beginning of the time
period for which the time log is recorded. This variable is
available under any system that produces an EXITINFO.BBS file.
int od_control.timelog_busyperhour[24];
```

This variable is an array of 24 elements, with each element indicating the total number of times the BBS was in use during each of the 24 hours of the day. Element 0 corresponds to the time period of 0:00-1:00, element 1 corresponds to the time period of 1:00-2:00, and so on. In order to determine the frequency of system use during any hour as a percentage, simply calculate the total of all 24 entries in the array, and divide any given entry by the total, in order to come up with an average. This variable is available under any system that produces an EXITINFO.BBS file.

```c
int od_control.timelog_busyperday[7];
```

This variable is an array of 7 elements, with each element indicating the total number of times the BBS was in use during each of the 7 days of the week. Here, elements 0 corresponds to Sunday, element 1 to Monday, and so on. In order to calculate the frequency of system use during any day of the week, use the same method as for calculating the frequency of calls during each hour, as described above. This is only available under systems that produces an EXITINFO.BBS file. Note that at least some, if not all, versions of RemoteAccess do not maintain this variable correctly, and thus even with the presence of an EXITINFO.BBS file, this array may contain all zero entries.

#### `user_ansi`

```c
char od_control.user_ansi;
```

This variable contains a Boolean value, indicating whether or not the user has ANSI mode turned on. If ANSI graphics mode is enabled, this variable will contain a value of TRUE, and if ANSI graphics mode is disabled, this variable will contain a value of FALSE. Many of the OpenDoors functions test the setting of this variable in order to determine whether or not they should send ANSI-graphics control characters. Also, if this variable contains a TRUE value, OpenDoors will display an "[ANSI]" indicator on the status line.

You may change the value of this variable at any time after the first call to [`od_init()`](../api/od_init.md) or any other OpenDoors functions. Depending upon what BBS system your door is running under, changes to this variable may or may not result in changes to the user's ANSI setting upon return to the BBS.

This variable is available under all door information file formats.

#### `user_attribute`

```c
unsigned char od_control.user_attribute;
```

This variable is a bitmap of eight flags, each of which represent individual pieces of information pertaining to the user that is currently online. These flags are as follows:

```c
+-----+------+-----------------------+
| BIT | MASK | DESCRIPTION           |
+-----+------+-----------------------+
|  0  | 0x01 | Is the user deleted   |
|  1  | 0x02 | Is screen clearing on |
|  2  | 0x04 | Is "more" prompt on   |
|  3  | 0x08 | Is ANSI mode on       |
|  4  | 0x10 | User no-kill setting  |
|  5  | 0x20 | Transfer-priority     |
|  6  | 0x40 | Full screen editor    |
|  7  | 0x80 | Quiet mode            |
+-----+------+-----------------------+
```

For more information on using and setting bit-mapped flags, please see the entry entitled "BITMAPED FLAGS" in the glossary of this manual.

Note that this variable is only available under systems that produce and EXITINFO.BBS format door information file.

#### `user_attrib2`

```c
unsigned char od_control.user_attrib2;
```

attrib2 See the user_attrib variable for more information. This variable is like the user_attrib variable, except that it contains different information. The bit-mapped flags for the [`od_control.user_attrib2`](#user_attrib2) variable are as follows:

```c
+-----+------+-----------------------+
| BIT | MASK | DESCRIPTION           |
+-----+------+-----------------------+
|  0  | 0x01 | User hot-keys setting |
|  1  | 0x02 | Is AVATAR graphics on |
|  2  | 0x04 | Full screen reader    |
|  3  | 0x08 | Hidden from userlist  |
+-----+------+-----------------------+
```

Note that this variable is only available under systems that produce an EXITINFO.BBS door information file.

#### `user_attrib3`

```c
unsigned char od_control.user_attrib3;
```

attrib3 This variable contains user attribute flags when a RA 2.50 or later EXITINFO.BBS file is used.

#### `user_avatar`

```c
char od_control.user_avatar;
```

This variable is a Boolean value indicating whether or not AVATAR graphics mode is on. If AVATAR graphics is available, then many of the OpenDoors functions will make use of AVATAR graphics codes for greater display speed. If AVATAR graphics mode is on, a [AVT] indicator will appear on the status line. If your door is running under a system which produces an RA 1.00+ style extended EXITINFO.BBS door information file, the user_avatar variable is set automatically. If the extended EXITINFO.BBS file is not available, this value will default to FALSE. In this case, you may wish to ask the user whether or not they wish to use AVATAR graphics, and thus set this variable yourself.

#### `user_birthday`

```c
char od_control.user_birthday[9];
```

This variable is a string, in the same format as the od_control.user_lastcall variable, which stores the date of the user's birthday, if it is available. This variable is only available under systems that produce an RA 1.00 and later style extended EXITINFO.BBS or Wildcat style DOOR.SYS file.

#### `user_callsign`

```c
char od_control.user_callsign[12];
```

This variable is a string which contains the user's amateur radio call sign, if any. This variable is only available under systems that produce a CHAIN.TXT file.

#### `user_combinedrecord`

```c
unsigned char od_control.user_combinedrecord[25];
```

This variable is an array of bit-mapped flags, with each flag corresponding to an individual message area. In this case, the first bit of od_control.ra_combinedrecord[0] corresponds to the first message area, the second bit to the second message area, and so on. If any given bit-flag is turned on, then the user has corresponding message area enabled for combined access, and if the bit is turned off, the user does not have the area enabled for combined access. A detailed description of the combined message access is beyond the scope of this manual. This variable is only available under systems that produce an RA 1.00 or later style extended EXITINFO.BBS door information file.

#### `user_comment`

```c
char od_control.user_comment[81];
```

This variable is a string which contains the sysop's comment about the user that is currently online. This comment may be displayed on the OpenDoors status line, if this variable is available. This variable is available under systems that produce an RA 1.00 and later style extended EXITINFO.BBS or Wildcat style DOOR.SYS file.

#### `user_credit`

```c
INT32 user_credit; /* physical member in tODControl */
```

`tODControl` physically contains this member, but after the structure
declaration [`OpenDoor.h`](../api/index.md) defines the source-compatibility macro
`user_credit` as `user_net_credit`. Consequently, ordinary source such as
`od_control.user_credit` is preprocessed into
`od_control.user_net_credit` and does not access this physical [`INT32`](../types.md#int32) member.

The current drop-file readers and writers do not populate or consume the
physical member. It remains part of the structure layout for binary
compatibility and should not be addressed by undefining the compatibility
macro. Use [`user_net_credit`](#user_net_credit) for the NetMail credit value
handled by OpenDoors.

#### `user_dataphone`

```c
char od_control.user_dataphone[13];
```

This string contains the user's data or business phone number, if available. This value is only available under system that produce EXITINFO.BBS, PC-Board/GAP style DOOR.SYS and WildCat DOOR.SYS format door information files.

#### `user_deducted_time`

```c
int od_control.user_deducted_time;
```

This variable contains a signed integer value, which indicates the total amount of time that has been deducted from the user during this call. This variable is only available under systems that produce an RA 1.00 and later style extended EXITINFO.BBS door information file.

#### `user_downk`

```c
unsigned int od_control.user_downk;
```

This variable contains the total kilobytes of files that the current user has downloaded from the BBS, and is available under systems that produce EXITINFO.BBS, Wildcat style DOOR.SYS or SFDOORS.DAT format door information files.

#### `user_downlimit`

```c
unsigned int od_control.user_downlimit;
```

This variable contains the total number of kilobytes that the caller is permitted to download during this call. If your door allows files do be downloaded, you will probably want to compare the value of this variable to the size of any file to be transferred and the total kilobytes already downloaded, as stored in the [`od_control.user_todayk`](#user_todayk) variable. This variable is only available under systems that produce an EXITINFO.BBS file.

#### `user_downloads`

```c
unsigned int od_control.user_downloads;
```

This variable contains the total number of files that the current user has downloaded from the BBS, and is available under systems that produce EXITINFO.BBS, PC-Board/GAP style DOOR.SYS, WildCat style DOOR.SYS or SFDOORS.DAT format door information files.

#### `user_echomailentered`

```c
char od_control.user_echomailentered;
```

This variable is a Boolean value, indicating whether or not the user has entered new EchoMail during this call. If this variable has a value of TRUE, then EchoMail has been entered, and if it has a value of FALSE, then EchoMail has not been entered. This variable will contain a valid value only after [`od_init()`](../api/od_init.md) or some OpenDoors function has been called. Any changes made to this variable will be reflected within the BBS software when control is returned to the BBS. This variable is accessible only under systems which produce an EXITINFO.BBS door information file.

#### `user_error_free`

```c
     char od_control.user_error_free;
```

This variable contains a Boolean value indicating whether or not the user is connected to the BBS via an error free connection (eg. a V.42/MNP or similar modem protocol). This variable is only available under systems that produce an SFDOORS.DAT, Wildcat style DOOR.SYS or RA 1.00 or later style extended EXITINFO.BBS door information file.

#### `user_firstcall`

```c
char od_control.user_firstcall[9];
```

This variable is a string which contains the date of the user's first call, in the same format as the od_control. user_lastcall variable. This variable is only available under systems which produce an RA 1.00 and later style extended EXITINFO.BBS door information file.

#### `user_flags`

```c
unsigned char od_control.user_flags[4];
```

The [`od_control.user_flags`](#user_flags) variable is an array of four sysop defined bit-mapped flags, which represent some sort of information about the user. [`od_control.user_flags`](#user_flags)[0] stores flags A1 - A8 in bits 0 through 7, respectively. Likewise, [`od_control.user_flags`](#user_flags)[1] stores flags B1 - B8, and so on. This variable is only available under systems that produce EXITINFO.BBS format door information files.

#### `user_handle`

```c
char od_control.user_handle[36];
```

This variable contains the user's alias or handle name, if any. If the user does not have and alias or handle, this variable will be blank. This variable is only available under systems that produce a CHAIN.TXT, RA 1.00 and later extended EXITINFO.BBS or Wildcat style DOOR.SYS door information file.

#### `user_homephone`

```c
char od_control.user_homephone[13];
```

This string contains the user's home or data phone number, if available. This value is only available under system that produce one of the following door information files: EXITINFO.BBS, PC-Board/GAP style DOOR.SYS, WildCat style DOOR.SYS or SFDOORS.DAT.

#### `user_last_pwdchange`

```c
unsigned char od_control.user_last_pwdchange;
```

This variable contains the number of calls that the user has made since they last changed their password. This variable is only available under EXITINFO.BBS files.

#### `user_lastdate`

```c
char od_control.user_lastdate[9];
```

This variable is a string containing the date of the user's last call to the BBS, and should always be of the format:

```c
"MM-DD-YY"
```

Where MM is two digits representing the number of the month of the user's call, with 1 being January, 2 being February, and so on. DD should be two digits representing the day of the month of the user's last call, beginning with 1, and MM should be the last two digits of the year of the user's last call.

This variable is only available under systems that produce one of the following door information files: CHAIN.TXT, EXITINFO.BBS, PC-Board/GAP style DOOR.SYS or WildCat style DOOR.SYS files.

#### `user_lastread`

```c
unsigned int od_control.user_lastread;
```

This variable contains the number of the highest message number that the user has read, and is only available under EXITINFO.BBS format door information files.

#### `user_lasttime`

```c
char od_control.user_lasttime[6];
```

This variable contains a string representing the time of the user's last call to the BBS, and should always be of the format:

```c
"HH:MM"
```

Where HH is two digits representing the 24-hour format hour of the user's last call, and MM is two digits representing the minute of the user's last call. Thus, the following strings would be valid entries for this string:

"00:01"    (12:01 am) "03:47"    (3:47 am) "18:20"    (6:20 pm)

This variable is only available under systems that produce an EXITINFO.BBS or Wildcat style DOOR.SYS format door information file.

#### `user_location`

```c
char od_control.user_location[26];
```

This string contains the name of the location from which the current user is calling from. This will usually be the name of the city, region (province, state, etc.) and sometimes country where the user lives. The contents of this variable are displayed on the OpenDoors status line. The value of this variable is valid after [`od_init()`](../api/od_init.md) or any other OpenDoors function has been called. Also, you may change the value of this variable if you wish. However, not that these changes may not immediately be reflected in the status line, and may or may not cause the setting to be changed after the user returns to the BBS. This variable is available under systems that produce one of the following door information files: DORINFO?.DEF, EXITINFO.BBS, PC-Board/GAP style DOOR.SYS, WildCat style DOOR.SYS SFDOORS.DAT and CALLINFO.BBS, but is not available under CHAIN.TXT or DoorWay style DOOR.SYS files.

```c
user           char od_control.caller_logindate[9];
_logindate
This variable contains a string representing the date on which
the current call to the BBS began. This variable is in the same
format as the od_control.user_lastdate variable, described
```

below. This variable is only available under systems which produce an EXITINFO.BBS file.

#### `user_loginsec`

```c
long od_control.user_loginsec;
```

This variable contains the user's security at login, and can be used to detect changes by the sysop or other programs during the course of the call, by comparing it's value with the [`od_control.user_security`](#user_security) variable. This variable is only available under systems which produce an EXITINFO.BBS file.

#### `user_logintime`

```c
char od_control.user_logintime[6];
```

This variable contains a string representing the time of day at which the current call to the BBS began. This variable is in the same format as the [`od_control.user_lasttime`](#user_lasttime) variable, which is also described below. This variable is available under systems which produce an EXITINFO.BBS, a Wildcat style DOOR.SYS, or an SFDOORS.DAT file.

#### `user_logonpassword`

```c
char od_control.user_logonpassword[16];
```

This variable is a string which contains the user's password at the time at which the current call to the BBS began. This variable can be used to detect changes by the sysop or other programs to the user's password, which have taken place during the course of the call. In order to detect such changes, simply compare the contents of this string with the contents of the [`od_control.user_password`](#user_password) variable. This variable is only available under systems which produce an EXITINFO.BBS format door information file.

#### `user_menustack`

```c
char od_control.user_menustack[50][9];
```

This variable is an array of 50 strings, containing the stack of BBS menus that have been executed, and is used to record the current position of the user within the BBS's menu system. Each string contains just the base portion of the filename of the menu, without the extension. The od_control.ra_menustackpointer variable points to the top of the menu stack. However, a complete discussion of the menu stack is beyond the scope of this manual. This variable is only available under systems that produce an RA 1.00 and later style extended EXITINFO.BBS door information file.

#### `user_menustackpointer`

```c
unsigned char od_control.user_menustackpointer;
```

This variable points to the top of the current menu stack. For more information on the menu stack, please refer to the od_control.ra_menustack variable, above. This variable is only available under systems that produce an RA 1.00 and later style extended EXITINFO.BBS door information file.

#### `user_messages`

```c
unsigned int od_control.user_messages;
```

This variable contains a value representing the total number of messages that have been written by the user, and is available under EXITINFO.BBS or Wildcat style DOOR.SYS format door information files.

#### `user_name`

```c
char od_control.user_name[36];
```

This string contains the name of the user that is currently on- line, and is used by OpenDoors to display the current user name on the status line, and will most likely be used by your door for differentiating among different users. In most cases, you should probably not change the value of this variable, as a user's name does not usually change, and doing so could results in problems when returning to some BBS systems. For an example of using this variable, see the EX_VOTE.C example program. This variable is available under all BBS systems.

#### `user_net_credit`

```c
DWORD od_control.user_net_credit;
```

Contains the caller's remaining NetMail credit from supported
`EXITINFO.BBS` records. OpenDoors reads the field during initialization and
writes its current value back when updating those records. The public
compatibility name `user_credit` expands to this member, as do the applicable
older caller-credit aliases.

The value is a [`DWORD`](../types.md#dword) in `tODControl`. Individual legacy records can store a
narrower credit field; for example, the primitive `EXITINFO.BBS` writer casts
the value to [`WORD`](../types.md#word). A door targeting such a format must not assume that a
value above 65,535 will survive the write-back conversion.

#### `user_netmailentered`

```c
char od_control.user_netmailentered;
```

This variable is a Boolean value, indicating whether or not the user has entered new NetMail or GroupMail during this call. If this variable has a value of TRUE, then NetMail/GroupMail has been entered, and if it has a value of FALSE, then NetMail/GroupMail has not been entered. This variable will contain a valid value only after [`od_init()`](../api/od_init.md) or some OpenDoors function has been called. Any changes made to this variable will be reflected within the BBS software when control is returned to the BBS. This variable is accessible only under systems which produce an EXITINFO.BBS door information file.

#### `user_num`

```c
unsigned int od_control.user_num;
```

This variable contains the number of the user's record in the user database file, where 0 is the first record. This can be useful for changing user settings that are not re-read by the BBS, such as the user's phone number or security level which might be altered by a call back verification door. However, the value of this variable itself should not be altered.

This variable is available under systems which produce any of the following door information file formats: CHAIN.TXT, PC- Board/GAP style DOOR.SYS, Wildcat style DOOR.SYS SFDOORS.DAT and EXITINFO.BBS.

#### `user_numcalls`

```c
unsigned int od_control.user_numcalls;
```

This variable contains the total number of calls that the current user has placed to the BBS, and is available under systems that produce EXITINFO.BBS or PC-Board/GAP and Wildcat style DOOR.SYS door information files.

#### `user_numpages`

```c
unsigned int od_control.user_numpages;
```

The value of this variable contains the total number of times that the user has paged the sysop, and can be used to limit the number of times that the user is permitted to page the sysop. OpenDoors increments this variable every time that the user pages the sysop, via the [`od_page()`](../api/od_page.md) function. This variable is used with all types of door information files. However, this variable will only reflect the value within the BBS if an EXITINFO.BBS file is produced. Otherwise, the variable will only contain the number of times that the user has paged within the door, but not the total number of times the user has paged. Under EXITINFO.BBS systems, changes to the value of this variable will be reflected within the BBS upon return by the DOOR.

#### `user_password`

```c
char od_control.user_password[16];
```

This variable contains the user's password for accessing the BBS. OpenDoors does not use this value itself. This variable will contain a valid value only after [`od_init()`](../api/od_init.md) or some OpenDoors function has been called. You may change the value of this variable. Note, however, that changes in this variable may or may not cause the setting to be changed when control returns to the BBS - this will depend upon the particular BBS system your door is running under. This variable is only available under systems that produce one of the following door information files: EXITINFO.BBS, PC-Board/GAP and Wildcat style DOOR.SYS, SFDOORS.DAT, and CALLINFO.BBS.

#### `user_pending`

```c
unsigned int od_control.user_pending;
```

This variable represents the total value of NetMail that has been written by the current user, but not yet exported from the message base. This variable is only available under systems that produce an EXITINFO.BBS door information file.

#### `user_reasonforchat`

```c
char od_control.user_reasonforchat[78];
```

This variable is a string, containing the reason for which the user wishes to chat with the sysop, as they entered at the time of paging the sysop. This variable will contain an empty string if the user has not paged the sysop, or if the reason the user wishes to chat is unknown. See also the [`od_control.user_wantchat`](#user_wantchat) variable. This variable is available under all BBS systems, regardless of what style of door information file they produce. However, this variable will not be passed between the door and BBS, and thus the user's reason for chat within the door will not necessarily correspond to their reason for chat outside the door.

user_rip       char user_rip;

This variable is set to TRUE if the user has RIP (Remote Imaging Protocol) graphics enabled, and FALSE if they do not. This setting can be determined from the door information (drop) file in many cases. In other cases, you can automatically determine whether or not the user's system supports RIP graphics using the [`od_autodetect()`](../api/od_autodetect.md) function (see page 48).

user_rip_ver   BYTE user_rip_ver;

This variable contains the version of the RIP protocol that is in use. This variable is only available under a RemoteAccess 2.50 EXITINFO.BBS file.

#### `user_screen_length`

```c
unsigned int od_control.user_screen_length;
```

This value of this variable represents the total number of lines that can be displayed on the user's screen at once, and is usually either 24 or 25. You may wish to make use of this variable to allow your door to pause the display of long pieces of text after every screen length, in order to allow the user to read this information before it passes off of their screen. In this case, you would simply maintain a counter of the total number of lines displayed, and when this value reaches one less than the length of the user screen, display a prompt asking the user to whether or not they wish to continue.

This variable is set to the user's setting within the BBS under systems that produce any of the following door information file formats: CHAIN.TXT, EXITINFO.BBS, PC-Board/GAP and Wildcat style DOOR.SYS and CALLINFO.BBS files.

This variable is used by the OpenDoors function, [`od_list_files()`](../api/od_list_files.md). If this variable contains a valid value, OpenDoors will pause the listing of files after every screen, and give the user the option of continuing, aborting, or disabling the "Continue?" prompt for the rest of the file listing. Thus, if you are using the [`od_list_files()`](../api/od_list_files.md) under a system that does not produce one of the door information files listed above, you may wish to obtain the user's screen length from the user themselves. If the screen length is not available from the particular type of door information file that is found, and you do not set this value yourself, this variable will default to 23. If you are going to set the value of this variable yourself, you should do so after having called [`od_init()`](../api/od_init.md) or some OpenDoors function.

#### `user_screenwidth`

```c
unsigned char od_control.user_screenwidth;
```

This variable contains a value representing the width of the user's screen, and will most often be equal to 80. This variable is only available under systems that produce a CHAIN.TXT or RA 1.00 and later style extended EXITINFO.BBS door information file.

#### `user_security`

```c
unsigned int od_control.user_security;
```

This variable contains a numerical value representing the user's security access level on the BBS. You may wish to use this value to determine whether or not the current user of your door should have access to certain sysop-only functions. In this case, you may wish to have a configuration file used by your door, in which the sysop may define the minimum security level for sysop access. You would then be able to compare this configuration setting to the security level stored in this variable, in order to determine whether or not sysop function should be available. An alternative method, used by the EX_VOTE.C sample door, of determining whether or not the current user is the sysop is to compare the user's name with the value of the [`od_control.sysop_name`](#sysop_name) variable. This method has the advantage of not requiring a configuration program, but the disadvantage that the door will not function correctly under all BBS systems, as the [`od_control.sysop_name`](#sysop_name) variable is not available under all BBS systems.

The [`od_control.user_security`](#user_security) variable is available under BBS systems that produce any of the following door information file

formats: CHAIN.TXT, EXITINFO.BBS, PC-Board/GAP and Wildcat style DOOR.SYS, SFDOORS.DAT or CALLINFO.BBS.

#### `user_sex`

```c
char od_control.user_sex;
```

This variable contains a single character representing the gender of the user that is currently online. This variable will contain an upper-case 'F' if the user is female, and an upper- case 'M' if the user is male. This variable is available under systems that produce a CHAIN.TXT or RA 2.x style EXITINFO.BBS file.

#### `user_subdate`

```c
char od_control.user_subdate[9];
```

This variable is a string, in the same format as the [`od_control.user_lastdate`](#user_lastdate) variable, which stores the date of expiry of the user's subscription to the BBS. This variable is only available under systems which produce a PC-Board/GAP and Wildcat style DOOR.SYS or RA 1.00 and later style extended EXITINFO.BBS door information file.

#### `user_timelimit`

```c
int od_control.user_timelimit;
```

This variable contains the amount of time, in minutes, that the user has left in the door. Note that this value may or may not be equal to the total amount of time that the user has left on the BBS, depending upon whether the BBS or a third-party door manager program only allows a limited amount of time in this door. This variable contains a valid value after [`od_init()`](../api/od_init.md) or some OpenDoors function has been called. OpenDoors uses this variable to keep track of how much time the user has left in the door, and will automatically warn the user when nearly all of his or her time has been used up. OpenDoors will also force the user out of the door when their time in the door has expired. OpenDoors automatically subtracts one minute from this variable every minute that OpenDoors is active, unless chat mode has been activated (in which case the user's time will freeze), and also adjusts the value of this variable when the sysop uses the time adjustment function keys. Hence, you will not normally have any need to alter the value of this variable yourself. However, there may be some cases in which you wish to subtract a penalty or add a bonus to the user's time, such as in a "timebank" door or a door game that permits the user to "gamble time". Depending on which BBS system your door is running under, the value of this variable may or may not effect the user's time left upon return to the BBS. The BBS system will either reset the user's time to the value re-written to the door information file (this variable), or will always subtract the amount of time spent in the door from the user's remaining time.

This variable is available under all door information file formats.

#### `user_todayk`

```c
unsigned int od_control.user_todayk;
```

This variable contains the total kilobytes of files that the current user has downloaded from the BBS during the current day, and is available under systems that produce EXITINFO.BBS, PC- Board/GAP and Wildcat style DOOR.SYS, or SFDOORS.DAT format door information files.

#### `user_upk`

```c
unsigned int od_control.user_upk;
```

This variable contains the total kilobytes of files that the current user has uploaded to the BBS, and is available under systems that produce EXITINFO.BBS, Wildcat style DOOR.SYS or SFDOORS.DAT files.

#### `user_uploads`

```c
unsigned int od_control.user_uploads;
```

This variable contains the total number of files that the current user has uploaded to the BBS, and is available under systems that produce EXITINFO.BBS, PC-Board/GAP and Wildcat style DOOR.SYS, or SFDOORS.DAT format door information files.

#### `user_wantchat`

```c
char od_control.user_wantchat;
```

This variable is a Boolean value which indicates whether or not the user wishes to chat with the sysop (ie, the user has paged the sysop, but has yet to receive a chat with the sysop). This variable is used under all door information file formats. However, changes to this variable are only reflected on the BBS when the door is running under a system that produces an EXITINFO.BBS door information file.

This variable is automatically turned on (ie., set to TRUE), when the user begins to page the sysop for chat, within the [`od_page()`](../api/od_page.md) function, and is automatically turned off (ie., set to FALSE), when the sysop breaks in for chat via the chat function key. Also, setting this variable to TRUE will turn on the flashing want-chat indicator on the OpenDoors status line.

#### `user_xi_record`

```c
unsigned int od_control.user_xi_record;
```

This variable contains the number of the user's record in the USERXI.BBS file, if any. This variable is only available under system that produce a Remote Access 1.00 and later style extended door information file.

### Traffic-log fields

#### `timelog_busyperhour`

```c
INT16 od_control.timelog_busyperhour[24];
```

Each element contains the count recorded by `EXITINFO.BBS` for the corresponding
hour of the day: element 0 covers 00:00 through 00:59, element 1 covers 01:00
through 01:59, and so forth through element 23. The array is zero-initialized
and is populated only when OpenDoors reads an `EXITINFO.BBS` record containing
the traffic log. OpenDoors does not otherwise use these counts; they are
provided for the application and are included when the record is rewritten.

#### `timelog_busyperday`

```c
INT16 od_control.timelog_busyperday[7];
```

These are the `EXITINFO.BBS` traffic counts for Sunday through Saturday, in
that order. The array is zero-initialized and unavailable values therefore
cannot be distinguished from valid zero counts. OpenDoors does not interpret
the counts. Historic RemoteAccess versions were known not to maintain this
array consistently, so the presence of an `EXITINFO.BBS` record does not imply
that nonzero data will be present.

### Extended RemoteAccess identity

#### `user_org`

```c
char od_control.user_org[51];
```

This is the caller's organization from a RemoteAccess 2.x extended
`EXITINFO.BBS` record, converted from a Pascal string of at most 50 bytes to a
null-terminated C string. It is empty for formats which do not provide the
field. OpenDoors does not display or interpret it, but writes an application
replacement back to a RemoteAccess 2.x record.

#### `user_address`

```c
char od_control.user_address[3][51];
```

These are the three postal-address lines in a RemoteAccess 2.x extended
`EXITINFO.BBS` record. Each source Pascal string can contain at most 50 bytes;
OpenDoors exposes it as a null-terminated C string. All elements are empty when
the active format does not supply them. OpenDoors does not otherwise read the
address and writes all three elements back only to the RemoteAccess 2.x
record.

#### `user_pwd_crc`

```c
INT32 od_control.user_pwd_crc;
```

This is the signed 32-bit `password_crc` value stored in a RemoteAccess 2.x
extended `EXITINFO.BBS` user record. OpenDoors copies the value without
calculating, validating, or using it. Its initial zero can be either an
unavailable value or a value supplied by the BBS; only
[`od_info_type`](connection.md#od_info_type) establishes whether the RA2 field
was read. Application changes are written back to that format.

#### `user_logon_pwd_crc`

```c
INT32 od_control.user_logon_pwd_crc;
```

This is the RemoteAccess 2.x `logonpasswordcrc` value captured at the start of
the current BBS session. It permits an application which understands that BBS
field to compare the login value with [`user_pwd_crc`](#user_pwd_crc).
OpenDoors does not calculate or compare either checksum. The field begins at
zero, is populated only by a full RemoteAccess 2.x record, and is written back
to the same format.

#### `user_last_cost_menu`

```c
char od_control.user_last_cost_menu[9];
```

This is the base menu name stored in the RemoteAccess 2.x
`last_cost_menu` field, exposed as a C string of at most eight bytes plus the
terminator. It is empty when unavailable. OpenDoors does not use the name and
writes an application replacement back only to the RemoteAccess 2.x record.

#### `user_menu_cost`

```c
WORD od_control.user_menu_cost;
```

This is the unsigned 16-bit RemoteAccess 2.x `menu_cost_per_min` value
associated with [`user_last_cost_menu`](#user_last_cost_menu). OpenDoors
preserves the numeric value but does not apply it to time or credit accounting.
It is zero-initialized and written back only to the full RemoteAccess 2.x
record.

### Areas, groups, and preferences

#### `user_group`

```c
WORD od_control.user_group;
```

This is the caller's RemoteAccess group number. It is populated by
`EXITINFO.BBS` formats which contain the group field and otherwise remains
zero. The RemoteAccess personality displays the value on one local status
screen; the remainder of OpenDoors treats it as application data. On write-back
to the older record layout it is narrowed to one byte, while the RemoteAccess
2.x layout retains the full [`WORD`](../types.md#word).

#### `user_msg_area`

```c
WORD od_control.user_msg_area;
```

This is the number of the caller's current message area. OpenDoors can obtain
it from SFDOORS.DAT and from the extended RemoteAccess 1.x and 2.x
`EXITINFO.BBS` layouts. It is zero when unavailable. OpenDoors does not use it
outside door-information input and output; the RemoteAccess 1.x extended
layout stores only one byte and therefore narrows values above 255.

#### `user_file_area`

```c
WORD od_control.user_file_area;
```

This is the number of the caller's current file area. Its sources and
write-back behavior are the same as [`user_msg_area`](#user_msg_area):
SFDOORS.DAT and extended RemoteAccess `EXITINFO.BBS`, with a one-byte field in
the RemoteAccess 1.x extension. OpenDoors otherwise leaves the value for the
application.

#### `user_file_group`

```c
WORD od_control.user_file_group;
```

This is the current file-group number from a full RemoteAccess 2.x extended
`EXITINFO.BBS` record. It is copied as an unsigned 16-bit value, is not used by
OpenDoors, and is written back to that record. Zero means either group zero or
that the active format did not provide the field.

#### `user_msg_group`

```c
WORD od_control.user_msg_group;
```

This is the current message-group number from a full RemoteAccess 2.x extended
`EXITINFO.BBS` record. OpenDoors preserves it as an unsigned 16-bit value but
does not otherwise read it. It is written back to that format; zero is
ambiguous when another format is active.

#### `user_protocol`

```c
char od_control.user_protocol;
```

This is the single-byte `default_protocol` selector from a RemoteAccess 2.x
extended `EXITINFO.BBS` record. It is the BBS's protocol-selection character,
not an OpenDoors enumeration. OpenDoors neither selects a transfer protocol
from it nor changes it automatically; application changes are written back to
the same format.

#### `user_last_birthday_check`

```c
BYTE od_control.user_last_birthday_check;
```

This is the raw one-byte `last_dob_check` value from RemoteAccess 2.x. The
current OpenDoors implementation defines no date conversion or constants for
the field and does not inspect it. It remains zero for other formats and is
preserved when the RA2 record is rewritten.

#### `user_language`

```c
BYTE od_control.user_language;
```

This is the RemoteAccess language-table index from an extended 1.x or 2.x
`EXITINFO.BBS` record. OpenDoors does not use the index to translate its own
prompts; an application must select any corresponding strings itself. The
field begins at zero and is written back to either extended layout.

#### `user_date_format`

```c
BYTE od_control.user_date_format;
```

This is the RemoteAccess `dateformat` selector from an extended 1.x or 2.x
`EXITINFO.BBS` record. OpenDoors copies the one-byte value without interpreting
or normalizing date strings and supplies no manifest constants for its values.
It begins at zero and is written back to either extended layout.

#### `user_forward_to`

```c
char od_control.user_forward_to[36];
```

This is the RemoteAccess message-forwarding destination, converted from a
Pascal string of at most 35 bytes. Extended RemoteAccess 1.x and 2.x records
provide and accept it. OpenDoors does not perform forwarding or validate the
name; the application receives an empty string when the field is unavailable.

#### `user_expert`

```c
BOOL od_control.user_expert;
```

TRIBBS.SYS line 5 supplies [`TRUE`](../constants/general.md#true) when its first character is `Y` and [`FALSE`](../constants/general.md#false)
otherwise. OpenDoors writes it back as `Y` or `N` when rewriting that format,
but does not alter menus, prompts, or other behavior in response to the value.
It remains the static value [`FALSE`](../constants/general.md#false) for formats which do not supply it unless
the application assigns it.

### IEMSI and held-message data

#### `user_emsi_session`

```c
BYTE od_control.user_emsi_session;
```

This is the one-byte IEMSI-session flag supplied by extended RemoteAccess 1.x
and 2.x `EXITINFO.BBS` records. A nonzero value reports that the BBS had an
interactive EMSI session; OpenDoors does not establish or negotiate the
session itself. The value is zero when unavailable and is written back with
the other extended fields.

#### `user_emsi_crtdef`

```c
char od_control.user_emsi_crtdef[41];
```

This is the IEMSI terminal-definition string from extended RemoteAccess
`EXITINFO.BBS`, converted from a Pascal field of at most 40 bytes. OpenDoors
does not parse the IEMSI token and writes the C string back to either supported
extended layout.

#### `user_emsi_protocols`

```c
char od_control.user_emsi_protocols[41];
```

This is the IEMSI protocols string from the same extended records. It is a
null-terminated copy of the BBS's at-most-40-byte Pascal field. OpenDoors does
not use it for protocol selection.

#### `user_emsi_capabilities`

```c
char od_control.user_emsi_capabilities[41];
```

This is the IEMSI capabilities string from the same extended records,
preserved as at most 40 bytes plus a null terminator. Capability interpretation
is left to the application; OpenDoors only reads and writes the field.

#### `user_emsi_requests`

```c
char od_control.user_emsi_requests[41];
```

This is the IEMSI requests string from the same extended records, converted to
a null-terminated C string. OpenDoors does not act on the requests.

#### `user_emsi_software`

```c
char od_control.user_emsi_software[41];
```

This is the IEMSI client-software identification string from the same extended
records. It can contain at most 40 bytes before the terminator. OpenDoors does
not parse it.

#### `user_hold_attr1`

```c
BYTE od_control.user_hold_attr1;
```

This is the first raw held-message attribute byte from extended RemoteAccess
1.x and 2.x `EXITINFO.BBS`. OpenDoors defines no bit masks for it and does not
inspect it; the value is preserved on write-back.

#### `user_hold_attr2`

```c
BYTE od_control.user_hold_attr2;
```

This is the second raw held-message attribute byte. Its availability and
handling are identical to [`user_hold_attr1`](#user_hold_attr1).

#### `user_hold_len`

```c
BYTE od_control.user_hold_len;
```

This is the raw held-message length byte adjacent to the two held-message
attributes in extended RemoteAccess `EXITINFO.BBS`. OpenDoors copies it but
does not use it to address or allocate any buffer.

### Session and terminal extensions

#### `user_logindate`

```c
char od_control.user_logindate[9];
```

This is the date on which the current BBS session began, from
`EXITINFO.BBS`, stored as an eight-character string plus its terminator. The
representation is supplied by the BBS and is normally the same as
[`user_lastdate`](#user_lastdate). OpenDoors does not parse it and writes a
replacement back when the active `EXITINFO.BBS` layout supports the field.

#### `user_timeofcreation`

```c
char od_control.user_timeofcreation[6];
```

This is the creation time stored in an `EXITINFO.BBS` record, exposed as five
characters plus a terminator. It normally has the `HH:MM` form used by
[`user_lasttime`](#user_lasttime). OpenDoors does not parse the string and
writes it back with supported `EXITINFO.BBS` variants.

#### `user_readthru`

```c
INT16 od_control.user_readthru;
```

This is the signed 16-bit `readthru` value from `EXITINFO.BBS`. It is copied
without interpretation and written back with that record. The field is
initially zero, and OpenDoors itself does not read it outside the format
conversion path.

#### `user_time_used`

```c
WORD od_control.user_time_used;
```

This field begins with the `elapsed` minutes from `EXITINFO.BBS`; other formats
leave it zero. During [`od_exit()`](../api/od_exit.md), OpenDoors adds its
calculated change in used time before rewriting the door-information record.
The terminal-emulation control-code path can also display the current numeric
value. Applications may read or adjust it, but a zero value is ambiguous when
no `EXITINFO.BBS` elapsed field was available.

#### `user_rip`

```c
BYTE od_control.user_rip;
```

This Boolean reports whether RIP graphics are active for the caller. Several
text drop-file formats, custom configuration definitions, QuickBBS and
RemoteAccess `EXITINFO.BBS` variants can supply it; otherwise it begins as
[`FALSE`](../constants/general.md#false). [`od_autodetect()`](../api/od_autodetect.md) can set it to [`TRUE`](../constants/general.md#true) after
a successful RIP probe.

OpenDoors reads this field when selecting RIP-specific clear-screen behavior,
terminal-emulation commands, status-line indicators, and `.RIP` file handling.
Application changes take effect immediately and are written back only by
door-information formats which have a RIP field.

#### `user_rip_ver`

```c
BYTE od_control.user_rip_ver;
```

This is the raw RIP version byte supplied by a full RemoteAccess 2.x extended
`EXITINFO.BBS` record. OpenDoors preserves it on write-back but does not use it
to select or reject RIP commands; [`user_rip`](#user_rip) is the active-mode
switch. The field remains zero when the version is unavailable.
