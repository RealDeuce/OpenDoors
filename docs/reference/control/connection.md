# Connection and door information

These fields describe how OpenDoors found and communicates with the current
session.

| Members | Description |
| --- | --- |
| `info_path` | Door-information filename or location. Set before initialization to override discovery. |
| `baud`, `od_connect_speed` | Effective I/O and reported connection speeds. A zero `baud` denotes local mode. |
| `od_com_address`, `od_com_irq` | Legacy serial hardware address and interrupt. |
| `od_com_method` | One of the [`COM_*`](../constants/session.md) communication methods. |
| `od_com_flow_control`, `od_com_rx_buf`, `od_com_tx_buf` | Flow-control and serial-buffer settings. |
| `od_com_fifo_trigger`, `od_com_no_fifo`, `od_no_fossil` | Legacy UART and FOSSIL controls. |
| `od_use_socket`, `port`, `od_open_handle` | Socket/Door32 selection, port, and caller-supplied native handle. |
| `od_cp437_to_utf8_out` | Converts outgoing CP437 text to UTF-8 on supported byte-stream connections. |
| `od_info_type`, `od_extended_info`, `od_ra_info` | Door-information format and format-specific state. |
| `od_node` | Current BBS node number. |

The lower-level serial fields remain for established DOS and BBS configurations.
New network-hosted doors normally receive their communication method and handle
from Door32 or the invoking server.

On [`ODPLAT_DOS32`](../constants/general.md#platform-selection), the FOSSIL
method is available, but the direct-UART method selected by
[`COM_INTERNAL`](../constants/session.md#communication-methods) is not.


## Detailed reference

### Door Info File Stats

The following OpenDoors control structure variables provide your program with information concerning the door information file from which OpenDoors obtained the BBS and caller information that is found elsewhere in the control structure. The following control structure items are listed in this section:

info_path                Sets the location and, optionally, the name of the door information file

od_info_type             Type of door information file that was found

od_node                  Node number the door is running under

user_timeofcreation      The time at which the door information file was created

#### `info_path`

```c
char od_control.info_path[60];
```

If used, this variable should be set prior to calling [`od_init()`](../api/od_init.md) or any other OpenDoors function. This variable allows you to control where OpenDoors will look for the door information (drop file). By default, OpenDoors searches for the door information file in the current directory. If this variable is set to the name of some other directory, OpenDoors will first search for any door information files in that directory. If you only wish OpenDoors to look for a particular type of door information file (for instance, you want OpenDoors to only read a DORINFO1.DEF, and ignore any DOOR.SYS file), you can specify the full path and filename of the file you wish OpenDoors to use.

It is usually a good idea to design your door to allow the system operator to set the location of the door information file. This will allow the sysop to place your door in its own directory, and will facilitate the use of your door on multi- line BBS systems. If you are using the OpenDoors configuration file system, then the system operator can set the door information file location and/or name using the BBSDir keyword. However, you may also wish to allow the location of the door information file to be set on the command line. The following example illustrates a method of reading and setting the location of the door information file from the door's command line:

```c
#include "opendoor.h"
```

```c
main(int argc, char *argv[])
   {
   if(argc>1) strncpy(od_control.info_path,argv[1],59);
```

```c
od_disp_str("This is a sample OpenDoors door.\n\r");
od_disp_str("Press any key to continue...\n\r");
od_get_key(TRUE);
od_exit(20);
}
```

#### `od_info_type`

```c
char od_control.od_info_type;
```

This variable indicates the type of information file from which OpenDoors has obtained the BBS and caller information that is found elsewhere in the OpenDoors control structure. This variable will have one of the following values, indicating that the door information file was of the corresponding type:

```c
      +----------------+----------------------------+
      |  od_info_type  | Door Information File Type |
      |      Value     |                            |
      +----------------+----------------------------+
      | DORINFO1       | DORINFO?.DEF               |
      | EXITINFO       | EXITINFO.BBS (Normal)      |
      | RA1EXITINFO    | EXITINFO.BBS (Extended)    |
      | RA2EXITINFO    | EXITINFO.BBS (RA 2.x)      |
      | QBBS275EXITINFO| EXITINFO.BBS (QuickBBS)    |
      | CHAINTXT       | CHAIN.TXT                  |
      | SFDOORSDAT     | SFDOORS.DAT                |
      | CALLINFO       | CALLINFO.BBS               |
      | DOORSYS_GAP    | DOOR.SYS (GAP/PC-Board)    |
      | DOORSYS_DRWY   | DOOR.SYS (Doorway style)   |
      | DOORSYS_WILDCAT| DOOR.SYS (WildCat standard)|
      | CUSTOM         | Custom door information    |
      |                | file, defined in config    |
      |                | file.                      |
      | NO_DOOR_FILE   | No drop file was found.    |
      +----------------+----------------------------+
```

The value of this variable is only valid AFTER [`od_init()`](../api/od_init.md) or some OpenDoors function has been called.

Note that this variable should be treated as a read-only variable, and should not normally be altered by your program. Altering this variable may cause OpenDoors to re-write a different type of door information file upon exiting, than was read upon startup.

#### `od_node`

```c
char od_control.od_node;
```

This variable indicates the node number that the door is running under. If this information is supplied by the BBS in the door information file, the node number will be automatically by OpenDoors. Specifically, the node number can be determined automatically from systems that produce an SFDOORS.DAT, PC- Board/GAP style DOOR.SYS or Wildcat style DOOR.SYS door information file. If this information is not supplied in the door information file, but is provided by the sysop in the door's configuration file, OpenDoors will use the value found there. Alternatively, you can set this variable manually.

On systems that produce a DORINFO?.DEF file, OpenDoors will use this variable to determine which DORINFO?.DEF file to search for. For instance, if [`od_control.od_node`](#od_node) is set to 3, OpenDoors will first search for a DORINFO3.DEF file. If this file is not found, OpenDoors will then default to the DORINFO1.DEF filename.

#### `user_timeofcreation`

```c
char od_control.user_timeofcreation[6];
```

This variable contains the time of day at which the door information file was created. This variable is available only when the door is running under a system that produces an EXITINFO.BBS file. To determine what type of door information file your door is running under, see the [`od_control.od_info_type`](#od_info_type) variable, below.

### Serial Port Settings

The following OpenDoors control structure items store the communications settings that OpenDoors uses to communicate with the modem. These values are normally set upon the first call to an OpenDoors function, during the [`od_init()`](../api/od_init.md) procedure. However, you may need to manual set this variables if:

\- you wish to allow greater configurability of your door - you are reading the door information file yourself - you are using the OpenDoors to write a non-door program

Some of these variables are always used by OpenDoors, while others are only relevant if OpenDoor's built-in serial communications code is being used instead of a FOSSIL driver. Those that are only used when no FOSSIL driver is present are denoted by an [*] in the list below.

The control structure variables controlling OpenDoor's serial port settings are as follows:

[`od_control.baud`](#baud)            Serial Port BPS rate

od_control.od_connect_sppedThe modem connection BPS rate

[`od_control.od_com_address`](#od_com_address)  Serial Port address [*]

" " .od_com_fifo_trigger  16550A FIFO trigger size

" " .od_com_flow_control  Type of flow control to use.

[`od_control.od_com_irq`](#od_com_irq)      Serial Port IRQ number [*}

[`od_control.od_com_method`](#od_com_method)   Is FOSSIL or built-in serial I/O being used

[`od_control.od_com_no_fifo`](#od_com_no_fifo)  Disables use of 16550A FIFOs [*]

[`od_control.od_com_rx_buf`](#od_com_rx_buf)   Size of receive buffer [*]

[`od_control.od_com_tx_buf`](#od_com_tx_buf)   Size of transmit buffer [*]

[`od_control.od_no_fossil`](#od_no_fossil)    Prevents OpenDoors from using a FOSSIL driver, even if one is available.

[`od_control.od_open_handle`](#od_open_handle)  Allows a live serial port handle to be passed to OpenDoors.

[`od_control.port`](#port)            Serial port number, 0 based.

#### `baud`

```c
unsigned long od_control.baud;
```

This variable contains the BPS rate at which the computer is communicating with the modem, not to be confused with the BPS rate at which the local modem is communicating with the remote modem.

A value of 0 indicates that the program is operating in local mode.

If a FOSSIL driver is being used for serial I/O, this value is ignored if it does not correspond to one of the baud rates that an application can directly set a FOSSIL driver to. The BPS rates recognized by FOSSIL drivers are: 300, 600, 1200, 2400, 4800, 9600, 19200, 38400. If any other BPS rate is to be used, the FOSSIL driver must be locked at that BPS from the FOSSIL driver command-line. When locked, FOSSIL drivers ignore any attempt by an application to change the BPS rate of the locked port. For this reason, the [`od_control.baud`](#baud) setting has no effect on the FOSSIL driver if it is locked.

#### `od_com_address`

```c
int od_control.od_com_address;
```

This variable is only used when OpenDoors is NOT performing serial I/O using a FOSSIL driver. (When a FOSSIL driver is being used, the serial port address can be set from the FOSSIL driver command line).

This variable may optionally be set to specify the base address of the serial port to be used. For ports COM1: through COM4:, OpenDoors can normally determine the serial port address automatically. However, for other serial ports, the port address must be specified using this variable. If you are not specifying a serial port address with this variable, do not change it's default value of 0.

#### `od_com_fifo_trigger`

```c
char od_control.od_com_fifo_trigger;
```

This variable is only used when OpenDoors is NOT performing serial I/O using a FOSSIL driver. (When a FOSSIL driver is being used, the IRQ line can be set from the FOSSIL driver command line). This variable sets the number of bytes that will be placed in the 16550A UART FIFO buffers before an interrupt is triggered, if the 16550A UART FIFOs are used. Valid values are 1, 4, 8 and 14.

#### `od_com_flow_control`

```c
unsigned char od_control.od_com_flow_control;
```

This variable sets the type of serial I/O flow control to use. By default, this variable is set to COM_DEFAULT_FLOW, which specifies the default mode of flow control. Most often, this will be RTS/CTS flow control. A value of COM_RTSCTS_FLOW explicitly enables RTS/CTS flow control. A value of COM_NO_FLOW disables all flow control. If you are going to change the value of this variable, it should be set prior to your first call to any OpenDoors function.

#### `od_com_irq`

```c
unsigned char od_control.od_com_irq;
```

This variable is only used when OpenDoors is NOT performing serial I/O using a FOSSIL driver. (When a FOSSIL driver is being used, the IRQ line can be set from the FOSSIL driver command line).

This variable may optionally be set to specify the IRQ line to be used for the serial port. By default, OpenDoors uses the normal IRQ 4 line for ports COM1: and COM3:, and IRQ 3 for ports COM2: and COM4:. To override this default, the IRQ line can be set using this variable. If you are not specifying an IRQ line with this variable, do not change it's default value of 0.

#### `od_com_method`

```c
char od_control.od_com_method;
```

This read-only variable reports the method that OpenDoors is using for serial I/O. This variable is set during [`od_init()`](../api/od_init.md) or the first call to an OpenDoors function. This variable can be one of the following values:

COM_FOSSIL          - Indicates that a FOSSIL driver is being COM_INTERNAL   - Indicates that OpenDoor's internal serial I/O code is being used. COM_WIN32      - Indicates that the Win32 communication system is being used.

#### `od_com_no_fifo`

```c
char od_control.od_com_no_fifo;
```

This variable is only used when OpenDoors is NOT performing serial I/O using a FOSSIL driver. (When a FOSSIL driver is being used, the receive buffer size can be set from the FOSSIL driver command line).

Normally, OpenDoors will use a 16550A FIFO buffer if a 16550A UART is installed. You can disable the use of the 16550A FIFO buffer by setting this variable to TRUE.

#### `od_com_rx_buf`

```c
unsigned int od_control.od_com_rx_buf;
```

This variable is only used when OpenDoors is NOT performing serial I/O using a FOSSIL driver. (When a FOSSIL driver is being used, the receive buffer size can be set from the FOSSIL driver command line).

This variable allows you to set the size of OpenDoor's serial I/O receive buffer. If you do not set this buffer size, a default value of 256 characters is used. Normally, this buffer size is more than large enough for door programs. However, if you find that inbound characters are lost before they can be processed by your program, you may wish to increase the size of this buffer.

This variable should only be changed before your first call to [`od_init()`](../api/od_init.md) or any other OpenDoors function.

#### `od_com_tx_buf`

```c
unsigned int od_control.od_com_tx_buf;
```

This variable is only used when OpenDoors is NOT performing serial I/O using a FOSSIL driver. (When a FOSSIL driver is being used, the receive buffer size can be set from the FOSSIL driver command line).

This variable allows you to set the size of OpenDoor's serial I/O transmit buffer. If you do not set this buffer size, a default value of 1024 characters is used.

This variable should only be changed before your first call to [`od_init()`](../api/od_init.md) or any other OpenDoors function.

#### `od_connect_speed`

```c
DWORD od_control.od_connect_speed;
```

This variable contains the best guess at the current modem connection speed. This information is currently only accurate if a DOOR.SYS file is being used. In other situations, it will always be set to be equal to [`od_control.baud`](#baud).

#### `od_open_handle`

```c
DWORD od_control.od_open_handle;
```

Under platforms where this is supported (currently only the Win32 version of OpenDoors), this variable can be used to pass a live serial port handle to OpenDoors, which OpenDoors will use. OpenDoors will not close this handle when it exits. If this value is set to 0, OpenDoors will open and close the serial port itself.

#### `port`

```c
char od_control.port;
```

This variable contains the serial port number that the modem is connected. This number is 0 based, so that a value of 0 corresponds to COM1:, a value of 1 corresponds to COM2:, and so on. This value will normally be set by the [`od_init()`](../api/od_init.md) function, when the door information file is read, and should not be changed after modem initialization has been carried out by the [`od_init()`](../api/od_init.md) function.
