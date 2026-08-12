#ifdef OD_MULTITHREADED
#define UT_CUSTOM_MOCK_ODMutexLock
#define UT_CUSTOM_MOCK_ODMutexUnlock
#define UT_CUSTOM_MOCK_ODSemaphoreUp
#define UT_CUSTOM_MOCK_ODKrnlJoinThread
#define UT_CUSTOM_MOCK_ODSemaphoreFree
static unsigned ut_up_calls, ut_join_calls, ut_free_calls;
void utm_ODMutexLock(tODMutex *mutex) { UT_ASSERT_EQ_PTR(&KernelStateLock, mutex); }
void utm_ODMutexUnlock(tODMutex *mutex) { UT_ASSERT_EQ_PTR(&KernelStateLock, mutex); }
void utm_ODSemaphoreUp(tODSemaphoreHandle semaphore, INT count)
{ ++ut_up_calls; UT_ASSERT(semaphore == hKernelShutdownSemaphore); UT_ASSERT_EQ_UINT(3, count); }
void utm_ODKrnlJoinThread(tODThreadHandle *thread, BOOL *started)
{ ++ut_join_calls; UT_ASSERT_NOT_NULL(thread); UT_ASSERT_NOT_NULL(started); }
void utm_ODSemaphoreFree(tODSemaphoreHandle semaphore)
{ ++ut_free_calls; UT_ASSERT(semaphore == (tODSemaphoreHandle)1); }
#endif
#if defined(OD_DIAGNOSTICS) && defined(ODPLAT_WIN32)
#define UT_CUSTOM_MOCK_MessageBoxA
static unsigned ut_message_calls;
int WINAPI utm_MessageBoxA(HWND window, LPCSTR text, LPCSTR title, UINT type)
{
   UT_ASSERT_NULL(window);
   UT_ASSERT_NOT_NULL(text);
   UT_ASSERT(strcmp(title, "OpenDoors Diagnostics") == 0);
   UT_ASSERT_EQ_UINT(MB_OK, type);
   ++ut_message_calls;
   return(IDOK);
}
#endif
static void requests_stop_and_releases_only_an_existing_semaphore(void)
{
#ifdef OD_MULTITHREADED
   bKernelStopRequested = FALSE; hKernelShutdownSemaphore = NULL;
#if defined(OD_DIAGNOSTICS) && defined(ODPLAT_WIN32)
   od_control.od_internal_debug = FALSE;
   ut_message_calls = 0;
#endif
   ut_up_calls = ut_join_calls = ut_free_calls = 0; utt_ODKrnlShutdown();
   UT_ASSERT(bKernelStopRequested); UT_ASSERT_EQ_UINT(0, ut_up_calls);
   UT_ASSERT_EQ_UINT(3, ut_join_calls); UT_ASSERT_EQ_UINT(0, ut_free_calls);
#if defined(OD_DIAGNOSTICS) && defined(ODPLAT_WIN32)
   UT_ASSERT_EQ_UINT(0, ut_message_calls);
#endif
   hKernelShutdownSemaphore = (tODSemaphoreHandle)1;
   ut_up_calls = ut_join_calls = ut_free_calls = 0; utt_ODKrnlShutdown();
   UT_ASSERT_EQ_UINT(1, ut_up_calls); UT_ASSERT_EQ_UINT(3, ut_join_calls);
   UT_ASSERT_EQ_UINT(1, ut_free_calls); UT_ASSERT(hKernelShutdownSemaphore == NULL);
#else
   utt_ODKrnlShutdown();
#endif
}

static void reports_each_diagnostic_shutdown_stage(void)
{
#if defined(OD_DIAGNOSTICS) && defined(ODPLAT_WIN32)
   hKernelShutdownSemaphore = NULL;
   od_control.od_internal_debug = TRUE;
   ut_message_calls = 0;
   utt_ODKrnlShutdown();
   UT_ASSERT_EQ_UINT(4, ut_message_calls);
#endif
}

static const UTTestCase ut_cases[] = {
   {"shutdown", requests_stop_and_releases_only_an_existing_semaphore},
   {"diagnostics", reports_each_diagnostic_shutdown_stage}
};
