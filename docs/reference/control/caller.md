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
| `user_forward_to`, `sysop_next` | Message forwarding and sysop-next flags. |
| `user_emsi_session`, `user_emsi_crtdef`, `user_emsi_protocols` | EMSI session and terminal/protocol descriptions. |
| `user_emsi_capabilities`, `user_emsi_requests`, `user_emsi_software` | Remaining EMSI negotiation strings. |
| `user_hold_attr1`, `user_hold_attr2`, `user_hold_len` | Format-specific held-message attributes. |
| `user_protocol` | Preferred transfer-protocol character. |
| `user_last_birthday_check`, `user_sex`, `user_last_cost_menu` | Additional BBS user-record fields. |

These members preserve the source format's representation. Consult the BBS
format specification before assigning them a meaning not stated above.
