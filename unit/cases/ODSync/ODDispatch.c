#define UT_CUSTOM_MOCK_ODKrnlDispatchPending
#define UT_CUSTOM_MOCK_ODSyncControlWriteLock
#define UT_CUSTOM_MOCK_ODSyncControlWriteUnlock
#define UT_CUSTOM_MOCK_ODSyncIsOwnerThread
#ifdef ODPLAT_WIN32
#define UT_CUSTOM_MOCK_ODScrnPublish
#endif
static BOOL ut_owner;
static unsigned ut_dispatches;
static unsigned ut_locks;
static unsigned ut_unlocks;
static BOOL ut_allow_callbacks;
#ifdef ODPLAT_WIN32
static unsigned ut_publishes;
void utm_ODScrnPublish(void) { ++ut_publishes; }
#endif

BOOL utm_ODSyncIsOwnerThread(void) { return ut_owner; }
void utm_ODSyncControlWriteLock(void) { ++ut_locks; }
void utm_ODSyncControlWriteUnlock(void) { ++ut_unlocks; }
void utm_ODKrnlDispatchPending(BOOL allow_callbacks)
{
   ++ut_dispatches;
   ut_allow_callbacks = allow_callbacks;
   UT_ASSERT_EQ_INT(TRUE, bDispatching);
   UT_ASSERT_EQ_UINT(1, nAPILevel);
}

static void reset_dispatch(void)
{
   bDispatching = FALSE;
   bSyncActive = TRUE;
   ut_owner = TRUE;
   nAPILevel = 0;
   ut_dispatches = 0;
   ut_locks = 0;
   ut_unlocks = 0;
#ifdef ODPLAT_WIN32
   ut_publishes = 0;
#endif
}

static void dispatches_only_for_an_active_idle_owner(void)
{
   reset_dispatch();
   bDispatching = TRUE;
   utt_ODDispatch(TRUE);
   UT_ASSERT_EQ_UINT(0, ut_dispatches);

   reset_dispatch();
   bSyncActive = FALSE;
   utt_ODDispatch(TRUE);
   UT_ASSERT_EQ_UINT(0, ut_dispatches);

   reset_dispatch();
   ut_owner = FALSE;
   utt_ODDispatch(TRUE);
   UT_ASSERT_EQ_UINT(0, ut_dispatches);

   reset_dispatch();
   utt_ODDispatch(FALSE);
   UT_ASSERT_EQ_UINT(1, ut_dispatches);
   UT_ASSERT_EQ_INT(FALSE, ut_allow_callbacks);
   UT_ASSERT_EQ_UINT(1, ut_locks);
   UT_ASSERT_EQ_UINT(1, ut_unlocks);
   UT_ASSERT_EQ_UINT(0, nAPILevel);
   UT_ASSERT_EQ_INT(FALSE, bDispatching);
#ifdef ODPLAT_WIN32
   UT_ASSERT_EQ_UINT(1, ut_publishes);
#endif
}

static const UTTestCase ut_cases[] = {
   {"dispatch ownership", dispatches_only_for_an_active_idle_owner}
};
