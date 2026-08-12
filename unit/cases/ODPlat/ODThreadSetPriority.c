#ifdef ODPLAT_WIN32
#define UT_CUSTOM_MOCK_SetThreadPriority
static int ut_expected_priority;
static BOOL ut_set_result;
static unsigned ut_set_calls;

BOOL WINAPI utm_SetThreadPriority(HANDLE thread, int priority)
{
   ++ut_set_calls;
   UT_ASSERT_EQ_PTR((HANDLE)(DWORD_PTR)41, thread);
   UT_ASSERT_EQ_INT(ut_expected_priority, priority);
   return(ut_set_result);
}

static void maps_each_priority_and_reports_platform_results(void)
{
   static const tODThreadPriority priorities[] = {
      OD_PRIORITY_LOWEST,
      OD_PRIORITY_BELOW_NORMAL,
      OD_PRIORITY_NORMAL,
      OD_PRIORITY_ABOVE_NORMAL,
      OD_PRIORITY_HIGHEST
   };
   static const int native_priorities[] = {
      THREAD_PRIORITY_LOWEST,
      THREAD_PRIORITY_BELOW_NORMAL,
      THREAD_PRIORITY_NORMAL,
      THREAD_PRIORITY_ABOVE_NORMAL,
      THREAD_PRIORITY_HIGHEST
   };
   unsigned index;

   ut_set_calls = 0;
   ut_set_result = TRUE;
   for(index = 0; index < sizeof(priorities) / sizeof(priorities[0]); ++index)
   {
      ut_expected_priority = native_priorities[index];
      UT_ASSERT_EQ_INT(kODRCSuccess, utt_ODThreadSetPriority(
         (HANDLE)(DWORD_PTR)41, priorities[index]));
   }
   UT_ASSERT_EQ_UINT(sizeof(priorities) / sizeof(priorities[0]), ut_set_calls);

   ut_set_result = FALSE;
   ut_expected_priority = THREAD_PRIORITY_NORMAL;
   UT_ASSERT_EQ_INT(kODRCGeneralFailure, utt_ODThreadSetPriority(
      (HANDLE)(DWORD_PTR)41, OD_PRIORITY_NORMAL));
   UT_ASSERT_EQ_INT(kODRCInvalidCall, utt_ODThreadSetPriority(
      (HANDLE)(DWORD_PTR)41, (tODThreadPriority)99));
}
#else
static void accepts_priorities_on_the_pthread_platform(void)
{
   UT_ASSERT_EQ_INT(kODRCSuccess, utt_ODThreadSetPriority(
      (tODThreadHandle)0, OD_PRIORITY_NORMAL));
}
#endif

static const UTTestCase ut_cases[] = {
#ifdef ODPLAT_WIN32
   {"priority mapping", maps_each_priority_and_reports_platform_results}
#else
   {"pthread no-op", accepts_priorities_on_the_pthread_platform}
#endif
};
