# `od_log_write()`

Function to write an entry to the log file

## Synopsis

```c
BOOL od_log_write(const char *pszMessage);
```

## Return value

`TRUE` if logging is disabled or a log stream is available; `FALSE` if
OpenDoors could not open the configured log file.

## Description

This function writes an application entry to the OpenDoors activity log. If
the log has not already been opened, `od_log_write()` first calls
[`od_log_open()`](od_log_open.md). If
[`od_control.od_logfile_disable`](../control/customization.md#od_logfile_disable)
is `TRUE`, it performs no file operation and returns `TRUE`.

To create an entry, pass the text to be written in `pszMessage`. Do not include
line endings or other control characters. OpenDoors prefixes the message with
the current local time and appends the newline required by the log format. A
message of no more than 67 characters keeps the complete entry below 80
columns. `pszMessage` must not be `NULL`.

Log file entries do not usually contain periods or other punctuation at the end of the line. Also, log file entries are usually written in the present tense. The first character of the entry is usually upper-case, with all other entries in lower case. Also, since excessive numbers or lengths of log file entries can quickly use a lot of disk space, it is best to think carefully about what events should be recorded in the log file. It is also a good idea to minimize the number of words used in the entry, without being too cryptic. As an example, "User entering options menu" should be used instead of "user entered the options menu."

## Examples

Calling the `od_log_write()` function is as simple as follows:

```c
od_log_write("Awarding user 5 additional minutes");
```

The current implementation does not check the result of `fprintf()`. Once the
file has been opened successfully, the function returns `TRUE` even if the
individual entry could not be written.

## See also

[`od_log_open()`](od_log_open.md), [Log-file system](../../guides/logging.md),
[`od_init()`](od_init.md)
