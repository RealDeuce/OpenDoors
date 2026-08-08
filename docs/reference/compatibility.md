# Compatibility interfaces

OpenDoors retains several names and entry points so established source and
binary consumers continue to build.

British and American spellings such as
[`od_set_colour`](api/od_set_color.md)/[`od_set_color`](api/od_set_color.md) and
the corresponding [`od_control`](control/index.md) field aliases refer to the
same interface. Older global-style configuration names are macros which select
members of [`od_control`](control/index.md).

`od_init_with_config` is an obsolete macro which assigns the built-in
configuration component and then calls [`od_init()`](api/od_init.md). New code
should make those assignments explicitly. `od_log_open()` and `od_emulate()` are
obsolete public entry points retained for older applications; use the logging
component and [`od_disp_emu()`](api/od_disp_emu.md) in new work.

Optional component routines (`ODConfigInit`, `ODLogEnable`, and `ODMPSEnable`)
and built-in personality procedures are normally selected through the
[`INCLUDE_*`](constants.md) and [`PER_*`](constants.md) macros rather than
called directly.

The lower-level module headers are installed because historical programs may
include them. Their interfaces are not covered by the high-level stability and
reference guarantees described on this site.
