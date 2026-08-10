#include <string.h>
#include <time.h>

#include "OpenDoor.h"
#include "ODCore.h"
#include "ODInQue.h"
#include "ODKrnl.h"
#include "ODPlat.h"
#include "ODSync.h"

#define CHECK(condition) do { if(!(condition)) return(__LINE__); } while(0)

static tODSemaphoreHandle hWorkerStarted;
static tODSemaphoreHandle hWorkerAcquired;
static BOOL bWorkerPublicLockRejected;
static BOOL bTimerCallbackOnOwner;
static unsigned nTimerCallbackCount;
static unsigned nKernelCallbackCount;

static void KernelCallback(void)
{
   ++nKernelCallbackCount;
}

static void TimeMessage(char *pszMessage)
{
   const tODControl *pControl = od_control_read_lock();
   (void)pszMessage;
   bTimerCallbackOnOwner = ODSyncIsOwnerThread() && pControl == &od_control;
   if(pControl != NULL) od_control_read_unlock();
   ++nTimerCallbackCount;
}

static DWORD OD_THREAD_FUNC ReaderThread(void *pParam)
{
   (void)pParam;
   bWorkerPublicLockRejected = od_control_read_lock() == NULL;
   ODSemaphoreUp(hWorkerStarted, 1);
   ODSyncControlReadLock();
   ODSemaphoreUp(hWorkerAcquired, 1);
   ODSyncControlReadUnlock();
   return(0);
}

static DWORD OD_THREAD_FUNC InputProducerThread(void *pParam)
{
   tODInputEvent Event;

   ODSemaphoreUp(hWorkerStarted, 1);
   ODSyncControlReadLock();
   Event.EventType = EVENT_CHARACTER;
   Event.bFromRemote = TRUE;
   Event.chKeyPress = *(char *)pParam;
   ODInQueueAddEvent(hODInputQueue, &Event);
   ODSyncControlReadUnlock();
   return(0);
}

int main(void)
{
   tODThreadHandle Worker;
   tODInputEvent Event;
   char chProduced;

   CHECK(ODSyncSessionInitialize() == kODRCSuccess);
   CHECK(ODSyncIsOwnerThread());
   bODInitialized = TRUE;
   ODSyncInitializationComplete();
   CHECK(ODSemaphoreAlloc(&hWorkerStarted, 0, 1) == kODRCSuccess);
   CHECK(ODSemaphoreAlloc(&hWorkerAcquired, 0, 1) == kODRCSuccess);

   CHECK(od_control_read_lock() == &od_control);
   CHECK(od_control_write_lock() == NULL);
   od_control_read_unlock();

   CHECK(od_control_write_lock() == &od_control);
   CHECK(od_control_write_lock() == &od_control);
   CHECK(od_control_read_lock() == &od_control);
   od_control_read_unlock();
   od_control_write_unlock();

   CHECK(ODThreadCreate(&Worker, ReaderThread, NULL) == kODRCSuccess);
   CHECK(ODSemaphoreDown(hWorkerStarted, 1000) == kODRCSuccess);
   CHECK(ODSemaphoreDown(hWorkerAcquired, 50) == kODRCTimeout);
   od_control_write_unlock();
   CHECK(ODSemaphoreDown(hWorkerAcquired, 1000) == kODRCSuccess);
   ODThreadWaitForExit(Worker);
   CHECK(bWorkerPublicLockRejected);

   CHECK(ODInQueueAlloc(&hODInputQueue, 4) == kODRCSuccess);
   memset(&od_control, 0, sizeof(od_control));
   od_control.od_ker_exec = KernelCallback;
   od_control.od_disable_inactivity = TRUE;
   od_control.od_time_msg_func = TimeMessage;
   od_control.od_time_warning = (char *)"%d";
   od_control.user_timelimit = 2;
   CHECK(ODKrnlInitialize() == kODRCSuccess);
   ODSyncAPIEntry();
   CHECK(ODThreadCreate(&Worker, ReaderThread, NULL) == kODRCSuccess);
   CHECK(ODSemaphoreDown(hWorkerStarted, 1000) == kODRCSuccess);
   CHECK(ODSemaphoreDown(hWorkerAcquired, 50) == kODRCTimeout);
   od_sleep(1);
   CHECK(ODSemaphoreDown(hWorkerAcquired, 0) == kODRCSuccess);
   ODSyncAPIExit();
   ODThreadWaitForExit(Worker);

   chProduced = 'I';
   ODSyncAPIEntry();
   CHECK(ODThreadCreate(&Worker, InputProducerThread, &chProduced)
      == kODRCSuccess);
   CHECK(ODSemaphoreDown(hWorkerStarted, 1000) == kODRCSuccess);
   CHECK(od_get_input(&Event, 1000, GETIN_RAW));
   CHECK(Event.EventType == EVENT_CHARACTER);
   CHECK(Event.bFromRemote);
   CHECK(Event.chKeyPress == chProduced);
   ODSyncAPIExit();
   ODThreadWaitForExit(Worker);

   chProduced = 'K';
   ODSyncAPIEntry();
   CHECK(ODThreadCreate(&Worker, InputProducerThread, &chProduced)
      == kODRCSuccess);
   CHECK(ODSemaphoreDown(hWorkerStarted, 1000) == kODRCSuccess);
   CHECK(od_get_key(TRUE) == chProduced);
   ODSyncAPIExit();
   ODThreadWaitForExit(Worker);

   od_kernel();
   od_kernel();
   CHECK(nKernelCallbackCount == 2);
   nNextTimeDeductTime = time(NULL);
   ODKrnlRequestTimeUpdate();
   ODKrnlDispatchPending(FALSE);
   CHECK(nTimerCallbackCount == 0);
   CHECK(od_control.user_timelimit == 1);
   od_control.od_ker_exec = NULL;
   od_kernel();
   CHECK(nTimerCallbackCount == 1);
   CHECK(bTimerCallbackOnOwner);
   ODKrnlShutdown();
   bODInitialized = FALSE;
   ODInQueueFree(hODInputQueue);
   hODInputQueue = NULL;

   ODSemaphoreFree(hWorkerAcquired);
   ODSemaphoreFree(hWorkerStarted);
   ODSyncSessionShutdown();
   return(0);
}
