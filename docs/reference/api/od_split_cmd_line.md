# `od_split_cmd_line()`

Splits a command-line string into an `argc`/`argv`-style array.

## Synopsis

```c
char **od_split_cmd_line(const char *pszCmdLine, INT *nArgCount);
```

`pszCmdLine` is the string to split and `nArgCount` receives the number of
arguments. On success the returned array is null-terminated and its first item
represents the program name in the form available to the current platform.

The result is dynamically allocated and must be released only with
[`od_free_split_cmd_line()`](od_free_split_cmd_line.md). A null argument sets
[`ERR_PARAMETER`](../constants/errors.md); allocation failure sets
[`ERR_MEMORY`](../constants/errors.md) and returns `NULL`.

## See also

[`od_free_split_cmd_line()`](od_free_split_cmd_line.md),
[`od_parse_cmd_line()`](od_parse_cmd_line.md)
