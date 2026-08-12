#define UT_CUSTOM_MOCK_ODEmulateFromBuffer
#define UT_CUSTOM_MOCK_ODSyncAPIEntry
#define UT_CUSTOM_MOCK_ODSyncAPIExit
#define UT_CUSTOM_MOCK_od_init

static unsigned ut_init_calls;
static unsigned ut_entries;
static unsigned ut_exits;
static unsigned ut_emulate_calls;
static char ut_character;
static char ut_terminator;

void ODCALL utm_od_init(void)
{
   ++ut_init_calls;
   bODInitialized = TRUE;
}

void utm_ODSyncAPIEntry(void) { ++ut_entries; }
void utm_ODSyncAPIExit(void) { ++ut_exits; }

void utm_ODEmulateFromBuffer(const char *text, BOOL remote,
   BOOL session_echo)
{
   ++ut_emulate_calls;
   ut_character = text[0];
   ut_terminator = text[1];
   UT_ASSERT_EQ_INT(TRUE, remote);
   UT_ASSERT_EQ_INT(TRUE, session_echo);
}

static void reset_emulation(void)
{
   bODInitialized = TRUE;
   ut_init_calls = ut_entries = ut_exits = ut_emulate_calls = 0;
   ut_character = ut_terminator = 'x';
}

static void sends_one_terminated_character(void)
{
   reset_emulation();
   utt_od_emulate('Q');
   UT_ASSERT_EQ_UINT(0, ut_init_calls);
   UT_ASSERT_EQ_UINT(1, ut_entries);
   UT_ASSERT_EQ_UINT(1, ut_exits);
   UT_ASSERT_EQ_UINT(1, ut_emulate_calls);
   UT_ASSERT_EQ_INT('Q', ut_character);
   UT_ASSERT_EQ_INT('\0', ut_terminator);
}

static void initializes_when_required(void)
{
   reset_emulation();
   bODInitialized = FALSE;
   utt_od_emulate('R');
   UT_ASSERT_EQ_UINT(1, ut_init_calls);
   UT_ASSERT_EQ_INT('R', ut_character);
}

static const UTTestCase ut_cases[] = {
   {"character", sends_one_terminated_character},
   {"initialization", initializes_when_required}
};
