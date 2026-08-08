# `od_spawnvpe()`

Runs a child program with an explicit argument vector and environment.

## Synopsis

```c
INT16 od_spawnvpe(INT16 nModeFlag, const char *pszPath,
    const char *const papszArg[], const char *const papszEnv[]);
```

## Parameters

`nModeFlag`
: [`P_WAIT`](../constants/general.md#p_wait) waits for the child to finish.
  [`P_NOWAIT`](../constants/general.md#p_nowait) requests asynchronous
  execution on targets which support it. The DOS implementations accept only
  `P_WAIT`.

`pszPath`
: Name or path of the executable. Path searching is platform dependent, as
  described below.

`papszArg`
: Null-terminated array of argument-string pointers. The platform launcher
  receives this array as the child's argument vector. Supply the executable
  name in element zero when required by the target program or C runtime.

`papszEnv`
: Null-terminated array of `name=value` environment strings, or `NULL` where
  the target implementation supports inheritance of the current environment.
  The Unix implementation is an important exception described below.

## Return value

For `P_WAIT`, DOS, DOS32 and Windows return the child result supplied by the
platform spawn runtime. `-1` reports a launch failure. The public return type
is [`INT16`](../types.md#int16), so a wider native result is narrowed.

For `P_NOWAIT`, the result is platform specific. The Microsoft runtime returns
a process handle, which is narrowed to `INT16` by this interface. The current
Unix implementation returns zero after calling `fork()`; it does not return a
process ID and also returns zero if `fork()` fails.

## Description

`od_spawnvpe()` is the vector-and-environment form of
[`od_spawn()`](od_spawn.md). Before a waited-for launch, OpenDoors drains
pending output, suspends the communications/kernel resources which cannot
remain active across the child, and saves the DOS local screen where
applicable. After the child finishes it reopens those resources, clears stale
input, restores the DOS screen and current directory, and adjusts the caller's
remaining-time accounting according to
[`od_spawn_freeze_time`](../control/customization.md#od_spawn_freeze_time).

The executable and environment rules are not uniform:

- On 16-bit DOS, OpenDoors searches the current directory and `PATH`. If the
  supplied name has no extension, it tries `.COM` and then `.EXE`. A null
  `papszEnv` inherits the parent DOS environment. DOS32 delegates the actual
  launch to the Open Watcom `spawnvpe()` runtime after applying the same
  OpenDoors wait-only and screen/session handling.
- On Windows, OpenDoors calls the Microsoft-compatible `_spawnvpe()` runtime.
  Its normal `PATH`, suffix, argument-vector, and null-environment rules apply.
- On Unix-like targets, the child calls `execve()` with `pszPath`, `papszArg`,
  and `papszEnv` exactly as supplied. There is no `PATH` search. OpenDoors does
  not substitute the process `environ` array when `papszEnv` is `NULL`; an
  application which wants inheritance must pass `environ` itself. With
  `P_NOWAIT`, OpenDoors also installs a process-wide `SIGCHLD` disposition
  with the `SIG_IGN` handler and `SA_NOCLDWAIT` flag, so the child is reaped
  automatically and cannot later be waited for through this interface. Any
  mode other than `P_WAIT` is treated as `P_NOWAIT` on this target.

The Unix `P_WAIT` implementation has a known process-handling defect. It sets
the process-wide `SIGCHLD` disposition to `SIG_IGN` before calling `waitpid()`,
does not check the result of `waitpid()`, and does not restore the previous
signal disposition. On systems which automatically reap children while
`SIGCHLD` is ignored, the wait fails and the function inspects an uninitialized
status value. A failed `fork()` is likewise not detected correctly. Until this
is corrected, applications must not depend on a meaningful Unix `P_WAIT`
result from this function.

On 16-bit DOS and DOS32, a mode other than `P_WAIT` returns `-1` and sets
[`od_control.od_error`](../control/runtime.md#od_error) to
[`ERR_PARAMETER`](../constants/errors.md#err_parameter). Allocation of the DOS
screen or directory buffers can fail with
[`ERR_MEMORY`](../constants/errors.md#err_memory). Other native launch failures
are represented by the return value and the platform runtime's error state;
this function does not translate every native failure into `od_error`.

## Example

This example supplies a complete vector and inherits the current environment
on the platforms which implement null-environment inheritance:

```c
const char *args[] = { "TEST.EXE", "--check", NULL };
INT16 result = od_spawnvpe(P_WAIT, "TEST.EXE", args, NULL);

if(result == -1)
    od_log_write("Unable to start TEST.EXE");
else
    od_printf("TEST.EXE returned %d\n\r", (int)result);
```

On Unix-like targets, use an executable path containing a slash and pass an
environment array, for example the process's `environ`, instead of using the
example's null `papszEnv`.

## See also

[`od_spawn()`](od_spawn.md), [Child-process modes](../constants/general.md#child-process-modes)
