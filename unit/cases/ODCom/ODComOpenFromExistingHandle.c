#ifdef INCLUDE_SOCKET_COM
#define UT_CUSTOM_MOCK_getsockopt
#define UT_CUSTOM_MOCK_setsockopt

static unsigned ut_get_option_calls;
static unsigned ut_set_option_calls;

#ifdef ODPLAT_WIN32
int PASCAL utm_getsockopt(SOCKET socket_handle, int level, int option,
   char FAR *value, int FAR *length)
#else
int utm_getsockopt(int socket_handle, int level, int option, void *value,
   socklen_t *length)
#endif
{
   ++ut_get_option_calls;
   UT_ASSERT_EQ_INT(42, socket_handle);
   UT_ASSERT_EQ_INT(IPPROTO_TCP, level);
   UT_ASSERT_EQ_INT(TCP_NODELAY, option);
   UT_ASSERT_NOT_NULL(value);
   UT_ASSERT_NOT_NULL(length);
   UT_ASSERT_EQ_UINT(sizeof(int), *length);
   *(int *)value = 7;
   return(0);
}

#ifdef ODPLAT_WIN32
int PASCAL utm_setsockopt(SOCKET socket_handle, int level, int option,
   const char FAR *value, int length)
#else
int utm_setsockopt(int socket_handle, int level, int option,
   const void *value, socklen_t length)
#endif
{
   ++ut_set_option_calls;
   UT_ASSERT_EQ_INT(42, socket_handle);
   UT_ASSERT_EQ_INT(IPPROTO_TCP, level);
   UT_ASSERT_EQ_INT(TCP_NODELAY, option);
   UT_ASSERT_NOT_NULL(value);
   UT_ASSERT_EQ_UINT(sizeof(int), length);
   UT_ASSERT_EQ_INT(FALSE, *(const int *)value);
   return(0);
}
#endif

#define UT_CUSTOM_MOCK_ODSemaphoreAlloc
#ifndef OD_MULTITHREADED
/* The Windows model includes this dependency, while host-side LLVM replay
 * compiles the generated unit through the non-threaded UNIX headers. */
typedef void *tODSemaphoreHandle;
#endif

static tODResult ut_semaphore_result;
static unsigned ut_semaphore_calls;

tODResult utm_ODSemaphoreAlloc(tODSemaphoreHandle *semaphore,
   INT initial_count, INT maximum_count)
{
   ++ut_semaphore_calls;
   UT_ASSERT_NOT_NULL(semaphore);
   UT_ASSERT_EQ_INT(0, initial_count);
   UT_ASSERT_EQ_INT(1, maximum_count);
   return(ut_semaphore_result);
}

static tPortInfo ut_port;

static void reset_existing_handle(void)
{
   memset(&ut_port, 0, sizeof(ut_port));
#ifdef INCLUDE_SOCKET_COM
   ut_get_option_calls = 0;
   ut_set_option_calls = 0;
#endif
#ifdef OD_MULTITHREADED
   ut_semaphore_result = kODRCSuccess;
   ut_semaphore_calls = 0;
#endif
}

#ifdef INCLUDE_SOCKET_COM
static void adopts_a_socket_and_disables_nagle(void)
{
   reset_existing_handle();
   ut_port.Method = kComMethodSocket;
   UT_ASSERT_EQ_INT(kODRCSuccess, utt_ODComOpenFromExistingHandle(
      ODPTR2HANDLE(&ut_port, tPortInfo), (DWORD_PTR)42));
   UT_ASSERT_EQ_INT(42, ut_port.socket);
   UT_ASSERT_EQ_INT(7, ut_port.old_delay);
   UT_ASSERT_EQ_INT(TRUE, ut_port.bIsOpen);
   UT_ASSERT_EQ_UINT(1, ut_get_option_calls);
   UT_ASSERT_EQ_UINT(1, ut_set_option_calls);
#ifdef OD_MULTITHREADED
   UT_ASSERT_EQ_UINT(1, ut_semaphore_calls);
#endif
}
#endif

#ifdef OD_MULTITHREADED
static void reports_socket_semaphore_failure_without_adopting_the_handle(void)
{
   reset_existing_handle();
   ut_port.Method = kComMethodSocket;
   ut_semaphore_result = kODRCNoMemory;
   UT_ASSERT_EQ_INT(kODRCNoMemory, utt_ODComOpenFromExistingHandle(
      ODPTR2HANDLE(&ut_port, tPortInfo), (DWORD_PTR)42));
   UT_ASSERT_EQ_INT(FALSE, ut_port.bIsOpen);
   UT_ASSERT_EQ_UINT(1, ut_semaphore_calls);
   UT_ASSERT_EQ_UINT(0, ut_get_option_calls);
   UT_ASSERT_EQ_UINT(0, ut_set_option_calls);
}
#endif

static void handles_a_non_socket_native_handle(void)
{
   reset_existing_handle();
   ut_port.Method = kComMethodWin32;
#ifdef INCLUDE_WIN32_COM
   UT_ASSERT_EQ_INT(kODRCSuccess, utt_ODComOpenFromExistingHandle(
      ODPTR2HANDLE(&ut_port, tPortInfo), (DWORD_PTR)43));
   UT_ASSERT_EQ_PTR((HANDLE)(DWORD_PTR)43, ut_port.hCommDev);
   UT_ASSERT_EQ_INT(kNotSet, ut_port.ReadTimeoutState);
   UT_ASSERT_EQ_INT(TRUE, ut_port.bUsingClientsHandle);
   UT_ASSERT_EQ_INT(TRUE, ut_port.bIsOpen);
#else
   UT_ASSERT_EQ_INT(kODRCInvalidCall, utt_ODComOpenFromExistingHandle(
      ODPTR2HANDLE(&ut_port, tPortInfo), (DWORD_PTR)43));
   UT_ASSERT_EQ_INT(FALSE, ut_port.bUsingClientsHandle);
   UT_ASSERT_EQ_INT(FALSE, ut_port.bIsOpen);
#endif
}

static const UTTestCase ut_cases[] = {
#ifdef INCLUDE_SOCKET_COM
   {"socket", adopts_a_socket_and_disables_nagle},
#endif
#ifdef OD_MULTITHREADED
   {"semaphore failure",
      reports_socket_semaphore_failure_without_adopting_the_handle},
#endif
   {"native handle", handles_a_non_socket_native_handle}
};
