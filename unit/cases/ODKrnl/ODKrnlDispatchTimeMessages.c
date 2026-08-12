#define UT_CUSTOM_MOCK_free
#define UT_CUSTOM_MOCK_ODKrnlDiscardTimeMessages
#define UT_CUSTOM_MOCK_ODKrnlForceOpenDoorsShutdown
static tODDeferredTimeMessage ut_first;
static tODDeferredTimeMessage ut_second;
static char ut_first_text[] = "first";
static char ut_second_text[] = "second";
static unsigned ut_callback_calls;
static unsigned ut_free_calls;
static unsigned ut_discard_calls;
static unsigned ut_shutdown_calls;
static BOOL ut_callback_deinitializes;
static BOOL ut_shutdown_deinitializes;
static void ut_callback(char *text)
{
   ++ut_callback_calls;
   UT_ASSERT(text == ut_first_text || text == ut_second_text);
   if(ut_callback_deinitializes) bODInitialized = FALSE;
}
void utm_free(void *memory)
{
   ++ut_free_calls;
   UT_ASSERT(memory == &ut_first || memory == &ut_second ||
      memory == ut_first_text || memory == ut_second_text);
}
void utm_ODKrnlDiscardTimeMessages(void)
{ ++ut_discard_calls; pFirstTimeMessage = pLastTimeMessage = NULL; }
void utm_ODKrnlForceOpenDoorsShutdown(BYTE reason)
{
   ++ut_shutdown_calls; UT_ASSERT_EQ_UINT(7, reason);
   if(ut_shutdown_deinitializes) bODInitialized = FALSE;
}
static void reset_dispatch(void)
{
   memset(&ut_first, 0, sizeof(ut_first)); memset(&ut_second, 0, sizeof(ut_second));
   ut_first.pszMessage = ut_first_text; ut_first.pfnCallback = ut_callback;
   ut_second.pszMessage = ut_second_text; ut_second.pfnCallback = ut_callback;
   pFirstTimeMessage = pLastTimeMessage = NULL; bTimeShutdownDeferred = FALSE;
   bODInitialized = TRUE; ut_callback_deinitializes = FALSE;
   ut_shutdown_deinitializes = FALSE; ut_callback_calls = ut_free_calls = 0;
   ut_discard_calls = ut_shutdown_calls = 0;
}
static void reports_an_empty_queue_without_work(void)
{
   reset_dispatch(); UT_ASSERT(!utt_ODKrnlDispatchTimeMessages());
   UT_ASSERT_EQ_UINT(0, ut_callback_calls);
}
static void dispatches_all_nonshutdown_messages_and_empties_the_queue(void)
{
   reset_dispatch(); ut_first.pNext = &ut_second;
   pFirstTimeMessage = &ut_first; pLastTimeMessage = &ut_second;
   UT_ASSERT(!utt_ODKrnlDispatchTimeMessages());
   UT_ASSERT_EQ_UINT(2, ut_callback_calls); UT_ASSERT_EQ_UINT(4, ut_free_calls);
   UT_ASSERT(pFirstTimeMessage == NULL); UT_ASSERT(pLastTimeMessage == NULL);
}
static void stops_and_discards_remainder_when_a_callback_deinitializes(void)
{
   reset_dispatch(); ut_first.pNext = &ut_second;
   pFirstTimeMessage = &ut_first; pLastTimeMessage = &ut_second;
   ut_callback_deinitializes = TRUE;
   UT_ASSERT(utt_ODKrnlDispatchTimeMessages());
   UT_ASSERT_EQ_UINT(1, ut_callback_calls); UT_ASSERT_EQ_UINT(1, ut_discard_calls);
}
static void performs_a_deferred_shutdown_and_clears_its_marker(void)
{
   reset_dispatch(); ut_first.btReasonForShutdown = 7;
   pFirstTimeMessage = pLastTimeMessage = &ut_first; bTimeShutdownDeferred = TRUE;
   UT_ASSERT(utt_ODKrnlDispatchTimeMessages());
   UT_ASSERT(!bTimeShutdownDeferred); UT_ASSERT_EQ_UINT(1, ut_shutdown_calls);
   UT_ASSERT_EQ_UINT(0, ut_discard_calls);
}
static void discards_remainder_when_shutdown_deinitializes(void)
{
   reset_dispatch(); ut_first.btReasonForShutdown = 7; ut_first.pNext = &ut_second;
   pFirstTimeMessage = &ut_first; pLastTimeMessage = &ut_second;
   bTimeShutdownDeferred = TRUE; ut_shutdown_deinitializes = TRUE;
   UT_ASSERT(utt_ODKrnlDispatchTimeMessages());
   UT_ASSERT_EQ_UINT(1, ut_shutdown_calls); UT_ASSERT_EQ_UINT(1, ut_discard_calls);
}
static const UTTestCase ut_cases[] = {
   {"empty", reports_an_empty_queue_without_work},
   {"normal", dispatches_all_nonshutdown_messages_and_empties_the_queue},
   {"callback exit", stops_and_discards_remainder_when_a_callback_deinitializes},
   {"shutdown", performs_a_deferred_shutdown_and_clears_its_marker},
   {"shutdown exit", discards_remainder_when_shutdown_deinitializes}
};
