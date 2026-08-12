#ifdef ODPLAT_DOS32
#define UT_CUSTOM_MOCK_memset
#define UT_CUSTOM_MOCK_int386
static unsigned ut_int_calls;
void *utm_memset(void *memory, int value, size_t count)
{
   BYTE *bytes;
   size_t index;
   bytes = (BYTE *)memory;
   for(index = 0; index < count; ++index) bytes[index] = (BYTE)value;
   return memory;
}
int utm_int386(int interrupt_number, const union REGS *input,
   union REGS *output)
{
   ++ut_int_calls; UT_ASSERT_EQ_INT(0x10, interrupt_number);
   UT_ASSERT(input == (const union REGS *)output);
   UT_ASSERT_EQ_UINT(0x02, input->h.ah);
   UT_ASSERT_EQ_UINT(0, input->h.bh);
   UT_ASSERT_EQ_UINT(7, input->h.dh); UT_ASSERT_EQ_UINT(9, input->h.dl);
   return 0;
}
#endif

#ifdef ODPLAT_WIN32
#define UT_CUSTOM_MOCK_PostMessageA
static unsigned ut_post_calls;
BOOL WINAPI utm_PostMessageA(HWND window, UINT message, WPARAM wparam,
   LPARAM lparam)
{
   ++ut_post_calls; UT_ASSERT(window == hwndScreenWindow);
   UT_ASSERT_EQ_UINT(WM_MOVE_YOUR_CARET, message);
   UT_ASSERT_EQ_UINT(0, wparam); UT_ASSERT_EQ_INT(0, lparam);
   return TRUE;
}
#endif

#if defined(ODPLAT_DOS) || defined(ODPLAT_DOS32)
static void does_not_move_a_disabled_caret(void)
{
   bCaretOn = FALSE;
#ifdef ODPLAT_DOS32
   ut_int_calls = 0;
#endif
   utt_ODScrnUpdateCaretPos();
#ifdef ODPLAT_DOS32
   UT_ASSERT_EQ_UINT(0, ut_int_calls);
#endif
}

static void moves_the_caret_to_the_boundary_relative_position(void)
{
   btDisplayPage = 0; btTopBoundary = 2; btLeftBoundary = 3;
   btCursorRow = 5; btCursorColumn = 6; bCaretOn = TRUE;
#ifdef ODPLAT_DOS32
   ut_int_calls = 0;
#endif
   utt_ODScrnUpdateCaretPos();
#ifdef ODPLAT_DOS32
   UT_ASSERT_EQ_UINT(1, ut_int_calls);
#else
   {
      union REGS registers;
      registers.h.ah = 0x03; registers.h.bh = 0;
      int86(0x10, &registers, &registers);
      UT_ASSERT_EQ_UINT(7, registers.h.dh); UT_ASSERT_EQ_UINT(9, registers.h.dl);
   }
#endif
}
#elif defined(ODPLAT_WIN32)
static void posts_only_when_the_screen_window_exists(void)
{
   ut_post_calls = 0; hwndScreenWindow = NULL;
   utt_ODScrnUpdateCaretPos(); UT_ASSERT_EQ_UINT(0, ut_post_calls);
   hwndScreenWindow = (HWND)1;
   utt_ODScrnUpdateCaretPos(); UT_ASSERT_EQ_UINT(1, ut_post_calls);
}
#else
static void is_a_noop_without_a_hardware_caret(void)
{
   utt_ODScrnUpdateCaretPos();
   UT_ASSERT(TRUE);
}
#endif

static const UTTestCase ut_cases[] = {
#if defined(ODPLAT_DOS) || defined(ODPLAT_DOS32)
   {"caret off", does_not_move_a_disabled_caret},
   {"caret position", moves_the_caret_to_the_boundary_relative_position}
#elif defined(ODPLAT_WIN32)
   {"window message", posts_only_when_the_screen_window_exists}
#else
   {"no hardware caret", is_a_noop_without_a_hardware_caret}
#endif
};
