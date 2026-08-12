#define UT_CUSTOM_MOCK_ODMutexLock
#define UT_CUSTOM_MOCK_ODMutexUnlock
#define UT_CUSTOM_MOCK_ODSemaphoreDown
static unsigned ut_down_calls;
static tODResult ut_down_result;
void utm_ODMutexLock(tODMutex *mutex) { UT_ASSERT_EQ_PTR(&KernelStateLock, mutex); }
void utm_ODMutexUnlock(tODMutex *mutex) { UT_ASSERT_EQ_PTR(&KernelStateLock, mutex); }
tODResult utm_ODSemaphoreDown(tODSemaphoreHandle semaphore,
   tODMilliSec milliseconds)
{
   ++ut_down_calls; UT_ASSERT(semaphore == hKernelShutdownSemaphore);
   UT_ASSERT_EQ_UINT(37, milliseconds); return ut_down_result;
}
static void stops_without_waiting_when_shutdown_was_requested(void)
{
   bKernelStopRequested = TRUE; ut_down_calls = 0;
   UT_ASSERT_EQ_INT(FALSE, utt_ODKrnlWorkerWait(37));
   UT_ASSERT_EQ_UINT(0, ut_down_calls);
}
static void continues_only_after_a_semaphore_timeout(void)
{
   hKernelShutdownSemaphore = (tODSemaphoreHandle)1;
   bKernelStopRequested = FALSE; ut_down_calls = 0;
   ut_down_result = kODRCTimeout; UT_ASSERT(utt_ODKrnlWorkerWait(37));
   ut_down_result = kODRCSuccess; UT_ASSERT(!utt_ODKrnlWorkerWait(37));
   UT_ASSERT_EQ_UINT(2, ut_down_calls);
}
static const UTTestCase ut_cases[] = {
   {"stop", stops_without_waiting_when_shutdown_was_requested},
   {"semaphore", continues_only_after_a_semaphore_timeout}
};
