#define UT_CUSTOM_MOCK_ODSyncControlReadLock
#define UT_CUSTOM_MOCK_ODSyncControlReadUnlock
#define UT_CUSTOM_MOCK_ODKrnlHandleReceivedChar
static char ut_characters[3];
static BOOL ut_remote[3];
static unsigned ut_calls;
void utm_ODSyncControlReadLock(void) { }
void utm_ODSyncControlReadUnlock(void) { }
void utm_ODKrnlHandleReceivedChar(char character, BOOL remote)
{
   UT_ASSERT(ut_calls < 3); ut_characters[ut_calls] = character;
   ut_remote[ut_calls++] = remote;
}
static void ignores_local_keys_when_local_input_is_disabled(void)
{
   memset(&od_control, 0, sizeof(od_control));
   od_control.od_disable = DIS_LOCAL_INPUT; ut_calls = 0;
   utt_ODKrnlHandleLocalKey((WORD)'A'); UT_ASSERT_EQ_UINT(0, ut_calls);
}
static void queues_an_extended_key_as_two_local_characters(void)
{
   memset(&od_control, 0, sizeof(od_control)); ut_calls = 0;
   utt_ODKrnlHandleLocalKey((WORD)((WORD)OD_KEY_LEFT << 8));
   UT_ASSERT_EQ_UINT(2, ut_calls); UT_ASSERT_EQ_INT(0, ut_characters[0]);
   UT_ASSERT_EQ_INT((char)OD_KEY_LEFT, ut_characters[1]);
   UT_ASSERT(!ut_remote[0] && !ut_remote[1]);
}
static void queues_an_ordinary_key_as_one_local_character(void)
{
   memset(&od_control, 0, sizeof(od_control)); ut_calls = 0;
   utt_ODKrnlHandleLocalKey((WORD)(((WORD)0x1e << 8) | (WORD)'A'));
   UT_ASSERT_EQ_UINT(1, ut_calls); UT_ASSERT_EQ_INT('A', ut_characters[0]);
   UT_ASSERT(!ut_remote[0]);
}
static const UTTestCase ut_cases[] = {
   {"disabled", ignores_local_keys_when_local_input_is_disabled},
   {"extended", queues_an_extended_key_as_two_local_characters},
   {"ordinary", queues_an_ordinary_key_as_one_local_character}
};
