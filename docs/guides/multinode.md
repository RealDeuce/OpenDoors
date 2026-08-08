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

## Detailed reference

The original examples use DOS-era sharing functions. Their concurrency requirements remain relevant, but applications should use the locking facilities appropriate to their current platform and runtime.

While the majority of BBS systems have only a single phone line, allowing only one user to access the system at a time, there are also many multi-node BBS systems. On such systems, it is quite possible that more than one user may be using your door program simultaneously. OpenDoors itself is designed for both single- node and multi-node operation. However, if you want your program to operate correctly on multi-node systems, there are a number of concurrency issues that you must keep in mind when writing your own code.

Some door programs are designed to behave on multi-node systems just as they would on single-line BBSes. Others add special features only possible in multi-node environments. For instance, you may want to permit users to interact or chat with one another in "real time". Many simple doors may not require any special attention to multi-node capabilities. However, if your door must access any data files or other resources that are to be shared among nodes, it is necessary to carefully coordinate access to these resources.

There are two primary issues that are often of concern when creating door programs for multi-node systems. The first issue discussed below is how to coordinate concurrent file access between multiple node. The second topic we will deal with is the installation of door programs on multi-node systems.

### Concurrent File Access

One of the most important issues that arises when writing door programs for multi-node systems is how to coordinate simultaneous access to a single data file by multiple instances of your program. While it is generally safe to have multiple nodes reading simultaneously from a single file, having multiple nodes updating a file without any coordination can lead to lost updates and other problems. Consider, for example, the EX_VOTE.C example program that is included in your OpenDoors package. When the user votes on a poll, EX_VOTE.C must update the total number of votes for the user's answer. Such a program that is only intended for single node operation could do this by simply reading the current number of votes for the appropriate option, adding one to this total, and writing the updated total back to the file. However, if this approach where to be used on a multi- node system, it is quite possible that two users would vote on the same poll after both nodes have read the poll record into memory. In this situation, one node would add one to the total number of votes for the poll record that it has in memory, and write the updated information to the file. The second node would then add one to its total, without reading the updated information written by the first node. When the second node then writes this information to the file, it overwrites the first node's total with its own. The final effect is that the second user's vote overwrites the first, and so the first user's vote is lost.

The solution to this problem is to lock a file unit for the entire update operation, to prevent other nodes from accessing the unit at the same time. This unit could be the entire file, or only a single record in the file. EX_VOTE.C locks its entire file when performing an update operation, but in other cases it may be more appropriate to only lock a single record in the file. The important thing to understand is that when one node locks a file unit, other nodes much wait until the first node is finished the update operation. This means that if one node is updating information that other nodes could possibly need access to, it should always perform the lock, read, write and unlock cycle as quickly as possible.

Let's look again at the approach taken by EX_VOTE.C. After the user has indicated which option he/she wishes to vote on, Vote attempts to open the file for exclusive access. By doing this, EX_VOTE.C in effect locks the entire file for the duration that it has the file open. If another node attempts to open the file while one node has it locked, the open operation will fail, and the C runtime library will set the errno variable to EACCES. This, in effect, tells you that another node is currently working on the file, and that you must wait your turn. In this case, EX_VOTE.C continues to retry the open operation until the other node is finished its update, at which time the open operation will succeed. This approach will even work when there are many nodes that are attempting to update the file at the same time. Whichever node first attempts to open the file will gain exclusive access to the file, and any additional nodes are forced to wait for access to the file. When one node finishes with the file, another node will gain access to the file (whichever happens to be the next node to re-attempt the open operation). This process continues until all waiting nodes have had a chance to perform their update. EX_VOTE.C will repeatedly try to open the file for up to 20 seconds, after which time it will give up, reporting an error which indicate that it is unable to access the file. During this waiting process, EX_VOTE.C repeatedly calls [`od_kernel()`](../reference/api/od_kernel.md), so that sysop function keys, carrier detection and other essential door operations can continue to be performed.

After EX_VOTE.C has successfully secured exclusive access to the file, it first reads the record that it is going to update. It is important that this be done after the file unit is locked, in order to ensure that the copy of the record in memory matches what is stored in the file. EX_VOTE.C then updates the record based on the question on which the user has voted, writes this information back to the file. EX_VOTE.C then immediately closes the file, allowing other nodes to also access the file. EX_VOTE.C is very carefully designed so that the file update operation can never be interrupted (for instance, no OpenDoors functions are called, which could detect a time-out and terminate the program while a file update operation is in progress), or delayed until the user makes a response. As such, the file unit is always unlocked (in this case, closed) within a fraction of a second after it was locked, or order that other nodes will never have to wait long for access to the file.

Here I have presented a detailed account of how EX_VOTE.C handles multi-node file access. While all of the details involved in coordinating multiple file access can be overwhelming at first, they will begin to come naturally to you, as you begin to always think in terms of multi-node scenarios. To summarize, the important elements that are typically involved in multi-node file access are:

A. Decide on an appropriate file unit to lock for your application. In simple cases, this can be the entire file. In other cases, you may wish to lock individual file records, using the appropriate runtime library functions.

B. Always perform update operations in lock, read, update, write, unlock cycles on individual file units. If there is a chance that other nodes will also need to access the file unit, ensure that the update operation cannot be interrupted or delayed until a user makes a response.

After you have designed your program for concurrent file access, how can you test it? If you don't have a multi-node BBS system that you have access to, you can perform most of your testing under a multitasking environment, with multiple copies of your program running in different windows.

### Multi-Node Configuration

A second issue that you may want to bear in mind is how door programs are typically setup on multi-node systems. Unfortunately, this may differ considerable depending upon which BBS software is being used. However, some of the issues that you may have to consider discussed below:

A. Your program must be able to locate the correct door information file for the appropriate node. Most BBS systems make separate door information files available to each node by one of the following means:

```c
- By naming each node's door information file
  uniquely. (e.g. DORINFO1.DEF, DORINFO2.DEF.)
```

```c
- By having a separate directory for each node's door
  information file. (e.g. \NODE1\DOOR.SYS,
  \NODE2\DOOR.SYS, etc.)
```

In the first case, OpenDoors can automatically select the correct door information file, assuming that it knows which node it is running on (see item C, below). In the later case, you must tell OpenDoors which directory it must look in to find the appropriate door information file. You may do this by any of the following means:

```c
- By specifying the location of the file on the
  command line, if od_parse_cmd_line() is used.
```

```c
- By providing a configuration file keyword to set
  the door information file location for each node.
```

```c
- By providing a different configuration file for
  each node (See item B, below).
```

B. If you are using the OpenDoors configuration file system, node-specific options should not be used if each node is accessing the same configuration file. While it is possible to have a different configuration file for each node (the filename can be specified on the command line if [`od_parse_cmd_line()`](../reference/api/od_parse_cmd_line.md) is used), in most cases the same configuration file will be used for all nodes. In this case, the node number, serial port information, and possible door information file location operations should not be used. If you are basing your configuration file on the example `door.cfg` file that is included in the OpenDoors package, you may want to remove these options from the file.

C. In many cases, your program must also be able to determine which node it is running under. If this information is available in the door information file, or is stored in a TASK environment variable, OpenDoors will automatically set the appropriate node number in [`od_control.od_node`](../reference/control/connection.md#od_node). Otherwise, if your program requires this information, it should be specified on the program's command line. The [`od_parse_cmd_line()`](../reference/api/od_parse_cmd_line.md) function supports this option. Reasons that your program might need to know the current node number include:

```c
- In order for OpenDoors to display this information
  correctly on the status line.
```

```c
- In order to determine which configuration file to
  read or which node directory in which to look for
  the door information file.
```

```c
- In order for OpenDoors to know which door
  information file to read (e.g. DORINFO1.DEF,
  DORINFO2.DEF. etc.)
```

```c
- In order to provide any form of real-time
  interaction between nodes, such as inter-node chat.
```

D. If your program is running under MS-DOS, and multi-node file access is being coordinated by locking part or all of a file, the SHARE.EXE utility must be installed.
