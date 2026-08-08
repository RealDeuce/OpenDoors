# Configuration and command lines

OpenDoors can receive settings from the BBS door-information file, its standard
command line, a configuration file, and assignments made by the door itself. The
exact source is less important than timing: settings which control
initialization must be established before
[`od_init()`](../reference/api/od_init.md).

On Unix-like and DOS programs, pass `argc` and `argv` to
[`od_parse_cmd_line()`](../reference/api/od_parse_cmd_line.md). The Windows form
accepts the command-line string supplied to `WinMain`. Programs which need to
split such a string independently may use
[`od_split_cmd_line()`](../reference/api/od_split_cmd_line.md) and must later
release its result with
[`od_free_split_cmd_line()`](../reference/api/od_free_split_cmd_line.md).

The standard parser recognizes OpenDoors options and can pass application
options to callbacks installed in [`od_control`](../reference/control/index.md).
The configuration component is selected through
[`od_control.od_config_file`](../reference/control/customization.md);
[`INCLUDE_CONFIG_FILE`](../reference/constants/components.md) enables the built-in
component, while
[`NO_CONFIG_FILE`](../reference/constants/components.md) disables it. Set
[`od_control.od_config_filename`](../reference/control/customization.md) to
override the default name and use
[`od_control.od_config_function`](../reference/control/customization.md) for
program-specific keywords.

Door-information files supply the caller, system, connection, and time-limit
state used to populate [`od_control`](../reference/control/index.md). OpenDoors
recognizes several established BBS formats as well as Door32 and socket-oriented
operation. Programs should read the normalized
[`od_control`](../reference/control/index.md) fields rather than parse the
original file a second time.
