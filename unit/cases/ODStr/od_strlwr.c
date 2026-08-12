static void converts_each_character_and_returns_input(void)
{
   char text[] = "Ab1";
   UT_ASSERT_EQ_PTR(text, utt_od_strlwr(text));
   UT_ASSERT_EQ_INT(0, strcmp("ab1", text));
}

static void accepts_empty_string(void)
{
   char text[] = "";
   UT_ASSERT_EQ_PTR(text, utt_od_strlwr(text));
}

static const UTTestCase ut_cases[] = {
   {"lowercase string", converts_each_character_and_returns_input},
   {"empty string", accepts_empty_string}
};
