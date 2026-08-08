# `od_set_attrib()`

Selects an IBM-PC text attribute for subsequent output.

## Synopsis

```c
void od_set_attrib(INT nColour);
```

The low four bits select the foreground, the next three select the background,
and the high bit has the traditional blink meaning. OpenDoors emits the
corresponding terminal sequence when appropriate and records the attribute in
the screen model. The function returns no value.

## See also

[`od_set_color()`](od_set_color.md), [`od_color_config()`](od_color_config.md)
