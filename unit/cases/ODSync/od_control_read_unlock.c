#define UT_CUSTOM_MOCK_ODSyncControlReadUnlock
static unsigned ut_unlocks;
void utm_ODSyncControlReadUnlock(void) { ++ut_unlocks; }

static void reset_read_unlock(unsigned depth)
{
   nPublicReadDepth = depth;
   nPublicWriteDepth = 0;
   nAPILevel = 0;
   bPublicLockPhysical = TRUE;
   ut_unlocks = 0;
}

static void returns_defensively_without_a_public_read(void)
{
   reset_read_unlock(0);
   utt_od_control_read_unlock();
   UT_ASSERT_EQ_UINT(0, ut_unlocks);
}

static void releases_only_the_final_uncovered_physical_read(void)
{
   reset_read_unlock(2);
   utt_od_control_read_unlock();
   UT_ASSERT_EQ_UINT(1, nPublicReadDepth);
   UT_ASSERT_EQ_UINT(0, ut_unlocks);

   reset_read_unlock(1);
   utt_od_control_read_unlock();
   UT_ASSERT_EQ_UINT(1, ut_unlocks);
   UT_ASSERT_EQ_INT(FALSE, bPublicLockPhysical);

   reset_read_unlock(1);
   nPublicWriteDepth = 1;
   utt_od_control_read_unlock();
   UT_ASSERT_EQ_UINT(0, ut_unlocks);
   reset_read_unlock(1);
   nAPILevel = 1;
   utt_od_control_read_unlock();
   UT_ASSERT_EQ_UINT(0, ut_unlocks);
   reset_read_unlock(1);
   bPublicLockPhysical = FALSE;
   utt_od_control_read_unlock();
   UT_ASSERT_EQ_UINT(0, ut_unlocks);
}

static const UTTestCase ut_cases[] = {
   {"inactive read", returns_defensively_without_a_public_read},
   {"final read", releases_only_the_final_uncovered_physical_read}
};
