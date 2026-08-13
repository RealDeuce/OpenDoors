#ifdef OD_THREAD_SUPPORT
#define UT_CUSTOM_MOCK_GetCurrentThreadId
static DWORD ut_thread_id;

DWORD WINAPI utm_GetCurrentThreadId(void)
{
   return ut_thread_id;
}
#endif

static void identifies_only_the_initialized_owner(void)
{
   bSyncActive = FALSE;
   UT_ASSERT_EQ_INT(TRUE, utt_ODSyncIsOwnerThread());
   bSyncActive = TRUE;
#ifdef OD_THREAD_SUPPORT
   dwOwnerThreadID = 42;
   ut_thread_id = 42;
   UT_ASSERT_EQ_INT(TRUE, utt_ODSyncIsOwnerThread());
   ut_thread_id = 43;
   UT_ASSERT_EQ_INT(FALSE, utt_ODSyncIsOwnerThread());
#else
   UT_ASSERT_EQ_INT(TRUE, utt_ODSyncIsOwnerThread());
#endif
}

static const UTTestCase ut_cases[] = {
   {"owner identity", identifies_only_the_initialized_owner}
};
