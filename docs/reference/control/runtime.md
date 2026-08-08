# Runtime state and policy

These members control or report the active session.

| Members | Description |
| --- | --- |
| `od_always_clear`, `od_clear_on_exit` | Screen-clearing policy. |
| `od_force_local`, `od_silent_mode` | Local-session and local-display behavior. |
| `od_chat_active`, `od_current_statusline`, `od_status_on` | Current chat and status-display state. |
| `od_error` | Most recent [`ERR_*`](../constants/errors.md) API error. |
| `od_last_input`, `od_last_hot` | Most recent ordinary and custom hotkey input. |
| `od_maxtime`, `od_maxtime_deduction` | Door-specific time cap and adjustment. |
| `od_okaytopage`, `od_pagestartmin`, `od_pageendmin` | Paging availability and hours. |
| `od_page_pausing`, `od_page_len`, `od_page_statusline` | Display-file paging behavior. |
| `od_user_keyboard_on` | Whether local operator keystrokes enter the door input queue. |
| `od_update_status_now`, `od_cur_attrib` | Pending status refresh and current text attribute. |
| `od_disable`, `od_disable_dtr`, `od_disable_inactivity` | Masks or conditions which disable automatic behaviors. |
| `od_inactivity`, `od_inactive_warning` | Inactivity timeout and warning interval. |
| `od_max_key_latency` | Maximum wait used while resolving multi-byte terminal key sequences. |
| `od_noexit`, `od_nocopyright`, `od_internal_debug` | Exit, startup display, and diagnostic controls. |
| `od_spawn_freeze_time`, `od_swapping_disable`, `od_swapping_noems`, `od_swapping_path` | Child-process and legacy DOS swapping policy. |
| `event_*` | Pending BBS event status, schedule, days, and exit level. |
| `od_errorlevel` | Exit levels indexed by the [`ERRORLEVEL_*`](../constants/errors.md) constants. |
| `od_logfile_disable`, `od_logfile_name` | Runtime log enablement and selected log filename. |
| `od_emu_simulate_modem` | Enables modem-style emulation behavior where supported. |
| `od_in_buf_size` | Requested communications input-buffer size. |
| `od_list_pause`, `od_list_stop` | Keys used to pause and stop file listings. |
| `event_status`, `event_starttime`, `event_errorlevel` | Current BBS event state, time, and exit level. |
| `event_days`, `event_force`, `event_last_run` | Event schedule, force flag, and last-run date. |

Many runtime members may be adjusted after initialization. Members used to
construct components, buffers, or communication state must be set beforehand.
