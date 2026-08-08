# `od_list_files()`

Displays entries from a BBS-style file listing.

## Synopsis

```c
BOOL od_list_files(char *pszFileSpec);
```

`pszFileSpec` identifies a directory containing `FILES.BBS` or the full path to
a compatible listing. A null or empty string selects the current directory.
OpenDoors formats names, sizes, descriptions, and offline indications with the
configured list colors, and honors the list pause and stop keys.

The function returns true when the listing completes. A directory or listing
which cannot be opened is reported through
[`od_control.od_error`](../control/runtime.md).

## See also

[`od_send_file()`](od_send_file.md), [The `od_control`
structure](../control/index.md)
