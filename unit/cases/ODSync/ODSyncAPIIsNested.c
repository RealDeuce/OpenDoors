#define UT_CUSTOM_MOCK_ODSyncIsOwnerThread
static BOOL ut_owner;
BOOL utm_ODSyncIsOwnerThread(void) { return(ut_owner); }

static void reports_owner_nesting_on_every_platform(void)
{
   bSyncActive = FALSE;
   nAPILevel = 1;
   ut_owner = TRUE;
   UT_ASSERT(!utt_ODSyncAPIIsNested());
   bSyncActive = TRUE;
   ut_owner = FALSE;
   UT_ASSERT(!utt_ODSyncAPIIsNested());
   ut_owner = TRUE;
   nAPILevel = 0;
   UT_ASSERT(!utt_ODSyncAPIIsNested());
   nAPILevel = 1;
   UT_ASSERT(utt_ODSyncAPIIsNested());
}

static const UTTestCase ut_cases[] = {
   {"API nesting", reports_owner_nesting_on_every_platform}
};
