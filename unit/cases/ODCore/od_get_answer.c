#define UT_CUSTOM_MOCK_ODSyncAPIEntry
#define UT_CUSTOM_MOCK_ODSyncAPIExit
#define UT_CUSTOM_MOCK_od_get_key
#define UT_CUSTOM_MOCK_od_init
#define UT_CUSTOM_MOCK_tolower

static char ut_keys[4];
static unsigned ut_key_count;
static unsigned ut_key_index;
static unsigned ut_shutdown_key;
static unsigned ut_init_calls;
static unsigned ut_entries;
static unsigned ut_exits;

void ODCALL utm_od_init(void)
{
   ++ut_init_calls;
   bODInitialized = TRUE;
}

void utm_ODSyncAPIEntry(void) { ++ut_entries; }
void utm_ODSyncAPIExit(void) { ++ut_exits; }

char ODCALL utm_od_get_key(BOOL wait)
{
   UT_ASSERT_EQ_INT(TRUE, wait);
   UT_ASSERT(ut_key_index < ut_key_count);
   ++ut_key_index;
   if(ut_key_index == ut_shutdown_key) bODInitialized = FALSE;
   return ut_keys[ut_key_index - 1];
}

int utm_tolower(int value)
{
   if(value >= 'A' && value <= 'Z') return value - 'A' + 'a';
   return value;
}

static void reset_answer(void)
{
   bODInitialized = TRUE;
   ut_key_count = 0;
   ut_key_index = 0;
   ut_shutdown_key = 0;
   ut_init_calls = 0;
   ut_entries = 0;
   ut_exits = 0;
}

static void ignores_invalid_keys_and_returns_the_option_spelling(void)
{
   reset_answer();
   bODInitialized = FALSE;
   ut_keys[0] = 'x';
   ut_keys[1] = 'y';
   ut_key_count = 2;
   UT_ASSERT_EQ_INT('Y', utt_od_get_answer("nY"));
   UT_ASSERT_EQ_UINT(1, ut_init_calls);
   UT_ASSERT_EQ_UINT(1, ut_entries);
   UT_ASSERT_EQ_UINT(1, ut_exits);
   UT_ASSERT_EQ_UINT(2, ut_key_index);
}

static void returns_nul_if_the_session_ends_while_waiting(void)
{
   reset_answer();
   ut_keys[0] = 'n';
   ut_key_count = 1;
   ut_shutdown_key = 1;
   UT_ASSERT_EQ_INT('\0', utt_od_get_answer("nY"));
   UT_ASSERT_EQ_UINT(1, ut_entries);
   UT_ASSERT_EQ_UINT(1, ut_exits);
}

static const UTTestCase ut_cases[] = {
   {"matching answer", ignores_invalid_keys_and_returns_the_option_spelling},
   {"session shutdown", returns_nul_if_the_session_ends_while_waiting}
};
