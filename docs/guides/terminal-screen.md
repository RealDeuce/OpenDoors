# Terminal and screen model

An OpenDoors session may have two displays: the remote user's terminal and the
local operator's OpenDoors screen. They are related, but they are not assumed
to have the same dimensions.

High-level display functions maintain a virtual representation of the remote
screen. Its size is established from `od_control.user_screenwidth` and
`od_control.user_screen_length`, subject to the legacy local-screen minimum.
The local display shows the portion which fits in its own output area. Content
outside that local area remains available to remote screen operations such as
`od_gettext()` and the extended save/restore functions.

Direct calls to the lower-level `ODScrn` interface affect the local display
only. They do not modify the remote screen model. This prevents local status
or operator output from leaking into data later returned or restored by the
high-level screen APIs.

## Display modes

Plain text output works for every connection. Functions which position the
cursor, manipulate rectangular screen areas, draw windows, or scroll arbitrary
regions require ANSI or AVATAR terminal support unless their individual page
states otherwise. The current mode is reported through fields such as
`od_control.user_ansi`, `user_avatar`, and `user_rip`.

Coordinates are one-based. Functions which accept a rectangle use inclusive
left, top, right, and bottom edges.

## Raw and interpreted output

`od_disp_str()` and `od_printf()` are the normal choices for text.
`od_disp_emu()` interprets terminal escape sequences while updating the screen
model. `od_disp()` sends an explicit byte count and can suppress local echo;
raw output sent without local echo cannot reliably be reconstructed and is not
added to the virtual screen.

The legacy `od_save_screen()` interface retains its fixed 80-column save
format for source and binary compatibility. New code which needs the complete
remote screen should obtain the required size with `od_save_screen_size()` and
use `od_save_screen_ex()` and `od_restore_screen_ex()`.
