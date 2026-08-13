#define UT_CUSTOM_MOCK_ODSyncControlWriteUnlock
#ifdef ODPLAT_WIN32
#define UT_CUSTOM_MOCK_ODScrnPublish
#endif
static unsigned ut_unlocks;
static unsigned ut_publishes;

void utm_ODSyncControlWriteUnlock(void) { ++ut_unlocks; }
#ifdef ODPLAT_WIN32
void utm_ODScrnPublish(void) { ++ut_publishes; }
#endif

static void releases_and_returns_the_complete_nesting_depth(void)
{
   nAPILevel = 3;
   ut_unlocks = 0;
   ut_publishes = 0;
   UT_ASSERT_EQ_UINT(3, utt_ODSyncAPIRelease());
   UT_ASSERT_EQ_UINT(0, nAPILevel);
   UT_ASSERT_EQ_UINT(1, ut_unlocks);
#ifdef ODPLAT_WIN32
   UT_ASSERT_EQ_UINT(1, ut_publishes);
#else
   UT_ASSERT_EQ_UINT(0, ut_publishes);
#endif
}

static const UTTestCase ut_cases[] = {
   {"release depth", releases_and_returns_the_complete_nesting_depth}
};
