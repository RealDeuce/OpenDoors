# `od_kernel()`

The OpenDoors Central Control function.

## Synopsis

```c
void od_kernel(void);
```

## Return value

N/A

## Description

In the DOS version of OpenDoors, the `od_kernel()` function is responsible for many vital OpenDoors tasks, such as monitoring the carrier detect signal, monitoring the amount of time that the user has remaining, updating the status line, responding to sysop hotkeys, and reading characters which are received from the modem. The `od_kernel()` function is automatically called on a frequent basis by the other OpenDoors functions, so most often you will not need to be concerned with this function. However, in order that OpenDoors can carry out the activities mentioned above with a quick response, it is important that `od_kernel()`, or some other OpenDoors function be called at least once every second. Thus, if your program will be carrying out some processing, in which it will not be calling any OpenDoors functions for more than a second or so, you should call the `od_kernel()` function yourself. The example below demonstrates one method of doing just this.

Note that if for some reason or other, it is not possible for your program to call the `od_kernel()` function, or any other OpenDoors functions for a period of several seconds, this will not cause your door to crash or fail in any way. The only problem will be that OpenDoors will not be able to respond to any action, such as the sysop pressing a function key, or the user dropping carrier, until such time as you next call `od_kernel()`, or some OpenDoors function. Hence, use of the `od_kernel()` function will improve the quality and response time of your program, but calling it or some OpenDoors function on a regular basis is not absolutely vital.

This function has no effect in the Win32 version of OpenDoors.

## Additional details

The kernel checks carrier state, time limits, inactivity, local sysop keys,
status updates, and configured callbacks. API functions invoke it frequently,
so ordinary doors seldom need to call it after every operation. Call it during
long computations or wait loops which otherwise make no OpenDoors calls.

The function returns no value. It may terminate the session when connection or
time-limit policy requires it.

## See also

[`od_sleep()`](od_sleep.md), [`od_carrier()`](od_carrier.md),
[Session lifecycle](../../guides/session-lifecycle.md)
