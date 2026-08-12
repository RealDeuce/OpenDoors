#define UT_CUSTOM_MOCK_od_exit
#ifdef OD_MULTITHREADED
#define UT_CUSTOM_MOCK_ODSyncIsOwnerThread
#define UT_CUSTOM_MOCK_ODKrnlQueueShutdown
#endif

static unsigned ut_exit_calls;
static INT ut_exit_level;
static BOOL ut_exit_hangup;
#ifdef OD_MULTITHREADED
static BOOL ut_owner;
static unsigned ut_queue_calls;
static BYTE ut_queued_reason;
#endif

void ODCALL utm_od_exit(INT error_level, BOOL hangup)
{
   ++ut_exit_calls;
   ut_exit_level = error_level;
   ut_exit_hangup = hangup;
}
#ifdef OD_MULTITHREADED
BOOL utm_ODSyncIsOwnerThread(void) { return(ut_owner); }
void utm_ODKrnlQueueShutdown(BYTE reason)
{
   ++ut_queue_calls;
   ut_queued_reason = reason;
}
#endif

static void reset_shutdown(void)
{
   memset(od_control.od_errorlevel, 0, sizeof(od_control.od_errorlevel));
   ut_exit_calls = 0;
   ut_exit_level = -1;
   ut_exit_hangup = FALSE;
#ifdef OD_MULTITHREADED
   ut_owner = TRUE;
   ut_queue_calls = 0;
   ut_queued_reason = 0;
#endif
}

static void maps_default_reasons_and_hangup_policy(void)
{
   reset_shutdown();
   utt_ODKrnlForceOpenDoorsShutdown(ERRORLEVEL_HANGUP);
   UT_ASSERT_EQ_UINT(1, ut_exit_calls);
   UT_ASSERT_EQ_INT(ERRORLEVEL_HANGUP - 1, ut_exit_level);
   UT_ASSERT(ut_exit_hangup);
   UT_ASSERT_EQ_UINT(ERRORLEVEL_HANGUP - 1, btExitReason);

   reset_shutdown();
   utt_ODKrnlForceOpenDoorsShutdown(ERRORLEVEL_INACTIVITY);
   UT_ASSERT(ut_exit_hangup);

   reset_shutdown();
   utt_ODKrnlForceOpenDoorsShutdown(ERRORLEVEL_TIMEOUT);
   UT_ASSERT(!ut_exit_hangup);
   UT_ASSERT_EQ_INT(ERRORLEVEL_TIMEOUT - 1, ut_exit_level);
}

static void uses_the_client_errorlevel_table_when_enabled(void)
{
   reset_shutdown();
   od_control.od_errorlevel[0] = 1;
   od_control.od_errorlevel[ERRORLEVEL_TIMEOUT] = 47;
   utt_ODKrnlForceOpenDoorsShutdown(ERRORLEVEL_TIMEOUT);
   UT_ASSERT_EQ_UINT(1, ut_exit_calls);
   UT_ASSERT_EQ_INT(47, ut_exit_level);
   UT_ASSERT(!ut_exit_hangup);
}

#ifdef OD_MULTITHREADED
static void defers_shutdown_from_a_worker_thread(void)
{
   reset_shutdown();
   ut_owner = FALSE;
   utt_ODKrnlForceOpenDoorsShutdown(ERRORLEVEL_NOCARRIER);
   UT_ASSERT_EQ_UINT(1, ut_queue_calls);
   UT_ASSERT_EQ_UINT(ERRORLEVEL_NOCARRIER, ut_queued_reason);
   UT_ASSERT_EQ_UINT(0, ut_exit_calls);
}
#endif

static const UTTestCase ut_cases[] = {
   {"default mapping", maps_default_reasons_and_hangup_policy},
   {"client mapping", uses_the_client_errorlevel_table_when_enabled},
#ifdef OD_MULTITHREADED
   {"worker", defers_shutdown_from_a_worker_thread}
#endif
};
