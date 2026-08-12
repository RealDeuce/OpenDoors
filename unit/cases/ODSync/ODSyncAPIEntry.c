#define UT_CUSTOM_MOCK_ODReleasePublicLock
#define UT_CUSTOM_MOCK_ODSyncControlWriteLock
#define UT_CUSTOM_MOCK_ODSyncIsOwnerThread
static unsigned ut_releases;
static unsigned ut_locks;

BOOL utm_ODSyncIsOwnerThread(void) { return TRUE; }
void utm_ODReleasePublicLock(void) { ++ut_releases; }
void utm_ODSyncControlWriteLock(void) { ++ut_locks; }

static void takes_exclusion_only_at_the_outermost_entry(void)
{
   nAPILevel = 0;
   ut_releases = 0;
   ut_locks = 0;
   utt_ODSyncAPIEntry();
   UT_ASSERT_EQ_UINT(1, nAPILevel);
   UT_ASSERT_EQ_UINT(1, ut_releases);
   UT_ASSERT_EQ_UINT(1, ut_locks);
   utt_ODSyncAPIEntry();
   UT_ASSERT_EQ_UINT(2, nAPILevel);
   UT_ASSERT_EQ_UINT(1, ut_releases);
   UT_ASSERT_EQ_UINT(1, ut_locks);
}

static const UTTestCase ut_cases[] = {
   {"nested entry", takes_exclusion_only_at_the_outermost_entry}
};
