#ifdef OD_THREAD_SUPPORT
#define UT_CUSTOM_MOCK_ODMutexLock
#define UT_CUSTOM_MOCK_ODMutexUnlock
void utm_ODMutexLock(tODMutex *mutex) { UT_ASSERT_EQ_PTR(&KernelStateLock, mutex); }
void utm_ODMutexUnlock(tODMutex *mutex) { UT_ASSERT_EQ_PTR(&KernelStateLock, mutex); }
#else
#define UT_CUSTOM_MOCK_ODKrnlForceOpenDoorsShutdown
static unsigned ut_shutdowns;
void utm_ODKrnlForceOpenDoorsShutdown(BYTE reason)
{ ++ut_shutdowns; UT_ASSERT_EQ_UINT(ERRORLEVEL_HANGUP, reason); }
#endif
static void records_lockout_and_preserves_an_existing_shutdown_reason(void)
{
#ifdef OD_THREAD_SUPPORT
   bLockoutPending = FALSE; btPendingShutdown = 0; utt_ODKrnlRequestLockout();
   UT_ASSERT(bLockoutPending); UT_ASSERT_EQ_UINT(ERRORLEVEL_HANGUP, btPendingShutdown);
   bLockoutPending = FALSE; btPendingShutdown = ERRORLEVEL_NOCARRIER;
   utt_ODKrnlRequestLockout(); UT_ASSERT(bLockoutPending);
   UT_ASSERT_EQ_UINT(ERRORLEVEL_NOCARRIER, btPendingShutdown);
#else
   memset(&od_control, 0, sizeof(od_control)); od_control.user_security = 100;
   ut_shutdowns = 0; utt_ODKrnlRequestLockout();
   UT_ASSERT_EQ_INT(0, od_control.user_security); UT_ASSERT_EQ_UINT(1, ut_shutdowns);
#endif
}
static const UTTestCase ut_cases[] = {{"lockout", records_lockout_and_preserves_an_existing_shutdown_reason}};
