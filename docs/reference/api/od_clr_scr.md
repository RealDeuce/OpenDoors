# `od_clr_scr()`

Clears the current output screen and places the cursor at its upper-left
corner.

## Synopsis

```c
void od_clr_scr(void);
```

The cleared cells receive the current display attribute. The operation is sent
to the remote user and reflected in the local presentation. User configuration
which disables screen clearing is honored unless the door has requested that
clearing always occur. The function returns no value.

## See also

[`od_clr_line()`](od_clr_line.md), [`od_scroll()`](od_scroll.md)
