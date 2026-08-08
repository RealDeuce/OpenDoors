# `od_page()`

Allows the remote user to request the sysop's attention.

## Synopsis

```c
void od_page(void);
```

Paging follows the configured availability hours, page count, sound, prompts,
and status-line behavior in [`od_control`](../control/index.md). The user is
told when paging is not available. While a page is active OpenDoors continues to
service the session and allows the sysop to answer through the local controls.

The function returns no value.

## See also

[`od_chat()`](od_chat.md), [The `od_control` structure](../control/index.md)
