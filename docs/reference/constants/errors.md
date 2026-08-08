# Errors and exit levels

## API errors

The most recent API error is stored in
[`od_control.od_error`](../control/runtime.md). A successful function does not
necessarily clear an earlier error, so consult each function's return contract
before reading this field.

| Name | Meaning |
| --- | --- |
| `ERR_NONE` | No error has been reported. |
| `ERR_MEMORY` | Required memory could not be allocated. |
| `ERR_NOGRAPHICS` | The operation requires ANSI, AVATAR, or RIP support. |
| `ERR_PARAMETER` | An argument or saved object is invalid. |
| `ERR_FILEOPEN` | A required file could not be opened. |
| `ERR_LIMIT` | A platform, representation, or internal limit was exceeded. |
| `ERR_FILEREAD` | A required file could not be read. |
| `ERR_NOREMOTE` | The operation requires a remote session and was called locally. |
| `ERR_GENERALFAILURE` | The operation failed without a more specific diagnosis. |
| `ERR_NOTHINGWAITING` | Requested data was not ready. |
| `ERR_NOMATCH` | A requested named object was not found. |
| `ERR_UNSUPPORTED` | The operation is unavailable on this target or build. |

## Configured process exit levels

[`od_control.od_errorlevel`](../control/runtime.md) is an eight-element array.
Element `ERRORLEVEL_ENABLE` controls whether the remaining elements override
the default process exit levels.

| Index | Meaning |
| --- | --- |
| `ERRORLEVEL_ENABLE` | Boolean enable flag for custom exit levels. |
| `ERRORLEVEL_CRITICAL` | Exit level for a fatal OpenDoors error. |
| `ERRORLEVEL_NOCARRIER` | Exit level when carrier is lost. |
| `ERRORLEVEL_HANGUP` | Exit level when the local operator terminates the call. |
| `ERRORLEVEL_TIMEOUT` | Exit level when the caller's time expires. |
| `ERRORLEVEL_INACTIVITY` | Exit level for an inactivity timeout. |
| `ERRORLEVEL_DROPTOBBS` | Exit level when the local operator returns the caller to the BBS. |
| `ERRORLEVEL_NORMAL` | Exit level for normal termination. |
