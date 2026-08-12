#define UT_CUSTOM_MOCK_ODAcquirePublicLock
#define UT_CUSTOM_MOCK_ODDispatch
#define UT_CUSTOM_MOCK_ODSyncControlWriteUnlock
static unsigned ut_acquires;
static unsigned ut_dispatches;
static unsigned ut_unlocks;
static BOOL ut_allow_callbacks;

void utm_ODAcquirePublicLock(void) { ++ut_acquires; }
void utm_ODDispatch(BOOL allow_callbacks)
{
   ++ut_dispatches;
   ut_allow_callbacks = allow_callbacks;
}
void utm_ODSyncControlWriteUnlock(void) { ++ut_unlocks; }

static void returns_defensively_when_no_api_level_is_active(void)
{
   nAPILevel = 0;
   ut_acquires = 0;
   ut_dispatches = 0;
   ut_unlocks = 0;
   utt_ODSyncAPIExit();
   UT_ASSERT_EQ_UINT(0, ut_unlocks);
}

static void defers_release_until_the_outermost_exit(void)
{
   nAPILevel = 2;
   ut_acquires = 0;
   ut_dispatches = 0;
   ut_unlocks = 0;
   utt_ODSyncAPIExit();
   UT_ASSERT_EQ_UINT(1, nAPILevel);
   UT_ASSERT_EQ_UINT(0, ut_unlocks);

   utt_ODSyncAPIExit();
   UT_ASSERT_EQ_UINT(0, nAPILevel);
   UT_ASSERT_EQ_UINT(1, ut_unlocks);
   UT_ASSERT_EQ_UINT(1, ut_dispatches);
   UT_ASSERT_EQ_INT(TRUE, ut_allow_callbacks);
   UT_ASSERT_EQ_UINT(1, ut_acquires);
}

static const UTTestCase ut_cases[] = {
   {"inactive exit", returns_defensively_when_no_api_level_is_active},
   {"nested exit", defers_release_until_the_outermost_exit}
};
