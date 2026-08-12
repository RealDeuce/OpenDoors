#if defined(OD_THREAD_SUPPORT) && defined(ODPLAT_WIN32)
#define UT_CUSTOM_MOCK_GetCurrentThreadId
static DWORD ut_thread_id;

DWORD WINAPI utm_GetCurrentThreadId(void)
{
   return ut_thread_id;
}
#elif defined(OD_THREAD_SUPPORT)
#define UT_CUSTOM_MOCK_pthread_equal
#define UT_CUSTOM_MOCK_pthread_self
static pthread_t ut_thread;
static int ut_equal;

pthread_t utm_pthread_self(void)
{
   return ut_thread;
}

int utm_pthread_equal(pthread_t left, pthread_t right)
{
   UT_ASSERT(left == ControlLock.owner);
   UT_ASSERT(right == ut_thread);
   return ut_equal;
}
#endif

static void identifies_only_the_initialized_owner(void)
{
   bSyncActive = FALSE;
   UT_ASSERT_EQ_INT(TRUE, utt_ODSyncIsOwnerThread());
   bSyncActive = TRUE;
#if defined(OD_THREAD_SUPPORT) && defined(ODPLAT_WIN32)
   ControlLock.owner = 42;
   ut_thread_id = 42;
   UT_ASSERT_EQ_INT(TRUE, utt_ODSyncIsOwnerThread());
   ut_thread_id = 43;
   UT_ASSERT_EQ_INT(FALSE, utt_ODSyncIsOwnerThread());
#elif defined(OD_THREAD_SUPPORT)
   ControlLock.owner = (pthread_t)1;
   ut_thread = (pthread_t)2;
   ut_equal = 1;
   UT_ASSERT_EQ_INT(TRUE, utt_ODSyncIsOwnerThread());
   ut_equal = 0;
   UT_ASSERT_EQ_INT(FALSE, utt_ODSyncIsOwnerThread());
#else
   UT_ASSERT_EQ_INT(TRUE, utt_ODSyncIsOwnerThread());
#endif
}

static const UTTestCase ut_cases[] = {
   {"owner identity", identifies_only_the_initialized_owner}
};
