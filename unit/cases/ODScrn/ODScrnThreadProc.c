#include <setjmp.h>

#define UT_CUSTOM_MOCK_GetCurrentThreadId
#define UT_CUSTOM_MOCK_PeekMessageA
#define UT_CUSTOM_MOCK_free
#define UT_CUSTOM_MOCK_ODScrnCreateWin
#define UT_CUSTOM_MOCK_SetEvent
#define UT_CUSTOM_MOCK_GetStockObject
#define UT_CUSTOM_MOCK_ODScrnSetCurrentFont
#define UT_CUSTOM_MOCK_DialogBoxParamA
#define UT_CUSTOM_MOCK_ODInitLoginDlgProc
#define UT_CUSTOM_MOCK_ODSyncControlReadLock
#define UT_CUSTOM_MOCK_ODSyncControlReadUnlock
#define UT_CUSTOM_MOCK_PostMessageA
#define UT_CUSTOM_MOCK_ShowWindow
#define UT_CUSTOM_MOCK_ODScrnMessageLoop
#define UT_CUSTOM_MOCK_IsWindow
#define UT_CUSTOM_MOCK_DestroyWindow
#define UT_CUSTOM_MOCK_exit

static tODScrnThreadInfo ut_info;
static HWND ut_frame = (HWND)1;
static HANDLE ut_instance = (HANDLE)2;
static HWND ut_screen = (HWND)3;
static HANDLE ut_event = (HANDLE)4;
static HFONT ut_font = (HFONT)5;
static BOOL ut_create_fails;
static BOOL ut_dialog_cancels;
static BOOL ut_window_exists;
static unsigned ut_free_calls;
static unsigned ut_event_calls;
static unsigned ut_font_calls;
static unsigned ut_dialog_calls;
static unsigned ut_post_calls;
static unsigned ut_show_calls;
static int ut_show_commands[2];
static unsigned ut_loop_calls;
static unsigned ut_destroy_calls;
static unsigned ut_lock_calls;
static unsigned ut_unlock_calls;
static unsigned ut_exit_calls;
static BOOL ut_expect_exit;
static jmp_buf ut_exit_target;

DWORD WINAPI utm_GetCurrentThreadId(void) { return 41; }

WINBOOL WINAPI utm_PeekMessageA(LPMSG message, HWND window, UINT first,
   UINT last, UINT remove)
{
   UT_ASSERT_NOT_NULL(message); UT_ASSERT(window == NULL);
   UT_ASSERT_EQ_UINT(WM_USER, first); UT_ASSERT_EQ_UINT(WM_USER, last);
   UT_ASSERT_EQ_UINT(PM_NOREMOVE, remove); return FALSE;
}

void utm_free(void *memory)
{
   ++ut_free_calls; UT_ASSERT_EQ_PTR(&ut_info, memory);
}

HWND utm_ODScrnCreateWin(HWND frame, HANDLE instance)
{
   UT_ASSERT(frame == ut_frame); UT_ASSERT(instance == ut_instance);
   return ut_create_fails ? NULL : ut_screen;
}

WINBOOL WINAPI utm_SetEvent(HANDLE event)
{
   ++ut_event_calls; UT_ASSERT(event == ut_event); return TRUE;
}

HGDIOBJ WINAPI utm_GetStockObject(int object)
{
   UT_ASSERT_EQ_INT(OEM_FIXED_FONT, object); return ut_font;
}

void utm_ODScrnSetCurrentFont(HWND screen, HFONT font)
{
   ++ut_font_calls; UT_ASSERT(screen == ut_screen); UT_ASSERT(font == ut_font);
}

INT_PTR CALLBACK utm_ODInitLoginDlgProc(HWND dialog, UINT message,
   WPARAM wparam, LPARAM lparam)
{
   (void)dialog; (void)message; (void)wparam; (void)lparam;
   ut_unexpected_mock(3, "ODInitLoginDlgProc"); return FALSE;
}

INT_PTR WINAPI utm_DialogBoxParamA(HINSTANCE instance, LPCSTR template_name,
   HWND owner, DLGPROC procedure, LPARAM parameter)
{
   ++ut_dialog_calls; UT_ASSERT(instance == ut_instance);
   UT_ASSERT(template_name == MAKEINTRESOURCE(IDD_LOGIN));
   UT_ASSERT(owner == ut_frame); UT_ASSERT(procedure == utm_ODInitLoginDlgProc);
   UT_ASSERT_EQ_INT(0, parameter);
   return ut_dialog_cancels ? IDCANCEL : IDOK;
}

void utm_ODSyncControlReadLock(void) { ++ut_lock_calls; }
void utm_ODSyncControlReadUnlock(void) { ++ut_unlock_calls; }

WINBOOL WINAPI utm_PostMessageA(HWND window, UINT message, WPARAM wparam,
   LPARAM lparam)
{
   ++ut_post_calls; UT_ASSERT(window == ut_screen);
   UT_ASSERT_EQ_UINT(WM_SETFOCUS, message); UT_ASSERT_EQ_UINT(0, wparam);
   UT_ASSERT_EQ_INT(0, lparam); return TRUE;
}

WINBOOL WINAPI utm_ShowWindow(HWND window, int command)
{
   UT_ASSERT(ut_show_calls < 2);
   UT_ASSERT(window == (ut_show_calls == 0 ? ut_frame : ut_screen));
   ut_show_commands[ut_show_calls++] = command; return TRUE;
}

void utm_ODScrnMessageLoop(HANDLE instance, HWND screen)
{
   ++ut_loop_calls; UT_ASSERT(instance == ut_instance); UT_ASSERT(screen == ut_screen);
}

WINBOOL WINAPI utm_IsWindow(HWND window)
{
   UT_ASSERT(window == ut_screen); return ut_window_exists;
}

WINBOOL WINAPI utm_DestroyWindow(HWND window)
{
   ++ut_destroy_calls; UT_ASSERT(window == ut_screen); return TRUE;
}

void utm_exit(int status)
{
   ++ut_exit_calls; UT_ASSERT(ut_expect_exit); UT_ASSERT_EQ_INT(73, status);
   longjmp(ut_exit_target, 1);
}

static void reset_thread(void)
{
   memset(&ut_info, 0, sizeof(ut_info)); memset(&od_control, 0, sizeof(od_control));
   ut_info.hwndFrame = ut_frame; ut_info.hInstance = ut_instance;
   ut_create_fails = ut_dialog_cancels = ut_window_exists = FALSE;
   ut_free_calls = ut_event_calls = ut_font_calls = ut_dialog_calls = 0;
   ut_post_calls = ut_show_calls = ut_loop_calls = ut_destroy_calls = 0;
   ut_lock_calls = ut_unlock_calls = ut_exit_calls = 0;
   ut_expect_exit = FALSE; bPromptForUserName = FALSE;
   hScreenStartedEvent = ut_event; hwndScreenWindow = NULL;
   dwScreenThreadID = 0; ScreenStartResult = kODRCGeneralFailure;
}

static void publishes_failure_when_the_screen_window_cannot_be_created(void)
{
   reset_thread(); ut_create_fails = TRUE;
   UT_ASSERT_EQ_UINT(FALSE, utt_ODScrnThreadProc(&ut_info));
   UT_ASSERT_EQ_UINT(1, ut_free_calls); UT_ASSERT_EQ_UINT(1, ut_event_calls);
   UT_ASSERT_EQ_INT(kODRCGeneralFailure, ScreenStartResult);
   UT_ASSERT_EQ_UINT(0, dwScreenThreadID); UT_ASSERT_EQ_UINT(0, ut_show_calls);
}

static void run_success_with_command(int command, int expected_frame_command,
   BOOL existing_window)
{
   reset_thread(); od_control.od_cmd_show = command;
   ut_window_exists = existing_window;
   UT_ASSERT_EQ_UINT(TRUE, utt_ODScrnThreadProc(&ut_info));
   UT_ASSERT_EQ_INT(kODRCSuccess, ScreenStartResult);
   UT_ASSERT_EQ_UINT(1, ut_event_calls); UT_ASSERT_EQ_UINT(1, ut_font_calls);
   UT_ASSERT_EQ_UINT(2, ut_show_calls);
   UT_ASSERT_EQ_INT(expected_frame_command, ut_show_commands[0]);
   UT_ASSERT_EQ_INT(SW_SHOW, ut_show_commands[1]);
   UT_ASSERT_EQ_UINT(1, ut_loop_calls);
   UT_ASSERT_EQ_UINT(existing_window ? 1 : 0, ut_destroy_calls);
   UT_ASSERT(hwndScreenWindow == NULL); UT_ASSERT_EQ_UINT(0, dwScreenThreadID);
}

static void chooses_minimized_display_for_each_minimized_command(void)
{
   run_success_with_command(SW_MINIMIZE, SW_SHOWMINNOACTIVE, FALSE);
   run_success_with_command(SW_SHOWMINIMIZED, SW_SHOWMINNOACTIVE, FALSE);
   run_success_with_command(SW_SHOWMINNOACTIVE, SW_SHOWMINNOACTIVE, FALSE);
}

static void restores_other_frame_states_and_destroys_an_existing_window(void)
{
   run_success_with_command(SW_SHOW, SW_RESTORE, TRUE);
}

static void accepted_login_focuses_the_screen_before_showing_it(void)
{
   reset_thread(); bPromptForUserName = TRUE; od_control.od_cmd_show = SW_SHOW;
   UT_ASSERT_EQ_UINT(TRUE, utt_ODScrnThreadProc(&ut_info));
   UT_ASSERT_EQ_UINT(1, ut_dialog_calls); UT_ASSERT_EQ_UINT(1, ut_post_calls);
}

static void cancelled_login_exits_with_the_configured_errorlevel(void)
{
   reset_thread(); bPromptForUserName = TRUE; ut_dialog_cancels = TRUE;
   od_control.od_errorlevel[1] = 73; ut_expect_exit = TRUE;
   if(setjmp(ut_exit_target) == 0)
   {
      (void)utt_ODScrnThreadProc(&ut_info);
      UT_ASSERT(FALSE);
   }
   UT_ASSERT_EQ_UINT(1, ut_exit_calls); UT_ASSERT_EQ_UINT(1, ut_lock_calls);
   UT_ASSERT_EQ_UINT(1, ut_unlock_calls); UT_ASSERT_EQ_UINT(0, ut_show_calls);
}

static const UTTestCase ut_cases[] = {
   {"create failure", publishes_failure_when_the_screen_window_cannot_be_created},
   {"minimized", chooses_minimized_display_for_each_minimized_command},
   {"restored", restores_other_frame_states_and_destroys_an_existing_window},
   {"login", accepted_login_focuses_the_screen_before_showing_it},
   {"cancel", cancelled_login_exits_with_the_configured_errorlevel}
};
