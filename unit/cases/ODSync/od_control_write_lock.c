#define UT_CUSTOM_MOCK_ODSyncControlWriteLock
#define UT_CUSTOM_MOCK_ODSyncIsOwnerThread
#define UT_CUSTOM_MOCK_ODSyncSessionInitialize
static tODResult ut_initialize_result;
static BOOL ut_activate_on_initialize;
static BOOL ut_owner;
static unsigned ut_initializes;
static unsigned ut_write_locks;

tODResult utm_ODSyncSessionInitialize(void)
{
   ++ut_initializes;
   if(ut_initialize_result == kODRCSuccess && ut_activate_on_initialize)
      bSyncActive = TRUE;
   return ut_initialize_result;
}
BOOL utm_ODSyncIsOwnerThread(void) { return ut_owner; }
void utm_ODSyncControlWriteLock(void) { ++ut_write_locks; }

static void reset_write_lock(void)
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
   ut_write_locks = 0;
}

static void initializes_on_demand_and_rejects_failure_or_nonowners(void)
{
   reset_write_lock();
   bSyncActive = FALSE;
   ut_initialize_result = kODRCGeneralFailure;
   UT_ASSERT(utt_od_control_write_lock() == NULL);

   reset_write_lock();
   bSyncActive = FALSE;
   UT_ASSERT_EQ_PTR(&od_control, utt_od_control_write_lock());
   UT_ASSERT_EQ_UINT(1, ut_initializes);

   reset_write_lock();
   bSyncActive = FALSE;
   ut_activate_on_initialize = FALSE;
   UT_ASSERT_EQ_PTR(&od_control, utt_od_control_write_lock());

   reset_write_lock();
   ut_owner = FALSE;
   UT_ASSERT(utt_od_control_write_lock() == NULL);
}

static void rejects_upgrade_from_a_read_but_allows_recursive_writes(void)
{
   reset_write_lock();
   nPublicReadDepth = 1;
   UT_ASSERT(utt_od_control_write_lock() == NULL);

   reset_write_lock();
   nPublicReadDepth = 1;
   nPublicWriteDepth = 1;
   UT_ASSERT_EQ_PTR(&od_control, utt_od_control_write_lock());
   UT_ASSERT_EQ_UINT(2, nPublicWriteDepth);
   UT_ASSERT_EQ_UINT(0, ut_write_locks);
}

static void takes_a_physical_write_only_for_the_first_initialized_outer_write(void)
{
   reset_write_lock();
   UT_ASSERT_EQ_PTR(&od_control, utt_od_control_write_lock());
   UT_ASSERT_EQ_UINT(1, ut_write_locks);
   UT_ASSERT_EQ_INT(TRUE, bPublicLockPhysical);

   reset_write_lock();
   nPublicWriteDepth = 1;
   utt_od_control_write_lock();
   UT_ASSERT_EQ_UINT(0, ut_write_locks);
   reset_write_lock();
   nAPILevel = 1;
   utt_od_control_write_lock();
   UT_ASSERT_EQ_UINT(0, ut_write_locks);
   reset_write_lock();
   bODInitialized = FALSE;
   utt_od_control_write_lock();
   UT_ASSERT_EQ_UINT(0, ut_write_locks);
}

static const UTTestCase ut_cases[] = {
   {"initialization and owner", initializes_on_demand_and_rejects_failure_or_nonowners},
   {"upgrade and recursion", rejects_upgrade_from_a_read_but_allows_recursive_writes},
   {"physical write", takes_a_physical_write_only_for_the_first_initialized_outer_write}
};
