#define UT_CUSTOM_MOCK_od_disp_str
#define UT_CUSTOM_MOCK_ODKrnlForceOpenDoorsShutdown
#ifdef OD_MULTITHREADED
#define UT_CUSTOM_MOCK_ODKrnlQueueTimeMessage
#define UT_CUSTOM_MOCK_ODKrnlQueueShutdown
#endif

static unsigned ut_display_calls;
static unsigned ut_callback_calls;
static unsigned ut_force_calls;
static BYTE ut_force_reason;
#ifdef OD_MULTITHREADED
static unsigned ut_queue_calls;
static unsigned ut_shutdown_queue_calls;
static BOOL ut_queue_result;
#endif

void ODCALL utm_od_disp_str(const char *message)
{
   ++ut_display_calls;
   UT_ASSERT(strcmp("notice", message) == 0);
}
void utm_ODKrnlForceOpenDoorsShutdown(BYTE reason)
{
   ++ut_force_calls;
   ut_force_reason = reason;
}
#ifdef ODPLAT_DOS32
static void ODCALL ut_message_callback(char *message)
#else
static void ut_message_callback(char *message)
#endif
{
   ++ut_callback_calls;
   UT_ASSERT(strcmp("notice", message) == 0);
}
#ifdef OD_MULTITHREADED
BOOL utm_ODKrnlQueueTimeMessage(char *message, BYTE reason)
{
   ++ut_queue_calls;
   UT_ASSERT(strcmp("notice", message) == 0);
   UT_ASSERT_EQ_UINT(ut_force_reason, reason);
   return(ut_queue_result);
}
void utm_ODKrnlQueueShutdown(BYTE reason)
{
   ++ut_shutdown_queue_calls;
   UT_ASSERT_EQ_UINT(ut_force_reason, reason);
}
#endif

static void reset_delivery(void)
{
   ut_display_calls = ut_callback_calls = ut_force_calls = 0;
   ut_force_reason = 0;
#ifdef OD_MULTITHREADED
   ut_queue_calls = ut_shutdown_queue_calls = 0;
   ut_queue_result = TRUE;
#endif
   bODInitialized = TRUE;
}

static void displays_or_calls_back_immediately(void)
{
   reset_delivery();
   od_control.od_time_msg_func = NULL;
   UT_ASSERT(!utt_ODKrnlDeliverTimeMessage("notice", 0, TRUE));
   UT_ASSERT_EQ_UINT(1, ut_display_calls);

   reset_delivery();
   od_control.od_time_msg_func = ut_message_callback;
   UT_ASSERT(!utt_ODKrnlDeliverTimeMessage("notice", 0, TRUE));
   UT_ASSERT_EQ_UINT(1, ut_callback_calls);
}

static void stops_when_the_callback_exits_or_a_reason_forces_shutdown(void)
{
   reset_delivery();
   od_control.od_time_msg_func = ut_message_callback;
   bODInitialized = FALSE;
   UT_ASSERT(utt_ODKrnlDeliverTimeMessage("notice", 0, TRUE));
   UT_ASSERT_EQ_UINT(0, ut_force_calls);

   reset_delivery();
   od_control.od_time_msg_func = NULL;
   UT_ASSERT(utt_ODKrnlDeliverTimeMessage("notice", 7, TRUE));
   UT_ASSERT_EQ_UINT(1, ut_force_calls);
   UT_ASSERT_EQ_UINT(7, ut_force_reason);
}

#ifdef OD_MULTITHREADED
static void defers_callbacks_and_preserves_a_failed_shutdown_request(void)
{
   reset_delivery();
   od_control.od_time_msg_func = ut_message_callback;
   ut_force_reason = 0;
   ut_queue_result = FALSE;
   UT_ASSERT(!utt_ODKrnlDeliverTimeMessage("notice", 0, FALSE));
   UT_ASSERT_EQ_UINT(1, ut_queue_calls);
   UT_ASSERT_EQ_UINT(0, ut_shutdown_queue_calls);

   reset_delivery();
   od_control.od_time_msg_func = ut_message_callback;
   ut_force_reason = 7;
   ut_queue_result = TRUE;
   UT_ASSERT(utt_ODKrnlDeliverTimeMessage("notice", 7, FALSE));
   UT_ASSERT_EQ_UINT(0, ut_shutdown_queue_calls);

   reset_delivery();
   od_control.od_time_msg_func = ut_message_callback;
   ut_force_reason = 7;
   ut_queue_result = FALSE;
   UT_ASSERT(utt_ODKrnlDeliverTimeMessage("notice", 7, FALSE));
   UT_ASSERT_EQ_UINT(1, ut_shutdown_queue_calls);

   reset_delivery();
   od_control.od_time_msg_func = NULL;
   UT_ASSERT(!utt_ODKrnlDeliverTimeMessage("notice", 0, FALSE));
   UT_ASSERT_EQ_UINT(0, ut_queue_calls);
   UT_ASSERT_EQ_UINT(1, ut_display_calls);
}
#endif

static const UTTestCase ut_cases[] = {
   {"immediate", displays_or_calls_back_immediately},
   {"stop", stops_when_the_callback_exits_or_a_reason_forces_shutdown},
#ifdef OD_MULTITHREADED
   {"defer", defers_callbacks_and_preserves_a_failed_shutdown_request}
#endif
};
