#define UT_CUSTOM_MOCK_ODKrnlStart
static unsigned ut_calls;
tODResult utm_ODKrnlStart(BOOL preserve)
{
   ++ut_calls; UT_ASSERT_EQ_INT(TRUE, preserve); return kODRCNoMemory;
}
static void starts_while_preserving_pending_work(void)
{
   ut_calls = 0;
   UT_ASSERT_EQ_INT(kODRCNoMemory, utt_ODKrnlRestart());
   UT_ASSERT_EQ_UINT(1, ut_calls);
}
static const UTTestCase ut_cases[] = {
   {"restart", starts_while_preserving_pending_work}
};
