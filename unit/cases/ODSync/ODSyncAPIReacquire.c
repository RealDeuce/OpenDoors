#define UT_CUSTOM_MOCK_ODSyncControlWriteLock
static unsigned ut_locks;

void utm_ODSyncControlWriteLock(void) { ++ut_locks; }

static void restores_depth_and_locks_only_an_active_session(void)
{
   bSyncActive = FALSE;
   nAPILevel = 0;
   ut_locks = 0;
   utt_ODSyncAPIReacquire(3);
   UT_ASSERT_EQ_UINT(3, nAPILevel);
   UT_ASSERT_EQ_UINT(0, ut_locks);

   bSyncActive = TRUE;
   utt_ODSyncAPIReacquire(2);
   UT_ASSERT_EQ_UINT(2, nAPILevel);
   UT_ASSERT_EQ_UINT(1, ut_locks);
}

static const UTTestCase ut_cases[] = {
   {"reacquire depth", restores_depth_and_locks_only_an_active_session}
};
