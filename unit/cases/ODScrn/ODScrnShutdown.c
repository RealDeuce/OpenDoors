#define UT_CUSTOM_MOCK_free
static unsigned ut_free_calls;
static void *ut_freed;
void utm_free(void *memory) { ++ut_free_calls; ut_freed = memory; }
static void releases_only_owned_screen_storage(void)
{
   static BYTE memory[2];
   ut_free_calls = 0; ut_freed = NULL;
#ifdef ODPLAT_WIN32
   pScrnBuffer = NULL; utt_ODScrnShutdown();
   UT_ASSERT_EQ_UINT(0, ut_free_calls);
   pScrnBuffer = memory; utt_ODScrnShutdown();
   UT_ASSERT_EQ_UINT(1, ut_free_calls); UT_ASSERT_EQ_PTR(memory, ut_freed);
   UT_ASSERT_EQ_PTR(NULL, pScrnBuffer);
#else
#ifdef ODPLAT_NIX
   pAllocatedBufferMemory = memory; pScrnBuffer = memory;
   utt_ODScrnShutdown();
   UT_ASSERT_EQ_UINT(1, ut_free_calls); UT_ASSERT_EQ_PTR(memory, ut_freed);
   UT_ASSERT_EQ_PTR(NULL, pAllocatedBufferMemory); UT_ASSERT_EQ_PTR(NULL, pScrnBuffer);
#else
   od_control.od_silent_mode = FALSE; pAllocatedBufferMemory = memory;
   utt_ODScrnShutdown(); UT_ASSERT_EQ_UINT(0, ut_free_calls);
   od_control.od_silent_mode = TRUE; pAllocatedBufferMemory = NULL;
   utt_ODScrnShutdown(); UT_ASSERT_EQ_UINT(0, ut_free_calls);
   pAllocatedBufferMemory = memory; pScrnBuffer = memory;
   utt_ODScrnShutdown(); UT_ASSERT_EQ_UINT(1, ut_free_calls);
   UT_ASSERT_EQ_PTR(NULL, pAllocatedBufferMemory); UT_ASSERT_EQ_PTR(NULL, pScrnBuffer);
#endif
#endif
}
static const UTTestCase ut_cases[] = {{"owned storage", releases_only_owned_screen_storage}};
