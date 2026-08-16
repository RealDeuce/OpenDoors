# OpenDoors V7 API Design

## Design Principles

- DOS remains a first-class platform. Serial/modem and socket I/O are core
  features.
- The public API uses accessor functions, not exposed structs, for ABI
  stability across library versions.
- C99 sized types (`int32_t`, `uint8_t`, etc.) are the standard. A
  compatibility header (`od_compat.h`) provides them for C89 compilers
  (Turbo C). Plain `int` is used where exact size does not matter.
- A single process serves a single session. There are no session handles;
  global state is appropriate.
- Initialization is explicit. [`od_init()`](../reference/api/od_init.md)
  returns an error code.
- Niche BBS-system-specific features from V6 are removed. The API surface
  covers what door authors actually need.

## Drop File Format: `opendoor.drp`

A new positional (line-number-based) drop file format. Lines are ordered by
importance so that a minimal BBS can write only the first few lines and a
door that reads fewer lines than the file contains simply uses defaults for
the rest.

No comments. No key=value. Extension is by appending new lines at the end;
old parsers stop at the last line they understand.

### Line Definitions

```
Line 1:  Transport type       0=LOCAL  1=SERIAL  2=SOCKET  3=STDIO
Line 2:  Handle               OS handle or socket descriptor; -1 if N/A
Line 3:  Time remaining       Minutes
Line 4:  User ID              String; opaque, BBS-assigned, persistent
Line 5:  User name            Real name
Line 6:  Screen rows          Terminal height (default 24)
Line 7:  Screen cols          Terminal width (default 80)
Line 8:  ANSI                 0 or 1 (default 0)
Line 9:  Node number          BBS node (default 1)
Line 10: Baud rate            DTE rate; 0 if N/A
Line 11: Encoding             0=CP437  1=UTF-8 (default 0)
Line 12: RIP version          0=none  1=RIPscrip 1.x  2=RIP 2.0
Line 13: Security level       Integer access level
Line 14: Connect speed        DCE rate; 0 if N/A
Line 15: User alias           Handle / nickname
Line 16: User location        City, State or similar
Line 17: BBS name
Line 18: Sysop name
```

### Design Rationale

The format fills three gaps present in every existing drop file format:

- **Screen width (columns)**: only CHAIN.TXT carries this; DOOR.SYS and
  DOOR32.SYS do not.
- **Character encoding**: no existing format specifies whether I/O is
  CP437 or UTF-8.
- **RIP version**: no text-based format distinguishes RIPscrip 1.x from
  RIP 2.0.

The user ID field is a string rather than a numeric record number. Numeric
record numbers can be recycled when users are deleted and recreated. A
string ID lets each BBS use whatever persistent identifier is natural: a
UUID, a database key, or a username that is guaranteed unique.

Handle value -1 means the handle is provided by other means (command-line
argument, stdio inheritance, etc.).

### What This Format Does Not Do

There is no return file. The audit of modern BBS software (Synchronet,
Mystic, ENiGMA 1/2, WWIV, Talisman, x/84) showed that only Synchronet
reads data back from doors in any meaningful way. Standardizing a return
format is not justified.

## Transport Layer

| Transport | DOS    | DOS/32 | Win32 | Unix       |
|-----------|--------|--------|-------|------------|
| FOSSIL    | Yes    | Yes    | --    | --         |
| Serial    | --     | --     | Yes   | Yes (new)  |
| Socket    | --     | --     | Yes   | Yes        |
| StdIO     | --     | --     | --    | Yes        |
| Local     | Yes    | Yes    | Yes   | Yes        |

StdIO is not supported on Windows. Anonymous pipes on Windows do not
support overlapped I/O, and a blocking `ReadFile` on a pipe is not
interrupted when the write end closes, making reliable carrier detection
impossible. Unix ptys and signal delivery do not have this problem.
Windows doors should use socket transport instead.

Unix serial support is new for V7: termios-based `/dev/tty*` I/O with
baud rate configuration and carrier detect via `TIOCM_CD`.

The internal UART ISR driver from V6 is removed. DOS serial I/O uses
FOSSIL exclusively.

## Drop File Readers

V7 reads three formats plus a custom callback:

- `opendoor.drp` (V7 native)
- `DOOR.SYS` (52-line variant, widely supported)
- `DOOR32.SYS` (socket/handle passing, lean)
- Custom callback (`od_on_no_drop_file`) for anything else

All other V6 formats are removed: DORINFO?.DEF, EXITINFO.BBS (all
variants), CHAIN.TXT, SFDOORS.DAT, CALLINFO.BBS, TRIBBS.SYS,
PCBOARD.SYS/USERS.SYS.

## Configuration

All configuration is done via `od_cfg_*()` setter functions called before
[`od_init()`](../reference/api/od_init.md).

### Transport

```
od_cfg_transport(type, port, baud)
od_cfg_handle(handle)
od_cfg_flow_control(method)
```

### Drop File

```
od_cfg_drop_file(path)
od_cfg_drop_type(type)
```

Drop type constants: `OD_DROP_AUTO`, `OD_DROP_V7`, `OD_DROP_DOORSYS`,
`OD_DROP_DOOR32`, `OD_DROP_CUSTOM`, `OD_DROP_NONE`.

### Program Identity

```
od_cfg_program(name, version)
```

### Behaviour

```
od_cfg_local_mode(enable)
od_cfg_inactivity(warn_sec, timeout_sec)
od_cfg_max_time(minutes)
od_cfg_door_encoding(encoding)
od_cfg_flags(flags)
```

## Callback Registration

```
od_on_exit(func)
od_on_idle(func)              -- called from od_kernel()
od_on_hangup(func)            -- carrier loss
od_on_time_warning(func)      -- receives minutes_left
od_on_no_drop_file(func)      -- custom drop file or local prompting
od_on_statusline(func)        -- custom status line renderer
od_on_chat_start(func)
od_on_chat_end(func)
```

## Initialization

```
int  od_init(void)            -- returns 0 on success, negative on error
void od_exit(errorlevel, hangup)
```

All `od_cfg_*()` and `od_on_*()` calls happen before
[`od_init()`](../reference/api/od_init.md). The
first call to any `od_cfg_*()` function triggers a one-time internal
defaults initialization (screen rows 24, cols 80, encoding CP437, drop
type auto, etc.). If [`od_init()`](../reference/api/od_init.md) is called
without any prior `od_cfg_*()` calls, it performs the same defaults
initialization as its first step.
This means door authors only need to set the values they want to override.

Init then reads the drop file, opens the transport, starts the status
line, and populates user/system state.

## User Info Accessors

```
const char *od_user_id(void)           -- opaque, persistent
const char *od_user_name(void)
const char *od_user_alias(void)
const char *od_user_location(void)
int         od_user_security(void)
int         od_user_time_remaining(void)
int         od_user_screen_rows(void)
int         od_user_screen_cols(void)
int         od_user_ansi(void)
int         od_user_rip(void)          -- 0, 1, or 2
int         od_user_encoding(void)     -- OD_ENC_CP437 or OD_ENC_UTF8
```

Corresponding `od_user_set_*()` functions exist for all of the above.
Setters are used by the `od_on_no_drop_file` callback and for runtime
changes (e.g. granting bonus time).

## System / Session Accessors

```
const char   *od_system_name(void)
const char   *od_sysop_name(void)
int           od_node_number(void)
int           od_transport_type(void)
uint32_t      od_baud_rate(void)
uint32_t      od_connect_speed(void)
int           od_local_mode(void)
int           od_drop_file_type(void)
```

## Terminal Output

```
od_printf(fmt, ...)           -- formatted output with backtick colour codes
od_print(str)                 -- plain string
od_putch(ch)                  -- single character
od_write(buf, len)            -- raw bytes to remote

od_set_color(fg, bg)
od_set_attrib(attr)           -- IBM-PC attribute byte
od_set_cursor(row, col)
od_get_cursor(*row, *col)

od_clr_scr()
od_clr_line()
od_draw_box(left, top, right, bottom)
od_scroll(left, top, right, bottom, distance)

od_screen_save_size()
od_screen_save(buf, bufsize)
od_screen_restore(buf, bufsize)

od_send_file(filename)        -- display ANSI/ASCII art file
```

The library handles CP437/UTF-8 conversion transparently based on the
door's encoding (`od_cfg_door_encoding()`) versus the remote terminal's
encoding (`od_user_encoding()`).

### Colour Syntax

Retained from V6. Backtick-delimited colour names within
[`od_printf()`](../reference/api/od_printf.md):

```
od_printf("`bright green`Welcome, `cyan`%s`white`!\n", od_user_name());
```

## Input

```
int  od_get_input(od_input_t *event, uint32_t timeout_ms)
int  od_get_key(int wait)
int  od_get_answer(const char *options)
int  od_input_str(char *buf, int maxlen, int min_ch, int max_ch)
int  od_edit_str(char *buf, char *format, int row, int col,
                 int normal_attr, int highlight_attr, int flags)
int  od_key_pending(void)
void od_clear_keybuffer(void)
```

The input event structure:

```
typedef struct {
    int     type;      -- OD_INPUT_CHAR or OD_INPUT_KEY
    int     remote;    -- from remote user?
    int32_t key;       -- Unicode codepoint or OD_KEY_* code
} od_input_t;
```

`int32_t` for the key value handles the full Unicode range on all
platforms including 16-bit DOS where `int` is only 16 bits.

## Session Control

```
od_kernel()                   -- cooperative processing tick
od_carrier()                  -- connection alive?
od_set_dtr(high)              -- DTR control (serial only)
od_sleep(ms)                  -- yield
od_time_remaining()           -- minutes remaining
od_chat()                     -- enter sysop chat mode
od_autodetect(flags)          -- detect ANSI / RIP version
od_statusline_update()        -- force status line refresh
```

## Windows and Menus (require ANSI)

```
od_popup_menu(title, items, left, top, flags)
od_window_create(left, top, right, bottom, title,
                 border_attr, title_attr, inside_attr)
od_window_remove(win)
```

## Command-Line Parser

Built-in parser with long and short options. Standard options:

```
-l, --local              Force local mode
-p, --port NUM           COM port number
-b, --baud NUM           Baud rate
-H, --handle NUM         Pre-opened handle
-d, --drop PATH          Drop file path
-n, --node NUM           Node number
-t, --time NUM           Max time (minutes)
-S, --socket             Handle is a socket
-h, --help               Show help
```

API:

```
int  od_parse_args(argc, argv)    -- parse and apply standard options
void od_on_arg(func)              -- callback for door-specific arguments
void od_cfg_help(text)            -- extra help text for --help output
```

The `od_on_arg` callback receives `(argc, argv, *i)` and may advance `*i`
to consume additional arguments. Returns 0 if handled, -1 if unrecognised.

## Error Handling

```
int         od_last_error(void)
const char *od_error_string(err)
```

Error codes are negative integers: `OD_OK` (0), `OD_ERR_MEMORY`,
`OD_ERR_NOANSI`, `OD_ERR_PARAM`, `OD_ERR_FILE`, `OD_ERR_NOREMOTE`,
`OD_ERR_GENERAL`, `OD_ERR_NOCARRIER`, `OD_ERR_TIMEOUT`.

## Status Line

One built-in personality that fills line 25 on local screens with user
name, time remaining, baud rate, and node number. A callback hook
(`od_on_statusline`) allows customisation. The default works without any
setup.

## Optional Modules

Linked only if used:

```
od_editor(buffer, bufsize, ...)       -- multi-line editor
od_config_load(filename, handler)     -- config file parser
od_reserve_init(shared_path)          -- named reservation setup
od_reserve_acquire(name, timeout_ms)  -- acquire named lock
od_reserve_release()                  -- release named lock
od_log(message)                       -- log file entry
od_spawn(cmdline)                     -- shell out
```

## IPC Protocol

An alternative to drop files and handle passing for modern platforms. The
BBS and door communicate over a local transport (Unix domain socket, TCP
loopback, Windows named pipe) using a simple framing protocol. The BBS
handles the user's actual connection (serial, telnet, SSH) and relays I/O
to the door. The door never touches the raw user transport.

This replaces drop files, socket handle passing, and carrier detect
polling. DOS doors continue to use FOSSIL + drop files; nobody is writing
new DOS BBS software.

### Framing

Raw I/O data flows in both directions with no framing. Control and
metadata messages are introduced by a NUL byte:

```
Raw I/O:    bytes that are not NUL, passed through as-is
Literal NUL: NUL NUL (escaped)
Message:     NUL + Type(1) + Length(2, big-endian) + Value(Length bytes)
```

NUL never appears in normal terminal I/O (ANSI sequences, text, user
input), so the common path has zero framing overhead. Both sides scan
outbound I/O for NUL bytes and double them.

Type 0x00 is not a valid message type: NUL followed by 0x00 is the
literal NUL escape sequence. The usable type space is 0x01-0xFF.

### Message Types

BBS to door:

| Type | Name           | Value                                 |
|------|----------------|---------------------------------------|
| 0x01 | SESSION        | Tagged values (sub-TLVs, see below)   |
| 0x02 | DISCONNECT     | Empty; user hung up                   |
| 0x03 | TIME_UPDATE    | New time remaining (minutes, 2 bytes) |
| 0x04 | TERM_RESIZE    | Rows (2 bytes) + Cols (2 bytes)       |

Door to BBS:

| Type | Name           | Value                                 |
|------|----------------|---------------------------------------|
| 0x81 | LOG            | Severity (1 byte) + message text      |

Severity values follow syslog levels (0=emergency through 7=debug).

### Session Message Tagged Values

The SESSION message value is a sequence of sub-TLVs:

```
Tag(1) + Length(2, big-endian) + Value(Length bytes)
```

Tags not recognised by the door are ignored. Tags not sent by the BBS
get default values. This naturally handles missing/optional fields
without the ambiguity of empty lines or the namespace collision risk
of ad-hoc key=value extension.

| Tag  | Name           | Type    | Default |
|------|----------------|---------|---------|
| 0x01 | USER_ID        | string  | (none)  |
| 0x02 | USER_NAME      | string  | (none)  |
| 0x03 | USER_ALIAS     | string  | (none)  |
| 0x04 | USER_LOCATION  | string  | (none)  |
| 0x05 | TIME_LEFT      | uint16  | 0       |
| 0x06 | SCREEN_ROWS    | uint16  | 24      |
| 0x07 | SCREEN_COLS    | uint16  | 80      |
| 0x08 | ANSI           | uint8   | 0       |
| 0x09 | RIP            | uint8   | 0       |
| 0x0A | ENCODING       | uint8   | 0 (CP437) |
| 0x0B | NODE           | uint16  | 1       |
| 0x0C | BAUD           | uint32  | 0       |
| 0x0D | CONNECT_SPEED  | uint32  | 0       |
| 0x0E | SECURITY       | uint16  | 0       |
| 0x0F | BBS_NAME       | string  | (none)  |
| 0x10 | SYSOP_NAME     | string  | (none)  |
| 0x11 | LOG_LEVELS     | uint8   | 0 (bitmask of accepted syslog levels) |

Multi-byte integers are big-endian.

### IPC Bindings

The protocol is transport-agnostic. Defined bindings:

- **Unix domain socket**: BBS listens, door connects. Path passed via
  command-line argument or environment variable.
- **TCP loopback**: BBS listens on localhost, door connects. Port passed
  via command-line argument.
- **Windows named pipe**: BBS creates pipe, door opens it. Pipe name
  passed via command-line argument.
- **Inherited fd (Unix)**: BBS forks door with an already-connected fd.
  Fd number passed via command-line argument.

### Relationship to Drop Files

The IPC protocol and drop files are independent paths to the same result.
The door library abstracts over both; the door author's code is identical
either way. `od_user_name()`,
[`od_printf()`](../reference/api/od_printf.md), and
[`od_carrier()`](../reference/api/od_carrier.md) all work the same regardless
of which path is active.

| Platform | Legacy path               | IPC path                      |
|----------|---------------------------|-------------------------------|
| DOS      | FOSSIL + drop file        | --                            |
| Win32    | Socket handle + drop file | Named pipe or TCP loopback    |
| Unix     | Socket/stdio + drop file  | Unix socket or inherited fd   |

## What Is Removed from V6

- Drop file formats: DORINFO?.DEF, EXITINFO.BBS (QBBS/RA 1.x/RA 2.x/
  QuickBBS 2.75+), CHAIN.TXT, SFDOORS.DAT, CALLINFO.BBS, TRIBBS.SYS,
  PCBOARD.SYS/USERS.SYS
- ~90 user fields mapped from dead BBS systems (RA menu stacks, EMSI
  session data, combined records, timelog data, event scheduling, etc.)
- RA/QBBS control code parsing (`^F` / `^K` sequences)
- Avatar terminal emulation
- FILES.BBS listing ([`od_list_files`](../reference/api/od_list_files.md))
- Four BBS personality modules (PCBoard, Remote Access, WildCat, standard)
- Registration key system
  ([`od_reg_key`](../reference/control/customization.md#od_reg_key),
  [`od_reg_name`](../reference/control/customization.md#od_reg_name))
- Memory-swapping spawn (`ODSwap.asm`)
- Internal UART ISR driver (replaced by FOSSIL-only on DOS)
- Win32 GUI frame window
- RIP graphics **sending** (detection and version reporting are kept)
- ~60 backward-compatibility `#define` aliases
- Implicit lazy initialization
- Return file / drop file write-back

## BBS Software Audit (August 2026)

Audited: Synchronet (local source), ENiGMA 1/2, Mystic, WWIV, Talisman,
x/84, GameSrv.

Formats generated by all modern BBSes: DOOR.SYS and DOOR32.SYS are
universal. CHAIN.TXT is common. Everything else is niche.

Confirmed gaps in all existing formats:

- Screen width (columns): only in CHAIN.TXT
- Character encoding: in no format
- RIP version: only in EXITINFO.BBS (binary)
- DOOR32.SYS has neither rows nor cols

Socket handle passing is platform-fragile: Node.js (ENiGMA 1/2) and
Python (x/84) cannot share socket descriptors, writing placeholders (-1
or 0). Talisman warns against DOOR32.SYS on Linux due to stdio/socket
conflicts.

Command-line specifiers are a parallel channel: Synchronet (%H, %W, %>),
Mystic (%0), WWIV (%H, %Z).

Read-back is rare: only Synchronet does it thoroughly.
