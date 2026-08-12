#ifdef ODPLAT_WIN32
#define UT_CUSTOM_MOCK_ODSyncControlReadLock
#define UT_CUSTOM_MOCK_ODSyncControlReadUnlock
#define UT_CUSTOM_MOCK_sprintf
#define UT_CUSTOM_MOCK_SetWindowTextA
#define UT_CUSTOM_MOCK_GetWindowRect
#define UT_CUSTOM_MOCK_InvalidateRect
#define UT_CUSTOM_MOCK_SendMessageA
static unsigned ut_lock_depth,ut_text_calls;
void utm_ODSyncControlReadLock(void){UT_ASSERT_EQ_UINT(0,ut_lock_depth);ut_lock_depth=1;}
void utm_ODSyncControlReadUnlock(void){UT_ASSERT_EQ_UINT(1,ut_lock_depth);ut_lock_depth=0;}
int utm_sprintf(char *destination,const char *format,...)
{ UT_ASSERT(strcmp(format,"%d min.")==0); strcpy(destination,"42 min."); return(7); }
WINBOOL WINAPI utm_SetWindowTextA(HWND window,LPCSTR text)
{UT_ASSERT_EQ_PTR((HWND)(UINT_PTR)1,window);UT_ASSERT(strcmp(text,"42 min.")==0);++ut_text_calls;return(TRUE);}
WINBOOL WINAPI utm_GetWindowRect(HWND window,LPRECT rectangle)
{UT_ASSERT_EQ_PTR((HWND)(UINT_PTR)1,window);rectangle->left=1;rectangle->right=20;rectangle->top=2;rectangle->bottom=10;return(TRUE);}
WINBOOL WINAPI utm_InvalidateRect(HWND window,const RECT *rectangle,WINBOOL erase)
{UT_ASSERT_EQ_PTR((HWND)(UINT_PTR)1,window);UT_ASSERT_EQ_INT(19,rectangle->right);UT_ASSERT(erase);return(TRUE);}
LRESULT WINAPI utm_SendMessageA(HWND window,UINT message,WPARAM first,LPARAM second)
{UT_ASSERT_EQ_PTR((HWND)(UINT_PTR)2,window);UT_ASSERT_EQ_UINT(UDM_SETPOS,message);UT_ASSERT_EQ_UINT(0,first);
 UT_ASSERT_EQ_INT(42,LOWORD(second));return(0);}
static void ignores_an_absent_edit_control(void)
{tODFrameWindowInfo info;memset(&info,0,sizeof(info));ut_text_calls=0;utt_ODFrameUpdateTimeLeft(&info);UT_ASSERT_EQ_UINT(0,ut_text_calls);}
static void updates_text_redraw_and_spinner(void)
{tODFrameWindowInfo info;memset(&info,0,sizeof(info));info.hwndTimeEdit=(HWND)(UINT_PTR)1;info.hwndTimeUpDown=(HWND)(UINT_PTR)2;
 od_control.user_timelimit=42;ut_lock_depth=ut_text_calls=0;utt_ODFrameUpdateTimeLeft(&info);UT_ASSERT_EQ_UINT(1,ut_text_calls);UT_ASSERT_EQ_UINT(0,ut_lock_depth);}
static const UTTestCase ut_cases[]={{"no edit",ignores_an_absent_edit_control},{"time update",updates_text_redraw_and_spinner}};
#endif
