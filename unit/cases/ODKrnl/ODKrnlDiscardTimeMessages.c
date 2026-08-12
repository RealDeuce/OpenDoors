#define UT_CUSTOM_MOCK_free
static tODDeferredTimeMessage ut_first;
static tODDeferredTimeMessage ut_second;
static char ut_first_text[2];
static char ut_second_text[2];
static void *ut_freed[4];
static unsigned ut_free_calls;
void utm_free(void *memory)
{
   UT_ASSERT(ut_free_calls < 4); ut_freed[ut_free_calls++] = memory;
}
static void clears_an_already_empty_queue(void)
{
   pFirstTimeMessage = pLastTimeMessage = NULL; bTimeShutdownDeferred = TRUE;
   ut_free_calls = 0; utt_ODKrnlDiscardTimeMessages();
   UT_ASSERT_EQ_UINT(0, ut_free_calls); UT_ASSERT(pLastTimeMessage == NULL);
   UT_ASSERT(!bTimeShutdownDeferred);
}
static void releases_each_message_text_then_its_node(void)
{
   ut_first.pNext = &ut_second; ut_first.pszMessage = ut_first_text;
   ut_second.pNext = NULL; ut_second.pszMessage = ut_second_text;
   pFirstTimeMessage = &ut_first; pLastTimeMessage = &ut_second;
   bTimeShutdownDeferred = TRUE; ut_free_calls = 0;
   utt_ODKrnlDiscardTimeMessages();
   UT_ASSERT_EQ_UINT(4, ut_free_calls);
   UT_ASSERT_EQ_PTR(ut_first_text, ut_freed[0]); UT_ASSERT_EQ_PTR(&ut_first, ut_freed[1]);
   UT_ASSERT_EQ_PTR(ut_second_text, ut_freed[2]); UT_ASSERT_EQ_PTR(&ut_second, ut_freed[3]);
   UT_ASSERT(pFirstTimeMessage == NULL); UT_ASSERT(pLastTimeMessage == NULL);
   UT_ASSERT(!bTimeShutdownDeferred);
}
static const UTTestCase ut_cases[] = {
   {"empty", clears_an_already_empty_queue},
   {"messages", releases_each_message_text_then_its_node}
};
