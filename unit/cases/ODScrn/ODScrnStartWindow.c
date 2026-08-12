#define UT_CUSTOM_MOCK_malloc
#define UT_CUSTOM_MOCK_free
#define UT_CUSTOM_MOCK_CreateEventA
#define UT_CUSTOM_MOCK_ODThreadCreate
#define UT_CUSTOM_MOCK_ODScrnThreadProc
#define UT_CUSTOM_MOCK_ODScrnWaitWithMessages
#define UT_CUSTOM_MOCK_CloseHandle
#define UT_CUSTOM_MOCK_PostThreadMessageA

static tODScrnThreadInfo ut_thread_info;
static HANDLE ut_event = (HANDLE)1;
static HANDLE ut_thread = (HANDLE)2;
static HANDLE ut_instance = (HANDLE)3;
static HWND ut_frame = (HWND)4;
static BOOL ut_malloc_fails;
static BOOL ut_event_fails;
static tODResult ut_create_result;
static BOOL ut_wait_results[3];
static DWORD ut_wait_timeouts[3];
static tODResult ut_screen_results[3];
static unsigned ut_wait_count;
static unsigned ut_wait_index;
static unsigned ut_free_calls;
static unsigned ut_close_event_calls;
static unsigned ut_close_thread_calls;
static unsigned ut_post_calls;

DWORD WINAPI utm_ODScrnThreadProc(void *parameter)
{
   (void)parameter;
   ut_unexpected_mock(3, "ODScrnThreadProc");
   return 0;
}

void *utm_malloc(size_t size)
{
   UT_ASSERT_EQ_UINT(sizeof(tODScrnThreadInfo), size);
   return ut_malloc_fails ? NULL : &ut_thread_info;
}

void utm_free(void *memory)
{
   ++ut_free_calls;
   UT_ASSERT_EQ_PTR(&ut_thread_info, memory);
}

HANDLE WINAPI utm_CreateEventA(LPSECURITY_ATTRIBUTES attributes,
   BOOL manual_reset, BOOL initial_state, LPCSTR name)
{
   UT_ASSERT(attributes == NULL); UT_ASSERT_EQ_INT(TRUE, manual_reset);
   UT_ASSERT_EQ_INT(FALSE, initial_state); UT_ASSERT(name == NULL);
   return ut_event_fails ? NULL : ut_event;
}

tODResult utm_ODThreadCreate(tODThreadHandle *thread,
   ptODThreadProc *procedure, void *parameter)
{
   UT_ASSERT_NOT_NULL(thread); UT_ASSERT(procedure == utm_ODScrnThreadProc);
   UT_ASSERT_EQ_PTR(&ut_thread_info, parameter);
   UT_ASSERT(ut_thread_info.hInstance == ut_instance);
   UT_ASSERT(ut_thread_info.hwndFrame == ut_frame);
   if(ut_create_result == kODRCSuccess) *thread = ut_thread;
   return ut_create_result;
}

BOOL utm_ODScrnWaitWithMessages(HANDLE object, HWND frame, DWORD timeout)
{
   unsigned index;
   UT_ASSERT(ut_wait_index < ut_wait_count);
   index = ut_wait_index;
   UT_ASSERT(frame == ut_frame);
   UT_ASSERT(object == (ut_wait_index == 0 ? ut_event :
      (ut_wait_timeouts[ut_wait_index] == INFINITE &&
       ut_wait_index + 1 == ut_wait_count ? ut_thread : ut_event)));
   UT_ASSERT_EQ_UINT(ut_wait_timeouts[ut_wait_index], timeout);
   ScreenStartResult = ut_screen_results[index];
   ++ut_wait_index;
   return ut_wait_results[index];
}

BOOL WINAPI utm_CloseHandle(HANDLE handle)
{
   if(handle == ut_event) ++ut_close_event_calls;
   else
   {
      UT_ASSERT(handle == ut_thread);
      ++ut_close_thread_calls;
   }
   return TRUE;
}

BOOL WINAPI utm_PostThreadMessageA(DWORD thread_id, UINT message,
   WPARAM wparam, LPARAM lparam)
{
   ++ut_post_calls; UT_ASSERT_EQ_UINT(37, thread_id);
   UT_ASSERT_EQ_UINT(WM_QUIT, message); UT_ASSERT_EQ_UINT(0, wparam);
   UT_ASSERT_EQ_INT(0, lparam); return TRUE;
}

static void reset_start(void)
{
   memset(&ut_thread_info, 0, sizeof(ut_thread_info));
   memset(ut_wait_results, 0, sizeof(ut_wait_results));
   memset(ut_wait_timeouts, 0, sizeof(ut_wait_timeouts));
   memset(ut_screen_results, 0, sizeof(ut_screen_results));
   ut_malloc_fails = ut_event_fails = FALSE;
   ut_create_result = kODRCSuccess;
   ut_wait_count = ut_wait_index = 0;
   ut_free_calls = ut_close_event_calls = ut_close_thread_calls = 0;
   ut_post_calls = 0; hScreenStartedEvent = NULL;
   ScreenStartResult = kODRCGeneralFailure; dwScreenThreadID = 0;
   ut_screen_results[0] = ut_screen_results[1] = ut_screen_results[2] =
      kODRCGeneralFailure;
}

static tODResult start_window(tODThreadHandle *thread)
{
   *thread = NULL;
   return utt_ODScrnStartWindow(ut_instance, thread, ut_frame);
}

static void reports_an_allocation_failure_without_creating_an_event(void)
{
   tODThreadHandle thread;
   reset_start(); ut_malloc_fails = TRUE;
   UT_ASSERT_EQ_INT(kODRCNoMemory, start_window(&thread));
   UT_ASSERT(hScreenStartedEvent == NULL); UT_ASSERT_EQ_UINT(0, ut_free_calls);
}

static void releases_startup_information_when_event_creation_fails(void)
{
   tODThreadHandle thread;
   reset_start(); ut_event_fails = TRUE;
   UT_ASSERT_EQ_INT(kODRCGeneralFailure, start_window(&thread));
   UT_ASSERT_EQ_UINT(1, ut_free_calls); UT_ASSERT_EQ_UINT(0, ut_close_event_calls);
}

static void releases_event_and_information_when_thread_creation_fails(void)
{
   tODThreadHandle thread;
   reset_start(); ut_create_result = kODRCNoMemory;
   UT_ASSERT_EQ_INT(kODRCNoMemory, start_window(&thread));
   UT_ASSERT(hScreenStartedEvent == NULL); UT_ASSERT_EQ_UINT(1, ut_free_calls);
   UT_ASSERT_EQ_UINT(1, ut_close_event_calls);
}

static void returns_the_result_published_by_a_started_thread(void)
{
   tODThreadHandle thread;
   reset_start(); ut_screen_results[0] = kODRCSuccess;
   ut_wait_results[0] = TRUE; ut_wait_timeouts[0] = OD_SCREEN_THREAD_TIMEOUT;
   ut_wait_count = 1;
   UT_ASSERT_EQ_INT(kODRCSuccess, start_window(&thread));
   UT_ASSERT(thread == ut_thread); UT_ASSERT(hScreenStartedEvent == NULL);
   UT_ASSERT_EQ_UINT(1, ut_close_event_calls);
}

static void joins_a_thread_that_publishes_startup_failure(void)
{
   tODThreadHandle thread;
   reset_start();
   ut_wait_results[0] = TRUE; ut_wait_results[1] = TRUE;
   ut_wait_timeouts[0] = OD_SCREEN_THREAD_TIMEOUT;
   ut_wait_timeouts[1] = INFINITE; ut_wait_count = 2;
   UT_ASSERT_EQ_INT(kODRCGeneralFailure, start_window(&thread));
   UT_ASSERT(thread == NULL); UT_ASSERT_EQ_UINT(1, ut_close_event_calls);
   UT_ASSERT_EQ_UINT(1, ut_close_thread_calls);
}

static void timeout_joins_a_failed_thread_without_posting_quit(void)
{
   tODThreadHandle thread;
   reset_start();
   ut_wait_results[0] = FALSE; ut_wait_results[1] = TRUE;
   ut_wait_results[2] = TRUE;
   ut_wait_timeouts[0] = OD_SCREEN_THREAD_TIMEOUT;
   ut_wait_timeouts[1] = INFINITE; ut_wait_timeouts[2] = INFINITE;
   ut_wait_count = 3; dwScreenThreadID = 37;
   UT_ASSERT_EQ_INT(kODRCGeneralFailure, start_window(&thread));
   UT_ASSERT(thread == NULL); UT_ASSERT_EQ_UINT(0, ut_post_calls);
   UT_ASSERT_EQ_UINT(1, ut_close_event_calls);
   UT_ASSERT_EQ_UINT(1, ut_close_thread_calls);
}

static void timeout_does_not_post_quit_without_a_thread_identifier(void)
{
   tODThreadHandle thread;
   reset_start(); ut_screen_results[1] = ut_screen_results[2] = kODRCSuccess;
   ut_wait_results[0] = FALSE; ut_wait_results[1] = TRUE;
   ut_wait_results[2] = TRUE;
   ut_wait_timeouts[0] = OD_SCREEN_THREAD_TIMEOUT;
   ut_wait_timeouts[1] = INFINITE; ut_wait_timeouts[2] = INFINITE;
   ut_wait_count = 3;
   UT_ASSERT_EQ_INT(kODRCGeneralFailure, start_window(&thread));
   UT_ASSERT_EQ_UINT(0, ut_post_calls);
}

static void timeout_requests_stop_for_a_successfully_started_thread(void)
{
   tODThreadHandle thread;
   reset_start(); ut_screen_results[1] = ut_screen_results[2] = kODRCSuccess;
   dwScreenThreadID = 37;
   ut_wait_results[0] = FALSE; ut_wait_results[1] = TRUE;
   ut_wait_results[2] = TRUE;
   ut_wait_timeouts[0] = OD_SCREEN_THREAD_TIMEOUT;
   ut_wait_timeouts[1] = INFINITE; ut_wait_timeouts[2] = INFINITE;
   ut_wait_count = 3;
   UT_ASSERT_EQ_INT(kODRCGeneralFailure, start_window(&thread));
   UT_ASSERT_EQ_UINT(1, ut_post_calls); UT_ASSERT_EQ_UINT(1, ut_close_event_calls);
   UT_ASSERT_EQ_UINT(1, ut_close_thread_calls);
}

static const UTTestCase ut_cases[] = {
   {"allocation", reports_an_allocation_failure_without_creating_an_event},
   {"event", releases_startup_information_when_event_creation_fails},
   {"thread", releases_event_and_information_when_thread_creation_fails},
   {"success", returns_the_result_published_by_a_started_thread},
   {"startup failure", joins_a_thread_that_publishes_startup_failure},
   {"timeout failure", timeout_joins_a_failed_thread_without_posting_quit},
   {"timeout no id", timeout_does_not_post_quit_without_a_thread_identifier},
   {"timeout stop", timeout_requests_stop_for_a_successfully_started_thread}
};
