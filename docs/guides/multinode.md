# Multi-node operation

Several copies of a door may run at the same time. OpenDoors reports the current
node through [`od_control.od_node`](../reference/control/connection.md), but the
application remains responsible for choosing node-specific temporary files and
protecting shared application data.

Do not assume that a successful file open gives exclusive access. Use the
locking facilities appropriate to the target platform when updating shared
records, and keep the locked interval as short as possible. Write complete
records while the lock is held; otherwise another node may observe a partially
updated file.

Node numbers are useful for log names, work directories, and temporary protocol
files. They are not a substitute for locking shared state. When a BBS does not
report a meaningful node, the application should use its own configured instance
identifier.

OpenDoors itself maintains session state separately for each process. The
multi-node precautions in this guide apply to resources introduced by the door
program.
