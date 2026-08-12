#define UT_CUSTOM_MOCK_ODComOutbound
#define UT_CUSTOM_MOCK_ODEditEstDrawBytes
static int ut_outbound_bytes;
static UINT ut_full_redraw_bytes;
static unsigned ut_outbound_calls;
static unsigned ut_estimate_calls;

tODResult utm_ODComOutbound(tPortHandle port, int *bytes)
{
   ++ut_outbound_calls;
   UT_ASSERT_EQ_PTR(hSerialPort, port);
   *bytes = ut_outbound_bytes;
   return(kODRCSuccess);
}

UINT utm_ODEditEstDrawBytes(tEditInstance *instance, UINT start_line,
   UINT start_column, UINT finish_line, UINT finish_column)
{
   ++ut_estimate_calls;
   UT_ASSERT_EQ_UINT(0, start_line);
   UT_ASSERT_EQ_UINT(0, start_column);
   UT_ASSERT_EQ_UINT(instance->unAreaHeight - 1, finish_line);
   UT_ASSERT_EQ_UINT(instance->unAreaWidth, finish_column);
   return(ut_full_redraw_bytes);
}

static void uses_defaults_when_no_estimate_is_possible(void)
{
   tEditInstance instance;

   ut_outbound_calls = 0;
   ut_estimate_calls = 0;
   od_control.baud = 0;
   UT_ASSERT_EQ_INT(TRUE,
      utt_ODEditRecommendFullRedraw(&instance, 50, TRUE));
   UT_ASSERT_EQ_INT(FALSE,
      utt_ODEditRecommendFullRedraw(&instance, 50, FALSE));
   UT_ASSERT_EQ_UINT(0, ut_outbound_calls);

   od_control.baud = 9600;
   hSerialPort = (tPortHandle)1;
   ut_outbound_bytes = SIZE_NON_ZERO;
   UT_ASSERT_EQ_INT(TRUE,
      utt_ODEditRecommendFullRedraw(&instance, 50, TRUE));
   UT_ASSERT_EQ_INT(FALSE,
      utt_ODEditRecommendFullRedraw(&instance, 50, FALSE));
   UT_ASSERT_EQ_UINT(2, ut_outbound_calls);
   UT_ASSERT_EQ_UINT(0, ut_estimate_calls);
}

static void compares_incremental_and_full_estimates(void)
{
   tEditInstance instance;

   instance.unAreaHeight = 10;
   instance.unAreaWidth = 40;
   od_control.baud = 9600;
   hSerialPort = (tPortHandle)1;
   ut_outbound_calls = 0;
   ut_estimate_calls = 0;
   ut_outbound_bytes = 20;
   ut_full_redraw_bytes = 100;
   UT_ASSERT_EQ_INT(TRUE,
      utt_ODEditRecommendFullRedraw(&instance, 81, FALSE));
   UT_ASSERT_EQ_INT(FALSE,
      utt_ODEditRecommendFullRedraw(&instance, 80, TRUE));
   UT_ASSERT_EQ_UINT(2, ut_outbound_calls);
   UT_ASSERT_EQ_UINT(2, ut_estimate_calls);
}

static const UTTestCase ut_cases[] = {
   {"default decision", uses_defaults_when_no_estimate_is_possible},
   {"estimated decision", compares_incremental_and_full_estimates}
};
