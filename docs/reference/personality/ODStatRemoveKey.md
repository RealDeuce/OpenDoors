# `ODStatRemoveKey()`

Removes a custom local sysop key.

## Synopsis

```c
void ODStatRemoveKey(WORD wKeyCode);
```

The first matching key previously installed with
[`ODStatAddKey()`](ODStatAddKey.md) is removed. The order of the remaining keys
is not preserved. A missing key has no effect.
