#ifdef ODPLAT_DOS
#define UT_CUSTOM_MOCK_clock
#define UT_CUSTOM_MOCK_ODDWordDivide

static clock_t ut_clock_value;
static unsigned ut_divide_calls;

clock_t utm_clock(void)
{
   return(ut_clock_value);
}

BOOL utm_ODDWordDivide(DWORD *quotient, DWORD *remainder,
   DWORD dividend, DWORD divisor)
{
   ++ut_divide_calls;
   UT_ASSERT_NOT_NULL(quotient);
   UT_ASSERT_NULL(remainder);
   UT_ASSERT_EQ_UINT(55, divisor);
   *quotient = dividend / divisor;
   return(TRUE);
}
#endif

#ifdef ODPLAT_DOS32
#define UT_CUSTOM_MOCK_OD32BIOSClock
static DWORD ut_clock_value;

DWORD utm_OD32BIOSClock(void)
{
   return(ut_clock_value);
}
#endif

#ifdef ODPLAT_WIN32
#define UT_CUSTOM_MOCK_GetTickCount
static DWORD ut_tick_value;

DWORD WINAPI utm_GetTickCount(void)
{
   return(ut_tick_value);
}
#endif

#ifdef ODPLAT_NIX
#define UT_CUSTOM_MOCK_gettimeofday
static struct timeval ut_time_value;

#ifdef __FreeBSD__
int utm_gettimeofday(struct timeval *value, struct timezone *zone)
#else
int utm_gettimeofday(struct timeval *value, void *zone)
#endif
{
   UT_ASSERT_NOT_NULL(value);
   UT_ASSERT_NULL(zone);
   *value = ut_time_value;
   return(0);
}
#endif

static void stores_the_current_time_and_requested_duration(void)
{
   tODTimer timer;
   memset(&timer, 0, sizeof(timer));

#ifdef ODPLAT_DOS
   ut_clock_value = (clock_t)99;
   ut_divide_calls = 0;
   utt_ODTimerStart(&timer, 789);
   UT_ASSERT_EQ_UINT(99, timer.Start);
   UT_ASSERT_EQ_UINT(14, timer.Duration);
   UT_ASSERT_EQ_UINT(1, ut_divide_calls);
#endif

#ifdef ODPLAT_DOS32
   ut_clock_value = 100;
   utt_ODTimerStart(&timer, 56);
   UT_ASSERT_EQ_UINT(100, timer.Start);
   UT_ASSERT_EQ_UINT(2, timer.Duration);
   utt_ODTimerStart(&timer, 55);
   UT_ASSERT_EQ_UINT(1, timer.Duration);
#endif

#ifdef ODPLAT_WIN32
   ut_tick_value = 1234;
   utt_ODTimerStart(&timer, 789);
   UT_ASSERT_EQ_UINT(1234, timer.Start);
   UT_ASSERT_EQ_UINT(789, timer.Duration);
#endif

#ifdef ODPLAT_NIX
   ut_time_value.tv_sec = 123;
   ut_time_value.tv_usec = 456000;
   utt_ODTimerStart(&timer, 789);
   UT_ASSERT_EQ_UINT(123456, timer.Start);
   UT_ASSERT_EQ_UINT(789, timer.Duration);
#endif
}

static const UTTestCase ut_cases[] = {
   {"start", stores_the_current_time_and_requested_duration}
};
