#define UT_CUSTOM_MOCK_ODSyncControlReadUnlock
#define UT_CUSTOM_MOCK_ODSyncControlWriteUnlock
static unsigned ut_read_unlocks;
static unsigned ut_write_unlocks;

void utm_ODSyncControlReadUnlock(void) { ++ut_read_unlocks; }
void utm_ODSyncControlWriteUnlock(void) { ++ut_write_unlocks; }

static void reset_release(void)
{
   ut_read_unlocks = 0;
   ut_write_unlocks = 0;
}

static void ignores_a_lock_which_is_not_physically_held(void)
{
   reset_release();
   bPublicLockPhysical = FALSE;
   nPublicWriteDepth = 1;
   utt_ODReleasePublicLock();
   UT_ASSERT_EQ_UINT(0, ut_write_unlocks);
}

static void releases_the_physical_lock_matching_the_public_depth(void)
{
   reset_release();
   bPublicLockPhysical = TRUE;
   nPublicWriteDepth = 1;
   nPublicReadDepth = 1;
   utt_ODReleasePublicLock();
   UT_ASSERT_EQ_UINT(1, ut_write_unlocks);
   UT_ASSERT_EQ_UINT(0, ut_read_unlocks);
   UT_ASSERT_EQ_INT(FALSE, bPublicLockPhysical);

   reset_release();
   bPublicLockPhysical = TRUE;
   nPublicWriteDepth = 0;
   nPublicReadDepth = 1;
   utt_ODReleasePublicLock();
   UT_ASSERT_EQ_UINT(0, ut_write_unlocks);
   UT_ASSERT_EQ_UINT(1, ut_read_unlocks);

   reset_release();
   bPublicLockPhysical = TRUE;
   nPublicWriteDepth = 0;
   nPublicReadDepth = 0;
   utt_ODReleasePublicLock();
   UT_ASSERT_EQ_UINT(0, ut_write_unlocks);
   UT_ASSERT_EQ_UINT(0, ut_read_unlocks);
   UT_ASSERT_EQ_INT(FALSE, bPublicLockPhysical);
}

static const UTTestCase ut_cases[] = {
   {"no physical lock", ignores_a_lock_which_is_not_physically_held},
   {"matching lock", releases_the_physical_lock_matching_the_public_depth}
};
