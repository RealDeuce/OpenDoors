#define UT_CUSTOM_MOCK_ODSyncControlReadLock
#define UT_CUSTOM_MOCK_ODSyncIsOwnerThread
#define UT_CUSTOM_MOCK_ODSyncSessionInitialize
static tODResult ut_initialize_result;
static BOOL ut_activate_on_initialize;
static BOOL ut_owner;
static unsigned ut_initializes;
static unsigned ut_read_locks;

tODResult utm_ODSyncSessionInitialize(void)
{
   ++ut_initializes;
   if(ut_initialize_result == kODRCSuccess && ut_activate_on_initialize)
      bSyncActive = TRUE;
   return ut_initialize_result;
}
BOOL utm_ODSyncIsOwnerThread(void) { return ut_owner; }
void utm_ODSyncControlReadLock(void) { ++ut_read_locks; }

static void reset_read_lock(void)
{
   bSyncActive = TRUE;
   bODInitialized = TRUE;
   bPublicLockPhysical = FALSE;
   nAPILevel = 0;
   nPublicReadDepth = 0;
   nPublicWriteDepth = 0;
   ut_initialize_result = kODRCSuccess;
   ut_activate_on_initialize = TRUE;
   ut_owner = TRUE;
   ut_initializes = 0;
   ut_read_locks = 0;
}

static void initializes_on_demand_and_rejects_failure_or_nonowners(void)
{
   reset_read_lock();
   bSyncActive = FALSE;
   ut_initialize_result = kODRCGeneralFailure;
   UT_ASSERT(utt_od_control_read_lock() == NULL);
   UT_ASSERT_EQ_UINT(1, ut_initializes);

   reset_read_lock();
   bSyncActive = FALSE;
   UT_ASSERT_EQ_PTR(&od_control, utt_od_control_read_lock());
   UT_ASSERT_EQ_UINT(1, ut_initializes);

   reset_read_lock();
   bSyncActive = FALSE;
   ut_activate_on_initialize = FALSE;
   UT_ASSERT_EQ_PTR(&od_control, utt_od_control_read_lock());

   reset_read_lock();
   ut_owner = FALSE;
   UT_ASSERT(utt_od_control_read_lock() == NULL);
}

static void nests_reads_inside_a_public_write_without_a_physical_read(void)
{
   reset_read_lock();
   nPublicWriteDepth = 1;
   UT_ASSERT_EQ_PTR(&od_control, utt_od_control_read_lock());
   UT_ASSERT_EQ_UINT(1, nPublicReadDepth);
   UT_ASSERT_EQ_UINT(0, ut_read_locks);
}

static void takes_a_physical_read_only_for_the_first_initialized_outer_read(void)
{
   reset_read_lock();
   UT_ASSERT_EQ_PTR(&od_control, utt_od_control_read_lock());
   UT_ASSERT_EQ_UINT(1, ut_read_locks);
   UT_ASSERT_EQ_INT(TRUE, bPublicLockPhysical);

   reset_read_lock();
   nPublicReadDepth = 1;
   utt_od_control_read_lock();
   UT_ASSERT_EQ_UINT(2, nPublicReadDepth);
   UT_ASSERT_EQ_UINT(0, ut_read_locks);

   reset_read_lock();
   nAPILevel = 1;
   utt_od_control_read_lock();
   UT_ASSERT_EQ_UINT(0, ut_read_locks);

   reset_read_lock();
   bODInitialized = FALSE;
   utt_od_control_read_lock();
   UT_ASSERT_EQ_UINT(0, ut_read_locks);
}

static const UTTestCase ut_cases[] = {
   {"initialization and owner", initializes_on_demand_and_rejects_failure_or_nonowners},
   {"read under write", nests_reads_inside_a_public_write_without_a_physical_read},
   {"physical read", takes_a_physical_read_only_for_the_first_initialized_outer_read}
};
