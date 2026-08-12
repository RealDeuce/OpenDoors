#if defined(ODPLAT_NIX) && defined(USE_KERNEL_SIGNAL)
#define UT_CUSTOM_MOCK_ODKrnlForceOpenDoorsShutdown

static unsigned ut_shutdown_calls;

static void utm_ODKrnlForceOpenDoorsShutdown(BYTE reason)
{
   ++ut_shutdown_calls;
   UT_ASSERT_EQ_INT(ERRORLEVEL_NOCARRIER, reason);
}

static void reset_no_carrier(void)
{
   ut_shutdown_calls = 0;
   od_control.baud = 0;
   od_control.od_disable = 0;
}

static void requires_a_remote_connection(void)
{
   reset_no_carrier();
   utt_sig_no_carrier(SIGHUP);
   UT_ASSERT_EQ_UINT(0, ut_shutdown_calls);
}

static void honors_disabled_carrier_detection(void)
{
   reset_no_carrier(); od_control.baud = 38400;
   od_control.od_disable = DIS_CARRIERDETECT;
   utt_sig_no_carrier(SIGHUP);
   UT_ASSERT_EQ_UINT(0, ut_shutdown_calls);
}

static void requests_shutdown_for_an_enabled_remote_session(void)
{
   reset_no_carrier(); od_control.baud = 38400;
   utt_sig_no_carrier(SIGHUP);
   UT_ASSERT_EQ_UINT(1, ut_shutdown_calls);
}

static const UTTestCase ut_cases[] = {
   {"local session", requires_a_remote_connection},
   {"detection disabled", honors_disabled_carrier_detection},
   {"carrier loss", requests_shutdown_for_an_enabled_remote_session}
};
#endif
