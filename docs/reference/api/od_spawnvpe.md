# `od_spawnvpe()`

Runs a child program with explicit arguments and environment and returns its
exit status.

## Synopsis

```c
INT16 od_spawnvpe(INT16 nModeFlag, const char *pszPath,
    const char *const papszArg[], const char *const papszEnv[]);
```

`nModeFlag` selects the spawn mode, normally the platform's wait mode.
`pszPath` names the executable. `papszArg` and `papszEnv` are null-terminated
argument and environment arrays; the environment pointer may be null where the
current environment should be inherited.

OpenDoors searches the path as supported by the target and restores its local
state after the child. The function returns the child's exit status on success
and `-1` on failure.

## See also

[`od_spawn()`](od_spawn.md)
