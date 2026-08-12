#ifdef OD_MULTITHREADED
#define UT_CUSTOM_MOCK_ODControlReadRelease
#define UT_CUSTOM_MOCK_ODSyncIsOwnerThread
static BOOL ut_owner;
static unsigned ut_releases;

void utm_ODControlReadRelease(void) { ++ut_releases; }
BOOL utm_ODSyncIsOwnerThread(void) { return ut_owner; }
#endif

static void releases_unless_an_active_api_owner_retains_exclusion(void)
{
#ifdef OD_MULTITHREADED
   ut_releases = 0;
   bSyncActive = FALSE;
   nAPILevel = 0;
   utt_ODSyncControlReadUnlock();
   UT_ASSERT_EQ_UINT(0, ut_releases);
   bSyncActive = TRUE;
   utt_ODSyncControlReadUnlock();
   UT_ASSERT_EQ_UINT(1, ut_releases);
   nAPILevel = 1;
   ut_owner = FALSE;
   utt_ODSyncControlReadUnlock();
   UT_ASSERT_EQ_UINT(2, ut_releases);
   ut_owner = TRUE;
   utt_ODSyncControlReadUnlock();
   UT_ASSERT_EQ_UINT(2, ut_releases);
#else
   utt_ODSyncControlReadUnlock();
#endif
}

static const UTTestCase ut_cases[] = {
   {"read release dispatch", releases_unless_an_active_api_owner_retains_exclusion}
};
