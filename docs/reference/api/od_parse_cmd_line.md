# `od_parse_cmd_line()`

Processes the standard OpenDoors command-line options.

## Synopsis

```c
/* Windows */
void od_parse_cmd_line(LPSTR pszCmdLine);

/* Other targets */
void od_parse_cmd_line(INT nArgCount, char *papszArguments[]);
```

Call this function before [`od_init()`](od_init.md). The Windows argument is the
string received by `WinMain`; other programs pass `argc` and `argv` from `main`.

Recognized options update initialization settings. Unknown application options
can be handled through
[`od_control.od_cmd_line_flag_handler`](../control/customization.md) and
[`od_control.od_cmd_line_handler`](../control/customization.md); help text and
callbacks are also configurable. The function returns no value.

## See also

[`od_split_cmd_line()`](od_split_cmd_line.md), [`od_init()`](od_init.md),
[Configuration and command lines](../../guides/configuration.md)
