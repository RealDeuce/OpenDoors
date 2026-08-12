#define UT_CUSTOM_MOCK_ODSyncAPIWriterHeldByCurrentThread

BOOL utm_ODSyncAPIWriterHeldByCurrentThread(void)
{
   return(FALSE);
}

#ifdef ODPLAT_WIN32
#define UT_CUSTOM_MOCK_Sleep
static DWORD ut_expected_milliseconds;
static unsigned ut_sleep_calls;

void WINAPI utm_Sleep(DWORD milliseconds)
{
   ++ut_sleep_calls;
   UT_ASSERT_EQ_UINT(ut_expected_milliseconds, milliseconds);
}
#else
#define UT_CUSTOM_MOCK_nanosleep
static unsigned ut_sleep_calls;

int utm_nanosleep(const struct timespec *requested,
   struct timespec *remaining)
{
   ++ut_sleep_calls;
   UT_ASSERT_NOT_NULL(requested);
   UT_ASSERT_NOT_NULL(remaining);
   if(ut_sleep_calls == 1)
   {
      UT_ASSERT_EQ_UINT(1, requested->tv_sec);
      UT_ASSERT_EQ_UINT(234000000, requested->tv_nsec);
      remaining->tv_sec = 0;
      remaining->tv_nsec = 500000000;
      return(EINTR);
   }
   UT_ASSERT_EQ_UINT(0, requested->tv_sec);
   UT_ASSERT_EQ_UINT(500000000, requested->tv_nsec);
   return(0);
}
#endif

static void sleeps_for_the_requested_interval(void)
{
   ut_sleep_calls = 0;
#ifdef ODPLAT_WIN32
   ut_expected_milliseconds = 1234;
#endif
   utt_ODThreadSleep(1234);
#ifdef ODPLAT_WIN32
   UT_ASSERT_EQ_UINT(1, ut_sleep_calls);
#else
   UT_ASSERT_EQ_UINT(2, ut_sleep_calls);
#endif
}

static const UTTestCase ut_cases[] = {
   {"sleep", sleeps_for_the_requested_interval}
};
