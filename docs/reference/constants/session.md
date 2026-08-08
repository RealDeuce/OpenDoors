# Session and connection

## Door-information formats

These values identify the normalized source recorded in
[`od_control.od_info_type`](../control/connection.md).

| Name | Source |
| --- | --- |
| `DORINFO1` | `DORINFO?.DEF`. |
| `EXITINFO` | QuickBBS 2.6-style `EXITINFO.BBS` and `DORINFO?.DEF`. |
| `RA1EXITINFO` | RemoteAccess 1.x `EXITINFO.BBS` and `DORINFO?.DEF`. |
| `CHAINTXT` | `CHAIN.TXT`. |
| `SFDOORSDAT` | `SFDOORS.DAT`. |
| `CALLINFO` | `CALLINFO.BBS`. |
| `DOORSYS_GAP` | GAP/PCBoard-style `DOOR.SYS`. |
| `DOORSYS_DRWY` | DoorWay-style `DOOR.SYS`. |
| `QBBS275EXITINFO` | QuickBBS 2.75-or-later `EXITINFO.BBS`. |
| `CUSTOM` | Caller-supplied custom door-information handling. |
| `DOORSYS_WILDCAT` | Wildcat!-style `DOOR.SYS`. |
| `RA2EXITINFO` | RemoteAccess 2.x `EXITINFO.BBS`. |
| `TRIBBSSYS` | `TRIBBS.SYS`. |
| `DOOR32SYS` | `DOOR32.SYS`. |
| `NO_DOOR_FILE` | No door-information file was found or selected. |

## Communication methods

| Name | Method |
| --- | --- |
| `COM_FOSSIL` | DOS FOSSIL driver. |
| `COM_INTERNAL` | OpenDoors internal serial driver. |
| `COM_WIN32` | Win32 serial communications. |
| `COM_DOOR32` | Existing Door32 communications handle. |
| `COM_SOCKET` | TCP socket connection. |
| `COM_STDIO` | Standard input and output streams. |

[`od_control.od_com_flow_control`](../control/connection.md) uses:

| Name | Method |
| --- | --- |
| `COM_DEFAULT_FLOW` | Library or platform default flow control. |
| `COM_RTSCTS_FLOW` | RTS/CTS hardware flow control. |
| `COM_NO_FLOW` | No flow control. |

## Paging policy

| Name | Meaning |
| --- | --- |
| `PAGE_DISABLE` | Paging the local operator is unavailable. |
| `PAGE_ENABLE` | Paging is always available. |
| `PAGE_USE_HOURS` | Paging is available only between the configured start and end minutes. |

These values configure
[`od_control.od_okaytopage`](../control/runtime.md).

## Disabled automatic behavior

Combine these bits in [`od_control.od_disable`](../control/runtime.md). Setting
a bit disables the named behavior.

| Name | Disabled behavior |
| --- | --- |
| `DIS_INFOFILE` | Reading and rewriting door-information files. The application must supply the resulting session fields. |
| `DIS_CARRIERDETECT` | Automatic exit when carrier is lost. |
| `DIS_TIMEOUT` | Automatic exit when the caller's available time expires. |
| `DIS_LOCAL_OVERRIDE` | A zero BPS value in door information overriding a configured locked BPS rate with local mode. |
| `DIS_BPS_SETTING` | Setting the BPS rate through a FOSSIL driver. |
| `DIS_LOCAL_INPUT` | Local keyboard input to the door; local operator function keys remain active. |
| `DIS_SYSOP_KEYS` | Both local door input and local operator function keys. |
| `DIS_DTR_DISABLE` | Modem DTR-response disabling during shutdown. |
| `DIS_NAME_PROMPT` | Caller-name prompting in automatic local mode. |

## BBS event status

| Name | Meaning |
| --- | --- |
| `ES_DELETED` | The event is deleted or absent. |
| `ES_ENABLED` | The event is enabled. |
| `ES_DISABLED` | The event exists but is disabled. |
