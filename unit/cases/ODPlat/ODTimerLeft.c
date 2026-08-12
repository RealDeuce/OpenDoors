#ifdef ODPLAT_DOS
#define UT_CUSTOM_MOCK_clock
#define UT_CUSTOM_MOCK_ODDWordMultiply
static clock_t ut_clock_value;
static unsigned ut_multiply_calls;

clock_t utm_clock(void)
{
   return(ut_clock_value);
}

DWORD utm_ODDWordMultiply(DWORD multiplicand, DWORD multiplier)
{
   ++ut_multiply_calls;
   UT_ASSERT_EQ_UINT(55, multiplier);
   return(multiplicand * multiplier);
}
#elif defined(ODPLAT_DOS32)
#define UT_CUSTOM_MOCK_OD32BIOSClock
static tODMilliSec ut_clock_value;

DWORD utm_OD32BIOSClock(void)
{
   return(ut_clock_value);
}
#elif defined(ODPLAT_WIN32)
#define UT_CUSTOM_MOCK_GetTickCount
static tODMilliSec ut_clock_value;

DWORD WINAPI utm_GetTickCount(void)
{
   return(ut_clock_value);
}
#elif defined(ODPLAT_NIX)
#define UT_CUSTOM_MOCK_clock_gettime
static struct timespec ut_time_value;

int utm_clock_gettime(clockid_t clock_id, struct timespec *value)
{
   UT_ASSERT_EQ_INT(CLOCK_MONOTONIC, clock_id);
   UT_ASSERT_NOT_NULL(value);
   *value = ut_time_value;
   return(0);
}
#endif

static tODTimer ut_timer;

static void reports_time_remaining_and_expiration(void)
{
   memset(&ut_timer, 0, sizeof(ut_timer));
#ifdef ODPLAT_DOS
   ut_timer.Start = (clock_t)100;
   ut_timer.Duration = (clock_t)20;
   ut_multiply_calls = 0;
   ut_clock_value = (clock_t)110;
   UT_ASSERT_EQ_UINT(550, utt_ODTimerLeft(&ut_timer));
   UT_ASSERT_EQ_UINT(1, ut_multiply_calls);
   ut_clock_value = (clock_t)121;
   UT_ASSERT_EQ_UINT(0, utt_ODTimerLeft(&ut_timer));
   ut_clock_value = (clock_t)99;
   UT_ASSERT_EQ_UINT(0, utt_ODTimerLeft(&ut_timer));
#elif defined(ODPLAT_DOS32)
   ut_timer.Start = 100;
   ut_timer.Duration = 10;
   ut_clock_value = 105;
   UT_ASSERT_EQ_UINT(275, utt_ODTimerLeft(&ut_timer));
   ut_clock_value = 110;
   UT_ASSERT_EQ_UINT(0, utt_ODTimerLeft(&ut_timer));
   ut_timer.Start = OD_DOS32_TICKS_PER_DAY - 5;
   ut_timer.Duration = 7;
   ut_clock_value = 1;
   UT_ASSERT_EQ_UINT(55, utt_ODTimerLeft(&ut_timer));
   ut_clock_value = 2;
   UT_ASSERT_EQ_UINT(0, utt_ODTimerLeft(&ut_timer));
#elif defined(ODPLAT_WIN32)
   ut_timer.Start = 100;
   ut_timer.Duration = 20;
   ut_clock_value = 110;
   UT_ASSERT_EQ_UINT(10, utt_ODTimerLeft(&ut_timer));
   ut_clock_value = 121;
   UT_ASSERT_EQ_UINT(0, utt_ODTimerLeft(&ut_timer));
   ut_clock_value = 99;
   UT_ASSERT_EQ_UINT(0, utt_ODTimerLeft(&ut_timer));
#elif defined(ODPLAT_NIX)
   ut_timer.Start = 100000;
   ut_timer.Duration = 20;
   ut_time_value.tv_sec = 100;
   ut_time_value.tv_nsec = 10000000;
   UT_ASSERT_EQ_UINT(10, utt_ODTimerLeft(&ut_timer));
   ut_time_value.tv_nsec = 20000000;
   UT_ASSERT_EQ_UINT(0, utt_ODTimerLeft(&ut_timer));
#endif
}

static const UTTestCase ut_cases[] = {
   {"time left", reports_time_remaining_and_expiration}
};
