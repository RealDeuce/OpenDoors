#define UT_CUSTOM_MOCK_ODKrnlDispatchPending
#ifdef ODPLAT_WIN32
#define UT_CUSTOM_MOCK_ODScrnPublish
#endif
static unsigned ut_publishes;
static BOOL ut_allow_callbacks;
static unsigned ut_pending_dispatches;
static char ut_call_order[3];
static unsigned ut_call_order_length;

void utm_ODKrnlDispatchPending(BOOL allow_callbacks)
{
   ++ut_pending_dispatches;
   ut_allow_callbacks = allow_callbacks;
   ut_call_order[ut_call_order_length++] = 'D';
}
#ifdef ODPLAT_WIN32
void utm_ODScrnPublish(void)
{
   ++ut_publishes;
   ut_call_order[ut_call_order_length++] = 'P';
}
#endif

static void returns_defensively_when_no_api_level_is_active(void)
{
   nAPILevel = 0;
   ut_publishes = 0;
   ut_pending_dispatches = 0;
   ut_call_order_length = 0;
   utt_ODSyncAPIExit();
   UT_ASSERT_EQ_UINT(0, ut_publishes);
   UT_ASSERT_EQ_UINT(0, ut_pending_dispatches);
}

static void defers_release_until_the_outermost_exit(void)
{
   nAPILevel = 2;
   ut_publishes = 0;
   ut_pending_dispatches = 0;
   ut_call_order_length = 0;
   utt_ODSyncAPIExit();
   UT_ASSERT_EQ_UINT(1, nAPILevel);
   UT_ASSERT_EQ_UINT(0, ut_publishes);
   UT_ASSERT_EQ_UINT(0, ut_pending_dispatches);

   utt_ODSyncAPIExit();
   UT_ASSERT_EQ_UINT(0, nAPILevel);
   UT_ASSERT_EQ_UINT(1, ut_pending_dispatches);
   UT_ASSERT_EQ_INT(TRUE, ut_allow_callbacks);
#ifdef ODPLAT_WIN32
   UT_ASSERT_EQ_UINT(1, ut_publishes);
   UT_ASSERT_EQ_UINT(2, ut_call_order_length);
   UT_ASSERT_EQ_INT('D', ut_call_order[0]);
   UT_ASSERT_EQ_INT('P', ut_call_order[1]);
#else
   UT_ASSERT_EQ_UINT(0, ut_publishes);
#endif
}

static const UTTestCase ut_cases[] = {
   {"inactive exit", returns_defensively_when_no_api_level_is_active},
   {"nested exit", defers_release_until_the_outermost_exit}
};
