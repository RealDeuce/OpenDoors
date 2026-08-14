#define UT_CUSTOM_MOCK_ODConsoleAvailable
#define UT_CUSTOM_MOCK_ODConsoleSetSize
#define UT_CUSTOM_MOCK_ODConsoleWrite
#define UT_CUSTOM_MOCK_ODScrnConsoleCopyPersonalityRow
#define UT_CUSTOM_MOCK_ODSessionScreenCell
#define UT_CUSTOM_MOCK_free
#define UT_CUSTOM_MOCK_malloc

static BYTE ut_allocation[128];
static BYTE ut_remote[12];
static BOOL ut_console_available;
static BOOL ut_allocate;
static BOOL ut_write_result;
static BOOL ut_local_composition;
static unsigned ut_personality_rows;
static unsigned ut_free_calls;

BOOL utm_ODConsoleAvailable(void) { return(ut_console_available); }
void utm_ODConsoleSetSize(INT requested_width, INT requested_height,
   INT *width, INT *height)
{
   *width = requested_width; *height = requested_height;
}
void *utm_malloc(size_t size)
{
   UT_ASSERT(size <= sizeof(ut_allocation));
   return(ut_allocate ? ut_allocation : NULL);
}
void utm_free(void *memory)
{
   UT_ASSERT_EQ_PTR(ut_allocation, memory); ++ut_free_calls;
}
void utm_ODScrnConsoleCopyPersonalityRow(BYTE *destination, INT width,
   INT source_row)
{
   INT column; (void)source_row; ++ut_personality_rows;
   for(column = 0; column < width; ++column)
   { destination[column * 2] = 'P'; destination[column * 2 + 1] = 0x17; }
}
BYTE *utm_ODSessionScreenCell(INT column, INT row)
{
   return(ut_remote + (row * 3 + column) * 2);
}
BOOL utm_ODConsoleWrite(const BYTE *cells, INT width, INT height,
   INT cursor_column, INT cursor_row, BOOL cursor_on)
{
   UT_ASSERT_EQ_INT(3, width); UT_ASSERT_EQ_INT(4, height);
   UT_ASSERT_EQ_INT('P', cells[0]);
   UT_ASSERT_EQ_INT(ut_local_composition ? 'P' : 'R', cells[6]);
   UT_ASSERT_EQ_INT('P', cells[18]);
   if(ut_local_composition)
   {
      UT_ASSERT_EQ_INT(2, cursor_column); UT_ASSERT_EQ_INT(3, cursor_row);
   }
   else
   {
      UT_ASSERT_EQ_INT(1, cursor_column); UT_ASSERT_EQ_INT(2, cursor_row);
   }
   UT_ASSERT(cursor_on);
   return(ut_write_result);
}

static void reset_fixture(void)
{
   memset(&od_control, 0, sizeof(od_control));
   memset(&SessionScreen, 0, sizeof(SessionScreen));
   memset(ut_remote, 0, sizeof(ut_remote));
   ut_remote[0] = ut_remote[6] = 'R';
   ut_console_available = ut_allocate = ut_write_result = TRUE;
   ut_local_composition = FALSE;
   ut_personality_rows = ut_free_calls = 0;
   od_control.user_screenwidth = 3; od_control.user_screen_length = 2;
   btOutputTop = 2; btOutputBottom = 24;
   bSessionScreenAvailable = TRUE;
   SessionScreen.nWidth = 3; SessionScreen.nHeight = 2;
   SessionScreen.nLeft = 0; SessionScreen.nTop = 0;
   SessionScreen.nCursorColumn = 1; SessionScreen.nCursorRow = 1;
   bCaretOn = TRUE;
}

static void composes_the_fixed_local_screen_without_a_session_buffer(void)
{
   reset_fixture();
   bSessionScreenAvailable = FALSE;
   btCursorColumn = 2; btCursorRow = 3;
   ut_local_composition = TRUE;
   UT_ASSERT(utt_ODScrnPublishConsole());
   UT_ASSERT_EQ_UINT(4, ut_personality_rows);
   UT_ASSERT_EQ_UINT(1, ut_free_calls);
}

static void unavailable_or_silent_console_needs_no_write(void)
{
   reset_fixture(); ut_console_available = FALSE;
   UT_ASSERT(utt_ODScrnPublishConsole()); UT_ASSERT_EQ_UINT(0, ut_free_calls);
   reset_fixture(); od_control.od_silent_mode = TRUE;
   UT_ASSERT(utt_ODScrnPublishConsole()); UT_ASSERT_EQ_UINT(0, ut_free_calls);
}

static void composes_remote_and_personality_rows(void)
{
   reset_fixture();
   UT_ASSERT(utt_ODScrnPublishConsole());
   UT_ASSERT_EQ_UINT(2, ut_personality_rows);
   UT_ASSERT_EQ_UINT(1, ut_free_calls);
}

static void reports_allocation_and_write_failures(void)
{
   reset_fixture(); ut_allocate = FALSE;
   UT_ASSERT(!utt_ODScrnPublishConsole()); UT_ASSERT_EQ_UINT(0, ut_free_calls);
   reset_fixture(); ut_write_result = FALSE;
   UT_ASSERT(!utt_ODScrnPublishConsole()); UT_ASSERT_EQ_UINT(1, ut_free_calls);
}

static const UTTestCase ut_cases[] = {
   {"inactive presentation", unavailable_or_silent_console_needs_no_write},
   {"composition", composes_remote_and_personality_rows},
   {"local composition",
      composes_the_fixed_local_screen_without_a_session_buffer},
   {"failures", reports_allocation_and_write_failures}
};
