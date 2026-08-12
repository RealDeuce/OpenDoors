#ifdef ODPLAT_WIN32
#define UT_CUSTOM_MOCK_ODSyncControlReadLock
#define UT_CUSTOM_MOCK_ODSyncControlReadUnlock
#define UT_CUSTOM_MOCK_ODStringCopy
#define UT_CUSTOM_MOCK_strcmp
#define UT_CUSTOM_MOCK_strlen
#define UT_CUSTOM_MOCK_GetDlgItem
#define UT_CUSTOM_MOCK_SetWindowTextA
#define UT_CUSTOM_MOCK_ODFrameCenterWindowInParent
#define UT_CUSTOM_MOCK_EndDialog
static unsigned ut_lock_depth,ut_text_calls,ut_center_calls,ut_end_calls;
static char ut_text[3][80];
static size_t ut_length(const char *text){size_t n=0;while(text[n])++n;return(n);}
static BOOL ut_equal(const char *a,const char *b){while(*a&&*a==*b){++a;++b;}return(*a==*b);}
void utm_ODSyncControlReadLock(void){UT_ASSERT_EQ_UINT(0,ut_lock_depth);ut_lock_depth=1;}
void utm_ODSyncControlReadUnlock(void){UT_ASSERT_EQ_UINT(1,ut_lock_depth);ut_lock_depth=0;}
void utm_ODStringCopy(char *destination,const char *source,INT size){size_t n=ut_length(source);UT_ASSERT_EQ_UINT(1,ut_lock_depth);if(n>=(size_t)size)n=(size_t)size-1;memcpy(destination,source,n);destination[n]='\0';}
int utm_strcmp(const char *a,const char *b){while(*a&&*a==*b){++a;++b;}return((unsigned char)*a-(unsigned char)*b);}
size_t utm_strlen(const char *text){return(ut_length(text));}
HWND WINAPI utm_GetDlgItem(HWND dialog,int id){(void)dialog;return((HWND)(UINT_PTR)id);}
WINBOOL WINAPI utm_SetWindowTextA(HWND window,LPCSTR text){unsigned index=ut_text_calls++;UT_ASSERT(index<3);UT_ASSERT(window!=NULL);strcpy(ut_text[index],text);return(TRUE);}
void utm_ODFrameCenterWindowInParent(HWND window){UT_ASSERT_EQ_PTR((HWND)(UINT_PTR)1,window);++ut_center_calls;}
WINBOOL WINAPI utm_EndDialog(HWND dialog,INT_PTR result){UT_ASSERT_EQ_PTR((HWND)(UINT_PTR)1,dialog);UT_ASSERT_EQ_INT(IDOK,result);++ut_end_calls;return(TRUE);}
static void reset_about(void){ut_lock_depth=ut_text_calls=ut_center_calls=ut_end_calls=0;}
static void fills_only_programmer_supplied_fields(void)
{reset_about();strcpy(od_control.od_prog_name,"Door");strcpy(od_control.od_prog_copyright,"Copyright");strcpy(od_control.od_prog_version,"1.2");
 UT_ASSERT(utt_ODFrameAboutDlgProc((HWND)(UINT_PTR)1,WM_INITDIALOG,0,0));UT_ASSERT_EQ_UINT(3,ut_text_calls);UT_ASSERT(ut_equal("Door",ut_text[0]));UT_ASSERT_EQ_UINT(1,ut_center_calls);
 reset_about();strcpy(od_control.od_prog_name,OD_VER_SHORTNAME);od_control.od_prog_copyright[0]='\0';od_control.od_prog_version[0]='\0';
 UT_ASSERT(utt_ODFrameAboutDlgProc((HWND)(UINT_PTR)1,WM_INITDIALOG,0,0));UT_ASSERT_EQ_UINT(0,ut_text_calls);UT_ASSERT_EQ_UINT(1,ut_center_calls);}
static void handles_close_commands_and_rejects_other_messages(void)
{reset_about();UT_ASSERT(utt_ODFrameAboutDlgProc((HWND)(UINT_PTR)1,WM_COMMAND,IDOK,0));
 UT_ASSERT(utt_ODFrameAboutDlgProc((HWND)(UINT_PTR)1,WM_COMMAND,IDCANCEL,0));UT_ASSERT_EQ_UINT(2,ut_end_calls);
 UT_ASSERT(utt_ODFrameAboutDlgProc((HWND)(UINT_PTR)1,WM_COMMAND,99,0));UT_ASSERT_EQ_UINT(2,ut_end_calls);
 UT_ASSERT(!utt_ODFrameAboutDlgProc((HWND)(UINT_PTR)1,WM_PAINT,0,0));}
static const UTTestCase ut_cases[]={{"dialog initialization",fills_only_programmer_supplied_fields},{"dialog commands",handles_close_commands_and_rejects_other_messages}};
#endif
