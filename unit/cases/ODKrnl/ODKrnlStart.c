#define UT_CUSTOM_MOCK_time
static unsigned ut_time_calls;
time_t utm_time(time_t *result)
{
   (void)result;
   ++ut_time_calls;
   return((time_t)(100 + ut_time_calls));
}

#ifdef ODPLAT_NIX
#ifndef __APPLE__
#define UT_CUSTOM_MOCK_sigemptyset
#define UT_CUSTOM_MOCK_sigaddset
#endif
#define UT_CUSTOM_MOCK_sigprocmask
static unsigned ut_signal_calls;
#ifndef __APPLE__
int utm_sigemptyset(sigset_t *set)
{
   ++ut_signal_calls;
   UT_ASSERT_NOT_NULL(set);
   return(0);
}
int utm_sigaddset(sigset_t *set, int signal_number)
{
   ++ut_signal_calls;
   UT_ASSERT_NOT_NULL(set);
   UT_ASSERT_EQ_INT(SIGHUP, signal_number);
   return(0);
}
#endif
int utm_sigprocmask(int operation, const sigset_t *set, sigset_t *old_set)
{
   ++ut_signal_calls;
   UT_ASSERT_EQ_INT(SIG_BLOCK, operation);
   UT_ASSERT_NOT_NULL(set);
#ifdef __APPLE__
   UT_ASSERT_EQ_INT(1, sigismember(set, SIGHUP));
#endif
   UT_ASSERT_NULL(old_set);
   return(0);
}
#endif

#ifdef OD_MULTITHREADED
#define UT_CUSTOM_MOCK_ODMutexInitialize
#define UT_CUSTOM_MOCK_ODKrnlDiscardTimeMessages
#define UT_CUSTOM_MOCK_ODKrnlRemoteInputThread
#define UT_CUSTOM_MOCK_ODKrnlNoCarrierThread
#define UT_CUSTOM_MOCK_ODKrnlTimeUpdateThread
#define UT_CUSTOM_MOCK_ODSemaphoreAlloc
#define UT_CUSTOM_MOCK_ODMutexLock
#define UT_CUSTOM_MOCK_ODMutexUnlock
#define UT_CUSTOM_MOCK_ODThreadCreate
#define UT_CUSTOM_MOCK_ODThreadSetPriority
#define UT_CUSTOM_MOCK_ODSemaphoreUp
#define UT_CUSTOM_MOCK_ODSyncAPIWriterHeldByCurrentThread
#define UT_CUSTOM_MOCK_ODSyncAPIRelease
#define UT_CUSTOM_MOCK_ODSyncAPIReacquire
#define UT_CUSTOM_MOCK_ODKrnlJoinThread
#define UT_CUSTOM_MOCK_ODSemaphoreFree

static tODResult ut_mutex_result;
static tODResult ut_semaphore_result;
static tODResult ut_thread_results[3];
static unsigned ut_thread_index;
static unsigned ut_discard_calls;
static unsigned ut_priority_calls;
static unsigned ut_up_calls;
static unsigned ut_join_calls;
static unsigned ut_free_calls;
static unsigned ut_reacquire_level;
static BOOL ut_writer_held;
static unsigned ut_release_level;

DWORD OD_THREAD_FUNC utm_ODKrnlRemoteInputThread(void *parameter)
{
   (void)parameter;
   return(0);
}
DWORD OD_THREAD_FUNC utm_ODKrnlNoCarrierThread(void *parameter)
{
   (void)parameter;
   return(0);
}
DWORD OD_THREAD_FUNC utm_ODKrnlTimeUpdateThread(void *parameter)
{
   (void)parameter;
   return(0);
}

tODResult utm_ODMutexInitialize(tODMutex *mutex)
{
   UT_ASSERT_EQ_PTR(&KernelStateLock, mutex);
   return(ut_mutex_result);
}
void utm_ODKrnlDiscardTimeMessages(void) { ++ut_discard_calls; }
tODResult utm_ODSemaphoreAlloc(tODSemaphoreHandle *semaphore,
   INT initial_count, INT maximum_count)
{
   UT_ASSERT_EQ_UINT(0, initial_count);
   UT_ASSERT_EQ_UINT(3, maximum_count);
   if(ut_semaphore_result == kODRCSuccess)
      *semaphore = (tODSemaphoreHandle)1;
   return(ut_semaphore_result);
}
void utm_ODMutexLock(tODMutex *mutex) { UT_ASSERT_EQ_PTR(&KernelStateLock, mutex); }
void utm_ODMutexUnlock(tODMutex *mutex) { UT_ASSERT_EQ_PTR(&KernelStateLock, mutex); }
tODResult utm_ODThreadCreate(tODThreadHandle *thread,
   ptODThreadProc *procedure, void *parameter)
{
   unsigned index = ut_thread_index++;
   UT_ASSERT(index < 3);
   UT_ASSERT_NOT_NULL(thread);
   UT_ASSERT_NOT_NULL(procedure);
   UT_ASSERT_NULL(parameter);
   return(ut_thread_results[index]);
}
tODResult utm_ODThreadSetPriority(tODThreadHandle thread,
   tODThreadPriority priority)
{
   (void)thread;
   ++ut_priority_calls;
   UT_ASSERT_EQ_INT(OD_PRIORITY_NORMAL, priority);
   return(kODRCSuccess);
}
void utm_ODSemaphoreUp(tODSemaphoreHandle semaphore, INT count)
{
   ++ut_up_calls;
   UT_ASSERT(semaphore == (tODSemaphoreHandle)1);
   UT_ASSERT_EQ_UINT(3, count);
}
BOOL utm_ODSyncAPIWriterHeldByCurrentThread(void) { return(ut_writer_held); }
unsigned utm_ODSyncAPIRelease(void) { return(ut_release_level); }
void utm_ODSyncAPIReacquire(unsigned level) { ut_reacquire_level = level; }
void utm_ODKrnlJoinThread(tODThreadHandle *thread, BOOL *started)
{
   ++ut_join_calls;
   UT_ASSERT_NOT_NULL(thread);
   UT_ASSERT_NOT_NULL(started);
}
void utm_ODSemaphoreFree(tODSemaphoreHandle semaphore)
{
   ++ut_free_calls;
   UT_ASSERT(semaphore == (tODSemaphoreHandle)1);
}

static void reset_threaded_state(void)
{
   unsigned index;
   ut_mutex_result = kODRCSuccess;
   ut_semaphore_result = kODRCSuccess;
   for(index = 0; index < 3; ++index)
      ut_thread_results[index] = kODRCSuccess;
   ut_thread_index = ut_discard_calls = ut_priority_calls = 0;
   ut_up_calls = ut_join_calls = ut_free_calls = 0;
   ut_reacquire_level = 0;
   ut_writer_held = FALSE;
   ut_release_level = 0;
   bKernelStateLockInitialized = FALSE;
   od_control.baud = 0;
}

static void reports_lock_and_semaphore_initialization_failures(void)
{
   reset_threaded_state();
   ut_mutex_result = kODRCGeneralFailure;
   UT_ASSERT_EQ_INT(kODRCGeneralFailure, utt_ODKrnlStart(FALSE));
   UT_ASSERT(!bKernelStateLockInitialized);

   reset_threaded_state();
   ut_semaphore_result = kODRCGeneralFailure;
   UT_ASSERT_EQ_INT(kODRCGeneralFailure, utt_ODKrnlStart(FALSE));
   UT_ASSERT(bKernelStateLockInitialized);
   UT_ASSERT_EQ_UINT(1, ut_discard_calls);
}

static void starts_local_and_remote_workers_and_preserves_when_requested(void)
{
   reset_threaded_state();
   UT_ASSERT_EQ_INT(kODRCSuccess, utt_ODKrnlStart(FALSE));
   UT_ASSERT_EQ_UINT(1, ut_discard_calls);
   UT_ASSERT_EQ_UINT(1, ut_thread_index);
   UT_ASSERT_EQ_UINT(1, ut_priority_calls);

   reset_threaded_state();
   bKernelStateLockInitialized = TRUE;
   od_control.baud = 9600;
   bChatTogglePending = TRUE;
   UT_ASSERT_EQ_INT(kODRCSuccess, utt_ODKrnlStart(TRUE));
   UT_ASSERT_EQ_UINT(0, ut_discard_calls);
   UT_ASSERT_EQ_UINT(3, ut_thread_index);
   UT_ASSERT_EQ_UINT(3, ut_priority_calls);
   UT_ASSERT(bChatTogglePending);
}

static void unwinds_each_worker_creation_failure(void)
{
   unsigned failing_thread;
   for(failing_thread = 0; failing_thread < 3; ++failing_thread)
   {
      reset_threaded_state();
      bKernelStateLockInitialized = TRUE;
      od_control.baud = 9600;
      ut_thread_results[failing_thread] = kODRCGeneralFailure;
      ut_writer_held = (failing_thread != 0);
      ut_release_level = (failing_thread == 2) ? 2 : 0;
      UT_ASSERT_EQ_INT(kODRCGeneralFailure, utt_ODKrnlStart(FALSE));
      UT_ASSERT_EQ_UINT(failing_thread + 1, ut_thread_index);
      UT_ASSERT_EQ_UINT(failing_thread, ut_priority_calls);
      UT_ASSERT_EQ_UINT(1, ut_up_calls);
      UT_ASSERT_EQ_UINT(3, ut_join_calls);
      UT_ASSERT_EQ_UINT(1, ut_free_calls);
      UT_ASSERT_NULL(hKernelShutdownSemaphore);
      UT_ASSERT_EQ_UINT(ut_release_level, ut_reacquire_level);
   }
}
#endif

static void initializes_common_kernel_state(void)
{
   ut_time_calls = 0;
#ifdef ODPLAT_NIX
   ut_signal_calls = 0;
#endif
#ifdef OD_MULTITHREADED
   reset_threaded_state();
#endif
   od_control.od_status_on = FALSE;
   nKrnlFuncPending = 9;
   bKernelActive = TRUE;
   UT_ASSERT_EQ_INT(kODRCSuccess, utt_ODKrnlStart(FALSE));
   UT_ASSERT_EQ_UINT(2, ut_time_calls);
#ifdef ODPLAT_NIX
#ifdef __APPLE__
   UT_ASSERT_EQ_UINT(1, ut_signal_calls);
#else
   UT_ASSERT_EQ_UINT(3, ut_signal_calls);
#endif
#endif
   UT_ASSERT_EQ_INT((time_t)104, nNextStatusUpdateTime);
   UT_ASSERT_EQ_INT((time_t)162, nNextTimeDeductTime);
   UT_ASSERT(od_control.od_status_on);
   UT_ASSERT(bLastStatusSetting);
   UT_ASSERT_EQ_INT(0, nKrnlFuncPending);
   UT_ASSERT(!bKernelActive);
}

static const UTTestCase ut_cases[] = {
   {"common state", initializes_common_kernel_state},
#ifdef OD_MULTITHREADED
   {"initialization failures", reports_lock_and_semaphore_initialization_failures},
   {"workers", starts_local_and_remote_workers_and_preserves_when_requested},
   {"unwind", unwinds_each_worker_creation_failure}
#endif
};
