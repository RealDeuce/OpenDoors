# DOS personalities

The DOS version of OpenDoors can replace its local status display and sysop
function keys as a unit. Each such unit is called a *personality*. OpenDoors
includes Standard, PCBoard, RemoteAccess, and Wildcat personalities; a door may
also supply its own.

Personality support is local to the DOS console. Output produced through the
personality screen functions is never sent to the caller's remote terminal.

## Writing a personality

Include [`OpenDoor.h`](../reference/api/index.md) and
[`ODStat.h`](../reference/personality/index.md), then define an
[`OD_PERSONALITY_PROC`](../reference/types.md). OpenDoors calls the procedure
with a [`PEROP_*`](../reference/constants/components.md#personality-callback-operations)
operation:

- [`PEROP_INITIALIZE`](../reference/constants/components.md#personality-callback-operations)
  establishes function keys and initial state.
- [`PEROP_DISPLAY1`](../reference/constants/components.md#personality-callback-operations)
  through [`PEROP_DISPLAY8`](../reference/constants/components.md#personality-callback-operations)
  draw complete displays.
- [`PEROP_UPDATE1`](../reference/constants/components.md#personality-callback-operations)
  through [`PEROP_UPDATE8`](../reference/constants/components.md#personality-callback-operations)
  update changing fields.
- [`PEROP_CUSTOMKEY`](../reference/constants/components.md#personality-callback-operations)
  handles the key in
  [`od_control.od_last_hot`](../reference/control/runtime.md).
- [`PEROP_DEINITIALIZE`](../reference/constants/components.md#personality-callback-operations)
  removes keys and releases personality state.

Install custom keys with
[`ODStatAddKey()`](../reference/personality/ODStatAddKey.md), and remove every
installed key with
[`ODStatRemoveKey()`](../reference/personality/ODStatRemoveKey.md) during
deinitialization. After changing a value displayed by the current status line,
[`ODStatForceStatusUpdate()`](../reference/personality/ODStatForceStatusUpdate.md)
requests its incremental update operation.

## Drawing the local display

The output procedures in [`ODStat.h`](../reference/personality/index.md) use
one-based local-screen coordinates. During a display or update callback,
OpenDoors permits access to the complete 80 by 25 DOS screen and restores the
door's output boundary afterward.

[`ODScrnSetAttribute()`](../reference/personality/ODScrnSetAttribute.md) and
[`ODScrnSetCursorPos()`](../reference/personality/ODScrnSetCursorPos.md) select
the output settings. Text can then be written with
[`ODScrnDisplayChar()`](../reference/personality/ODScrnDisplayChar.md),
[`ODScrnDisplayBuffer()`](../reference/personality/ODScrnDisplayBuffer.md),
[`ODScrnDisplayString()`](../reference/personality/ODScrnDisplayString.md), or
[`ODScrnPrintf()`](../reference/personality/ODScrnPrintf.md).

Rectangles can be cached with
[`ODScrnGetText()`](../reference/personality/ODScrnGetText.md) and restored with
[`ODScrnPutText()`](../reference/personality/ODScrnPutText.md). Their buffers
contain row-major character/attribute pairs and therefore require
`2 * width * height` bytes. Both corners are inclusive.

[`szStatusText`](../reference/personality/szStatusText.md) is the traditional
80-byte shared work area. A personality may instead use its own storage.

## Selecting a personality

To use one personality, assign its procedure to
[`od_control.od_default_personality`](../reference/control/customization.md)
before initialization. To permit runtime selection, set
[`od_control.od_mps`](../reference/control/customization.md) to
[`INCLUDE_MPS`](../reference/constants/components.md), register additional
procedures with
[`od_add_personality()`](../reference/api/od_add_personality.md), and select one
with [`od_set_personality()`](../reference/api/od_set_personality.md).

The output bounds passed to [`od_add_personality()`](../reference/api/od_add_personality.md)
describe the portion reserved for normal door output. For example, lines 1
through 23 leave lines 24 and 25 for the personality.
