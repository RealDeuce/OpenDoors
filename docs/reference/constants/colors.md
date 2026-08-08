# Colors

The color constants are the traditional four-bit IBM-PC color values. All 16
values may be used as foreground colors. For backgrounds, values 0 through 7
select the background color and values 8 through 15 select the same colors with
the blink bit set.

| Value | Foreground | Background |
| --- | --- | --- |
| 0 | `D_BLACK` — dark black | `D_BLACK` — black |
| 1 | `D_BLUE` — dark blue | `D_BLUE` — blue |
| 2 | `D_GREEN` — dark green | `D_GREEN` — green |
| 3 | `D_CYAN` — dark cyan | `D_CYAN` — cyan |
| 4 | `D_RED` — dark red | `D_RED` — red |
| 5 | `D_MAGENTA` — dark magenta | `D_MAGENTA` — magenta |
| 6 | `D_BROWN` — dark brown | `D_BROWN` — brown |
| 7 | `D_GREY` — grey (dark white) | `D_GREY` — grey |
| 8 | `L_BLACK` — light black (grey) | `B_BLACK` — blinking black |
| 9 | `L_BLUE` — light blue | `B_BLUE` — blinking blue |
| 10 | `L_GREEN` — light green | `B_GREEN` — blinking green |
| 11 | `L_CYAN` — light cyan | `B_CYAN` — blinking cyan |
| 12 | `L_RED` — light red | `B_RED` — blinking red |
| 13 | `L_MAGENTA` — light magenta | `B_MAGENTA` — blinking magenta |
| 14 | `L_YELLOW` — yellow | `B_BROWN` — blinking brown |
| 15 | `L_WHITE` — white | `B_GREY` — blinking grey |

`B_YELLOW` and `B_WHITE` are compatibility aliases for values 14 and 15. Pass
separate foreground and background values to
[`od_set_color()`](../api/od_set_color.md), or combine them into a single
IBM-PC attribute for [`od_set_attrib()`](../api/od_set_attrib.md).
