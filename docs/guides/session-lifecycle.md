# Session lifecycle

OpenDoors separates settings which must be supplied before startup from state
which is discovered while the door is running. A typical program performs the
following operations:

1. Set program identity and any initialization options in `od_control`.
2. Parse the standard command line with `od_parse_cmd_line()`.
3. Call `od_init()`.
4. Perform input and output through the OpenDoors API.
5. Call `od_kernel()` during long periods in which no other API function is
   called.
6. Test `od_carrier()` when the application needs explicit connection-state
   handling.
7. Finish with `od_exit()`.

Most API calls initialize OpenDoors automatically. That convenience is useful
for small programs, but it also means that changing initialization settings
after an arbitrary API call may be too late. Set them first and initialize
explicitly.

`od_kernel()` services time limits, connection status, local function keys,
and other housekeeping. Normal API traffic invokes it as needed. A program
which spends a long time computing or waiting outside OpenDoors should call it
periodically or use `od_sleep()`, which continues to yield appropriately.

`od_exit()` performs the library shutdown work, including connection and door
information handling. Do not substitute the C library's `exit()` where an
orderly OpenDoors shutdown is required.
