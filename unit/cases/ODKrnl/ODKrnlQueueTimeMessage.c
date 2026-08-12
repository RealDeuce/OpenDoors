#define UT_CUSTOM_MOCK_strlen
#define UT_CUSTOM_MOCK_malloc
#define UT_CUSTOM_MOCK_free
#define UT_CUSTOM_MOCK_memcpy
static tODDeferredTimeMessage ut_message;
static tODDeferredTimeMessage ut_existing;
static char ut_text[32];
static unsigned ut_malloc_call;
static unsigned ut_fail_malloc_call;
static unsigned ut_free_calls;
static void ut_callback(char *text) { (void)text; }
size_t utm_strlen(const char *text)
{ size_t length = 0; while(text[length] != '\0') ++length; return length; }
void *utm_malloc(size_t size)
{
   ++ut_malloc_call;
   if(ut_malloc_call == ut_fail_malloc_call) return NULL;
   if(ut_malloc_call == 1)
   { UT_ASSERT_EQ_UINT(sizeof(ut_message), size); return &ut_message; }
   UT_ASSERT_EQ_UINT(6, size); return ut_text;
}
void utm_free(void *memory)
{ ++ut_free_calls; UT_ASSERT_EQ_PTR(&ut_message, memory); }
void *utm_memcpy(void *destination, const void *source, size_t size)
{
   size_t index; UT_ASSERT_EQ_PTR(ut_text, destination); UT_ASSERT_EQ_UINT(6, size);
   for(index = 0; index < size; ++index)
      ((char *)destination)[index] = ((const char *)source)[index];
   return destination;
}
static void reset_queue(void)
{
   memset(&ut_message, 0, sizeof(ut_message)); memset(&ut_existing, 0, sizeof(ut_existing));
   memset(ut_text, 0, sizeof(ut_text)); ut_malloc_call = ut_fail_malloc_call = 0;
   ut_free_calls = 0; pFirstTimeMessage = pLastTimeMessage = NULL;
   bTimeShutdownDeferred = FALSE; od_control.od_time_msg_func = ut_callback;
}
static void reports_each_allocation_failure(void)
{
   reset_queue(); ut_fail_malloc_call = 1;
   UT_ASSERT(!utt_ODKrnlQueueTimeMessage("hello", 0));
   UT_ASSERT_EQ_UINT(0, ut_free_calls);
   reset_queue(); ut_fail_malloc_call = 2;
   UT_ASSERT(!utt_ODKrnlQueueTimeMessage("hello", 0));
   UT_ASSERT_EQ_UINT(1, ut_free_calls);
}
static void creates_the_first_nonshutdown_message(void)
{
   reset_queue(); UT_ASSERT(utt_ODKrnlQueueTimeMessage("hello", 0));
   UT_ASSERT(pFirstTimeMessage == &ut_message); UT_ASSERT(pLastTimeMessage == &ut_message);
   UT_ASSERT(ut_message.pNext == NULL); UT_ASSERT_EQ_PTR(ut_text, ut_message.pszMessage);
   UT_ASSERT(strcmp("hello", ut_text) == 0); UT_ASSERT(ut_message.pfnCallback == ut_callback);
   UT_ASSERT_EQ_UINT(0, ut_message.btReasonForShutdown);
   UT_ASSERT(!bTimeShutdownDeferred);
}
static void appends_a_shutdown_message_and_marks_it_deferred(void)
{
   reset_queue(); pFirstTimeMessage = pLastTimeMessage = &ut_existing;
   UT_ASSERT(utt_ODKrnlQueueTimeMessage("hello", 7));
   UT_ASSERT(ut_existing.pNext == &ut_message); UT_ASSERT(pLastTimeMessage == &ut_message);
   UT_ASSERT_EQ_UINT(7, ut_message.btReasonForShutdown);
   UT_ASSERT(bTimeShutdownDeferred);
}
static const UTTestCase ut_cases[] = {
   {"allocation", reports_each_allocation_failure},
   {"first", creates_the_first_nonshutdown_message},
   {"append shutdown", appends_a_shutdown_message_and_marks_it_deferred}
};
