#define UT_CUSTOM_MOCK_ODKrnlStart
static unsigned ut_calls;
tODResult utm_ODKrnlStart(BOOL preserve)
{
   ++ut_calls; UT_ASSERT_EQ_INT(FALSE, preserve); return kODRCGeneralFailure;
}
static void starts_without_preserving_pending_work(void)
{
   ut_calls = 0;
   UT_ASSERT_EQ_INT(kODRCGeneralFailure, utt_ODKrnlInitialize());
   UT_ASSERT_EQ_UINT(1, ut_calls);
}
static const UTTestCase ut_cases[] = {
   {"start", starts_without_preserving_pending_work}
};
