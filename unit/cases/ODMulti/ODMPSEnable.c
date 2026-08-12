#define UT_CUSTOM_MOCK_od_set_personality

static unsigned ut_set_calls;
static const char ut_test_name[] = "TEST";

#ifdef OD_TEXTMODE
void ODCALL pdef_opendoors(BYTE operation) { (void)operation; }
void ODCALL pdef_ra(BYTE operation) { (void)operation; }
void ODCALL pdef_wildcat(BYTE operation) { (void)operation; }
void ODCALL pdef_pcboard(BYTE operation) { (void)operation; }
#endif

BOOL ODCALL utm_od_set_personality(const char *name)
{
   UT_ASSERT_EQ_PTR(ut_test_name, name);
   ++ut_set_calls;
   return TRUE;
}

static void installs_the_public_selector(void)
{
   pfSetPersonality = NULL;
   ut_set_calls = 0;
   utt_ODMPSEnable();
   UT_ASSERT_NOT_NULL(pfSetPersonality);
   UT_ASSERT((*pfSetPersonality)(ut_test_name));
   UT_ASSERT_EQ_UINT(1, ut_set_calls);
}

static const UTTestCase ut_cases[] = {
   {"selector hook", installs_the_public_selector}
};
