# `od_split_cmd_line()`

Splits a command-line string into an `argc`/`argv`-style argument array.

## Synopsis

```c
char **od_split_cmd_line(const char *pszCmdLine, INT *nArgCount);
```

## Parameters

`pszCmdLine`
: Pointer to the nul-terminated command-line text to split. This is the text
  containing the arguments, not an existing `argv` array.

`nArgCount`
: Pointer to an [`INT`](../types.md#int) which receives the number of entries
  in the returned array.

## Return value

On success, returns a dynamically allocated, null-terminated array of pointers
to dynamically allocated argument text. `*nArgCount` receives the number of
arguments, not counting the final null pointer.

On failure, returns `NULL` and sets `*nArgCount` to 0 when `nArgCount` itself
is valid.

## Description

The Windows form of [`od_parse_cmd_line()`](od_parse_cmd_line.md) receives the
single command-line string passed to `WinMain`, while the other platform forms
receive the `argc` and `argv` values passed to `main`. [`od_split_cmd_line()`](od_split_cmd_line.md)
provides the conversion from a single string to the latter representation. It
is public so applications which need the same conversion can use OpenDoors'
rules rather than duplicating them.

The first returned element represents the program-name position normally found
in `argv[0]`. On Windows, OpenDoors attempts to recover the executable portion
from the process command line. If it cannot do so unambiguously, the first
element is an empty string. On non-Windows platforms the first element is an
empty string because the supplied string contains no program name. The words
read from `pszCmdLine` begin at element 1.

Arguments are separated by one or more characters for which the C library
`isspace()` function returns nonzero. Consecutive whitespace is discarded and
does not create empty arguments. An empty or all-whitespace command string is
valid and produces only the element in the `argv[0]` position.

This is deliberately a simple splitter. Quote marks, backslashes, and other
shell metacharacters have no special meaning. For example:

```text
-name "Jane Smith" -local
```

is split into `-name`, `"Jane`, `Smith"`, and `-local`; the quote characters
remain in the two affected arguments. The function does not implement the
Microsoft C runtime quoting algorithm, a Unix shell grammar, variable
expansion, wildcard expansion, or command substitution. A caller which needs
one of those grammars must parse it separately.

The array contains at most 4,096 entries including the program-name position.
If the input contains more words, only the arguments which fit are returned.
The final array entry after the counted arguments is always `NULL`, permitting
code which expects a conventional null-terminated `argv` array.

The returned storage has an internal shared layout. Some argument pointers
refer into one common copy of the command string and must not be freed or
reallocated individually. When finished, pass the original array pointer to
[`od_free_split_cmd_line()`](od_free_split_cmd_line.md). Do not use `free()`
on the individual entries.

The function does not modify `pszCmdLine`. It does not initialize OpenDoors or
run the OpenDoors kernel, and may be used while preparing pre-initialization
settings.

## Errors

The function reports the following errors through
[`od_control.od_error`](../control/runtime.md#od_error):

- [`ERR_PARAMETER`](../constants/errors.md#err_parameter) if `pszCmdLine` or
  `nArgCount` is `NULL`.
- [`ERR_MEMORY`](../constants/errors.md#err_memory) if storage for the pointer
  array, program-name element, or command-line copy cannot be allocated.

When `nArgCount` is non-null, it is set to 0 for either failure. The caller must
not call [`od_free_split_cmd_line()`](od_free_split_cmd_line.md) when the return
value is `NULL`.

## Example

This example obtains an argument array and passes it to code shared with a
normal `main()` implementation:

```c
static void process_arguments(INT argc, char *argv[])
{
    INT index;

    for(index = 1; index < argc; ++index)
        od_log_write(argv[index]);
}

static BOOL process_command_string(const char *command_line)
{
    INT argc;
    char **argv = od_split_cmd_line(command_line, &argc);

    if(argv == NULL)
        return FALSE;

    process_arguments(argc, argv);
    od_free_split_cmd_line(argv);
    return TRUE;
}
```

Use [`od_parse_cmd_line()`](od_parse_cmd_line.md) directly when the objective
is simply to process the standard OpenDoors command-line options.

## See also

[`od_free_split_cmd_line()`](od_free_split_cmd_line.md),
[`od_parse_cmd_line()`](od_parse_cmd_line.md)
