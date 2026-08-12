#define UT_CUSTOM_MOCK_ODScrnEnableCaret
#define UT_CUSTOM_MOCK_ODComClearOutbound
#define UT_CUSTOM_MOCK_ODEditDrawAreaLine

static BOOL ut_caret_states[2];
static unsigned ut_caret_calls;
static unsigned ut_clear_calls;
static UINT ut_draw_lines[4];
static unsigned ut_draw_calls;

void utm_ODScrnEnableCaret(BOOL enabled)
{
   UT_ASSERT(ut_caret_calls < 2);
   ut_caret_states[ut_caret_calls++] = enabled;
}

tODResult utm_ODComClearOutbound(tPortHandle port)
{
   ++ut_clear_calls;
   UT_ASSERT_EQ_PTR(hSerialPort, port);
   return(kODRCSuccess);
}

void utm_ODEditDrawAreaLine(tEditInstance *instance, UINT line)
{
   UT_ASSERT_NOT_NULL(instance);
   UT_ASSERT(ut_draw_calls < 4);
   ut_draw_lines[ut_draw_calls++] = line;
}

static void reset_redraw(void)
{
   ut_caret_calls = 0;
   ut_clear_calls = 0;
   ut_draw_calls = 0;
}

static void redraws_an_empty_local_area(void)
{
   tEditInstance instance;
   reset_redraw();
   od_control.baud = 0;
   instance.unAreaHeight = 0;
   utt_ODEditRedrawArea(&instance);
   UT_ASSERT_EQ_UINT(2, ut_caret_calls);
   UT_ASSERT_EQ_INT(FALSE, ut_caret_states[0]);
   UT_ASSERT_EQ_INT(TRUE, ut_caret_states[1]);
   UT_ASSERT_EQ_UINT(0, ut_clear_calls);
   UT_ASSERT_EQ_UINT(0, ut_draw_calls);
}

static void clears_remote_output_and_draws_each_area_line(void)
{
   tEditInstance instance;
   reset_redraw();
   od_control.baud = 38400;
   instance.unAreaHeight = 3;
   utt_ODEditRedrawArea(&instance);
   UT_ASSERT_EQ_UINT(2, ut_caret_calls);
   UT_ASSERT_EQ_UINT(1, ut_clear_calls);
   UT_ASSERT_EQ_UINT(3, ut_draw_calls);
   UT_ASSERT_EQ_UINT(0, ut_draw_lines[0]);
   UT_ASSERT_EQ_UINT(1, ut_draw_lines[1]);
   UT_ASSERT_EQ_UINT(2, ut_draw_lines[2]);
}

static const UTTestCase ut_cases[] = {
   {"empty local area", redraws_an_empty_local_area},
   {"remote redraw", clears_remote_output_and_draws_each_area_line}
};
