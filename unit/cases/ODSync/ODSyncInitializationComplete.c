static void requires_no_late_synchronization_transition(void)
{
   bSyncActive = TRUE;
   nAPILevel = 3;
   utt_ODSyncInitializationComplete();
   UT_ASSERT(bSyncActive);
   UT_ASSERT_EQ_UINT(3, nAPILevel);
}

static const UTTestCase ut_cases[] = {
   {"no-op completion", requires_no_late_synchronization_transition}
};
