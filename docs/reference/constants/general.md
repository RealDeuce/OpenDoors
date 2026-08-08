# General and platform definitions

These definitions describe the library version and target, common logical
values, timeouts, and child-process modes.

## Version and paths

| Name | Meaning |
| --- | --- |
| `OD_VERSION` | OpenDoors API version encoded as hexadecimal `0x630` for version 6.30. |
| `DIRSEP` | Native directory separator as a character: `\\` on Windows and DOS, `/` on Unix-like systems. |
| `DIRSEP_STR` | Native directory separator as a one-character string. |

## Platform selection

Exactly one target macro is selected while compiling OpenDoors headers.

| Name | Meaning |
| --- | --- |
| `ODPLAT_WIN32` | A Win32-family target. |
| `ODPLAT_NIX` | A supported Unix-like target, including macOS. |
| `ODPLAT_DOS` | A 16-bit DOS target. |
| `OD_WIN32_STATIC` | Define this in a Windows consumer which links the static library instead of the DLL import library. |
| `OD_DLL` | The header selected DLL import/export declarations. Applications normally test this value rather than define it. |

The remaining macros preserve calling conventions, linkage, visibility, and
legacy far-pointer syntax in [`OpenDoor.h`](../api/index.md). Application code
should use them when reproducing a compatible declaration and should not
redefine them.

| Name | Purpose |
| --- | --- |
| `OD_NAMING_CONVENTION` | Adds C linkage to public declarations in C++ translation units. |
| `OD_EXPORT` | Marks a definition for DLL or shared-library export where required. |
| `OD_IMPORT` | Marks a declaration for DLL import or hidden visibility where required. |
| `ODCALL` | Calling convention for ordinary API functions. |
| `ODVCALL` | Calling convention for variadic API functions. |
| `OD_GLOBAL_CONV` | Historical platform convention applied to the exported global control object where required. |
| `ODAPIDEF` | Complete linkage and visibility prefix for API function declarations. |
| `OD_API_VAR_DEFN` | Linkage and visibility prefix for an exported global definition. |
| `OD_API_VAR_DECL` | Linkage and visibility prefix for an imported or exported global declaration. |
| `ODFAR` | Expands to `far` on 16-bit DOS and to nothing on flat-memory targets. |

## Logical values and timeouts

| Name | Meaning |
| --- | --- |
| `FALSE` | False value for [`BOOL`](../types.md). Defined only when the toolchain has not already supplied it. |
| `TRUE` | True value for [`BOOL`](../types.md). Defined only when the toolchain has not already supplied it. |
| `MAYBE` | Third state used by legacy tri-state options. |
| `OD_NO_TIMEOUT` | A [`tODMilliSec`](../types.md) value requesting an indefinite wait. |

## Child-process modes

[`od_spawnvpe()`](../api/od_spawnvpe.md) accepts the platform `P_WAIT` and
`P_NOWAIT` modes. OpenDoors defines them where the C runtime does not.

| Name | Meaning |
| --- | --- |
| `P_WAIT` | Suspend normal OpenDoors processing and wait for the child to finish. This is the only supported DOS mode. |
| `P_NOWAIT` | Start the child without waiting where the target supports asynchronous spawning. |
| `CURRENT`, `IRET` | Legacy DOS swapping implementation markers exposed by the historical header; they are not spawn modes for application use. |
