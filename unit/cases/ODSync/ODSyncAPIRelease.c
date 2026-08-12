#define UT_CUSTOM_MOCK_ODSyncControlWriteUnlock
static unsigned ut_unlocks;

void utm_ODSyncControlWriteUnlock(void) { ++ut_unlocks; }

static void releases_and_returns_the_complete_nesting_depth(void)
{
   nAPILevel = 3;
   ut_unlocks = 0;
   UT_ASSERT_EQ_UINT(3, utt_ODSyncAPIRelease());
   UT_ASSERT_EQ_UINT(0, nAPILevel);
   UT_ASSERT_EQ_UINT(1, ut_unlocks);
}

static const UTTestCase ut_cases[] = {
   {"release depth", releases_and_returns_the_complete_nesting_depth}
};
