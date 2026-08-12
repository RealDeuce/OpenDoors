#if defined(ODPLAT_DOS) || defined(ODPLAT_DOS32)
#define UT_CUSTOM_MOCK___get_errno_ptr
#define __get_errno_ptr utm___get_errno_ptr
#elif defined(ODPLAT_WIN32)
#define UT_CUSTOM_MOCK__errno
#define _errno utm__errno
#else
#define UT_CUSTOM_MOCK___error
#define __error utm___error
#endif

static int ut_errno_value;

#if defined(ODPLAT_DOS) || defined(ODPLAT_DOS32)
int *utm___get_errno_ptr(void) { return &ut_errno_value; }
#elif defined(ODPLAT_WIN32)
int *utm__errno(void) { return &ut_errno_value; }
#else
int *utm___error(void) { return &ut_errno_value; }
#endif
