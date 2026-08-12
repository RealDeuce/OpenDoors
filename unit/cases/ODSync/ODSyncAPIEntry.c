#define UT_CUSTOM_MOCK_ODReleasePublicLock
#define UT_CUSTOM_MOCK_ODSyncControlWriteLock
#define UT_CUSTOM_MOCK_ODSyncIsOwnerThread
#define UT_CUSTOM_MOCK_ODKrnlDispatchPending
static unsigned ut_releases;
static unsigned ut_locks;
static unsigned ut_dispatches;
static unsigned ut_dispatch_level;
static BOOL ut_dispatch_allow_callbacks;

BOOL utm_ODSyncIsOwnerThread(void) { return TRUE; }
void utm_ODReleasePublicLock(void) { ++ut_releases; }
void utm_ODSyncControlWriteLock(void) { ++ut_locks; }
void utm_ODKrnlDispatchPending(BOOL allow_callbacks)
{
   ++ut_dispatches;
   ut_dispatch_level = nAPILevel;
   ut_dispatch_allow_callbacks = allow_callbacks;
}

static void takes_exclusion_only_at_the_outermost_entry(void)
{
   nAPILevel = 0;
   bSyncActive = TRUE;
   bDispatching = FALSE;
   ut_releases = 0;
   ut_locks = 0;
   ut_dispatches = 0;
   ut_dispatch_level = 0;
   ut_dispatch_allow_callbacks = FALSE;
   utt_ODSyncAPIEntry();
   UT_ASSERT_EQ_UINT(1, nAPILevel);
   UT_ASSERT_EQ_UINT(1, ut_releases);
   UT_ASSERT_EQ_UINT(1, ut_locks);
   UT_ASSERT_EQ_UINT(1, ut_dispatches);
   UT_ASSERT_EQ_UINT(1, ut_dispatch_level);
   UT_ASSERT(ut_dispatch_allow_callbacks);
   utt_ODSyncAPIEntry();
   UT_ASSERT_EQ_UINT(2, nAPILevel);
   UT_ASSERT_EQ_UINT(1, ut_releases);
   UT_ASSERT_EQ_UINT(1, ut_locks);
   UT_ASSERT_EQ_UINT(1, ut_dispatches);
}

static const UTTestCase ut_cases[] = {
   {"nested entry", takes_exclusion_only_at_the_outermost_entry}
};
