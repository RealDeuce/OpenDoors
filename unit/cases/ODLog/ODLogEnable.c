#define UT_CUSTOM_MOCK_od_log_open

static unsigned ut_open_calls;

BOOL ODCALL utm_od_log_open(void)
{
   ++ut_open_calls;
   return FALSE;
}

static void enables_by_opening_log(void)
{
   ut_open_calls = 0;
   utt_ODLogEnable();
   UT_ASSERT_EQ_UINT(1, ut_open_calls);
}

static const UTTestCase ut_cases[] = {
   {"opens log", enables_by_opening_log}
};
