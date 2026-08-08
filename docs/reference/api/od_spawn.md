# `od_spawn()`

Temporarily suspends the OpenDoors display and runs another program.

## Synopsis

```c
BOOL od_spawn(const char *pszCommandLine);
```

`pszCommandLine` contains the program and its arguments. OpenDoors prepares the
local display, applies the configured time-freezing and swapping behavior,
runs the child, and restores the door screen afterward.

The function returns true when the child was started and completed through the
quick-spawn path. Invalid input, an unsupported target, or process creation
failure is reported through `od_control.od_error`.

## See also

[`od_spawnvpe()`](od_spawnvpe.md), [`od_save_screen()`](od_save_screen.md)
