#ifdef ODPLAT_DOS
#define UT_CUSTOM_MOCK_clock
static clock_t ut_clock_values[2];
static unsigned ut_clock_index;

clock_t utm_clock(void)
{
   UT_ASSERT(ut_clock_index < 2);
   return(ut_clock_values[ut_clock_index++]);
}
#else
#define UT_CUSTOM_MOCK_ODTimerLeft
static tODMilliSec ut_time_left;
static unsigned ut_time_left_calls;

tODMilliSec utm_ODTimerLeft(tODTimer *timer)
{
   ++ut_time_left_calls;
   UT_ASSERT_NOT_NULL(timer);
   return(ut_time_left);
}
#endif

static tODTimer ut_timer;

static void reports_elapsed_and_pending_timers(void)
{
   memset(&ut_timer, 0, sizeof(ut_timer));
#ifdef ODPLAT_DOS
   ut_timer.Start = (clock_t)100;
   ut_timer.Duration = (clock_t)20;

   ut_clock_index = 0;
   ut_clock_values[0] = (clock_t)110;
   ut_clock_values[1] = (clock_t)110;
   UT_ASSERT_EQ_INT(FALSE, utt_ODTimerElapsed(&ut_timer));
   UT_ASSERT_EQ_UINT(2, ut_clock_index);

   ut_clock_index = 0;
   ut_clock_values[0] = (clock_t)121;
   UT_ASSERT_EQ_INT(TRUE, utt_ODTimerElapsed(&ut_timer));
   UT_ASSERT_EQ_UINT(1, ut_clock_index);

   ut_clock_index = 0;
   ut_clock_values[0] = (clock_t)110;
   ut_clock_values[1] = (clock_t)99;
   UT_ASSERT_EQ_INT(TRUE, utt_ODTimerElapsed(&ut_timer));
   UT_ASSERT_EQ_UINT(2, ut_clock_index);
#else
   ut_time_left_calls = 0;
   ut_time_left = 1;
   UT_ASSERT_EQ_INT(FALSE, utt_ODTimerElapsed(&ut_timer));
   ut_time_left = 0;
   UT_ASSERT_EQ_INT(TRUE, utt_ODTimerElapsed(&ut_timer));
   UT_ASSERT_EQ_UINT(2, ut_time_left_calls);
#endif
}

static const UTTestCase ut_cases[] = {
   {"elapsed state", reports_elapsed_and_pending_timers}
};
