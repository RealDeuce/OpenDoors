#define UT_CUSTOM_MOCK_ODKrnlWorkerWait
#define UT_CUSTOM_MOCK_ODComCarrier
#define UT_CUSTOM_MOCK_ODSyncControlReadLock
#define UT_CUSTOM_MOCK_ODSyncControlReadUnlock
#define UT_CUSTOM_MOCK_ODKrnlQueueShutdown
static BOOL ut_wait_results[8];
static tODResult ut_carrier_results[4];
static BOOL ut_carriers[4];
static unsigned ut_wait_index, ut_wait_count, ut_carrier_index, ut_carrier_count;
static unsigned ut_queue_calls, ut_lock_calls, ut_unlock_calls;
BOOL utm_ODKrnlWorkerWait(tODMilliSec milliseconds)
{
   UT_ASSERT(ut_wait_index < ut_wait_count);
   if((ut_wait_index & 1) == 0) UT_ASSERT_EQ_UINT(0, milliseconds);
   else UT_ASSERT_EQ_UINT(100, milliseconds);
   return ut_wait_results[ut_wait_index++];
}
tODResult utm_ODComCarrier(tPortHandle handle, BOOL *carrier)
{
   unsigned index = ut_carrier_index++;
   UT_ASSERT(handle == hSerialPort); UT_ASSERT(index < ut_carrier_count);
   *carrier = ut_carriers[index]; return ut_carrier_results[index];
}
void utm_ODSyncControlReadLock(void) { ++ut_lock_calls; }
void utm_ODSyncControlReadUnlock(void) { ++ut_unlock_calls; }
void utm_ODKrnlQueueShutdown(BYTE reason)
{ ++ut_queue_calls; UT_ASSERT_EQ_UINT(ERRORLEVEL_NOCARRIER, reason); }
static void reset_carrier(void)
{
   memset(&od_control, 0, sizeof(od_control)); hSerialPort = (tPortHandle)1;
   ut_wait_index = ut_wait_count = ut_carrier_index = ut_carrier_count = 0;
   ut_queue_calls = ut_lock_calls = ut_unlock_calls = 0;
}
static void ignores_failed_and_present_carrier_checks_until_stopped(void)
{
   reset_carrier();
   ut_wait_results[0] = TRUE; ut_wait_results[1] = TRUE;
   ut_wait_results[2] = TRUE; ut_wait_results[3] = FALSE; ut_wait_count = 4;
   ut_carrier_results[0] = kODRCGeneralFailure; ut_carriers[0] = FALSE;
   ut_carrier_results[1] = kODRCSuccess; ut_carriers[1] = TRUE;
   ut_carrier_count = 2;
   UT_ASSERT_EQ_UINT(0, utt_ODKrnlNoCarrierThread(NULL));
   UT_ASSERT_EQ_UINT(0, ut_lock_calls); UT_ASSERT_EQ_UINT(0, ut_queue_calls);
}
static void ignores_loss_when_detection_is_disabled(void)
{
   reset_carrier(); od_control.od_disable = DIS_CARRIERDETECT;
   ut_wait_results[0] = TRUE; ut_wait_results[1] = FALSE; ut_wait_count = 2;
   ut_carrier_results[0] = kODRCSuccess; ut_carriers[0] = FALSE;
   ut_carrier_count = 1;
   UT_ASSERT_EQ_UINT(0, utt_ODKrnlNoCarrierThread(NULL));
   UT_ASSERT_EQ_UINT(1, ut_lock_calls); UT_ASSERT_EQ_UINT(1, ut_unlock_calls);
   UT_ASSERT_EQ_UINT(0, ut_queue_calls);
}
static void queues_shutdown_when_carrier_is_lost(void)
{
   reset_carrier();
   ut_wait_results[0] = TRUE; ut_wait_count = 1;
   ut_carrier_results[0] = kODRCSuccess; ut_carriers[0] = FALSE;
   ut_carrier_count = 1;
   UT_ASSERT_EQ_UINT(0, utt_ODKrnlNoCarrierThread(NULL));
   UT_ASSERT_EQ_UINT(1, ut_queue_calls); UT_ASSERT_EQ_UINT(1, ut_wait_index);
}
static void exits_before_checking_carrier_when_stopped(void)
{
   reset_carrier(); ut_wait_results[0] = FALSE; ut_wait_count = 1;
   UT_ASSERT_EQ_UINT(0, utt_ODKrnlNoCarrierThread(NULL));
   UT_ASSERT_EQ_UINT(0, ut_carrier_index);
}
static const UTTestCase ut_cases[] = {
   {"retry", ignores_failed_and_present_carrier_checks_until_stopped},
   {"disabled", ignores_loss_when_detection_is_disabled},
   {"lost", queues_shutdown_when_carrier_is_lost},
   {"initial stop", exits_before_checking_carrier_when_stopped}
};
