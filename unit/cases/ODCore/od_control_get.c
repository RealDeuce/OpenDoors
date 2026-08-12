static void returns_the_process_control_structure(void)
{
   UT_ASSERT_EQ_PTR(&od_control, utt_od_control_get());
}

static const UTTestCase ut_cases[] = {
   {"control pointer", returns_the_process_control_structure}
};
