# Configuration and command lines

OpenDoors can receive settings from the BBS door-information file, its standard
command line, a configuration file, and assignments made by the door itself.
The exact source is less important than timing: settings which control
initialization must be established before `od_init()`.

On Unix-like and DOS programs, pass `argc` and `argv` to
`od_parse_cmd_line()`. The Windows form accepts the command-line string supplied
to `WinMain`. Programs which need to split such a string independently may use
`od_split_cmd_line()` and must later release its result with
`od_free_split_cmd_line()`.

The standard parser recognizes OpenDoors options and can pass application
options to callbacks installed in `od_control`. The configuration component is
selected through `od_control.od_config_file`; `INCLUDE_CONFIG_FILE` enables the
built-in component, while `NO_CONFIG_FILE` disables it. Set
`od_control.od_config_filename` to override the default name and use
`od_control.od_config_function` for program-specific keywords.

Door-information files supply the caller, system, connection, and time-limit
state used to populate `od_control`. OpenDoors recognizes several established
BBS formats as well as Door32 and socket-oriented operation. Programs should
read the normalized `od_control` fields rather than parse the original file a
second time.
