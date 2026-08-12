#define UT_CUSTOM_MOCK_ODKrnlWorkerWait
#define UT_CUSTOM_MOCK_ODComGetByte
#define UT_CUSTOM_MOCK_ODKrnlHandleReceivedChar
static BOOL ut_wait_results[8];
static tODMilliSec ut_wait_milliseconds[8];
static tODResult ut_get_results[4];
static char ut_get_characters[4];
static unsigned ut_wait_index, ut_wait_count, ut_get_index, ut_get_count;
static unsigned ut_handle_calls;
BOOL utm_ODKrnlWorkerWait(tODMilliSec milliseconds)
{
   UT_ASSERT(ut_wait_index < ut_wait_count);
   UT_ASSERT_EQ_UINT(ut_wait_milliseconds[ut_wait_index], milliseconds);
   return ut_wait_results[ut_wait_index++];
}
tODResult utm_ODComGetByte(tPortHandle handle, char *character,
   BOOL wait_for_character)
{
   unsigned index = ut_get_index++;
   UT_ASSERT(handle == hSerialPort); UT_ASSERT(!wait_for_character);
   UT_ASSERT(index < ut_get_count);
   if(ut_get_results[index] == kODRCSuccess) *character = ut_get_characters[index];
   return ut_get_results[index];
}
void utm_ODKrnlHandleReceivedChar(char character, BOOL remote)
{
   ++ut_handle_calls; UT_ASSERT(remote); UT_ASSERT_EQ_INT('A', character);
}
static void handles_input_and_retries_until_shutdown(void)
{
   hSerialPort = (tPortHandle)1;
   ut_wait_results[0] = TRUE;
   ut_wait_results[1] = TRUE;
   ut_wait_results[2] = TRUE;
   ut_wait_results[3] = TRUE; ut_wait_results[4] = FALSE;
   ut_wait_milliseconds[0] = 0; ut_wait_milliseconds[1] = 0;
   ut_wait_milliseconds[2] = 10; ut_wait_milliseconds[3] = 0;
   ut_wait_milliseconds[4] = 10;
   ut_wait_count = 5; ut_wait_index = 0;
   ut_get_results[0] = kODRCSuccess; ut_get_characters[0] = 'A';
   ut_get_results[1] = kODRCGeneralFailure; ut_get_results[2] = kODRCGeneralFailure;
   ut_get_count = 3; ut_get_index = 0; ut_handle_calls = 0;
   UT_ASSERT_EQ_UINT(0, utt_ODKrnlRemoteInputThread(NULL));
   UT_ASSERT_EQ_UINT(1, ut_handle_calls); UT_ASSERT_EQ_UINT(3, ut_get_index);
}
static void exits_before_reading_when_the_initial_wait_stops(void)
{
   ut_wait_results[0] = FALSE; ut_wait_count = 1; ut_wait_index = 0;
   ut_wait_milliseconds[0] = 0;
   ut_get_index = ut_get_count = ut_handle_calls = 0;
   UT_ASSERT_EQ_UINT(0, utt_ODKrnlRemoteInputThread(NULL));
   UT_ASSERT_EQ_UINT(0, ut_get_index);
}
static const UTTestCase ut_cases[] = {
   {"input", handles_input_and_retries_until_shutdown},
   {"initial stop", exits_before_reading_when_the_initial_wait_stops}
};
