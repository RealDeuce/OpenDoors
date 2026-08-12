#ifdef OD_MULTITHREADED
#define UT_CUSTOM_MOCK_ODSyncIsOwnerThread
#define UT_CUSTOM_MOCK_ODKrnlDispatchTimeMessages
#define UT_CUSTOM_MOCK_ODMutexLock
#define UT_CUSTOM_MOCK_ODMutexUnlock
#define UT_CUSTOM_MOCK_ODKrnlDiscardTimeMessages
#define UT_CUSTOM_MOCK_ODKrnlForceOpenDoorsShutdown
#define UT_CUSTOM_MOCK_ODKrnlTimeUpdate
#define UT_CUSTOM_MOCK_ODKrnlChatMode
#define UT_CUSTOM_MOCK_ODKrnlEndChatMode
#ifdef ODPLAT_WIN32
#define UT_CUSTOM_MOCK_ODFrameUpdateCmdUI
#define UT_CUSTOM_MOCK_ODFrameUpdateTimeDisplay
#endif

static BOOL ut_owner;
static BOOL ut_dispatch_result;
static BOOL ut_time_result;
static BOOL ut_time_clears_initialized;
static unsigned ut_owner_calls;
static unsigned ut_dispatch_calls;
static unsigned ut_discard_calls;
static unsigned ut_force_calls;
static unsigned ut_time_calls;
static unsigned ut_chat_start_calls;
static unsigned ut_chat_end_calls;
static BYTE ut_force_reason;
static BOOL ut_time_allows_callbacks;
#ifdef ODPLAT_WIN32
static unsigned ut_ui_calls;
static unsigned ut_time_display_calls;
static BOOL ut_ui_clears_initialized;
#endif

BOOL utm_ODSyncIsOwnerThread(void)
{
   ++ut_owner_calls;
   return(ut_owner);
}
BOOL utm_ODKrnlDispatchTimeMessages(void)
{
   ++ut_dispatch_calls;
   return(ut_dispatch_result);
}
void utm_ODMutexLock(tODMutex *mutex) { UT_ASSERT_EQ_PTR(&KernelStateLock, mutex); }
void utm_ODMutexUnlock(tODMutex *mutex) { UT_ASSERT_EQ_PTR(&KernelStateLock, mutex); }
void utm_ODKrnlDiscardTimeMessages(void) { ++ut_discard_calls; }
void utm_ODKrnlForceOpenDoorsShutdown(BYTE reason)
{
   ++ut_force_calls;
   ut_force_reason = reason;
}
BOOL utm_ODKrnlTimeUpdate(BOOL allow_callbacks)
{
   ++ut_time_calls;
   ut_time_allows_callbacks = allow_callbacks;
   if(ut_time_clears_initialized)
      bODInitialized = FALSE;
   return(ut_time_result);
}
void utm_ODKrnlChatMode(void) { ++ut_chat_start_calls; }
void utm_ODKrnlEndChatMode(void) { ++ut_chat_end_calls; }
#ifdef ODPLAT_WIN32
void utm_ODFrameUpdateCmdUI(void)
{
   ++ut_ui_calls;
   if(ut_ui_clears_initialized)
      bODInitialized = FALSE;
}
void utm_ODFrameUpdateTimeDisplay(void) { ++ut_time_display_calls; }
#endif

static void reset_pending(void)
{
   bKernelStateLockInitialized = TRUE;
   ut_owner = TRUE;
   ut_dispatch_result = FALSE;
   ut_time_result = FALSE;
   ut_time_clears_initialized = FALSE;
   ut_owner_calls = ut_dispatch_calls = ut_discard_calls = 0;
   ut_force_calls = ut_time_calls = 0;
   ut_chat_start_calls = ut_chat_end_calls = 0;
   ut_force_reason = 0;
   ut_time_allows_callbacks = FALSE;
#ifdef ODPLAT_WIN32
   ut_ui_calls = ut_time_display_calls = 0;
   ut_ui_clears_initialized = FALSE;
#endif
   bODInitialized = TRUE;
   bTimerUpdatePending = FALSE;
   bChatTogglePending = FALSE;
   bKeyboardTogglePending = FALSE;
   bSysopNextTogglePending = FALSE;
   bInactivityTogglePending = FALSE;
   bTimeValuePending = FALSE;
   nPendingTimeValue = 0;
   nPendingTimeAdjustment = 0;
   bLockoutPending = FALSE;
   btPendingShutdown = 0;
   bTimeShutdownDeferred = FALSE;
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

   reset_pending();
   ut_owner = FALSE;
   utt_ODKrnlDispatchPending(FALSE);
   UT_ASSERT_EQ_UINT(1, ut_owner_calls);

   reset_pending();
   utt_ODKrnlDispatchPending(FALSE);
   UT_ASSERT_EQ_UINT(1, ut_owner_calls);
   UT_ASSERT_EQ_UINT(0, ut_dispatch_calls);
}

static void dispatches_deferred_messages_only_when_callbacks_are_allowed(void)
{
   reset_pending();
   ut_dispatch_result = TRUE;
   utt_ODKrnlDispatchPending(FALSE);
   UT_ASSERT_EQ_UINT(0, ut_dispatch_calls);

   reset_pending();
   ut_dispatch_result = FALSE;
   utt_ODKrnlDispatchPending(TRUE);
   UT_ASSERT_EQ_UINT(1, ut_dispatch_calls);

   reset_pending();
   ut_dispatch_result = TRUE;
   bKeyboardTogglePending = TRUE;
   utt_ODKrnlDispatchPending(TRUE);
   UT_ASSERT_EQ_UINT(1, ut_dispatch_calls);
   UT_ASSERT(!od_control.od_user_keyboard_on);
   UT_ASSERT(bKeyboardTogglePending);
}

static void discards_messages_after_a_prior_shutdown(void)
{
   reset_pending();
   bODInitialized = FALSE;
   bKeyboardTogglePending = TRUE;
   utt_ODKrnlDispatchPending(FALSE);
   UT_ASSERT_EQ_UINT(1, ut_discard_calls);
   UT_ASSERT(!od_control.od_user_keyboard_on);
   UT_ASSERT(!bKeyboardTogglePending);
}

static void applies_each_independent_toggle_and_lockout(void)
{
   reset_pending();
   bKeyboardTogglePending = TRUE;
   utt_ODKrnlDispatchPending(FALSE);
   UT_ASSERT(od_control.od_user_keyboard_on);
#ifdef ODPLAT_WIN32
   UT_ASSERT_EQ_UINT(1, ut_ui_calls);
#endif

   reset_pending();
   bSysopNextTogglePending = TRUE;
   utt_ODKrnlDispatchPending(FALSE);
   UT_ASSERT(od_control.sysop_next);
#ifdef ODPLAT_WIN32
   UT_ASSERT_EQ_UINT(1, ut_ui_calls);
#endif

   reset_pending();
   bInactivityTogglePending = TRUE;
   utt_ODKrnlDispatchPending(FALSE);
   UT_ASSERT(od_control.od_disable_inactivity);
#ifdef ODPLAT_WIN32
   UT_ASSERT_EQ_UINT(1, ut_ui_calls);
#endif

   reset_pending();
   bLockoutPending = TRUE;
   utt_ODKrnlDispatchPending(FALSE);
   UT_ASSERT_EQ_UINT(0, od_control.user_security);
#ifdef ODPLAT_WIN32
   UT_ASSERT_EQ_UINT(0, ut_ui_calls);
#endif
}

static void applies_and_clamps_absolute_and_relative_time(void)
{
   reset_pending();
   utt_ODKrnlDispatchPending(FALSE);
#ifdef ODPLAT_WIN32
   UT_ASSERT_EQ_UINT(0, ut_time_display_calls);
#endif

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
#ifdef ODPLAT_WIN32
   UT_ASSERT_EQ_UINT(1, ut_time_display_calls);
#endif
}

static void gives_shutdown_precedence_over_timer_and_chat(void)
{
   reset_pending();
   btPendingShutdown = 7;
   bTimerUpdatePending = TRUE;
   bChatTogglePending = TRUE;
   utt_ODKrnlDispatchPending(TRUE);
   UT_ASSERT_EQ_UINT(1, ut_force_calls);
   UT_ASSERT_EQ_UINT(7, ut_force_reason);
   UT_ASSERT_EQ_UINT(0, ut_time_calls);
   UT_ASSERT_EQ_UINT(0, ut_chat_start_calls);
}

static void gates_timer_delivery_and_preserves_the_callback_policy(void)
{
   reset_pending();
   utt_ODKrnlDispatchPending(FALSE);
   UT_ASSERT_EQ_UINT(0, ut_time_calls);

#ifdef ODPLAT_WIN32
   reset_pending();
   bKeyboardTogglePending = TRUE;
   bTimerUpdatePending = TRUE;
   ut_ui_clears_initialized = TRUE;
   utt_ODKrnlDispatchPending(FALSE);
   UT_ASSERT_EQ_UINT(1, ut_ui_calls);
   UT_ASSERT_EQ_UINT(0, ut_time_calls);
#endif

   reset_pending();
   bTimerUpdatePending = TRUE;
   bTimeShutdownDeferred = TRUE;
   utt_ODKrnlDispatchPending(FALSE);
   UT_ASSERT_EQ_UINT(0, ut_time_calls);

   reset_pending();
   bTimerUpdatePending = TRUE;
   utt_ODKrnlDispatchPending(FALSE);
   UT_ASSERT_EQ_UINT(1, ut_time_calls);
   UT_ASSERT(!ut_time_allows_callbacks);

   reset_pending();
   bTimerUpdatePending = TRUE;
   ut_time_result = TRUE;
   bChatTogglePending = TRUE;
   utt_ODKrnlDispatchPending(TRUE);
   UT_ASSERT_EQ_UINT(1, ut_time_calls);
   UT_ASSERT(ut_time_allows_callbacks);
   UT_ASSERT_EQ_UINT(0, ut_chat_start_calls);
}

static void starts_ends_or_suppresses_chat_after_timer_dispatch(void)
{
   reset_pending();
   bChatTogglePending = TRUE;
   utt_ODKrnlDispatchPending(FALSE);
   UT_ASSERT_EQ_UINT(1, ut_chat_start_calls);

   reset_pending();
   bChatTogglePending = TRUE;
   od_control.od_chat_active = TRUE;
   utt_ODKrnlDispatchPending(FALSE);
   UT_ASSERT_EQ_UINT(1, ut_chat_end_calls);

   reset_pending();
   bTimerUpdatePending = TRUE;
   bChatTogglePending = TRUE;
   ut_time_clears_initialized = TRUE;
   utt_ODKrnlDispatchPending(FALSE);
   UT_ASSERT_EQ_UINT(1, ut_time_calls);
   UT_ASSERT_EQ_UINT(0, ut_chat_start_calls);
}
#else
static void accepts_the_callback_policy_in_the_single_threaded_build(void)
{
   utt_ODKrnlDispatchPending(FALSE);
   utt_ODKrnlDispatchPending(TRUE);
}
#endif

static const UTTestCase ut_cases[] = {
#ifdef OD_MULTITHREADED
   {"ownership", rejects_uninitialized_or_non_owner_dispatch},
   {"messages", dispatches_deferred_messages_only_when_callbacks_are_allowed},
   {"prior shutdown", discards_messages_after_a_prior_shutdown},
   {"toggles", applies_each_independent_toggle_and_lockout},
   {"time values", applies_and_clamps_absolute_and_relative_time},
   {"shutdown", gives_shutdown_precedence_over_timer_and_chat},
   {"timer", gates_timer_delivery_and_preserves_the_callback_policy},
   {"chat", starts_ends_or_suppresses_chat_after_timer_dispatch}
#else
   {"single threaded", accepts_the_callback_policy_in_the_single_threaded_build}
#endif
};
