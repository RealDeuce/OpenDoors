# OpenDoors Reference

OpenDoors is a C and C++ toolkit for writing online software such as BBS
doors. It provides the connection handling, terminal output, keyboard input,
configuration, status display, and door-information support that would
otherwise need to be built into every program.

This site documents the current OpenDoors 6.30 source tree. The function
reference describes the interface declared by `OpenDoor.h`; the accompanying
guides explain how those functions fit together in a working door.

If you are writing your first OpenDoors program, begin with
[Getting started](getting-started.md). If you already know which operation you
need, proceed directly to the [API reference](reference/api/index.md).

!!! note "The original manual"

    The OpenDoors 6.00 manual remains available in the repository under
    `historic/OPENDOOR.TXT`. It is a CP437 text file laid out as printed pages
    with form-feed separators. It is retained as an historical artifact, not
    as the description of current behavior.

## What is covered

- All high-level `od_*` API functions declared by `OpenDoor.h`.
- The `od_control` structure and the settings most often used by a door.
- Public data types, flags, error values, and compatibility definitions.
- Current native, Windows, and 16-bit DOS build procedures.
- The relationship between the remote terminal and the local OpenDoors
  display.

The lower-level `ODScrn`, `ODCom`, and kernel module interfaces are library
implementation interfaces. They are not part of this high-level reference.
