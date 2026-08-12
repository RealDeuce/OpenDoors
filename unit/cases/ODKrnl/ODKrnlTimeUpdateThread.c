#define UT_CUSTOM_MOCK_ODKrnlWorkerWait
#define UT_CUSTOM_MOCK_ODKrnlRequestTimeUpdate
static BOOL ut_wait_results[4];
static unsigned ut_wait_index, ut_wait_count, ut_update_calls;
BOOL utm_ODKrnlWorkerWait(tODMilliSec milliseconds)
{
   UT_ASSERT_EQ_UINT(TIME_UPDATE_THREAD_SLEEP_TIME, milliseconds);
   UT_ASSERT(ut_wait_index < ut_wait_count);
   return ut_wait_results[ut_wait_index++];
}
void utm_ODKrnlRequestTimeUpdate(void) { ++ut_update_calls; }
static void requests_one_update_after_each_completed_wait(void)
{
   ut_wait_results[0] = TRUE; ut_wait_results[1] = TRUE;
   ut_wait_results[2] = FALSE; ut_wait_index = 0; ut_wait_count = 3;
   ut_update_calls = 0;
   UT_ASSERT_EQ_UINT(0, utt_ODKrnlTimeUpdateThread(NULL));
   UT_ASSERT_EQ_UINT(2, ut_update_calls); UT_ASSERT_EQ_UINT(3, ut_wait_index);
}
static const UTTestCase ut_cases[] = {
   {"updates", requests_one_update_after_each_completed_wait}
};
