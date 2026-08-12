#if defined(INCLUDE_UART_COM) || defined(INCLUDE_FOSSIL_COM) || \
   defined(INCLUDE_STDIO_COM) || defined(INCLUDE_WIN32_COM)
#define UT_CUSTOM_MOCK_ODComCarrier
static int ut_carrier_calls;
static int ut_carrier_before_loss;

tODResult utm_ODComCarrier(tPortHandle port, BOOL *carrier)
{
   UT_ASSERT_NOT_NULL(port);
   UT_ASSERT_NOT_NULL(carrier);
   *carrier = ut_carrier_calls++ < ut_carrier_before_loss;
   return(kODRCSuccess);
}
#endif

#if defined(INCLUDE_UART_COM) || defined(INCLUDE_FOSSIL_COM) || \
   defined(INCLUDE_STDIO_COM)
static unsigned ut_idle_calls;

static void ODCALL ut_idle(void)
{
   ++ut_idle_calls;
}

static void waits_for_carrier_loss(tComMethod method, BOOL with_idle)
{
   tPortInfo port;
   memset(&port, 0, sizeof(port));
   port.bIsOpen = TRUE;
   port.Method = method;
   port.pfIdleCallback = with_idle ? ut_idle : NULL;
   ut_carrier_calls = 0;
   ut_carrier_before_loss = 1;
   ut_idle_calls = 0;
   UT_ASSERT_EQ_INT(kODRCSuccess,
      utt_ODComWaitEvent(ODPTR2HANDLE(&port, tPortInfo), kNoCarrier));
   UT_ASSERT_EQ_INT(2, ut_carrier_calls);
   UT_ASSERT_EQ_UINT(with_idle ? 1 : 0, ut_idle_calls);
}

static void waits_with_each_direct_serial_method(void)
{
   waits_for_carrier_loss(kComMethodFOSSIL, TRUE);
   waits_for_carrier_loss(kComMethodUART, FALSE);
   waits_for_carrier_loss(kComMethodStdIO, TRUE);
}

static void ignores_an_invalid_direct_serial_event(void)
{
   tPortInfo port;
   memset(&port, 0, sizeof(port));
   port.bIsOpen = TRUE;
#ifdef INCLUDE_FOSSIL_COM
   port.Method = kComMethodFOSSIL;
#else
   port.Method = kComMethodStdIO;
#endif
   UT_ASSERT_EQ_INT(kODRCSuccess,
      utt_ODComWaitEvent(ODPTR2HANDLE(&port, tPortInfo), (tComEvent)99));
}
#endif

#ifdef INCLUDE_WIN32_COM
#define UT_CUSTOM_MOCK_GetCommMask
#define UT_CUSTOM_MOCK_SetCommMask
#define UT_CUSTOM_MOCK_WaitCommEvent
static int ut_wait_calls;
static BOOL ut_get_mask_result;
static BOOL ut_set_mask_result;
static BOOL ut_wait_result;
static DWORD ut_expected_set_mask;
static int ut_wait_fail_after;

BOOL WINAPI utm_GetCommMask(HANDLE device, LPDWORD mask)
{
   UT_ASSERT_EQ_PTR((HANDLE)(DWORD_PTR)41, device);
   UT_ASSERT_NOT_NULL(mask);
   *mask = EV_RXCHAR;
   return(ut_get_mask_result);
}

BOOL WINAPI utm_SetCommMask(HANDLE device, DWORD mask)
{
   UT_ASSERT_EQ_PTR((HANDLE)(DWORD_PTR)41, device);
   UT_ASSERT_EQ_UINT(ut_expected_set_mask, mask);
   return(ut_set_mask_result);
}

BOOL WINAPI utm_WaitCommEvent(HANDLE device, LPDWORD mask,
   LPOVERLAPPED overlapped)
{
   UT_ASSERT_EQ_PTR((HANDLE)(DWORD_PTR)41, device);
   UT_ASSERT_NOT_NULL(mask);
   UT_ASSERT_NULL(overlapped);
   ++ut_wait_calls;
   *mask = EV_RLSD;
   if(ut_wait_fail_after != 0 && ut_wait_calls >= ut_wait_fail_after)
      return(FALSE);
   return(ut_wait_result);
}

static void reset_windows_port(tPortInfo *port)
{
   memset(port, 0, sizeof(*port));
   port->bIsOpen = TRUE;
   port->Method = kComMethodWin32;
   port->hCommDev = (HANDLE)(DWORD_PTR)41;
   ut_get_mask_result = TRUE;
   ut_set_mask_result = TRUE;
   ut_wait_result = TRUE;
   ut_expected_set_mask = EV_RXCHAR | EV_RLSD;
   ut_wait_fail_after = 0;
   ut_wait_calls = 0;
   ut_carrier_calls = 0;
   ut_carrier_before_loss = 0;
}

static void reports_each_windows_api_failure(void)
{
   tPortInfo port;
   reset_windows_port(&port);
   ut_get_mask_result = FALSE;
   UT_ASSERT_EQ_INT(kODRCGeneralFailure,
      utt_ODComWaitEvent(ODPTR2HANDLE(&port, tPortInfo), kNoCarrier));
   reset_windows_port(&port);
   ut_set_mask_result = FALSE;
   UT_ASSERT_EQ_INT(kODRCGeneralFailure,
      utt_ODComWaitEvent(ODPTR2HANDLE(&port, tPortInfo), kNoCarrier));
   reset_windows_port(&port);
   ut_wait_result = FALSE;
   UT_ASSERT_EQ_INT(kODRCGeneralFailure,
      utt_ODComWaitEvent(ODPTR2HANDLE(&port, tPortInfo), kNoCarrier));
}

static void waits_for_windows_carrier_loss(void)
{
   tPortInfo port;
   reset_windows_port(&port);
   ut_carrier_before_loss = 1;
   UT_ASSERT_EQ_INT(kODRCSuccess,
      utt_ODComWaitEvent(ODPTR2HANDLE(&port, tPortInfo), kNoCarrier));
   UT_ASSERT_EQ_INT(2, ut_wait_calls);
   UT_ASSERT_EQ_INT(2, ut_carrier_calls);
}

static void ignores_an_invalid_windows_event(void)
{
   tPortInfo port;
   reset_windows_port(&port);
   ut_expected_set_mask = EV_RXCHAR;
   ut_wait_fail_after = 2;
   UT_ASSERT_EQ_INT(kODRCGeneralFailure,
      utt_ODComWaitEvent(ODPTR2HANDLE(&port, tPortInfo), (tComEvent)99));
}
#endif

#ifdef INCLUDE_DOOR32_COM
#define UT_CUSTOM_MOCK_WaitForSingleObject
static HANDLE WINAPI ut_offline_event(void)
{
   return((HANDLE)(DWORD_PTR)43);
}

DWORD WINAPI utm_WaitForSingleObject(HANDLE object, DWORD timeout)
{
   UT_ASSERT_EQ_PTR((HANDLE)(DWORD_PTR)43, object);
   UT_ASSERT_EQ_UINT(INFINITE, timeout);
   return(WAIT_OBJECT_0);
}

static void waits_for_the_door32_offline_event(void)
{
   tPortInfo port;
   memset(&port, 0, sizeof(port));
   port.bIsOpen = TRUE;
   port.Method = kComMethodDoor32;
   port.pfDoorGetOfflineEventHandle = ut_offline_event;
   UT_ASSERT_EQ_INT(kODRCSuccess,
      utt_ODComWaitEvent(ODPTR2HANDLE(&port, tPortInfo), kNoCarrier));
}

static void ignores_an_invalid_door32_event(void)
{
   tPortInfo port;
   memset(&port, 0, sizeof(port));
   port.bIsOpen = TRUE;
   port.Method = kComMethodDoor32;
   port.pfDoorGetOfflineEventHandle = ut_offline_event;
   UT_ASSERT_EQ_INT(kODRCSuccess,
      utt_ODComWaitEvent(ODPTR2HANDLE(&port, tPortInfo), (tComEvent)99));
}
#endif

#if defined(INCLUDE_SOCKET_COM) && defined(OD_MULTITHREADED)
#define UT_CUSTOM_MOCK_ODSemaphoreDown
#define UT_CUSTOM_MOCK_ODSemaphoreUp
static unsigned ut_semaphore_down_calls;

tODResult utm_ODSemaphoreDown(tODSemaphoreHandle semaphore,
   tODMilliSec timeout)
{
   UT_ASSERT_EQ_PTR((tODSemaphoreHandle)(DWORD_PTR)47, semaphore);
   UT_ASSERT_EQ_UINT(OD_NO_TIMEOUT, timeout);
   return(ut_semaphore_down_calls++ == 0 ? kODRCGeneralFailure : kODRCSuccess);
}

void utm_ODSemaphoreUp(tODSemaphoreHandle semaphore, INT increment)
{
   UT_ASSERT_EQ_PTR((tODSemaphoreHandle)(DWORD_PTR)47, semaphore);
   UT_ASSERT_EQ_INT(1, increment);
}

static void waits_for_threaded_socket_carrier_loss(void)
{
   tPortInfo port;
   memset(&port, 0, sizeof(port));
   port.bIsOpen = TRUE;
   port.Method = kComMethodSocket;
   port.hCarrierLostSemaphore = (tODSemaphoreHandle)(DWORD_PTR)47;
   ut_semaphore_down_calls = 0;
   UT_ASSERT_EQ_INT(kODRCSuccess,
      utt_ODComWaitEvent(ODPTR2HANDLE(&port, tPortInfo), kNoCarrier));
   UT_ASSERT_EQ_UINT(2, ut_semaphore_down_calls);
}
#endif

#if defined(INCLUDE_SOCKET_COM) && !defined(OD_MULTITHREADED)
#define UT_CUSTOM_MOCK_recv
#define UT_CUSTOM_MOCK___error
static unsigned ut_recv_calls;
static int ut_socket_errno;
static BOOL ut_recv_would_block;

int *utm___error(void)
{
   return(&ut_socket_errno);
}

ssize_t utm_recv(int socket_handle, void *buffer, size_t size, int flags)
{
   UT_ASSERT_EQ_INT(45, socket_handle);
   UT_ASSERT_NOT_NULL(buffer);
   UT_ASSERT_EQ_UINT(1, size);
   UT_ASSERT_EQ_INT(MSG_PEEK, flags);
   ++ut_recv_calls;
   if(ut_recv_calls == 1)
   {
      ut_socket_errno = EAGAIN;
      if(!ut_recv_would_block)
         ut_socket_errno = EIO;
      return(SOCKET_ERROR);
   }
   return(ut_recv_calls == 2 ? 1 : 0);
}

static void waits_for_unthreaded_socket_carrier_loss(void)
{
   tPortInfo port;
   memset(&port, 0, sizeof(port));
   port.bIsOpen = TRUE;
   port.Method = kComMethodSocket;
   port.socket = 45;
   ut_recv_calls = 0;
   ut_recv_would_block = TRUE;
   UT_ASSERT_EQ_INT(kODRCSuccess,
      utt_ODComWaitEvent(ODPTR2HANDLE(&port, tPortInfo), kNoCarrier));
   UT_ASSERT_EQ_UINT(3, ut_recv_calls);
}

static void stops_on_an_unthreaded_socket_error(void)
{
   tPortInfo port;
   memset(&port, 0, sizeof(port));
   port.bIsOpen = TRUE;
   port.Method = kComMethodSocket;
   port.socket = 45;
   ut_recv_calls = 0;
   ut_recv_would_block = FALSE;
   UT_ASSERT_EQ_INT(kODRCSuccess,
      utt_ODComWaitEvent(ODPTR2HANDLE(&port, tPortInfo), kNoCarrier));
   UT_ASSERT_EQ_UINT(1, ut_recv_calls);
}
#endif


#ifdef INCLUDE_SOCKET_COM
static void ignores_an_invalid_socket_event(void)
{
   tPortInfo port;
   memset(&port, 0, sizeof(port));
   port.bIsOpen = TRUE;
   port.Method = kComMethodSocket;
   UT_ASSERT_EQ_INT(kODRCSuccess,
      utt_ODComWaitEvent(ODPTR2HANDLE(&port, tPortInfo), (tComEvent)99));
}
#endif

static void accepts_an_unhandled_method_as_the_legacy_assert_build_does(void)
{
   tPortInfo port;
   memset(&port, 0, sizeof(port));
   port.bIsOpen = TRUE;
   port.Method = (tComMethod)99;
   UT_ASSERT_EQ_INT(kODRCSuccess,
      utt_ODComWaitEvent(ODPTR2HANDLE(&port, tPortInfo), kNoCarrier));
}

static const UTTestCase ut_cases[] = {
#if defined(INCLUDE_UART_COM) || defined(INCLUDE_FOSSIL_COM) || defined(INCLUDE_STDIO_COM)
   {"direct serial methods", waits_with_each_direct_serial_method},
   {"invalid direct event", ignores_an_invalid_direct_serial_event},
#endif
#ifdef INCLUDE_WIN32_COM
   {"Windows failures", reports_each_windows_api_failure},
   {"Windows carrier loss", waits_for_windows_carrier_loss},
   {"invalid Windows event", ignores_an_invalid_windows_event},
#endif
#ifdef INCLUDE_DOOR32_COM
   {"Door32 offline event", waits_for_the_door32_offline_event},
   {"invalid Door32 event", ignores_an_invalid_door32_event},
#endif
#if defined(INCLUDE_SOCKET_COM) && defined(OD_MULTITHREADED)
   {"threaded socket", waits_for_threaded_socket_carrier_loss},
#endif
#if defined(INCLUDE_SOCKET_COM) && !defined(OD_MULTITHREADED)
   {"unthreaded socket", waits_for_unthreaded_socket_carrier_loss},
   {"unthreaded socket error", stops_on_an_unthreaded_socket_error},
#endif
#ifdef INCLUDE_SOCKET_COM
   {"invalid socket event", ignores_an_invalid_socket_event},
#endif
   {"unhandled method", accepts_an_unhandled_method_as_the_legacy_assert_build_does}
};
