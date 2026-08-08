# `od_free_split_cmd_line()`

Releases an argument array returned by `od_split_cmd_line()`.

## Synopsis

```c
void od_free_split_cmd_line(char **papszArguments);
```

Pass the returned array unchanged and exactly once. After this call neither the
array nor any pointer within it may be used. Passing `NULL` sets
`od_control.od_error` to `ERR_PARAMETER`. The function returns no value.

## See also

[`od_split_cmd_line()`](od_split_cmd_line.md),
[`od_parse_cmd_line()`](od_parse_cmd_line.md)
