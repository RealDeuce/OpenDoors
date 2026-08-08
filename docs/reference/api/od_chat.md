# `od_chat()`

Starts an immediate local-sysop chat with the remote user.

## Synopsis

```c
void od_chat(void);
```

This is the programmatic equivalent of the configured chat function key.
OpenDoors changes to chat mode, runs the before/after chat callbacks, displays
the configured prompts, and returns when chat ends. Local mode or an unavailable
operator may limit the useful effect. The function returns no value.

## See also

[`od_page()`](od_page.md), [The `od_control` structure](../control/index.md)
