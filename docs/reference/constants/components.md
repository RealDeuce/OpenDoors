# Components and personalities

## Optional components

Assign these values to the corresponding component fields before
[`od_init()`](../api/od_init.md).

| Enable | Disable | Component |
| --- | --- | --- |
| `INCLUDE_CONFIG_FILE` | `NO_CONFIG_FILE` | Built-in configuration-file reader. |
| `INCLUDE_LOGFILE` | `NO_LOGFILE` | Built-in session logger. |
| `INCLUDE_MPS` | `NO_MPS` | Multiple-personality system. |

The enabled values select [`ODConfigInit`](../compatibility.md),
[`ODLogEnable`](../compatibility.md), and
[`ODMPSEnable`](../compatibility.md) respectively. The disabled values are null
component pointers.

## Built-in personalities

| Name | Personality |
| --- | --- |
| `PER_OPENDOORS` | Native OpenDoors status and key behavior. |
| `PER_PCBOARD` | PCBoard-compatible behavior. |
| `PER_RA` | RemoteAccess-compatible behavior. |
| `PER_WILDCAT` | Wildcat!-compatible behavior. |

Pass one of these identifiers to
[`od_add_personality()`](../api/od_add_personality.md), or install the procedure
through [`od_control.od_default_personality`](../control/customization.md).

## Personality callback operations

OpenDoors invokes an [`OD_PERSONALITY_PROC`](../types.md) with one of these
operations:

| Name | Meaning |
| --- | --- |
| `PEROP_DISPLAY1` | Draw status display 1. |
| `PEROP_DISPLAY2` | Draw status display 2. |
| `PEROP_DISPLAY3` | Draw status display 3. |
| `PEROP_DISPLAY4` | Draw status display 4. |
| `PEROP_DISPLAY5` | Draw status display 5. |
| `PEROP_DISPLAY6` | Draw status display 6. |
| `PEROP_DISPLAY7` | Draw status display 7. |
| `PEROP_DISPLAY8` | Draw status display 8. |
| `PEROP_UPDATE1` | Update dynamic content for display 1. |
| `PEROP_UPDATE2` | Update dynamic content for display 2. |
| `PEROP_UPDATE3` | Update dynamic content for display 3. |
| `PEROP_UPDATE4` | Update dynamic content for display 4. |
| `PEROP_UPDATE5` | Update dynamic content for display 5. |
| `PEROP_UPDATE6` | Update dynamic content for display 6. |
| `PEROP_UPDATE7` | Update dynamic content for display 7. |
| `PEROP_UPDATE8` | Update dynamic content for display 8. |
| `PEROP_INITIALIZE` | Install the personality's keys and initial state. |
| `PEROP_CUSTOMKEY` | Handle the custom key reported in [`od_control.od_last_hot`](../control/runtime.md). |
| `PEROP_DEINITIALIZE` | Remove personality-specific state and keys. |
