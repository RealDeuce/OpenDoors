#define UT_CUSTOM_MOCK_memcpy
#define UT_CUSTOM_MOCK_time
#ifdef OD_THREAD_SUPPORT
#define UT_CUSTOM_MOCK_ODMutexLock
#define UT_CUSTOM_MOCK_ODMutexUnlock
#define UT_CUSTOM_MOCK_ODSemaphoreUp
#endif

#include "common.h"

static unsigned ut_copy_calls;
#ifdef OD_THREAD_SUPPORT
static unsigned ut_locks;
static unsigned ut_unlocks;
static unsigned ut_semaphore_ups;
void utm_ODMutexLock(tODMutex *mutex)
{
   UT_ASSERT_EQ_PTR(&ut_queue.QueueMutex, mutex);
   ++ut_locks;
}
void utm_ODMutexUnlock(tODMutex *mutex)
{
   UT_ASSERT_EQ_PTR(&ut_queue.QueueMutex, mutex);
   ++ut_unlocks;
}
void utm_ODSemaphoreUp(tODSemaphoreHandle semaphore, INT increment)
{
   UT_ASSERT(semaphore == ut_queue.hItemCountSemaphore);
   UT_ASSERT_EQ_INT(1, increment);
   ++ut_semaphore_ups;
}
#endif

time_t utm_time(time_t *result)
{
   UT_ASSERT(result == NULL);
   return (time_t)99;
}

void *utm_memcpy(void *output, const void *input, size_t size)
{
   unsigned char *destination = output;
   const unsigned char *source = input;
   size_t index;
   UT_ASSERT_EQ_UINT(sizeof(tODInputEvent), size);
   for(index = 0; index < size; ++index) destination[index] = source[index];
   ++ut_copy_calls;
   return output;
}

static void reset_counts(void)
{
   ut_copy_calls = 0;
#ifdef OD_THREAD_SUPPORT
   ut_locks = ut_unlocks = ut_semaphore_ups = 0;
#endif
}

static void rejects_each_null_argument(void)
{
   tODInputEvent event;
   tODInQueueHandle handle = ut_queue_handle(3, 0, 0);
   reset_counts();
   UT_ASSERT_EQ_INT(kODRCInvalidCall,
      utt_ODInQueueAddEvent((tODInQueueHandle)0, &event));
   UT_ASSERT_EQ_INT(kODRCInvalidCall,
      utt_ODInQueueAddEvent(handle, NULL));
   UT_ASSERT_EQ_INT(0, ut_copy_calls);
}

static void reports_full_queue_after_recording_activity(void)
{
   tODInputEvent event;
   tODInQueueHandle handle = ut_queue_handle(3, 1, 2);
   reset_counts();
   UT_ASSERT_EQ_INT(kODRCNoMemory, utt_ODInQueueAddEvent(handle, &event));
   UT_ASSERT_EQ_INT(99, ut_queue.nLastActivityTime);
   UT_ASSERT_EQ_INT(0, ut_copy_calls);
#ifdef OD_THREAD_SUPPORT
   UT_ASSERT_EQ_INT(1, ut_locks);
   UT_ASSERT_EQ_INT(1, ut_unlocks);
   UT_ASSERT_EQ_INT(0, ut_semaphore_ups);
#endif
}

static void appends_and_wraps_events(void)
{
   tODInputEvent event;
   tODInQueueHandle handle = ut_queue_handle(3, 0, 0);
   event.EventType = EVENT_CHARACTER;
   event.bFromRemote = TRUE;
   event.chKeyPress = 'x';
   reset_counts();
   UT_ASSERT_EQ_INT(kODRCSuccess, utt_ODInQueueAddEvent(handle, &event));
   UT_ASSERT_EQ_INT(1, ut_queue.nInIndex);
   UT_ASSERT_EQ_INT('x', ut_events[0].chKeyPress);
   ut_queue.nInIndex = 2;
   ut_queue.nOutIndex = 1;
   event.chKeyPress = 'y';
   UT_ASSERT_EQ_INT(kODRCSuccess, utt_ODInQueueAddEvent(handle, &event));
   UT_ASSERT_EQ_INT(0, ut_queue.nInIndex);
   UT_ASSERT_EQ_INT('y', ut_events[2].chKeyPress);
   UT_ASSERT_EQ_INT(2, ut_copy_calls);
#ifdef OD_THREAD_SUPPORT
   UT_ASSERT_EQ_INT(2, ut_locks);
   UT_ASSERT_EQ_INT(2, ut_unlocks);
   UT_ASSERT_EQ_INT(2, ut_semaphore_ups);
#endif
}

static const UTTestCase ut_cases[] = {
   {"invalid event arguments", rejects_each_null_argument},
   {"full queue", reports_full_queue_after_recording_activity},
   {"append and wrap", appends_and_wraps_events}
};
