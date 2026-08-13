#ifdef ODPLAT_WIN32
#define UT_CUSTOM_MOCK_ODSyncIsOwnerThread
#define UT_CUSTOM_MOCK_ODMutexLock
#define UT_CUSTOM_MOCK_ODMutexUnlock
#define UT_CUSTOM_MOCK_ODKrnlForceOpenDoorsShutdown
#define UT_CUSTOM_MOCK_ODKrnlChatMode
#define UT_CUSTOM_MOCK_ODKrnlEndChatMode
#define UT_CUSTOM_MOCK_ODKrnlRefreshUIState
#define UT_CUSTOM_MOCK_free

static BOOL ut_owner;
static BOOL ut_refresh;
static unsigned ut_refresh_failures;
static unsigned ut_force_calls;
static unsigned ut_chat_start_calls;
static unsigned ut_chat_end_calls;
static unsigned ut_free_calls;
static BYTE ut_reason;
static tODUIChange ut_changes[8];

BOOL utm_ODSyncIsOwnerThread(void) { return(ut_owner); }
void utm_ODMutexLock(tODMutex *mutex) { UT_ASSERT_EQ_PTR(&KernelStateLock, mutex); }
void utm_ODMutexUnlock(tODMutex *mutex) { UT_ASSERT_EQ_PTR(&KernelStateLock, mutex); }
void utm_ODKrnlForceOpenDoorsShutdown(BYTE reason) { ++ut_force_calls; ut_reason = reason; }
void utm_ODKrnlChatMode(void) { ++ut_chat_start_calls; od_control.od_chat_active = TRUE; }
void utm_ODKrnlEndChatMode(void) { ++ut_chat_end_calls; od_control.od_chat_active = FALSE; }
BOOL utm_ODKrnlRefreshUIState(void)
{ if(ut_refresh_failures != 0) { --ut_refresh_failures; return(FALSE); } return(ut_refresh); }
void utm_free(void *memory) { (void)memory; ++ut_free_calls; }

static void reset_pending(void)
{
   memset(&od_control, 0, sizeof(od_control));
   memset(ut_changes, 0, sizeof(ut_changes));
   bKernelStateLockInitialized = TRUE;
   pPendingUIHead = pPendingUITail = NULL;
   ut_owner = TRUE;
   ut_refresh = TRUE;
   ut_refresh_failures = 0;
   ut_force_calls = ut_chat_start_calls = ut_chat_end_calls = ut_free_calls = 0;
   ut_reason = 0;
   bODInitialized = TRUE;
   od_control.user_timelimit = 10;
   od_control.user_security = 50;
}

static void append_change(unsigned index, tODUIChangeType type, BOOL value,
   INT number, BYTE reason)
{
   tODUIChange *change = &ut_changes[index];
   change->Type = type; change->bValue = value; change->nValue = number;
   change->btReason = reason; change->pNext = NULL;
   if(pPendingUITail == NULL) pPendingUIHead = change;
   else pPendingUITail->pNext = change;
   pPendingUITail = change;
}

static void rejects_uninitialized_or_non_owner_dispatch(void)
{
   reset_pending(); bKernelStateLockInitialized = FALSE;
   utt_ODKrnlDispatchPending(FALSE); UT_ASSERT_EQ_UINT(0, ut_free_calls);
   reset_pending(); ut_owner = FALSE;
   utt_ODKrnlDispatchPending(TRUE); UT_ASSERT_EQ_UINT(0, ut_free_calls);
   reset_pending(); utt_ODKrnlDispatchPending(FALSE);
   UT_ASSERT_EQ_UINT(0, ut_free_calls);
   reset_pending(); ut_refresh_failures = 1;
   utt_ODKrnlDispatchPending(FALSE); UT_ASSERT_EQ_UINT(0, ut_free_calls);
}

static void applies_fifo_changes_in_order(void)
{
   reset_pending();
   append_change(0, kODUIChangeKeyboard, TRUE, 0, 0);
   append_change(1, kODUIChangeSysopNext, TRUE, 0, 0);
   append_change(2, kODUIChangeInactivity, TRUE, 0, 0);
   append_change(3, kODUIChangeTime, FALSE, -20, 0);
   append_change(4, kODUIChangeChat, TRUE, 0, 0);
   utt_ODKrnlDispatchPending(FALSE);
   UT_ASSERT(od_control.od_user_keyboard_on);
   UT_ASSERT(od_control.sysop_next);
   UT_ASSERT(od_control.od_disable_inactivity);
   UT_ASSERT_EQ_INT(0, od_control.user_timelimit);
   UT_ASSERT_EQ_UINT(1, ut_chat_start_calls);
   UT_ASSERT_EQ_UINT(5, ut_free_calls);

   reset_pending(); od_control.od_chat_active = TRUE;
   append_change(0, kODUIChangeChat, FALSE, 0, 0);
   append_change(1, kODUIChangeTime, FALSE, 2000, 0);
   utt_ODKrnlDispatchPending(TRUE);
   UT_ASSERT_EQ_UINT(1, ut_chat_end_calls);
   UT_ASSERT_EQ_INT(OD_MAX_USER_TIME_MINUTES, od_control.user_timelimit);

   reset_pending(); od_control.od_chat_active = TRUE;
   append_change(0, kODUIChangeChat, TRUE, 0, 0);
   append_change(1, kODUIChangeChat, FALSE, 0, 0);
   append_change(2, kODUIChangeChat, FALSE, 0, 0);
   append_change(3, (tODUIChangeType)99, FALSE, 0, 0);
   utt_ODKrnlDispatchPending(TRUE);
   UT_ASSERT_EQ_UINT(1, ut_chat_end_calls);
}

static void applies_lockout_and_shutdown(void)
{
   reset_pending();
   append_change(0, kODUIChangeLockout, FALSE, 0, 7);
   append_change(1, kODUIChangeShutdown, FALSE, 0, 8);
   utt_ODKrnlDispatchPending(TRUE);
   UT_ASSERT_EQ_INT(0, od_control.user_security);
   UT_ASSERT_EQ_UINT(2, ut_force_calls);
   UT_ASSERT_EQ_UINT(8, ut_reason);

   reset_pending(); bODInitialized = FALSE;
   append_change(0, kODUIChangeKeyboard, TRUE, 0, 0);
   append_change(1, kODUIChangeSysopNext, TRUE, 0, 0);
   utt_ODKrnlDispatchPending(TRUE);
   UT_ASSERT_EQ_UINT(2, ut_free_calls);

   reset_pending(); bODInitialized = FALSE;
   append_change(0, kODUIChangeKeyboard, TRUE, 0, 0);
   utt_ODKrnlDispatchPending(TRUE);
   UT_ASSERT_EQ_UINT(1, ut_free_calls);
}

static const UTTestCase ut_cases[] = {
   {"ownership", rejects_uninitialized_or_non_owner_dispatch},
   {"FIFO", applies_fifo_changes_in_order},
   {"shutdown", applies_lockout_and_shutdown}
};
#else
static void has_no_pending_ui_work(void)
{ utt_ODKrnlDispatchPending(FALSE); utt_ODKrnlDispatchPending(TRUE); UT_ASSERT(TRUE); }
static const UTTestCase ut_cases[] = {{"no UI queue", has_no_pending_ui_work}};
#endif
