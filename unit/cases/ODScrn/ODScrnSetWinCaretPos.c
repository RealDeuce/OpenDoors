#define UT_CUSTOM_MOCK_SetCaretPos
static unsigned ut_set_calls;
WINBOOL WINAPI utm_SetCaretPos(int x, int y)
{
   ++ut_set_calls; UT_ASSERT_EQ_INT(40, x); UT_ASSERT_EQ_INT(93, y);
   return TRUE;
}
static void moves_the_caret_only_while_the_screen_has_focus(void)
{
   nFontCellWidth = 8; nFontCellHeight = 16;
   btCursorColumn = 2; btLeftBoundary = 3;
   btCursorRow = 4; btTopBoundary = 1; ut_set_calls = 0;
   bScreenHasFocus = FALSE;
   utt_ODScrnSetWinCaretPos(); UT_ASSERT_EQ_UINT(0, ut_set_calls);
   bScreenHasFocus = TRUE;
   utt_ODScrnSetWinCaretPos(); UT_ASSERT_EQ_UINT(1, ut_set_calls);
}
static const UTTestCase ut_cases[] = {
   {"focused caret", moves_the_caret_only_while_the_screen_has_focus}
};
