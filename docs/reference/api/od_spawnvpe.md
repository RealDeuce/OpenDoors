# `od_spawnvpe()`

Runs a child program with explicit arguments and environment and returns its
exit status.

## Synopsis

```c
INT16 od_spawnvpe(INT16 nModeFlag, const char *pszPath,
    const char *const papszArg[], const char *const papszEnv[]);
```

`nModeFlag` selects [`P_WAIT`](../constants/general.md) to suspend OpenDoors
until the child exits, or [`P_NOWAIT`](../constants/general.md) to continue
where the target supports asynchronous spawning. DOS accepts only the waiting
mode; another mode fails with [`ERR_PARAMETER`](../constants/errors.md).
`pszPath` names the executable. `papszArg` and `papszEnv` are null-terminated
argument and environment arrays; the environment pointer may be null where the
current environment should be inherited.

OpenDoors searches the path as supported by the target and restores its local
state after a waited-for child. The function returns the platform spawn result
on success—the child's exit status when waiting—and `-1` on failure.

## See also

[`od_spawn()`](od_spawn.md)
