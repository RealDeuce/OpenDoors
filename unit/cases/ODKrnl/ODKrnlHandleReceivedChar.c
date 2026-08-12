#define UT_CUSTOM_MOCK_ODSyncControlReadLock
#define UT_CUSTOM_MOCK_ODSyncControlReadUnlock
#define UT_CUSTOM_MOCK_ODInQueueAddEvent
static unsigned ut_add_calls;
static tODInputEvent ut_event;
void utm_ODSyncControlReadLock(void) { }
void utm_ODSyncControlReadUnlock(void) { }
tODResult utm_ODInQueueAddEvent(tODInQueueHandle queue, tODInputEvent *event)
{
   ++ut_add_calls; UT_ASSERT(queue == hODInputQueue); ut_event = *event;
   return kODRCSuccess;
}
static void reset_received(BOOL keyboard_on)
{
   memset(&od_control, 0, sizeof(od_control)); memset(&ut_event, 0, sizeof(ut_event));
   od_control.od_user_keyboard_on = keyboard_on; hODInputQueue = (tODInQueueHandle)1;
   ut_add_calls = 0; chLastControlKey = 'x';
}
static void ignores_only_remote_input_when_the_user_keyboard_is_off(void)
{
   reset_received(FALSE); utt_ODKrnlHandleReceivedChar('A', TRUE);
   UT_ASSERT_EQ_UINT(0, ut_add_calls);
   utt_ODKrnlHandleReceivedChar('B', FALSE); UT_ASSERT_EQ_UINT(1, ut_add_calls);
   UT_ASSERT_EQ_INT('B', ut_event.chKeyPress); UT_ASSERT(!ut_event.bFromRemote);
}
static void queues_remote_input_with_character_event_metadata(void)
{
   reset_received(TRUE); utt_ODKrnlHandleReceivedChar('A', TRUE);
   UT_ASSERT_EQ_UINT(1, ut_add_calls); UT_ASSERT_EQ_INT(EVENT_CHARACTER, ut_event.EventType);
   UT_ASSERT(ut_event.bFromRemote); UT_ASSERT_EQ_INT('A', ut_event.chKeyPress);
}
static void assert_control_key(char input, char expected)
{
   reset_received(TRUE);
   utt_ODKrnlHandleReceivedChar(input, FALSE);
   UT_ASSERT_EQ_INT(expected, chLastControlKey);
}
static void recognizes_every_stop_and_pause_control_key(void)
{
   assert_control_key('s', 's');
   assert_control_key('S', 's');
   assert_control_key(3, 's');
   assert_control_key(11, 's');
   assert_control_key(0x18, 's');
   assert_control_key('p', 'p');
   assert_control_key('P', 'p');
   assert_control_key('z', 'x');
}
static const UTTestCase ut_cases[] = {
   {"disabled remote", ignores_only_remote_input_when_the_user_keyboard_is_off},
   {"remote event", queues_remote_input_with_character_event_metadata},
   {"controls", recognizes_every_stop_and_pause_control_key}
};
