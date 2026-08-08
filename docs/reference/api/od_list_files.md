# `od_list_files()`

Lists files in a particular file area (using FILES.BBS)

## Synopsis

```c
BOOL od_list_files(char *pszFileSpec);
```

## Return value

`TRUE` when the listing was opened and display completed or was stopped by the
user; `FALSE` when the directory or listing file could not be opened.

## Description

This function displays a list of files available for download from a file
area. `pszFileSpec` may name either a directory containing `FILES.BBS` or a
particular `FILES.BBS`-compatible index file. `NULL` and the empty string select
`FILES.BBS` in the current directory. Thus, to list the files available for
download in `C:\BBS\FILES\UPLOADS`, use:

```c
od_list_files("C:\\BBS\\FILES\\UPLOADS");
```

OpenDoors uses a third-generation FILES.BBS format, that is compatible with other FILES.BBS formats, but adds some additional features. Each line in the FILES.BBS file lists a filename, along with it's description. Thus, a typical FILES.BBS file might look as follows:

```text
PKZ110.EXE    PKZip file compressor, version 1.10
ODOORS60.ZIP  The newest version of OpenDoors
REC*.ZIP      A Record file
C:\BBS\*.*    All BBS files.
```

For each matching file, OpenDoors displays its name, size and description. If
no file matches an entry, the configurable
[`od_control.od_offline`](../control/customization.md#od_offline) string is
displayed in place of the size. A line whose first character is a space is
treated as a title or comment and displayed without filename processing. Thus,
the index could contain:

```text
   NEWEST UPLOADS
   ~~~~~~~~~~~~~~
PKZ110.EXE    PKZip file compressor, version 1.10
ODOORS60.ZIP  The newest version of OpenDoors
REC*.ZIP      A Record file
C:\BBS\*.*    All BBS files.
```

In addition to the standard `FILES.BBS` format, OpenDoors permits wildcards in
the filename field, such as `FNEWS???.*`. Every matching file is displayed with
the description from that entry. A filename may include a full directory path,
allowing one index to list files from several directories. A relative filename
is resolved relative to the directory named by `pszFileSpec`, or relative to
the directory containing the explicitly named index file.

The display uses a 12-character filename column, a six-character decimal size
column, and up to 56 characters of description. Input lines are read in
511-character pieces. If a physical line exceeds that size, the remainder of
that line is discarded rather than interpreted as another entry.

You may alter the colors used for the different portions of the listing with
[`od_control.od_list_title_col`](../control/customization.md#od_list_title_col),
[`od_control.od_list_name_col`](../control/customization.md#od_list_name_col),
[`od_control.od_list_size_col`](../control/customization.md#od_list_size_col),
[`od_control.od_list_comment_col`](../control/customization.md#od_list_comment_col),
and [`od_control.od_list_offline_col`](../control/customization.md#od_list_offline_col).

Page pausing initially follows
[`od_control.od_page_pausing`](../control/runtime.md#od_page_pausing). When an
extended door-information record is active, bit `0x04` of
[`od_control.user_attribute`](../control/caller.md#user_attribute) supplies the
caller's pausing preference instead. The configured pause and stop keys are
honored while the listing is displayed. Stopping a listing is considered a
successful completion and returns `TRUE`.

If the directory, index file, or initial directory entry cannot be opened,
OpenDoors returns `FALSE` and sets
[`od_control.od_error`](../control/runtime.md#od_error) to
[`ERR_FILEOPEN`](../constants/errors.md#err_fileopen). A later `fgets()` failure
is treated as end of file and is not reported separately.

Two current implementation defects require care when producing an index. A
blank line containing only LF can cause a read before the line buffer; use an
indented blank/comment line or a CRLF line ending instead. In addition,
`pszFileSpec` is copied into a 100-byte internal directory buffer without a
length check. These defects are recorded in the project `TODO.md` and are not
portable interface guarantees.

## See also

[`od_send_file()`](od_send_file.md), [The `od_control`
structure](../control/index.md)
