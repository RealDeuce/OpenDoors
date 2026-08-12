#ifdef OD_MULTITHREADED
#define UT_CUSTOM_MOCK_ODControlReadAcquire
#define UT_CUSTOM_MOCK_ODSyncIsOwnerThread
static BOOL ut_owner;
static unsigned ut_acquires;

void utm_ODControlReadAcquire(void) { ++ut_acquires; }
BOOL utm_ODSyncIsOwnerThread(void) { return ut_owner; }
#endif

static void acquires_unless_an_active_api_owner_already_has_exclusion(void)
{
#ifdef OD_MULTITHREADED
   ut_acquires = 0;
   bSyncActive = FALSE;
   nAPILevel = 0;
   utt_ODSyncControlReadLock();
   UT_ASSERT_EQ_UINT(0, ut_acquires);
   bSyncActive = TRUE;
   utt_ODSyncControlReadLock();
   UT_ASSERT_EQ_UINT(1, ut_acquires);
   nAPILevel = 1;
   ut_owner = FALSE;
   utt_ODSyncControlReadLock();
   UT_ASSERT_EQ_UINT(2, ut_acquires);
   ut_owner = TRUE;
   utt_ODSyncControlReadLock();
   UT_ASSERT_EQ_UINT(2, ut_acquires);
#else
   utt_ODSyncControlReadLock();
#endif
}

static const UTTestCase ut_cases[] = {
   {"read acquire dispatch", acquires_unless_an_active_api_owner_already_has_exclusion}
};
