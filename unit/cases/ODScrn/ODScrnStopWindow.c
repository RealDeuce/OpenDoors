#define UT_CUSTOM_MOCK_PostThreadMessageA
#define UT_CUSTOM_MOCK_ODThreadWaitForExit
#define UT_CUSTOM_MOCK_CloseHandle
static unsigned ut_post_calls;
static unsigned ut_wait_calls;
static unsigned ut_close_calls;
static HANDLE ut_thread;
static HANDLE ut_event;
BOOL WINAPI utm_PostThreadMessageA(DWORD thread_id, UINT message,
   WPARAM wparam, LPARAM lparam)
{
   ++ut_post_calls; UT_ASSERT_EQ_UINT(17, thread_id);
   UT_ASSERT_EQ_UINT(WM_QUIT, message); UT_ASSERT_EQ_UINT(0, wparam);
   UT_ASSERT_EQ_INT(0, lparam); return TRUE;
}
void utm_ODThreadWaitForExit(tODThreadHandle thread)
{
   ++ut_wait_calls; UT_ASSERT(thread == ut_thread);
}
BOOL WINAPI utm_CloseHandle(HANDLE handle)
{
   ++ut_close_calls;
   UT_ASSERT(handle == ut_thread || handle == ut_event);
   return TRUE;
}
static void reset_stop(void)
{
   ut_thread = (HANDLE)1; ut_event = (HANDLE)2;
   ut_post_calls = ut_wait_calls = ut_close_calls = 0;
   dwScreenThreadID = 0; hScreenStartedEvent = NULL;
}
static void ignores_an_empty_thread_handle(void)
{
   tODThreadHandle thread;
   reset_stop(); thread = NULL;
   utt_ODScrnStopWindow(&thread);
   UT_ASSERT_EQ_UINT(0, ut_wait_calls); UT_ASSERT_EQ_UINT(0, ut_close_calls);
}
static void joins_and_closes_a_thread_without_a_start_event(void)
{
   tODThreadHandle thread;
   reset_stop(); thread = ut_thread;
   utt_ODScrnStopWindow(&thread);
   UT_ASSERT(thread == NULL); UT_ASSERT_EQ_UINT(0, ut_post_calls);
   UT_ASSERT_EQ_UINT(1, ut_wait_calls); UT_ASSERT_EQ_UINT(1, ut_close_calls);
}
static void requests_stop_and_closes_the_pending_event_and_thread(void)
{
   tODThreadHandle thread;
   reset_stop(); thread = ut_thread;
   dwScreenThreadID = 17; hScreenStartedEvent = ut_event;
   utt_ODScrnStopWindow(&thread);
   UT_ASSERT(thread == NULL); UT_ASSERT(hScreenStartedEvent == NULL);
   UT_ASSERT_EQ_UINT(1, ut_post_calls); UT_ASSERT_EQ_UINT(1, ut_wait_calls);
   UT_ASSERT_EQ_UINT(2, ut_close_calls);
}
static const UTTestCase ut_cases[] = {
   {"empty handle", ignores_an_empty_thread_handle},
   {"join thread", joins_and_closes_a_thread_without_a_start_event},
   {"stop with event", requests_stop_and_closes_the_pending_event_and_thread}
};
