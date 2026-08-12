#ifdef OD_THREAD_SUPPORT
#define UT_CUSTOM_MOCK_ODMutexLock
#define UT_CUSTOM_MOCK_ODMutexUnlock
void utm_ODMutexLock(tODMutex *mutex) { UT_ASSERT_EQ_PTR(&KernelStateLock, mutex); }
void utm_ODMutexUnlock(tODMutex *mutex) { UT_ASSERT_EQ_PTR(&KernelStateLock, mutex); }
#endif
static void toggles_inactivity_detection(void)
{
#ifdef OD_THREAD_SUPPORT
   bInactivityTogglePending = FALSE; utt_ODKrnlRequestInactivityToggle();
   UT_ASSERT(bInactivityTogglePending); utt_ODKrnlRequestInactivityToggle();
   UT_ASSERT(!bInactivityTogglePending);
#else
   memset(&od_control, 0, sizeof(od_control)); utt_ODKrnlRequestInactivityToggle();
   UT_ASSERT(od_control.od_disable_inactivity); utt_ODKrnlRequestInactivityToggle();
   UT_ASSERT(!od_control.od_disable_inactivity);
#endif
}
static const UTTestCase ut_cases[] = {{"toggle", toggles_inactivity_detection}};
