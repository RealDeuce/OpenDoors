#ifdef OD_THREAD_SUPPORT
#define UT_CUSTOM_MOCK_ODSyncIsOwnerThread
static BOOL ut_owner;
static unsigned ut_owner_calls;

BOOL utm_ODSyncIsOwnerThread(void)
{
   ++ut_owner_calls;
   return(ut_owner);
}
#endif

static void reports_only_an_active_owner_inside_the_api(void)
{
#ifdef OD_THREAD_SUPPORT
   bSyncActive = FALSE;
   nAPILevel = 1;
   ut_owner = TRUE;
   ut_owner_calls = 0;
   UT_ASSERT_EQ_INT(FALSE, utt_ODSyncAPIActiveOnOwnerThread());
   UT_ASSERT_EQ_UINT(0, ut_owner_calls);

   bSyncActive = TRUE;
   ut_owner = FALSE;
   UT_ASSERT_EQ_INT(FALSE, utt_ODSyncAPIActiveOnOwnerThread());
   ut_owner = TRUE;
   nAPILevel = 0;
   UT_ASSERT_EQ_INT(FALSE, utt_ODSyncAPIActiveOnOwnerThread());
   nAPILevel = 1;
   UT_ASSERT_EQ_INT(TRUE, utt_ODSyncAPIActiveOnOwnerThread());
#else
   UT_ASSERT_EQ_INT(FALSE, utt_ODSyncAPIActiveOnOwnerThread());
#endif
}

static const UTTestCase ut_cases[] = {
   {"owner API activity", reports_only_an_active_owner_inside_the_api}
};
