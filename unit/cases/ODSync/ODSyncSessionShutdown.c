#ifdef OD_THREAD_SUPPORT
#define UT_CUSTOM_MOCK_ODMutexDestroy
#ifdef ODPLAT_WIN32
#define UT_CUSTOM_MOCK_CloseHandle
static HANDLE ut_closed;
BOOL WINAPI utm_CloseHandle(HANDLE handle)
{
   ut_closed = handle;
   return TRUE;
}
#else
#define UT_CUSTOM_MOCK_pthread_cond_destroy
static pthread_cond_t *ut_condition;
int utm_pthread_cond_destroy(pthread_cond_t *condition)
{
   ut_condition = condition;
   return 0;
}
#endif
static tODMutex *ut_mutex;
void utm_ODMutexDestroy(tODMutex *mutex) { ut_mutex = mutex; }
#endif

static void ignores_an_inactive_session(void)
{
   bSyncActive = FALSE;
   nAPILevel = 7;
   utt_ODSyncSessionShutdown();
   UT_ASSERT_EQ_UINT(7, nAPILevel);
}

static void destroys_platform_state_and_clears_session_depths(void)
{
   bSyncActive = TRUE;
   nAPILevel = 7;
   nPublicReadDepth = 3;
   nPublicWriteDepth = 2;
   bPublicLockPhysical = TRUE;
#ifdef OD_THREAD_SUPPORT
   ut_mutex = NULL;
#ifdef ODPLAT_WIN32
   ControlLock.changed = (HANDLE)1;
   ut_closed = NULL;
#else
   ut_condition = NULL;
#endif
#endif
   utt_ODSyncSessionShutdown();
   UT_ASSERT_EQ_INT(FALSE, bSyncActive);
   UT_ASSERT_EQ_UINT(0, nAPILevel);
   UT_ASSERT_EQ_UINT(0, nPublicReadDepth);
   UT_ASSERT_EQ_UINT(0, nPublicWriteDepth);
   UT_ASSERT_EQ_INT(FALSE, bPublicLockPhysical);
#ifdef OD_THREAD_SUPPORT
   UT_ASSERT_EQ_PTR(&ControlLock.state, ut_mutex);
#ifdef ODPLAT_WIN32
   UT_ASSERT_EQ_PTR(ControlLock.changed, ut_closed);
#else
   UT_ASSERT_EQ_PTR(&ControlLock.changed, ut_condition);
#endif
#endif
}

static const UTTestCase ut_cases[] = {
   {"inactive shutdown", ignores_an_inactive_session},
   {"active shutdown", destroys_platform_state_and_clears_session_depths}
};
