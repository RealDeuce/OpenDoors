# `od_exit()`

The OpenDoors program termination function

## Synopsis

```c
void od_exit(INT nErrorLevel, BOOL bTermCall);
```

## Return value

N/A

## Description

Use this function whenever an initialized door is to terminate. Calling the C
`exit()` function directly bypasses OpenDoors cleanup. `od_exit()` updates time
accounting, invokes the application's before-exit callback, rewrites supported
door-information files, closes the activity log, optionally disconnects the
caller, shuts down the kernel and communications objects, restores the local
display, and terminates the process with `nErrorLevel`.

If `bTermCall` is `TRUE`, OpenDoors treats the operation as termination of the
call. For a remote session it waits up to ten seconds for pending output,
lowers DTR or performs the equivalent disconnect operation supplied by the
active communications method, waits up to five seconds for carrier to
disappear, and then raises DTR again where that operation is supported. The BBS
can then perform its normal logoff processing. A value of `FALSE` leaves the
remote connection available for the BBS which launched the door.

If your program must always perform work before exiting, such as updating or
closing data files, install an
[`od_control.od_before_exit`](../control/customization.md#od_before_exit)
callback. OpenDoors invokes it even when the library initiates termination,
such as after the caller hangs up. The callback runs after OpenDoors has
calculated the caller's remaining and used time and restored the original
drop-file baud value, but before any door-information file is rewritten or
communications resource is closed. A recursive call to `od_exit()` from that
callback is ignored.

Only formats for which OpenDoors has a rewrite implementation are updated. For
the extended RemoteAccess and QuickBBS `EXITINFO.BBS` variants, the fields
available in that record are copied back. Several supported text formats are
rewritten from the values retained during initialization and the current
values in [`od_control`](../control/index.md). Fields which are written for
each format are identified individually in the control-structure reference.
The current text-format rewrite paths do not safely report an output-file open
or write failure; this defect is recorded in `TODO.md`.

Setting [`DIS_INFOFILE`](../constants/session.md#dis_infofile) in
[`od_control.od_disable`](../control/customization.md#od_disable) before
initialization prevents both reading and later rewriting a door-information
file. Setting
[`od_control.od_noexit`](../control/customization.md#od_noexit) causes
`od_exit()` to perform the complete OpenDoors shutdown and then return instead
of terminating the process. In that case OpenDoors is no longer initialized,
and the application may continue or begin another session deliberately.

On non-Windows text-mode targets, the local output window is reset to the full
80-by-25 display and the attribute is reset to grey on black. If
[`od_control.od_clear_on_exit`](../control/customization.md#od_clear_on_exit) is
`TRUE`, the local display is cleared; otherwise the cursor is moved to the
upper-left corner. Platform display resources are then closed in either case.

If `od_exit()` is called before any other OpenDoors function, it first calls
[`od_init()`](od_init.md) so that the resources and session information needed
for an orderly shutdown exist. The function normally does not return. It can
return when `od_noexit` is set, when a recursive exit is suppressed, or when it
is running as part of an already active process-exit handler.

## Examples

The example below demonstrates a function which a door could execute when the user chooses to exit the door. This function will ask the user whether they wish to exit the door and return to the BBS, simply logoff of the BBS, or continue using the door. The example function will then call `od_exit()` if the user wishes to exit the door, or return control to the function which called it, if the user does not wish to exit:

```c
void goodbye(void)
{
   char pressed;

   od_disp_str("You have chosen to exit this door.\n\r");
   od_disp_str("Do you wish to:\n\r");
   od_disp_str("      [R]eturn to the BBS\n\r");
   od_disp_str("      [L]og off the BBS\n\r");
   od_disp_str("      [C]ontinue using the door\n\r");

   for(;;)
   {
      pressed = od_get_key(TRUE);

      if(pressed == 'R' || pressed == 'r')
         od_exit(40, FALSE);

      if(pressed == 'L' || pressed == 'l')
         od_exit(41, TRUE);

      if(pressed == 'C' || pressed == 'c')
         return;
   }
}
```

## See also

[`od_init()`](od_init.md), [`od_set_dtr()`](od_set_dtr.md),
[Session lifecycle](../../guides/session-lifecycle.md)
