static tPortInfo ut_port;
static BYTE ut_converted[4] = {0xc2, 0xa3, 0x41, 0};
static unsigned ut_convert_calls;
static unsigned ut_free_calls;
static BOOL ut_convert_fails;

#ifdef ODPLAT_NIX
#define UT_CUSTOM_MOCK___error
#define __error utm___error
static int ut_errno;
int *utm___error(void) { return(&ut_errno); }
#endif

#define UT_CUSTOM_MOCK_ODComCP437ToUnicode
#define UT_CUSTOM_MOCK_ODComCallIdleFunction
#define UT_CUSTOM_MOCK_free
BYTE *utm_ODComCP437ToUnicode(BYTE *buffer, int *size)
{
   UT_ASSERT_NOT_NULL(buffer); UT_ASSERT_NOT_NULL(size);
   ++ut_convert_calls;
   if(ut_convert_fails) return(NULL);
   *size = 3; return(ut_converted);
}
void utm_free(void *memory)
{
   UT_ASSERT_EQ_PTR(ut_converted, memory); ++ut_free_calls;
}

static unsigned ut_idle_calls;
static void ODCALL ut_idle(void) { ++ut_idle_calls; }

void utm_ODComCallIdleFunction(tPortInfo *port)
{
   UT_ASSERT_NOT_NULL(port);
   if(port->pfIdleCallback != NULL) (*port->pfIdleCallback)();
}

#if defined(__TURBOC__) || (defined(__WATCOMC__) && !defined(ODPLAT_DOS32))
static void ut_fossil_block_limit(BYTE limit)
{
   ASM mov ah, 0xfc
   ASM mov al, limit
   ASM int 0x14
}
#endif

#ifdef ODPLAT_DOS32
#define UT_CUSTOM_MOCK_OD32FossilSendBlock
#define UT_CUSTOM_MOCK_OD32FossilSendByte
static int ut_block_results[4];
static unsigned ut_block_calls;
static BOOL ut_byte_results[4];
static unsigned ut_byte_calls;
INT utm_OD32FossilSendBlock(BYTE port, tOD32FossilBuffer *fossil,
   const BYTE *buffer, INT size)
{
   UT_ASSERT_EQ_INT(2, port); UT_ASSERT_EQ_PTR(&ut_port.FossilBuffer, fossil);
   UT_ASSERT_NOT_NULL(buffer); UT_ASSERT(size > 0); UT_ASSERT(ut_block_calls < 4);
   return(ut_block_results[ut_block_calls++]);
}
BOOL utm_OD32FossilSendByte(BYTE port, BYTE value)
{
   UT_ASSERT_EQ_INT(2, port); UT_ASSERT_NOT_NULL(&value);
   UT_ASSERT(ut_byte_calls < 4); return(ut_byte_results[ut_byte_calls++]);
}
#endif

#ifdef INCLUDE_UART_COM
#define UT_CUSTOM_MOCK_UTInp
#define UT_CUSTOM_MOCK_UTOutp
#define UT_CUSTOM_MOCK_UTDisable
#define UT_CUSTOM_MOCK_UTEnable
static BYTE ut_tx_queue[4];
static unsigned ut_disable_calls;
static unsigned ut_enable_calls;
static unsigned ut_port_writes;
#ifdef __TURBOC__
int
#else
unsigned
#endif
utm_UTInp(unsigned port)
{
   UT_ASSERT_EQ_UINT((unsigned)nIntEnableRegAddr, port); return(1);
}
#ifdef __TURBOC__
int
#else
unsigned
#endif
utm_UTOutp(unsigned port, unsigned value)
{
   UT_ASSERT_EQ_UINT((unsigned)nIntEnableRegAddr, port);
   UT_ASSERT_EQ_UINT(1 | THRE, value); ++ut_port_writes; return(value);
}
void utm_UTDisable(void) { ++ut_disable_calls; }
void utm_UTEnable(void) { ++ut_enable_calls; }
static void ODCALL ut_drain_uart(void)
{
   ++ut_idle_calls; nTXChars = 0;
}
#endif

#ifdef INCLUDE_WIN32_COM
#define UT_CUSTOM_MOCK_WriteFile
#define UT_CUSTOM_MOCK_ClearCommError
static BOOL ut_write_result;
static DWORD ut_written;
static unsigned ut_clear_calls;
BOOL WINAPI utm_WriteFile(HANDLE device, LPCVOID buffer, DWORD size,
   LPDWORD written, LPOVERLAPPED overlapped)
{
   UT_ASSERT_EQ_PTR((HANDLE)(DWORD_PTR)41, device); UT_ASSERT_NOT_NULL(buffer);
   UT_ASSERT(size == 2 || size == 3); UT_ASSERT_NULL(overlapped);
   *written = ut_written; return(ut_write_result);
}
BOOL WINAPI utm_ClearCommError(HANDLE device, LPDWORD errors,
   LPCOMSTAT status)
{
   UT_ASSERT_EQ_PTR((HANDLE)(DWORD_PTR)41, device);
   UT_ASSERT_NOT_NULL(errors); UT_ASSERT_NULL(status); ++ut_clear_calls;
   return(TRUE);
}
#endif

#ifdef INCLUDE_DOOR32_COM
static BOOL ut_door_result;
static BOOL WINAPI ut_door_write(const BYTE *buffer, DWORD size)
{
   UT_ASSERT_NOT_NULL(buffer); UT_ASSERT(size == 2 || size == 3);
   return(ut_door_result);
}
#endif

#ifdef INCLUDE_SOCKET_COM
#define UT_CUSTOM_MOCK_send
#ifdef ODPLAT_WIN32
#define UT_CUSTOM_MOCK_select
#define UT_CUSTOM_MOCK_WSAGetLastError
#else
#define UT_CUSTOM_MOCK_poll
#endif
#ifdef OD_MULTITHREADED
#define UT_CUSTOM_MOCK_ODThreadSleep
#else
#define UT_CUSTOM_MOCK_od_sleep
#endif
static int ut_ready_result;
static short ut_ready_events;
static int ut_send_results[3];
static unsigned ut_send_calls;
static int ut_socket_error;
static unsigned ut_sleep_calls;
#ifdef ODPLAT_WIN32
int PASCAL utm_select(int count, fd_set FAR *read_set,
   fd_set FAR *write_set, fd_set FAR *error_set,
   const struct timeval FAR *timeout)
{
   UT_ASSERT_EQ_INT(46, count); UT_ASSERT_NULL(read_set);
   UT_ASSERT_NOT_NULL(write_set); UT_ASSERT_NULL(error_set);
   UT_ASSERT_NOT_NULL(timeout); return(ut_ready_result);
}
int PASCAL utm_WSAGetLastError(void) { return(ut_socket_error); }
int PASCAL utm_send(SOCKET socket_handle, const char FAR *buffer,
   int size, int flags)
#else
int utm_poll(struct pollfd *descriptors, nfds_t count, int timeout)
{
   UT_ASSERT_NOT_NULL(descriptors); UT_ASSERT_EQ_UINT(1, count);
   UT_ASSERT_EQ_INT(1000, timeout); descriptors[0].revents = ut_ready_events;
   return(ut_ready_result);
}
ssize_t utm_send(int socket_handle, const void *buffer, size_t size, int flags)
#endif
{
   UT_ASSERT_EQ_INT(45, socket_handle); UT_ASSERT_NOT_NULL(buffer);
   UT_ASSERT(size == 2 || size == 3); UT_ASSERT_EQ_INT(0, flags);
   UT_ASSERT(ut_send_calls < 3); return(ut_send_results[ut_send_calls++]);
}
#ifdef OD_MULTITHREADED
void utm_ODThreadSleep(tODMilliSec delay)
#else
void ODCALL utm_od_sleep(tODMilliSec delay)
#endif
{
   UT_ASSERT(delay == 25 || delay == 1); ++ut_sleep_calls;
}
#endif

#ifdef INCLUDE_STDIO_COM
#define UT_CUSTOM_MOCK_select
#define UT_CUSTOM_MOCK_fwrite
static int ut_select_results[14];
static unsigned ut_select_calls;
static size_t ut_fwrite_results[4];
static unsigned ut_fwrite_calls;
int utm_select(int count, fd_set *read_set, fd_set *write_set,
   fd_set *error_set, struct timeval *timeout)
{
   UT_ASSERT_EQ_INT(STDOUT_FILENO + 1, count); UT_ASSERT_NULL(read_set);
   UT_ASSERT_NOT_NULL(write_set); UT_ASSERT_NULL(error_set);
   UT_ASSERT_NOT_NULL(timeout); return(ut_select_results[ut_select_calls++]);
}
size_t utm_fwrite(const void *buffer, size_t size, size_t count, FILE *stream)
{
   UT_ASSERT_NOT_NULL(buffer); UT_ASSERT_EQ_UINT(1, size);
   UT_ASSERT(count > 0); UT_ASSERT_EQ_PTR(stdout, stream);
   return(ut_fwrite_results[ut_fwrite_calls++]);
}
#endif

static void reset_send_buffer(void)
{
   unsigned index;
   memset(&ut_port, 0, sizeof(ut_port)); ut_port.bIsOpen = TRUE;
   ut_port.btPort = 2; ut_port.pfIdleCallback = ut_idle;
   od_control.od_cp437_to_utf8_out = FALSE;
   ut_convert_calls = ut_free_calls = 0; ut_convert_fails = FALSE;
   ut_idle_calls = 0;
#ifdef ODPLAT_DOS32
   ut_block_calls = ut_byte_calls = 0;
   for(index = 0; index < 4; ++index) {
      ut_block_results[index] = 2; ut_byte_results[index] = TRUE;
   }
#endif
#ifdef INCLUDE_UART_COM
   memset(ut_tx_queue, 0, sizeof(ut_tx_queue)); pbtTXQueue = ut_tx_queue;
   nTXQueueSize = 4; nTXInIndex = 1; nTXChars = 0;
   nIntEnableRegAddr = 0x3f9; ut_disable_calls = ut_enable_calls = 0;
   ut_port_writes = 0; ut_port.pfIdleCallback = ut_drain_uart;
#endif
#ifdef INCLUDE_WIN32_COM
   ut_port.hCommDev = (HANDLE)(DWORD_PTR)41; ut_write_result = TRUE;
   ut_written = 2; ut_clear_calls = 0;
#endif
#ifdef INCLUDE_DOOR32_COM
   ut_port.pfDoorWrite = ut_door_write; ut_door_result = TRUE;
#endif
#ifdef INCLUDE_SOCKET_COM
   ut_port.socket = 45; ut_ready_result = 1; ut_ready_events = POLLOUT;
   ut_send_calls = ut_sleep_calls = 0; ut_socket_error = 0;
   for(index = 0; index < 3; ++index) ut_send_results[index] = 2;
#endif
#ifdef INCLUDE_STDIO_COM
   ut_select_calls = ut_fwrite_calls = 0;
   for(index = 0; index < 14; ++index) ut_select_results[index] = 1;
   for(index = 0; index < 4; ++index) ut_fwrite_results[index] = 2;
#endif
}

static void handles_empty_and_conversion_results(void)
{
   BYTE data[2] = {0x9c, 0x41};
   reset_send_buffer(); ut_port.Method = (tComMethod)99;
   UT_ASSERT_EQ_INT(kODRCSuccess, utt_ODComSendBuffer(
      ODPTR2HANDLE(&ut_port, tPortInfo), data, 0));
   UT_ASSERT_EQ_UINT(0, ut_convert_calls);
   reset_send_buffer(); ut_port.Method = (tComMethod)99;
   od_control.od_cp437_to_utf8_out = TRUE; ut_convert_fails = TRUE;
   UT_ASSERT_EQ_INT(kODRCGeneralFailure, utt_ODComSendBuffer(
      ODPTR2HANDLE(&ut_port, tPortInfo), data, 2));
   UT_ASSERT_EQ_UINT(0, ut_free_calls);
}

#ifdef INCLUDE_FOSSIL_COM
static void sends_fossil_blocks_and_fallback_bytes(void)
{
   BYTE data[2] = {0x31, 0x32};
   reset_send_buffer(); ut_port.Method = kComMethodFOSSIL;
   UT_ASSERT_EQ_INT(kODRCSuccess, utt_ODComSendBuffer(
      ODPTR2HANDLE(&ut_port, tPortInfo), data, 2));
#if defined(__TURBOC__) || (defined(__WATCOMC__) && !defined(ODPLAT_DOS32))
   ut_fossil_block_limit(1);
   UT_ASSERT_EQ_INT(kODRCSuccess, utt_ODComSendBuffer(
      ODPTR2HANDLE(&ut_port, tPortInfo), data, 2));
   UT_ASSERT_EQ_UINT(1, ut_idle_calls);
   reset_send_buffer(); ut_port.Method = kComMethodFOSSIL;
   ut_port.pfIdleCallback = NULL; ut_fossil_block_limit(1);
   UT_ASSERT_EQ_INT(kODRCSuccess, utt_ODComSendBuffer(
      ODPTR2HANDLE(&ut_port, tPortInfo), data, 2));
   ut_fossil_block_limit(0xff);
#endif
#ifdef ODPLAT_DOS32
   reset_send_buffer(); ut_port.Method = kComMethodFOSSIL;
   ut_block_results[0] = 1; ut_block_results[1] = 1;
   utt_ODComSendBuffer(ODPTR2HANDLE(&ut_port, tPortInfo), data, 2);
   reset_send_buffer(); ut_port.Method = kComMethodFOSSIL;
   ut_block_results[0] = 0; ut_block_results[1] = 2;
   utt_ODComSendBuffer(ODPTR2HANDLE(&ut_port, tPortInfo), data, 2);
   UT_ASSERT_EQ_UINT(1, ut_idle_calls);
   reset_send_buffer(); ut_port.Method = kComMethodFOSSIL;
   ut_block_results[0] = 0; ut_block_results[1] = 2;
   ut_port.pfIdleCallback = NULL;
   utt_ODComSendBuffer(ODPTR2HANDLE(&ut_port, tPortInfo), data, 2);
   reset_send_buffer(); ut_port.Method = kComMethodFOSSIL;
   ut_block_results[0] = -1; ut_byte_results[0] = FALSE;
   utt_ODComSendBuffer(ODPTR2HANDLE(&ut_port, tPortInfo), data, 2);
   UT_ASSERT_EQ_UINT(3, ut_byte_calls); UT_ASSERT_EQ_UINT(1, ut_idle_calls);
   reset_send_buffer(); ut_port.Method = kComMethodFOSSIL;
   ut_block_results[0] = -1; ut_port.pfIdleCallback = NULL;
   ut_byte_results[0] = FALSE;
   utt_ODComSendBuffer(ODPTR2HANDLE(&ut_port, tPortInfo), data, 2);
#endif
}
#endif

#ifdef INCLUDE_UART_COM
static void fills_uart_queue_in_one_or_multiple_wrapped_parts(void)
{
   BYTE data[3] = {0x31, 0x32, 0x33};
   reset_send_buffer(); ut_port.Method = kComMethodUART; nTXInIndex = 3;
   UT_ASSERT_EQ_INT(kODRCSuccess, utt_ODComSendBuffer(
      ODPTR2HANDLE(&ut_port, tPortInfo), data, 3));
   UT_ASSERT_EQ_INT(0x31, ut_tx_queue[3]); UT_ASSERT_EQ_INT(2, nTXInIndex);
   reset_send_buffer(); ut_port.Method = kComMethodUART; nTXInIndex = 1;
   utt_ODComSendBuffer(ODPTR2HANDLE(&ut_port, tPortInfo), data, 3);
   UT_ASSERT_EQ_INT(0, nTXInIndex);
   reset_send_buffer(); ut_port.Method = kComMethodUART; nTXChars = 3;
   utt_ODComSendBuffer(ODPTR2HANDLE(&ut_port, tPortInfo), data, 3);
   UT_ASSERT_EQ_UINT(1, ut_idle_calls);
   reset_send_buffer(); ut_port.Method = kComMethodUART; nTXInIndex = 1;
   utt_ODComSendBuffer(ODPTR2HANDLE(&ut_port, tPortInfo), data, 1);
   UT_ASSERT_EQ_INT(2, nTXInIndex);
}
#endif

#ifdef INCLUDE_WIN32_COM
static void reports_win32_write_outcomes_and_frees_conversion(void)
{
   BYTE data[2] = {0x31, 0x32};
   reset_send_buffer(); ut_port.Method = kComMethodWin32;
   UT_ASSERT_EQ_INT(kODRCSuccess, utt_ODComSendBuffer(
      ODPTR2HANDLE(&ut_port, tPortInfo), data, 2));
   reset_send_buffer(); ut_port.Method = kComMethodWin32; ut_write_result = FALSE;
   UT_ASSERT_EQ_INT(kODRCGeneralFailure, utt_ODComSendBuffer(
      ODPTR2HANDLE(&ut_port, tPortInfo), data, 2));
   reset_send_buffer(); ut_port.Method = kComMethodWin32; ut_written = 1;
   od_control.od_cp437_to_utf8_out = TRUE;
   UT_ASSERT_EQ_INT(kODRCGeneralFailure, utt_ODComSendBuffer(
      ODPTR2HANDLE(&ut_port, tPortInfo), data, 2));
   UT_ASSERT_EQ_UINT(1, ut_free_calls);
   reset_send_buffer(); ut_port.Method = kComMethodDoor32;
   ut_door_result = FALSE;
   UT_ASSERT_EQ_INT(kODRCGeneralFailure, utt_ODComSendBuffer(
      ODPTR2HANDLE(&ut_port, tPortInfo), data, 2));
   UT_ASSERT_EQ_UINT(0, ut_free_calls);
}
#endif

#ifdef INCLUDE_DOOR32_COM
static void reports_door32_outcomes(void)
{
   BYTE data[2] = {0x31, 0x32};
   reset_send_buffer(); ut_port.Method = kComMethodDoor32;
   UT_ASSERT_EQ_INT(kODRCSuccess, utt_ODComSendBuffer(
      ODPTR2HANDLE(&ut_port, tPortInfo), data, 2));
   reset_send_buffer(); ut_port.Method = kComMethodDoor32; ut_door_result = FALSE;
   od_control.od_cp437_to_utf8_out = TRUE;
   UT_ASSERT_EQ_INT(kODRCGeneralFailure, utt_ODComSendBuffer(
      ODPTR2HANDLE(&ut_port, tPortInfo), data, 2));
   UT_ASSERT_EQ_UINT(1, ut_free_calls);
}
#endif

#ifdef INCLUDE_SOCKET_COM
static void reports_socket_readiness_retry_and_short_write(void)
{
   BYTE data[2] = {0x31, 0x32};
   reset_send_buffer(); ut_port.Method = kComMethodSocket; ut_ready_result = 0;
   UT_ASSERT_EQ_INT(kODRCGeneralFailure, utt_ODComSendBuffer(
      ODPTR2HANDLE(&ut_port, tPortInfo), data, 2));
#ifndef ODPLAT_WIN32
   reset_send_buffer(); ut_port.Method = kComMethodSocket;
   ut_ready_result = 1; ut_ready_events = POLLHUP;
   UT_ASSERT_EQ_INT(kODRCGeneralFailure, utt_ODComSendBuffer(
      ODPTR2HANDLE(&ut_port, tPortInfo), data, 2));
   reset_send_buffer(); ut_port.Method = kComMethodSocket; ut_ready_result = -1;
   UT_ASSERT_EQ_INT(kODRCGeneralFailure, utt_ODComSendBuffer(
      ODPTR2HANDLE(&ut_port, tPortInfo), data, 2));
#endif
   reset_send_buffer(); ut_port.Method = kComMethodSocket;
   UT_ASSERT_EQ_INT(kODRCSuccess, utt_ODComSendBuffer(
      ODPTR2HANDLE(&ut_port, tPortInfo), data, 2));
   reset_send_buffer(); ut_port.Method = kComMethodSocket;
   ut_send_results[0] = SOCKET_ERROR; ut_send_results[1] = 2;
   ut_socket_error = WSAEWOULDBLOCK; errno = WSAEWOULDBLOCK;
   UT_ASSERT_EQ_INT(kODRCSuccess, utt_ODComSendBuffer(
      ODPTR2HANDLE(&ut_port, tPortInfo), data, 2));
   reset_send_buffer(); ut_port.Method = kComMethodSocket;
   ut_send_results[0] = SOCKET_ERROR; ut_socket_error = 1; errno = 1;
   UT_ASSERT_EQ_INT(kODRCGeneralFailure, utt_ODComSendBuffer(
      ODPTR2HANDLE(&ut_port, tPortInfo), data, 2));
   reset_send_buffer(); ut_port.Method = kComMethodSocket; ut_send_results[0] = 1;
   od_control.od_cp437_to_utf8_out = TRUE;
   UT_ASSERT_EQ_INT(kODRCGeneralFailure, utt_ODComSendBuffer(
      ODPTR2HANDLE(&ut_port, tPortInfo), data, 2));
   UT_ASSERT_EQ_UINT(1, ut_free_calls);
   reset_send_buffer(); ut_port.Method = kComMethodSocket; ut_ready_result = 0;
   od_control.od_cp437_to_utf8_out = TRUE;
   UT_ASSERT_EQ_INT(kODRCGeneralFailure, utt_ODComSendBuffer(
      ODPTR2HANDLE(&ut_port, tPortInfo), data, 2));
   UT_ASSERT_EQ_UINT(1, ut_free_calls);
}
#endif

#ifdef INCLUDE_STDIO_COM
static void reports_stdio_timeout_error_retry_and_partial_writes(void)
{
   BYTE data[2] = {0x31, 0x32}; unsigned index;
   reset_send_buffer(); ut_port.Method = kComMethodStdIO;
   UT_ASSERT_EQ_INT(kODRCSuccess, utt_ODComSendBuffer(
      ODPTR2HANDLE(&ut_port, tPortInfo), data, 2));
   reset_send_buffer(); ut_port.Method = kComMethodStdIO;
   ut_fwrite_results[0] = 1; ut_fwrite_results[1] = 1;
   utt_ODComSendBuffer(ODPTR2HANDLE(&ut_port, tPortInfo), data, 2);
   UT_ASSERT_EQ_UINT(1, ut_sleep_calls);
   reset_send_buffer(); ut_port.Method = kComMethodStdIO;
   for(index = 0; index < 11; ++index) ut_select_results[index] = 0;
   od_control.od_cp437_to_utf8_out = TRUE;
   UT_ASSERT_EQ_INT(kODRCGeneralFailure, utt_ODComSendBuffer(
      ODPTR2HANDLE(&ut_port, tPortInfo), data, 2));
   UT_ASSERT_EQ_UINT(1, ut_free_calls);
   reset_send_buffer(); ut_port.Method = kComMethodStdIO;
   for(index = 0; index < 11; ++index) ut_select_results[index] = 0;
   UT_ASSERT_EQ_INT(kODRCGeneralFailure, utt_ODComSendBuffer(
      ODPTR2HANDLE(&ut_port, tPortInfo), data, 2));
   reset_send_buffer(); ut_port.Method = kComMethodStdIO;
   ut_select_results[0] = -1; errno = 1;
   UT_ASSERT_EQ_INT(kODRCGeneralFailure, utt_ODComSendBuffer(
      ODPTR2HANDLE(&ut_port, tPortInfo), data, 2));
   reset_send_buffer(); ut_port.Method = kComMethodStdIO;
   ut_select_results[0] = 2; od_control.od_cp437_to_utf8_out = TRUE;
   UT_ASSERT_EQ_INT(kODRCGeneralFailure, utt_ODComSendBuffer(
      ODPTR2HANDLE(&ut_port, tPortInfo), data, 2));
   UT_ASSERT_EQ_UINT(1, ut_free_calls);
   reset_send_buffer(); ut_port.Method = kComMethodStdIO;
   ut_select_results[0] = -1; ut_select_results[1] = 1; errno = EINTR;
   od_control.od_cp437_to_utf8_out = TRUE;
   ut_fwrite_results[0] = 3;
   UT_ASSERT_EQ_INT(kODRCSuccess, utt_ODComSendBuffer(
      ODPTR2HANDLE(&ut_port, tPortInfo), data, 2));
   UT_ASSERT_EQ_UINT(1, ut_free_calls);
}
#endif

static void accepts_unknown_method_and_frees_conversion(void)
{
   BYTE data[2] = {0x31, 0x32};
   reset_send_buffer(); ut_port.Method = (tComMethod)99;
   od_control.od_cp437_to_utf8_out = TRUE;
   UT_ASSERT_EQ_INT(kODRCSuccess, utt_ODComSendBuffer(
      ODPTR2HANDLE(&ut_port, tPortInfo), data, 2));
   UT_ASSERT_EQ_UINT(1, ut_free_calls);
}

static const UTTestCase ut_cases[] = {
   {"empty/conversion", handles_empty_and_conversion_results},
#ifdef INCLUDE_FOSSIL_COM
   {"FOSSIL", sends_fossil_blocks_and_fallback_bytes},
#endif
#ifdef INCLUDE_UART_COM
   {"UART", fills_uart_queue_in_one_or_multiple_wrapped_parts},
#endif
#ifdef INCLUDE_WIN32_COM
   {"Win32", reports_win32_write_outcomes_and_frees_conversion},
#endif
#ifdef INCLUDE_DOOR32_COM
   {"Door32", reports_door32_outcomes},
#endif
#ifdef INCLUDE_SOCKET_COM
   {"socket", reports_socket_readiness_retry_and_short_write},
#endif
#ifdef INCLUDE_STDIO_COM
   {"stdio", reports_stdio_timeout_error_retry_and_partial_writes},
#endif
   {"unknown", accepts_unknown_method_and_frees_conversion}
};
