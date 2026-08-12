#define UT_CUSTOM_MOCK_ODSyncControlReadLock
#define UT_CUSTOM_MOCK_ODSyncControlWriteUnlock
static unsigned ut_read_locks;
static unsigned ut_write_unlocks;
void utm_ODSyncControlReadLock(void) { ++ut_read_locks; }
void utm_ODSyncControlWriteUnlock(void) { ++ut_write_unlocks; }

static void reset_write_unlock(unsigned depth)
{
   nPublicWriteDepth = depth;
   nPublicReadDepth = 0;
   nAPILevel = 0;
   bPublicLockPhysical = TRUE;
   ut_read_locks = 0;
   ut_write_unlocks = 0;
}

static void returns_defensively_without_a_public_write(void)
{
   reset_write_unlock(0);
   utt_od_control_write_unlock();
   UT_ASSERT_EQ_UINT(0, ut_write_unlocks);
}

static void releases_only_the_final_uncovered_physical_write(void)
{
   reset_write_unlock(2);
   utt_od_control_write_unlock();
   UT_ASSERT_EQ_UINT(1, nPublicWriteDepth);
   UT_ASSERT_EQ_UINT(0, ut_write_unlocks);

   reset_write_unlock(1);
   utt_od_control_write_unlock();
   UT_ASSERT_EQ_UINT(1, ut_write_unlocks);
   UT_ASSERT_EQ_INT(FALSE, bPublicLockPhysical);

   reset_write_unlock(1);
   nAPILevel = 1;
   utt_od_control_write_unlock();
   UT_ASSERT_EQ_UINT(0, ut_write_unlocks);
   reset_write_unlock(1);
   bPublicLockPhysical = FALSE;
   utt_od_control_write_unlock();
   UT_ASSERT_EQ_UINT(0, ut_write_unlocks);
}

static void downgrades_to_a_pending_public_read(void)
{
   reset_write_unlock(1);
   nPublicReadDepth = 1;
   utt_od_control_write_unlock();
   UT_ASSERT_EQ_UINT(1, ut_write_unlocks);
   UT_ASSERT_EQ_UINT(1, ut_read_locks);
   UT_ASSERT_EQ_INT(TRUE, bPublicLockPhysical);

   reset_write_unlock(1);
   utt_od_control_write_unlock();
   UT_ASSERT_EQ_UINT(0, ut_read_locks);
}

static const UTTestCase ut_cases[] = {
   {"inactive write", returns_defensively_without_a_public_write},
   {"final write", releases_only_the_final_uncovered_physical_write},
   {"read downgrade", downgrades_to_a_pending_public_read}
};
