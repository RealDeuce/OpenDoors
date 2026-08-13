#ifdef OD_THREAD_SUPPORT
#define UT_CUSTOM_MOCK_GetCurrentThreadId
static DWORD ut_thread;
static unsigned ut_thread_calls;
DWORD WINAPI utm_GetCurrentThreadId(void)
{
   ++ut_thread_calls;
   return(ut_thread);
}
#endif

BOOL bODInitialized;

static void initializes_and_reuses_session_ownership(void)
{
   bSyncActive = FALSE;
   bODInitialized = FALSE;
   nAPILevel = 7;
#ifdef OD_THREAD_SUPPORT
   ut_thread = 42;
   ut_thread_calls = 0;
#endif
   UT_ASSERT_EQ_INT(kODRCSuccess, utt_ODSyncSessionInitialize());
   UT_ASSERT(bSyncActive);
   UT_ASSERT_EQ_UINT(0, nAPILevel);
#ifdef OD_THREAD_SUPPORT
   UT_ASSERT_EQ_UINT(42, dwOwnerThreadID);
   UT_ASSERT_EQ_UINT(1, ut_thread_calls);
#endif

   nAPILevel = 1;
   bODInitialized = TRUE;
   UT_ASSERT_EQ_INT(kODRCSuccess, utt_ODSyncSessionInitialize());
#ifdef OD_THREAD_SUPPORT
   UT_ASSERT_EQ_UINT(1, ut_thread_calls);

   bODInitialized = FALSE;
   UT_ASSERT_EQ_INT(kODRCSuccess, utt_ODSyncSessionInitialize());
   UT_ASSERT_EQ_UINT(1, ut_thread_calls);

   nAPILevel = 0;
   ut_thread = 77;
   UT_ASSERT_EQ_INT(kODRCSuccess, utt_ODSyncSessionInitialize());
   UT_ASSERT_EQ_UINT(77, dwOwnerThreadID);
   UT_ASSERT_EQ_UINT(2, ut_thread_calls);
#endif
}

static const UTTestCase ut_cases[] = {
   {"session ownership", initializes_and_reuses_session_ownership}
};
