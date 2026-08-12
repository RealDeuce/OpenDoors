#define UT_CUSTOM_MOCK_ODComGetByte
#define UT_CUSTOM_MOCK_ODKrnlHandleReceivedChar
static char ut_bytes[3];
static unsigned ut_byte_count, ut_byte_index, ut_received_count;
static char ut_received[3];
tODResult utm_ODComGetByte(tPortHandle handle, char *value, BOOL wait)
{
   UT_ASSERT_EQ_PTR(hSerialPort, handle);
   UT_ASSERT(value != NULL);
   UT_ASSERT(!wait);
   if(ut_byte_index == ut_byte_count)
      return(kODRCNothingWaiting);
   *value = ut_bytes[ut_byte_index++];
   return(kODRCSuccess);
}
void utm_ODKrnlHandleReceivedChar(char value, BOOL remote)
{
   UT_ASSERT(remote);
   UT_ASSERT(ut_received_count < sizeof(ut_received));
   ut_received[ut_received_count++] = value;
}
static void drains_each_currently_available_byte(void)
{
   ut_byte_index = ut_received_count = 0;
   ut_byte_count = 2;
   ut_bytes[0] = 'a'; ut_bytes[1] = 'b';
   utt_sig_get_char(SIGIO);
   UT_ASSERT_EQ_UINT(2, ut_received_count);
   UT_ASSERT_EQ_INT('a', ut_received[0]);
   UT_ASSERT_EQ_INT('b', ut_received[1]);

   ut_byte_index = ut_byte_count = ut_received_count = 0;
   utt_sig_get_char(0);
   UT_ASSERT_EQ_UINT(0, ut_received_count);
}
static const UTTestCase ut_cases[] = {
   {"drain", drains_each_currently_available_byte}
};
