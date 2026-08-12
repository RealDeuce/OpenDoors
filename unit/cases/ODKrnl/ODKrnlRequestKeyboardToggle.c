#ifdef OD_MULTITHREADED
#define UT_CUSTOM_MOCK_ODMutexLock
#define UT_CUSTOM_MOCK_ODMutexUnlock
void utm_ODMutexLock(tODMutex *mutex) { UT_ASSERT_EQ_PTR(&KernelStateLock, mutex); }
void utm_ODMutexUnlock(tODMutex *mutex) { UT_ASSERT_EQ_PTR(&KernelStateLock, mutex); }
#endif
static void toggles_keyboard_access(void)
{
#ifdef OD_MULTITHREADED
   bKeyboardTogglePending = FALSE; utt_ODKrnlRequestKeyboardToggle();
   UT_ASSERT(bKeyboardTogglePending); utt_ODKrnlRequestKeyboardToggle();
   UT_ASSERT(!bKeyboardTogglePending);
#else
   memset(&od_control, 0, sizeof(od_control)); utt_ODKrnlRequestKeyboardToggle();
   UT_ASSERT(od_control.od_user_keyboard_on); utt_ODKrnlRequestKeyboardToggle();
   UT_ASSERT(!od_control.od_user_keyboard_on);
#endif
}
static const UTTestCase ut_cases[] = {{"toggle", toggles_keyboard_access}};
