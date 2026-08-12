#ifdef OD_THREAD_SUPPORT
#define UT_CUSTOM_MOCK_ODMutexLock
#define UT_CUSTOM_MOCK_ODMutexUnlock
static unsigned ut_locks, ut_unlocks;
void utm_ODMutexLock(tODMutex *mutex) { ++ut_locks; UT_ASSERT_EQ_PTR(&KernelStateLock, mutex); }
void utm_ODMutexUnlock(tODMutex *mutex) { ++ut_unlocks; UT_ASSERT_EQ_PTR(&KernelStateLock, mutex); }
#endif
static void toggles_the_pending_chat_request(void)
{
#ifdef OD_THREAD_SUPPORT
   ut_locks = ut_unlocks = 0; bChatTogglePending = FALSE;
   utt_ODKrnlRequestChatToggle(); UT_ASSERT(bChatTogglePending);
   utt_ODKrnlRequestChatToggle(); UT_ASSERT(!bChatTogglePending);
   UT_ASSERT_EQ_UINT(2, ut_locks); UT_ASSERT_EQ_UINT(2, ut_unlocks);
#else
   nKrnlFuncPending = 0; utt_ODKrnlRequestChatToggle();
   UT_ASSERT((nKrnlFuncPending & KERNEL_FUNC_CHATTOGGLE) != 0);
   utt_ODKrnlRequestChatToggle(); UT_ASSERT_EQ_INT(0, nKrnlFuncPending);
#endif
}
static const UTTestCase ut_cases[] = {{"toggle", toggles_the_pending_chat_request}};
