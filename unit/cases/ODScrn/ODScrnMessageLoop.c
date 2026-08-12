#define UT_CUSTOM_MOCK_GetParent
#define UT_CUSTOM_MOCK_GetMessageA
#define UT_CUSTOM_MOCK_ODFrameTranslateAccelerator
#define UT_CUSTOM_MOCK_TranslateMessage
#define UT_CUSTOM_MOCK_PostMessageA
#define UT_CUSTOM_MOCK_DispatchMessageA

static HWND ut_screen = (HWND)1;
static HWND ut_frame = (HWND)2;
static HANDLE ut_instance = (HANDLE)3;
static MSG ut_messages[3];
static BOOL ut_get_results[4];
static BOOL ut_accel_results[3];
static unsigned ut_get_index;
static unsigned ut_get_count;
static unsigned ut_accel_index;
static unsigned ut_accel_count;
static unsigned ut_translate_calls;
static unsigned ut_post_calls;
static unsigned ut_dispatch_calls;

HWND WINAPI utm_GetParent(HWND window)
{
   UT_ASSERT(window == ut_screen);
   return ut_frame;
}

BOOL WINAPI utm_GetMessageA(LPMSG message, HWND window, UINT first,
   UINT last)
{
   BOOL result;
   UT_ASSERT(window == NULL); UT_ASSERT_EQ_UINT(0, first);
   UT_ASSERT_EQ_UINT(0, last); UT_ASSERT(ut_get_index < ut_get_count);
   result = ut_get_results[ut_get_index];
   if(result) *message = ut_messages[ut_get_index];
   ++ut_get_index;
   return result;
}

BOOL utm_ODFrameTranslateAccelerator(HWND frame, MSG *message)
{
   UT_ASSERT(frame == ut_frame); UT_ASSERT_NOT_NULL(message);
   UT_ASSERT(ut_accel_index < ut_accel_count);
   return ut_accel_results[ut_accel_index++];
}

BOOL WINAPI utm_TranslateMessage(const MSG *message)
{
   ++ut_translate_calls; UT_ASSERT_NOT_NULL(message); return TRUE;
}

BOOL WINAPI utm_PostMessageA(HWND window, UINT message, WPARAM wparam,
   LPARAM lparam)
{
   ++ut_post_calls; UT_ASSERT(window == ut_screen);
   UT_ASSERT_EQ_UINT(WM_KEYDOWN_RELAY, message);
   UT_ASSERT_EQ_UINT(23, wparam); UT_ASSERT_EQ_INT(47, lparam);
   return TRUE;
}

LRESULT WINAPI utm_DispatchMessageA(const MSG *message)
{
   ++ut_dispatch_calls; UT_ASSERT_NOT_NULL(message); return 0;
}

static void reset_loop(void)
{
   memset(ut_messages, 0, sizeof(ut_messages));
   memset(ut_get_results, 0, sizeof(ut_get_results));
   memset(ut_accel_results, 0, sizeof(ut_accel_results));
   ut_get_index = ut_get_count = ut_accel_index = ut_accel_count = 0;
   ut_translate_calls = ut_post_calls = ut_dispatch_calls = 0;
}

static void exits_without_dispatching_when_no_message_is_available(void)
{
   reset_loop(); ut_get_results[0] = FALSE; ut_get_count = 1;
   utt_ODScrnMessageLoop(ut_instance, ut_screen);
   UT_ASSERT_EQ_UINT(0, ut_accel_index); UT_ASSERT_EQ_UINT(0, ut_dispatch_calls);
}

static void an_accelerator_consumes_the_message(void)
{
   reset_loop(); ut_get_results[0] = TRUE; ut_get_results[1] = FALSE;
   ut_get_count = 2; ut_messages[0].message = WM_USER;
   ut_accel_results[0] = TRUE; ut_accel_count = 1;
   utt_ODScrnMessageLoop(ut_instance, ut_screen);
   UT_ASSERT_EQ_UINT(0, ut_translate_calls); UT_ASSERT_EQ_UINT(0, ut_post_calls);
   UT_ASSERT_EQ_UINT(0, ut_dispatch_calls);
}

static void translates_and_dispatches_an_ordinary_message(void)
{
   reset_loop(); ut_get_results[0] = TRUE; ut_get_results[1] = FALSE;
   ut_get_count = 2; ut_messages[0].message = WM_USER;
   ut_accel_results[0] = FALSE; ut_accel_count = 1;
   utt_ODScrnMessageLoop(ut_instance, ut_screen);
   UT_ASSERT_EQ_UINT(1, ut_translate_calls); UT_ASSERT_EQ_UINT(0, ut_post_calls);
   UT_ASSERT_EQ_UINT(1, ut_dispatch_calls);
}

static void relays_a_keydown_before_dispatching_it(void)
{
   reset_loop(); ut_get_results[0] = TRUE; ut_get_results[1] = FALSE;
   ut_get_count = 2; ut_messages[0].message = WM_KEYDOWN;
   ut_messages[0].wParam = 23; ut_messages[0].lParam = 47;
   ut_accel_results[0] = FALSE; ut_accel_count = 1;
   utt_ODScrnMessageLoop(ut_instance, ut_screen);
   UT_ASSERT_EQ_UINT(1, ut_translate_calls); UT_ASSERT_EQ_UINT(1, ut_post_calls);
   UT_ASSERT_EQ_UINT(1, ut_dispatch_calls);
}

static const UTTestCase ut_cases[] = {
   {"empty", exits_without_dispatching_when_no_message_is_available},
   {"accelerator", an_accelerator_consumes_the_message},
   {"ordinary", translates_and_dispatches_an_ordinary_message},
   {"keydown", relays_a_keydown_before_dispatching_it}
};
