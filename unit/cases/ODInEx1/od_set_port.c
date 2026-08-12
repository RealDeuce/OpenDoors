static void reset_port_state(void)
{
   memset(&od_control, 0, sizeof(od_control));
   bODInitialized = FALSE;
   nForcedPort = -1;
}

static void rejects_calls_after_initialization(void)
{
   reset_port_state();
   bODInitialized = TRUE;
   od_control.port = 7;
   UT_ASSERT(!utt_od_set_port(0));
   UT_ASSERT_EQ_INT(ERR_PARAMETER, od_control.od_error);
   UT_ASSERT_EQ_INT(7, od_control.port);
   UT_ASSERT_EQ_INT(-1, nForcedPort);
}

static void rejects_ports_outside_the_byte_range(void)
{
   reset_port_state();
   UT_ASSERT(!utt_od_set_port(-1));
   UT_ASSERT_EQ_INT(ERR_PARAMETER, od_control.od_error);
   UT_ASSERT_EQ_INT(-1, nForcedPort);

   reset_port_state();
   UT_ASSERT(!utt_od_set_port(256));
   UT_ASSERT_EQ_INT(ERR_PARAMETER, od_control.od_error);
   UT_ASSERT_EQ_INT(-1, nForcedPort);
}

static void records_explicit_com1_and_the_largest_port(void)
{
   reset_port_state();
   UT_ASSERT(utt_od_set_port(0));
   UT_ASSERT_EQ_INT(0, od_control.port);
   UT_ASSERT_EQ_INT(0, nForcedPort);

   reset_port_state();
   UT_ASSERT(utt_od_set_port(255));
   UT_ASSERT_EQ_INT(255, od_control.port);
   UT_ASSERT_EQ_INT(255, nForcedPort);
}

static const UTTestCase ut_cases[] = {
   {"already initialized", rejects_calls_after_initialization},
   {"invalid range", rejects_ports_outside_the_byte_range},
   {"explicit ports", records_explicit_com1_and_the_largest_port}
};
