# The `od_control` structure

`od_control` contains the information and settings associated with the current
OpenDoors session. It is available as an imported global and through
`od_control_get()`.

Some members are inputs which the door sets before `od_init()`. Others are
filled from the command line, configuration, or door-information file. A third
group changes as the call proceeds. The field pages identify the usual role;
when in doubt, set initialization options before calling any API function.

- [Connection and door information](connection.md)
- [Caller and system information](caller.md)
- [Runtime state and policy](runtime.md)
- [Customization and callbacks](customization.md)

The structure is packed for historical binary compatibility. Do not duplicate
its layout in another language from this documentation alone; bind the current
header or use a deliberately maintained foreign-function definition.
