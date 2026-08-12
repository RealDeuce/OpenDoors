#define UT_CUSTOM_MOCK_ODSyncControlReadLock
#define UT_CUSTOM_MOCK_ODSyncControlWriteLock
static unsigned ut_read_locks;
static unsigned ut_write_locks;

void utm_ODSyncControlReadLock(void) { ++ut_read_locks; }
void utm_ODSyncControlWriteLock(void) { ++ut_write_locks; }

static void reset_acquire(void)
{
   ut_read_locks = 0;
   ut_write_locks = 0;
   bPublicLockPhysical = FALSE;
}

static void acquires_the_strongest_requested_public_lock(void)
{
   reset_acquire();
   nPublicWriteDepth = 1;
   nPublicReadDepth = 1;
   utt_ODAcquirePublicLock();
   UT_ASSERT_EQ_UINT(1, ut_write_locks);
   UT_ASSERT_EQ_UINT(0, ut_read_locks);
   UT_ASSERT_EQ_INT(TRUE, bPublicLockPhysical);

   reset_acquire();
   nPublicWriteDepth = 0;
   nPublicReadDepth = 1;
   utt_ODAcquirePublicLock();
   UT_ASSERT_EQ_UINT(0, ut_write_locks);
   UT_ASSERT_EQ_UINT(1, ut_read_locks);
   UT_ASSERT_EQ_INT(TRUE, bPublicLockPhysical);

   reset_acquire();
   nPublicWriteDepth = 0;
   nPublicReadDepth = 0;
   utt_ODAcquirePublicLock();
   UT_ASSERT_EQ_UINT(0, ut_write_locks);
   UT_ASSERT_EQ_UINT(0, ut_read_locks);
   UT_ASSERT_EQ_INT(FALSE, bPublicLockPhysical);
}

static const UTTestCase ut_cases[] = {
   {"public lock strength", acquires_the_strongest_requested_public_lock}
};
