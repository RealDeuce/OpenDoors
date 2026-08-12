#define UT_CUSTOM_MOCK_ODSyncControlReadLock
#define UT_CUSTOM_MOCK_ODSyncControlWriteLock
static unsigned ut_read_locks;
static unsigned ut_write_locks;

void utm_ODSyncControlReadLock(void) { ++ut_read_locks; }
void utm_ODSyncControlWriteLock(void) { ++ut_write_locks; }

static void reset_complete(void)
{
   bSyncActive = TRUE;
   bPublicLockPhysical = FALSE;
   nAPILevel = 0;
   nPublicReadDepth = 0;
   nPublicWriteDepth = 0;
   ut_read_locks = 0;
   ut_write_locks = 0;
}

static void ignores_sessions_which_cannot_take_a_public_lock(void)
{
   reset_complete();
   bSyncActive = FALSE;
   utt_ODSyncInitializationComplete();
   reset_complete();
   bPublicLockPhysical = TRUE;
   utt_ODSyncInitializationComplete();
   reset_complete();
   nAPILevel = 1;
   utt_ODSyncInitializationComplete();
   UT_ASSERT_EQ_UINT(0, ut_read_locks);
   UT_ASSERT_EQ_UINT(0, ut_write_locks);
}

static void materializes_the_strongest_pending_public_lock(void)
{
   reset_complete();
   nPublicWriteDepth = 1;
   nPublicReadDepth = 1;
   utt_ODSyncInitializationComplete();
   UT_ASSERT_EQ_UINT(1, ut_write_locks);
   UT_ASSERT_EQ_UINT(0, ut_read_locks);
   UT_ASSERT_EQ_INT(TRUE, bPublicLockPhysical);

   reset_complete();
   nPublicReadDepth = 1;
   utt_ODSyncInitializationComplete();
   UT_ASSERT_EQ_UINT(0, ut_write_locks);
   UT_ASSERT_EQ_UINT(1, ut_read_locks);
   UT_ASSERT_EQ_INT(TRUE, bPublicLockPhysical);

   reset_complete();
   utt_ODSyncInitializationComplete();
   UT_ASSERT_EQ_INT(FALSE, bPublicLockPhysical);
}

static const UTTestCase ut_cases[] = {
   {"ineligible session", ignores_sessions_which_cannot_take_a_public_lock},
   {"pending lock", materializes_the_strongest_pending_public_lock}
};
