#ifdef ODPLAT_WIN32
#define UT_CUSTOM_MOCK_GetCurrentThreadId
#define UT_CUSTOM_MOCK_ODFrameRequestShutdown
#define UT_CUSTOM_MOCK_ODThreadWaitForExit
#define UT_CUSTOM_MOCK_CloseHandle
static DWORD ut_thread;static unsigned ut_request_calls,ut_wait_calls,ut_close_calls;
DWORD WINAPI utm_GetCurrentThreadId(void){return(ut_thread);}
void utm_ODFrameRequestShutdown(tODThreadHandle thread){UT_ASSERT_EQ_PTR((HANDLE)(UINT_PTR)1,thread);++ut_request_calls;}
void utm_ODThreadWaitForExit(tODThreadHandle thread){UT_ASSERT_EQ_PTR((HANDLE)(UINT_PTR)1,thread);++ut_wait_calls;}
BOOL WINAPI utm_CloseHandle(HANDLE handle){UT_ASSERT(handle==(HANDLE)(UINT_PTR)1||handle==(HANDLE)(UINT_PTR)4);++ut_close_calls;return(TRUE);}
static void reset_shutdown(tODThreadHandle *thread){*thread=(HANDLE)(UINT_PTR)1;hwndCurrentFrame=NULL;dwFrameThreadID=7;hFrameStartedEvent=NULL;ut_thread=8;
 ut_request_calls=ut_wait_calls=ut_close_calls=0;}
static void ignores_an_absent_frame_thread(void){tODThreadHandle thread=NULL;reset_shutdown(&thread);thread=NULL;utt_ODFrameShutdown(&thread);UT_ASSERT_EQ_UINT(0,ut_close_calls);}
static void posts_shutdown_and_joins(void)
{tODThreadHandle thread;reset_shutdown(&thread);hwndCurrentFrame=(HWND)(UINT_PTR)2;hFrameStartedEvent=(HANDLE)(UINT_PTR)4;
 utt_ODFrameShutdown(&thread);UT_ASSERT_EQ_UINT(1,ut_request_calls);UT_ASSERT_EQ_UINT(1,ut_wait_calls);UT_ASSERT_EQ_UINT(2,ut_close_calls);UT_ASSERT_NULL(thread);UT_ASSERT_NULL(hFrameStartedEvent);}
static void quits_a_windowless_thread_without_self_joining(void)
{tODThreadHandle thread;reset_shutdown(&thread);ut_thread=7;utt_ODFrameShutdown(&thread);UT_ASSERT_EQ_UINT(1,ut_request_calls);UT_ASSERT_EQ_UINT(0,ut_wait_calls);UT_ASSERT_EQ_UINT(1,ut_close_calls);}
static const UTTestCase ut_cases[]={{"absent thread",ignores_an_absent_frame_thread},{"joined shutdown",posts_shutdown_and_joins},{"self shutdown",quits_a_windowless_thread_without_self_joining}};
#endif
