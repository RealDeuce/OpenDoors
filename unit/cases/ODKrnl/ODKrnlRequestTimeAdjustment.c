#ifdef OD_MULTITHREADED
#define UT_CUSTOM_MOCK_ODMutexLock
#define UT_CUSTOM_MOCK_ODMutexUnlock
void utm_ODMutexLock(tODMutex *mutex) { UT_ASSERT_EQ_PTR(&KernelStateLock, mutex); }
void utm_ODMutexUnlock(tODMutex *mutex) { UT_ASSERT_EQ_PTR(&KernelStateLock, mutex); }
#endif
static void accumulates_and_clamps_the_appropriate_time_request(void)
{
#ifdef OD_MULTITHREADED
   bTimeValuePending = FALSE; nPendingTimeAdjustment = 10;
   utt_ODKrnlRequestTimeAdjustment(20); UT_ASSERT_EQ_INT(30, nPendingTimeAdjustment);
   utt_ODKrnlRequestTimeAdjustment(2000);
   UT_ASSERT_EQ_INT(OD_MAX_USER_TIME_MINUTES, nPendingTimeAdjustment);
   nPendingTimeAdjustment = -10; utt_ODKrnlRequestTimeAdjustment(-2000);
   UT_ASSERT_EQ_INT(-OD_MAX_USER_TIME_MINUTES, nPendingTimeAdjustment);
   bTimeValuePending = TRUE; nPendingTimeValue = 10;
   utt_ODKrnlRequestTimeAdjustment(20); UT_ASSERT_EQ_INT(30, nPendingTimeValue);
   utt_ODKrnlRequestTimeAdjustment(2000);
   UT_ASSERT_EQ_INT(OD_MAX_USER_TIME_MINUTES, nPendingTimeValue);
   utt_ODKrnlRequestTimeAdjustment(-2000);
   UT_ASSERT_EQ_INT(OD_MIN_USER_TIME_MINUTES, nPendingTimeValue);
#else
   memset(&od_control, 0, sizeof(od_control)); od_control.user_timelimit = 10;
   utt_ODKrnlRequestTimeAdjustment(-20);
   UT_ASSERT_EQ_INT(-10, od_control.user_timelimit);
#endif
}
static const UTTestCase ut_cases[] = {{"adjust", accumulates_and_clamps_the_appropriate_time_request}};
