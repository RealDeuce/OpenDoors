#ifdef ODPLAT_WIN32
#define UT_CUSTOM_MOCK_PostMessageA
#define UT_CUSTOM_MOCK_ODFramePostThreadQuit
static unsigned ut_post_calls;
static unsigned ut_quit_calls;
BOOL WINAPI utm_PostMessageA(HWND frame, UINT message, WPARAM first,
   LPARAM second)
{
   UT_ASSERT_EQ_PTR(hwndCurrentFrame, frame);
   UT_ASSERT_EQ_UINT(WM_OD_SHUTDOWN, message);
   UT_ASSERT_EQ_UINT(0, first);
   UT_ASSERT_EQ_INT(0, second);
   ++ut_post_calls;
   return(TRUE);
}
static void utm_ODFramePostThreadQuit(tODThreadHandle thread, DWORD id)
{
   UT_ASSERT_EQ_PTR((HANDLE)(UINT_PTR)1, thread);
   UT_ASSERT_EQ_UINT(dwFrameThreadID, id);
   ++ut_quit_calls;
}
static void reset_request(void)
{
   hwndCurrentFrame = NULL;
   dwFrameThreadID = 7;
   ut_post_calls = 0;
   ut_quit_calls = 0;
}
static void ignores_an_absent_thread(void)
{
   reset_request();
   utt_ODFrameRequestShutdown(NULL);
   UT_ASSERT_EQ_UINT(0, ut_post_calls);
   UT_ASSERT_EQ_UINT(0, ut_quit_calls);
}
static void posts_to_an_existing_window(void)
{
   reset_request();
   hwndCurrentFrame = (HWND)(UINT_PTR)2;
   utt_ODFrameRequestShutdown((HANDLE)(UINT_PTR)1);
   UT_ASSERT_EQ_UINT(1, ut_post_calls);
   UT_ASSERT_EQ_UINT(0, ut_quit_calls);
}
static void quits_a_windowless_thread(void)
{
   reset_request();
   utt_ODFrameRequestShutdown((HANDLE)(UINT_PTR)1);
   UT_ASSERT_EQ_UINT(0, ut_post_calls);
   UT_ASSERT_EQ_UINT(1, ut_quit_calls);
}
static const UTTestCase ut_cases[] = {
   {"absent thread", ignores_an_absent_thread},
   {"window shutdown", posts_to_an_existing_window},
   {"thread shutdown", quits_a_windowless_thread}
};
#endif
