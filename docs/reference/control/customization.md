# Customization and callbacks

OpenDoors exposes its built-in configuration, text, colors, local controls, and
lifecycle hooks through [`od_control`](index.md).

## Program and components

Set `od_prog_name`, `od_prog_version`, and `od_prog_copyright` before
initialization. `od_config_file`, `od_logfile`, and `od_mps` select optional
components; their filenames, callbacks, message tables, and component-specific
settings are stored in the adjacent `od_config_*`, `od_logfile_*`, and
personality fields. `od_reg_name` and `od_reg_key` remain solely for historical
source compatibility.

The exact component members are `od_config_filename`, `od_config_function`,
`od_config_callback`, `od_logfile_disable`, `od_logfile_name`,
`od_logfile_messages`, and the component selector pointers themselves.

## Command-line and lifecycle hooks

`od_cmd_line_flag_handler`, `od_cmd_line_handler`, `od_cmd_line_help_func`, and
`od_cmd_line_help` extend the standard parser. Lifecycle hooks include
`od_before_exit`, before/after chat and shell callbacks, configuration and help
callbacks, `od_ker_exec`, `od_local_input`, `od_time_msg_func`, and
`od_no_file_func`. Each callback executes synchronously; avoid blocking the
kernel indefinitely.

The chat and shell hook member names are `od_cbefore_chat`, `od_cafter_chat`,
`od_cbefore_shell`, and `od_cafter_shell`; the remaining general callbacks
include `od_help_callback` and `od_config_callback`.

## Local keys and personalities

`key_chat`, `key_dosshell`, `key_drop2bbs`, `key_hangup`, keyboard, time,
lockout, status, and sysop-next members select local function keys.
`od_hot_key`, `od_hot_function`, and `od_num_keys` add application-defined local
keys. `od_default_personality` chooses the initial local interface.

The complete built-in key fields are `key_keyboardoff`, `key_lesstime`,
`key_lockout`, `key_moretime`, `key_status`, and `key_sysopnext` in addition to
the four named above.

## Text, colors, and formatting

`od_cfg_text`, `od_cfg_lines`, `od_color_names`, `od_color_char`, and
`od_color_delimiter` control configuration parsing and inline color names.
`od_box_chars` supplies window borders. `od_full_color`, `od_full_put`,
`od_no_ra_codes`, and `od_default_rip_win` tune terminal output.

Prompt pointers such as `od_press_key`, `od_no_time`, `od_paging`, status text,
day/month names, chat text, and timeout messages may be replaced before they are
used. Color fields for chat, lists, windows, menus, and continuation prompts use
IBM-PC attributes.

The complete prompt set is `od_after_chat`, `od_after_shell`, `od_before_chat`,
`od_before_shell`, `od_chat_reason`, `od_continue`, `od_hanging_up`,
`od_exiting`, `od_help_text`, `od_help_text2`, `od_inactivity_timeout`,
`od_inactivity_warning`, `od_no_keyboard`, `od_no_sysop`, `od_no_response`,
`od_no_time`, `od_offline`, `od_paging`, `od_press_key`, `od_sending_rip`,
`od_status_line`, `od_sysop_next`, `od_time_left`, `od_time_warning`, and
`od_want_chat`. The `od_day` and `od_month` arrays supply localized names;
`od_continue_yes`, `od_continue_no`, and `od_continue_nonstop` supply response
keys.

Color members are `od_chat_color1`, `od_chat_color2`, `od_list_comment_col`,
`od_list_name_col`, `od_list_offline_col`, `od_list_size_col`,
`od_list_title_col`, `od_local_win_col`, `od_continue_col`, `od_menu_title_col`,
`od_menu_border_col`, `od_menu_text_col`, `od_menu_key_col`,
`od_menu_highlight_col`, and `od_menu_highkey_col`.

On Windows, `od_app_icon` and `od_cmd_show` customize the native local window.
