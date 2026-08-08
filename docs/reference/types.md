# Types and callbacks

[`OpenDoor.h`](api/index.md) defines fixed-width types where a protocol or
binary interface requires them and native-width types where the host's natural
integer is more appropriate.

## Primitive types

| Type | Meaning |
| --- | --- |
| `BYTE`, `INT8` | Unsigned and signed 8-bit values. |
| `WORD`, `INT16` | Unsigned and signed 16-bit values. |
| `DWORD`, `INT32` | Unsigned and signed 32-bit values. |
| `DWORD_PTR` | Unsigned value large enough to preserve a pointer. |
| `INT`, `UINT` | Native integers, at least 16 bits. |
| `BOOL` | Boolean value used with `TRUE` and `FALSE`. |
| `tODMilliSec` | Millisecond interval stored as a `DWORD`; [`OD_NO_TIMEOUT`](constants/general.md) means no deadline. |

## Input events

```c
typedef enum {
    EVENT_CHARACTER,
    EVENT_EXTENDED_KEY
} tODInputEventType;

typedef struct {
    tODInputEventType EventType;
    BOOL bFromRemote;
    char chKeyPress;
} tODInputEvent;
```

For [`EVENT_CHARACTER`](constants/input.md), `chKeyPress` is the received byte.
For [`EVENT_EXTENDED_KEY`](constants/input.md), it is an
[`OD_KEY_*`](constants/input.md) code. `bFromRemote` distinguishes remote input
from an enabled local keyboard.

## Multi-line editor options

`tODEditOptions` describes the edit rectangle, text format, optional menu and
reallocation callbacks, flags, and the final buffer returned by
[`od_multiline_edit()`](api/od_multiline_edit.md). Initialize every member
before use. `TextFormat` is one of
[`FORMAT_PARAGRAPH_BREAKS`](constants/input.md),
[`FORMAT_LINE_BREAKS`](constants/input.md),
[`FORMAT_FTSC_MESSAGE`](constants/input.md), or
[`FORMAT_NO_WORDWRAP`](constants/input.md).

The menu callback returns [`EDIT_MENU_DO_NOTHING`](constants/input.md) or
[`EDIT_MENU_EXIT_EDITOR`](constants/input.md). The buffer callback receives the
previous buffer and requested size and must obey the same ownership rules as
`realloc()`.

## Personalities and components

`OD_PERSONALITY_PROC` is a far-compatible callback receiving a
[`PEROP_*`](constants/components.md) operation. `OD_COMPONENT` is the
initialization-function type used by the configuration, logging, and
multiple-personality component selectors.

Function pointer fields in [`od_control`](control/index.md) use the signatures
shown in the structure declaration. Callbacks run synchronously inside OpenDoors
and must not retain pointers to temporary arguments unless their individual
contract permits it.
