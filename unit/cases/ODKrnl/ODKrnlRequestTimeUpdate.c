#ifdef OD_MULTITHREADED
#define UT_CUSTOM_MOCK_ODMutexLock
#define UT_CUSTOM_MOCK_ODMutexUnlock
static unsigned ut_locks, ut_unlocks;
void utm_ODMutexLock(tODMutex *mutex) { ++ut_locks; UT_ASSERT_EQ_PTR(&KernelStateLock, mutex); }
void utm_ODMutexUnlock(tODMutex *mutex) { ++ut_unlocks; UT_ASSERT_EQ_PTR(&KernelStateLock, mutex); }
#else
#define UT_CUSTOM_MOCK_ODKrnlTimeUpdate
static unsigned ut_updates;
BOOL utm_ODKrnlTimeUpdate(BOOL callbacks) { ++ut_updates; UT_ASSERT(callbacks); return FALSE; }
#endif
static void requests_a_time_update_in_the_platform_appropriate_context(void)
{
#ifdef OD_MULTITHREADED
   ut_locks = ut_unlocks = 0; bTimerUpdatePending = FALSE;
   utt_ODKrnlRequestTimeUpdate(); UT_ASSERT(bTimerUpdatePending);
   UT_ASSERT_EQ_UINT(1, ut_locks); UT_ASSERT_EQ_UINT(1, ut_unlocks);
#else
   ut_updates = 0; utt_ODKrnlRequestTimeUpdate(); UT_ASSERT_EQ_UINT(1, ut_updates);
#endif
}
static const UTTestCase ut_cases[] = {{"request", requests_a_time_update_in_the_platform_appropriate_context}};
