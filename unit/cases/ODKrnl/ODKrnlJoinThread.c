#define UT_CUSTOM_MOCK_ODThreadWaitForExit
#ifdef ODPLAT_WIN32
#define UT_CUSTOM_MOCK_CloseHandle
#endif
static unsigned ut_wait_calls;
#ifdef ODPLAT_WIN32
static tODThreadHandle ut_thread = (tODThreadHandle)1;
#else
static tODThreadHandle ut_thread = (tODThreadHandle)17;
#endif
void utm_ODThreadWaitForExit(tODThreadHandle thread)
{ ++ut_wait_calls; UT_ASSERT(thread == ut_thread); }
#ifdef ODPLAT_WIN32
WINBOOL WINAPI utm_CloseHandle(HANDLE handle)
{ UT_ASSERT(handle == ut_thread); return TRUE; }
#endif
static void ignores_a_thread_that_was_not_started(void)
{
   BOOL started = FALSE; tODThreadHandle thread = ut_thread; ut_wait_calls = 0;
   utt_ODKrnlJoinThread(&thread, &started); UT_ASSERT_EQ_UINT(0, ut_wait_calls);
   UT_ASSERT(thread == ut_thread); UT_ASSERT(!started);
}
static void waits_for_a_started_thread_and_clears_its_started_flag(void)
{
   BOOL started = TRUE; tODThreadHandle thread = ut_thread; ut_wait_calls = 0;
   utt_ODKrnlJoinThread(&thread, &started); UT_ASSERT_EQ_UINT(1, ut_wait_calls);
   UT_ASSERT(!started);
#ifdef ODPLAT_WIN32
   UT_ASSERT(thread == NULL);
#else
   UT_ASSERT(thread == ut_thread);
#endif
}
static const UTTestCase ut_cases[] = {
   {"not started", ignores_a_thread_that_was_not_started},
   {"join", waits_for_a_started_thread_and_clears_its_started_flag}
};
