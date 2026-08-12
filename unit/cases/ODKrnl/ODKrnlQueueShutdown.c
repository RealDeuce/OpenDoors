#define UT_CUSTOM_MOCK_ODMutexLock
#define UT_CUSTOM_MOCK_ODMutexUnlock
void utm_ODMutexLock(tODMutex *mutex) { UT_ASSERT_EQ_PTR(&KernelStateLock, mutex); }
void utm_ODMutexUnlock(tODMutex *mutex) { UT_ASSERT_EQ_PTR(&KernelStateLock, mutex); }
static void records_only_the_first_pending_shutdown_reason(void)
{
   btPendingShutdown = 0; utt_ODKrnlQueueShutdown(7);
   UT_ASSERT_EQ_UINT(7, btPendingShutdown);
   utt_ODKrnlQueueShutdown(9); UT_ASSERT_EQ_UINT(7, btPendingShutdown);
}
static const UTTestCase ut_cases[] = {
   {"first reason", records_only_the_first_pending_shutdown_reason}
};
