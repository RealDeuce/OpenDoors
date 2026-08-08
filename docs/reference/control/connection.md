# Connection and door information

These fields describe how OpenDoors found and communicates with the current
session.

| Members | Description |
| --- | --- |
| `info_path` | Door-information filename or location. Set before initialization to override discovery. |
| `baud`, `od_connect_speed` | Effective I/O and reported connection speeds. A zero `baud` denotes local mode. |
| `od_com_address`, `od_com_irq` | Legacy serial hardware address and interrupt. |
| `od_com_method` | One of the [`COM_*`](../constants.md) communication methods. |
| `od_com_flow_control`, `od_com_rx_buf`, `od_com_tx_buf` | Flow-control and serial-buffer settings. |
| `od_com_fifo_trigger`, `od_com_no_fifo`, `od_no_fossil` | Legacy UART and FOSSIL controls. |
| `od_use_socket`, `port`, `od_open_handle` | Socket/Door32 selection, port, and caller-supplied native handle. |
| `od_cp437_to_utf8_out` | Converts outgoing CP437 text to UTF-8 on supported byte-stream connections. |
| `od_info_type`, `od_extended_info`, `od_ra_info` | Door-information format and format-specific state. |
| `od_node` | Current BBS node number. |

The lower-level serial fields remain for established DOS and BBS configurations.
New network-hosted doors normally receive their communication method and handle
from Door32 or the invoking server.
