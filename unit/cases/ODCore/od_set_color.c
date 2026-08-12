#define UT_CUSTOM_MOCK_od_set_attrib

static unsigned ut_calls;
static INT ut_attribute;

void ODCALL utm_od_set_attrib(INT attribute)
{
   ++ut_calls;
   ut_attribute = attribute;
}

static void combines_the_pc_foreground_and_background_nibbles(void)
{
   ut_calls = 0;
   ut_attribute = 0;
   utt_od_set_color(5, 2);
   UT_ASSERT_EQ_UINT(1, ut_calls);
   UT_ASSERT_EQ_INT(0x25, ut_attribute);
}

static const UTTestCase ut_cases[] = {
   {"combined attribute", combines_the_pc_foreground_and_background_nibbles}
};
