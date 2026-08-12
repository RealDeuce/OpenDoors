#define UT_CUSTOM_MOCK_GetTickCount
#define UT_CUSTOM_MOCK_MsgWaitForMultipleObjects
#define UT_CUSTOM_MOCK_PeekMessageA
#define UT_CUSTOM_MOCK_PostQuitMessage
#define UT_CUSTOM_MOCK_ODFrameTranslateAccelerator
#define UT_CUSTOM_MOCK_TranslateMessage
#define UT_CUSTOM_MOCK_DispatchMessageA

static DWORD ut_ticks[4];
static DWORD ut_wait_results[4];
static DWORD ut_expected_remaining[4];
static MSG ut_messages[4];
static BOOL ut_peek_results[5];
static BOOL ut_accel_results[4];
static unsigned ut_tick_index;
static unsigned ut_tick_count;
static unsigned ut_wait_index;
static unsigned ut_wait_count;
static unsigned ut_peek_index;
static unsigned ut_peek_count;
static unsigned ut_accel_index;
static unsigned ut_accel_count;
static unsigned ut_quit_calls;
static unsigned ut_translate_calls;
static unsigned ut_dispatch_calls;
static HANDLE ut_object;
static HWND ut_frame;

DWORD WINAPI utm_GetTickCount(void)
{
   UT_ASSERT(ut_tick_index < ut_tick_count);
   return ut_ticks[ut_tick_index++];
}

DWORD WINAPI utm_MsgWaitForMultipleObjects(DWORD count,
   const HANDLE *objects, WINBOOL wait_all, DWORD milliseconds,
   DWORD wake_mask)
{
   UT_ASSERT_EQ_UINT(1, count); UT_ASSERT(objects != NULL);
   UT_ASSERT(*objects == ut_object); UT_ASSERT_EQ_INT(FALSE, wait_all);
   UT_ASSERT_EQ_UINT(QS_ALLINPUT, wake_mask);
   UT_ASSERT(ut_wait_index < ut_wait_count);
   UT_ASSERT_EQ_UINT(ut_expected_remaining[ut_wait_index], milliseconds);
   return ut_wait_results[ut_wait_index++];
}

WINBOOL WINAPI utm_PeekMessageA(LPMSG message, HWND window, UINT first,
   UINT last, UINT remove)
{
   BOOL result;
   UT_ASSERT(window == NULL); UT_ASSERT_EQ_UINT(0, first);
   UT_ASSERT_EQ_UINT(0, last); UT_ASSERT_EQ_UINT(PM_REMOVE, remove);
   UT_ASSERT(ut_peek_index < ut_peek_count);
   result = ut_peek_results[ut_peek_index];
   if(result) *message = ut_messages[ut_peek_index];
   ++ut_peek_index;
   return result;
}

void WINAPI utm_PostQuitMessage(int exit_code)
{
   ++ut_quit_calls; UT_ASSERT_EQ_INT(23, exit_code);
}

BOOL utm_ODFrameTranslateAccelerator(HWND frame, MSG *message)
{
   UT_ASSERT(frame == ut_frame); UT_ASSERT(message != NULL);
   UT_ASSERT(ut_accel_index < ut_accel_count);
   return ut_accel_results[ut_accel_index++];
}

WINBOOL WINAPI utm_TranslateMessage(const MSG *message)
{
   ++ut_translate_calls; UT_ASSERT(message != NULL); return TRUE;
}

LRESULT WINAPI utm_DispatchMessageA(const MSG *message)
{
   ++ut_dispatch_calls; UT_ASSERT(message != NULL); return 0;
}

static void reset_wait(void)
{
   memset(ut_ticks, 0, sizeof(ut_ticks));
   memset(ut_wait_results, 0, sizeof(ut_wait_results));
   memset(ut_expected_remaining, 0, sizeof(ut_expected_remaining));
   memset(ut_messages, 0, sizeof(ut_messages));
   memset(ut_peek_results, 0, sizeof(ut_peek_results));
   memset(ut_accel_results, 0, sizeof(ut_accel_results));
   ut_tick_index = ut_tick_count = ut_wait_index = ut_wait_count = 0;
   ut_peek_index = ut_peek_count = ut_accel_index = ut_accel_count = 0;
   ut_quit_calls = ut_translate_calls = ut_dispatch_calls = 0;
   ut_object = (HANDLE)1; ut_frame = (HWND)2;
}

static void returns_when_an_infinite_wait_object_is_signaled(void)
{
   reset_wait(); ut_ticks[0] = 100; ut_tick_count = 1;
   ut_wait_results[0] = WAIT_OBJECT_0; ut_expected_remaining[0] = INFINITE;
   ut_wait_count = 1;
   UT_ASSERT_EQ_INT(TRUE, utt_ODScrnWaitWithMessages(ut_object, ut_frame,
      INFINITE));
}

static void reports_a_finite_timeout_after_the_deadline(void)
{
   reset_wait(); ut_ticks[0] = 100; ut_ticks[1] = 110; ut_tick_count = 2;
   ut_wait_results[0] = WAIT_TIMEOUT; ut_expected_remaining[0] = 0;
   ut_wait_count = 1;
   UT_ASSERT_EQ_INT(FALSE, utt_ODScrnWaitWithMessages(ut_object, ut_frame, 10));
}

static void waits_only_for_the_remaining_finite_interval(void)
{
   reset_wait(); ut_ticks[0] = 100; ut_ticks[1] = 104; ut_tick_count = 2;
   ut_wait_results[0] = WAIT_OBJECT_0; ut_expected_remaining[0] = 6;
   ut_wait_count = 1;
   UT_ASSERT_EQ_INT(TRUE, utt_ODScrnWaitWithMessages(ut_object, ut_frame, 10));
}

static void resumes_waiting_when_the_message_queue_is_empty(void)
{
   reset_wait(); ut_ticks[0] = 100; ut_tick_count = 1;
   ut_wait_results[0] = WAIT_OBJECT_0 + 1; ut_wait_results[1] = WAIT_OBJECT_0;
   ut_expected_remaining[0] = INFINITE; ut_expected_remaining[1] = INFINITE;
   ut_wait_count = 2; ut_peek_results[0] = FALSE; ut_peek_count = 1;
   UT_ASSERT_EQ_INT(TRUE, utt_ODScrnWaitWithMessages(ut_object, ut_frame,
      INFINITE));
}

static void preserves_a_quit_message_and_stops_waiting(void)
{
   reset_wait(); ut_ticks[0] = 100; ut_tick_count = 1;
   ut_wait_results[0] = WAIT_OBJECT_0 + 1; ut_expected_remaining[0] = INFINITE;
   ut_wait_count = 1; ut_peek_results[0] = TRUE; ut_peek_count = 1;
   ut_messages[0].message = WM_QUIT; ut_messages[0].wParam = 23;
   UT_ASSERT_EQ_INT(FALSE, utt_ODScrnWaitWithMessages(ut_object, ut_frame,
      INFINITE));
   UT_ASSERT_EQ_UINT(1, ut_quit_calls);
}

static void handles_accelerators_and_dispatches_other_messages(void)
{
   reset_wait(); ut_ticks[0] = 100; ut_tick_count = 1;
   ut_wait_results[0] = WAIT_OBJECT_0 + 1; ut_wait_results[1] = WAIT_OBJECT_0;
   ut_expected_remaining[0] = INFINITE; ut_expected_remaining[1] = INFINITE;
   ut_wait_count = 2;
   ut_peek_results[0] = TRUE; ut_peek_results[1] = TRUE;
   ut_peek_results[2] = FALSE; ut_peek_count = 3;
   ut_messages[0].message = WM_USER; ut_messages[1].message = WM_USER + 1;
   ut_accel_results[0] = TRUE; ut_accel_results[1] = FALSE; ut_accel_count = 2;
   UT_ASSERT_EQ_INT(TRUE, utt_ODScrnWaitWithMessages(ut_object, ut_frame,
      INFINITE));
   UT_ASSERT_EQ_UINT(1, ut_translate_calls); UT_ASSERT_EQ_UINT(1, ut_dispatch_calls);
}

static const UTTestCase ut_cases[] = {
   {"infinite signal", returns_when_an_infinite_wait_object_is_signaled},
   {"timeout", reports_a_finite_timeout_after_the_deadline},
   {"finite remaining", waits_only_for_the_remaining_finite_interval},
   {"empty queue", resumes_waiting_when_the_message_queue_is_empty},
   {"quit", preserves_a_quit_message_and_stops_waiting},
   {"dispatch", handles_accelerators_and_dispatches_other_messages}
};
