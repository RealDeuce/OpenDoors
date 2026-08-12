#ifdef OD_THREAD_SUPPORT
#define UT_CUSTOM_MOCK_ODSyncIsOwnerThread
#define UT_CUSTOM_MOCK_ODMutexLock
#define UT_CUSTOM_MOCK_ODMutexUnlock
#define UT_CUSTOM_MOCK_ODKrnlForceOpenDoorsShutdown
#define UT_CUSTOM_MOCK_ODKrnlChatMode
#define UT_CUSTOM_MOCK_ODKrnlEndChatMode
#define UT_CUSTOM_MOCK_ODFrameUpdateCmdUI
#define UT_CUSTOM_MOCK_ODFrameUpdateTimeDisplay

static BOOL ut_owner;
static unsigned ut_owner_calls;
static unsigned ut_lock_calls;
static unsigned ut_unlock_calls;
static unsigned ut_force_calls;
static unsigned ut_chat_start_calls;
static unsigned ut_chat_end_calls;
static unsigned ut_ui_calls;
static unsigned ut_time_display_calls;
static BYTE ut_force_reason;

BOOL utm_ODSyncIsOwnerThread(void)
{
   ++ut_owner_calls;
   return(ut_owner);
}

void utm_ODMutexLock(tODMutex *mutex)
{
   UT_ASSERT_EQ_PTR(&KernelStateLock, mutex);
   ++ut_lock_calls;
}

void utm_ODMutexUnlock(tODMutex *mutex)
{
   UT_ASSERT_EQ_PTR(&KernelStateLock, mutex);
   ++ut_unlock_calls;
}

void utm_ODKrnlForceOpenDoorsShutdown(BYTE reason)
{
   ++ut_force_calls;
   ut_force_reason = reason;
}

void utm_ODKrnlChatMode(void) { ++ut_chat_start_calls; }
void utm_ODKrnlEndChatMode(void) { ++ut_chat_end_calls; }
void utm_ODFrameUpdateCmdUI(void) { ++ut_ui_calls; }
void utm_ODFrameUpdateTimeDisplay(void) { ++ut_time_display_calls; }

static void reset_pending(void)
{
   bKernelStateLockInitialized = TRUE;
   ut_owner = TRUE;
   ut_owner_calls = ut_lock_calls = ut_unlock_calls = 0;
   ut_force_calls = ut_chat_start_calls = ut_chat_end_calls = 0;
   ut_ui_calls = ut_time_display_calls = 0;
   ut_force_reason = 0;
   bODInitialized = TRUE;
   bChatTogglePending = FALSE;
   bKeyboardTogglePending = FALSE;
   bSysopNextTogglePending = FALSE;
   bInactivityTogglePending = FALSE;
   bTimeValuePending = FALSE;
   nPendingTimeValue = 0;
   nPendingTimeAdjustment = 0;
   bLockoutPending = FALSE;
   btPendingShutdown = 0;
   od_control.od_user_keyboard_on = FALSE;
   od_control.sysop_next = FALSE;
   od_control.od_disable_inactivity = FALSE;
   od_control.od_chat_active = FALSE;
   od_control.user_timelimit = 10;
   od_control.user_security = 50;
}

static void rejects_uninitialized_or_non_owner_dispatch(void)
{
   reset_pending();
   bKernelStateLockInitialized = FALSE;
   utt_ODKrnlDispatchPending(FALSE);
   UT_ASSERT_EQ_UINT(0, ut_owner_calls);
   UT_ASSERT_EQ_UINT(0, ut_lock_calls);

   reset_pending();
   ut_owner = FALSE;
   utt_ODKrnlDispatchPending(TRUE);
   UT_ASSERT_EQ_UINT(1, ut_owner_calls);
   UT_ASSERT_EQ_UINT(0, ut_lock_calls);

   reset_pending();
   utt_ODKrnlDispatchPending(FALSE);
   UT_ASSERT_EQ_UINT(1, ut_owner_calls);
   UT_ASSERT_EQ_UINT(1, ut_lock_calls);
   UT_ASSERT_EQ_UINT(1, ut_unlock_calls);
}

static void drains_pending_state_after_a_prior_shutdown(void)
{
   reset_pending();
   bODInitialized = FALSE;
   bKeyboardTogglePending = TRUE;
   utt_ODKrnlDispatchPending(FALSE);
   UT_ASSERT(!od_control.od_user_keyboard_on);
   UT_ASSERT(!bKeyboardTogglePending);
   UT_ASSERT_EQ_UINT(1, ut_lock_calls);
   UT_ASSERT_EQ_UINT(1, ut_unlock_calls);
}

static void applies_each_independent_toggle_and_lockout(void)
{
   reset_pending();
   bKeyboardTogglePending = TRUE;
   utt_ODKrnlDispatchPending(FALSE);
   UT_ASSERT(od_control.od_user_keyboard_on);
   UT_ASSERT_EQ_UINT(1, ut_ui_calls);

   reset_pending();
   bSysopNextTogglePending = TRUE;
   utt_ODKrnlDispatchPending(FALSE);
   UT_ASSERT(od_control.sysop_next);
   UT_ASSERT_EQ_UINT(1, ut_ui_calls);

   reset_pending();
   bInactivityTogglePending = TRUE;
   utt_ODKrnlDispatchPending(FALSE);
   UT_ASSERT(od_control.od_disable_inactivity);
   UT_ASSERT_EQ_UINT(1, ut_ui_calls);

   reset_pending();
   bLockoutPending = TRUE;
   utt_ODKrnlDispatchPending(FALSE);
   UT_ASSERT_EQ_UINT(0, od_control.user_security);
   UT_ASSERT_EQ_UINT(0, ut_ui_calls);
}

static void applies_and_clamps_absolute_and_relative_time(void)
{
   reset_pending();
   bTimeValuePending = TRUE;
   nPendingTimeValue = -1;
   utt_ODKrnlDispatchPending(FALSE);
   UT_ASSERT_EQ_INT(OD_MIN_USER_TIME_MINUTES, od_control.user_timelimit);

   reset_pending();
   bTimeValuePending = TRUE;
   nPendingTimeValue = OD_MAX_USER_TIME_MINUTES + 1;
   utt_ODKrnlDispatchPending(FALSE);
   UT_ASSERT_EQ_INT(OD_MAX_USER_TIME_MINUTES, od_control.user_timelimit);

   reset_pending();
   bTimeValuePending = TRUE;
   nPendingTimeValue = 17;
   utt_ODKrnlDispatchPending(FALSE);
   UT_ASSERT_EQ_INT(17, od_control.user_timelimit);
   UT_ASSERT_EQ_UINT(1, ut_time_display_calls);

   reset_pending();
   nPendingTimeAdjustment = -20;
   utt_ODKrnlDispatchPending(FALSE);
   UT_ASSERT_EQ_INT(OD_MIN_USER_TIME_MINUTES, od_control.user_timelimit);

   reset_pending();
   od_control.user_timelimit = 1400;
   nPendingTimeAdjustment = 100;
   utt_ODKrnlDispatchPending(FALSE);
   UT_ASSERT_EQ_INT(OD_MAX_USER_TIME_MINUTES, od_control.user_timelimit);

   reset_pending();
   nPendingTimeAdjustment = 5;
   utt_ODKrnlDispatchPending(FALSE);
   UT_ASSERT_EQ_INT(15, od_control.user_timelimit);
   UT_ASSERT_EQ_UINT(1, ut_time_display_calls);
}

static void gives_shutdown_precedence_over_chat(void)
{
   reset_pending();
   btPendingShutdown = 7;
   bChatTogglePending = TRUE;
   utt_ODKrnlDispatchPending(TRUE);
   UT_ASSERT_EQ_UINT(1, ut_force_calls);
   UT_ASSERT_EQ_UINT(7, ut_force_reason);
   UT_ASSERT_EQ_UINT(0, ut_chat_start_calls);
}

static void starts_and_ends_pending_chat(void)
{
   reset_pending();
   bChatTogglePending = TRUE;
   utt_ODKrnlDispatchPending(FALSE);
   UT_ASSERT_EQ_UINT(1, ut_chat_start_calls);

   reset_pending();
   bChatTogglePending = TRUE;
   od_control.od_chat_active = TRUE;
   utt_ODKrnlDispatchPending(TRUE);
   UT_ASSERT_EQ_UINT(1, ut_chat_end_calls);
}
#else
static void accepts_the_callback_policy_without_pending_ui_work(void)
{
   utt_ODKrnlDispatchPending(FALSE);
   utt_ODKrnlDispatchPending(TRUE);
   UT_ASSERT(TRUE);
}
#endif

static const UTTestCase ut_cases[] = {
#ifdef OD_THREAD_SUPPORT
   {"ownership", rejects_uninitialized_or_non_owner_dispatch},
   {"prior shutdown", drains_pending_state_after_a_prior_shutdown},
   {"toggles", applies_each_independent_toggle_and_lockout},
   {"time values", applies_and_clamps_absolute_and_relative_time},
   {"shutdown", gives_shutdown_precedence_over_chat},
   {"chat", starts_and_ends_pending_chat}
#else
   {"no pending UI work", accepts_the_callback_policy_without_pending_ui_work}
#endif
};
