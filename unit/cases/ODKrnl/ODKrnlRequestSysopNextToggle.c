#ifdef OD_MULTITHREADED
#define UT_CUSTOM_MOCK_ODMutexLock
#define UT_CUSTOM_MOCK_ODMutexUnlock
void utm_ODMutexLock(tODMutex *mutex) { UT_ASSERT_EQ_PTR(&KernelStateLock, mutex); }
void utm_ODMutexUnlock(tODMutex *mutex) { UT_ASSERT_EQ_PTR(&KernelStateLock, mutex); }
#endif
static void toggles_sysop_next(void)
{
#ifdef OD_MULTITHREADED
   bSysopNextTogglePending = FALSE; utt_ODKrnlRequestSysopNextToggle();
   UT_ASSERT(bSysopNextTogglePending); utt_ODKrnlRequestSysopNextToggle();
   UT_ASSERT(!bSysopNextTogglePending);
#else
   memset(&od_control, 0, sizeof(od_control)); utt_ODKrnlRequestSysopNextToggle();
   UT_ASSERT(od_control.sysop_next); utt_ODKrnlRequestSysopNextToggle();
   UT_ASSERT(!od_control.sysop_next);
#endif
}
static const UTTestCase ut_cases[] = {{"toggle", toggles_sysop_next}};
