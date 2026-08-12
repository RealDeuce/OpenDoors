#ifdef ODPLAT_WIN32
#define UT_CUSTOM_MOCK_ODSyncControlReadLock
#define UT_CUSTOM_MOCK_ODSyncControlReadUnlock
#define UT_CUSTOM_MOCK_ODStringCopy

static unsigned ut_lock_depth;
static unsigned ut_copy_calls;

void utm_ODSyncControlReadLock(void)
{ UT_ASSERT_EQ_UINT(0, ut_lock_depth); ut_lock_depth = 1; }
void utm_ODSyncControlReadUnlock(void)
{ UT_ASSERT_EQ_UINT(1, ut_lock_depth); ut_lock_depth = 0; }
void utm_ODStringCopy(char *destination, const char *source, INT size)
{
   size_t length;
   UT_ASSERT_EQ_UINT(1, ut_lock_depth);
   UT_ASSERT_EQ_PTR(od_control.od_prog_name, source);
   ++ut_copy_calls;
   if(size <= 0) return;
   length = strlen(source);
   if(length >= (size_t)size) length = (size_t)size - 1;
   memcpy(destination, source, length); destination[length] = '\0';
}
static void copies_under_the_control_read_lock(void)
{
   char destination[5];
   strcpy(od_control.od_prog_name, "OpenDoors");
   ut_lock_depth = ut_copy_calls = 0;
   utt_ODFrameCopyProgramName(destination, sizeof(destination));
   UT_ASSERT(strcmp(destination, "Open") == 0);
   UT_ASSERT_EQ_UINT(1, ut_copy_calls); UT_ASSERT_EQ_UINT(0, ut_lock_depth);
}
static const UTTestCase ut_cases[] = {
   {"locked copy", copies_under_the_control_read_lock}
};
#endif
