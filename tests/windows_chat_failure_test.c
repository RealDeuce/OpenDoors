#include <stdio.h>
#include <string.h>
#include <time.h>

#include "OpenDoor.h"
#include "ODCore.h"
#include "ODGen.h"
#include "ODInQue.h"
#include "ODKrnl.h"
#include "ODSync.h"

#define CHECK(condition) do { if(!(condition)) { \
   fprintf(stderr, "%s:%d: check failed: %s\n", __FILE__, __LINE__, \
      #condition); \
   return(__LINE__); \
} } while(0)

static DWORD dwOwnerThread;
static BOOL bTimerCallbackOnOwner;
static unsigned nKernelCallbackCount;

static void KernelCallback(void)
{
   ++nKernelCallbackCount;
}

static void TimeMessage(char *pszMessage)
{
   const tODControl *pControl = od_control_read_lock();
   (void)pszMessage;
   bTimerCallbackOnOwner = GetCurrentThreadId() == dwOwnerThread
      && pControl == &od_control;
   if(pControl != NULL) od_control_read_unlock();
}

int main(void)
{
   memset(&od_control, 0, sizeof(od_control));
   dwOwnerThread = GetCurrentThreadId();
   CHECK(ODSyncSessionInitialize() == kODRCSuccess);
   CHECK(ODInQueueAlloc(&hODInputQueue, 4) == kODRCSuccess);
   CHECK(ODKrnlInitialize() == kODRCSuccess);
   bODInitialized = TRUE;
   od_control.od_chat_active = TRUE;

   od_control.od_ker_exec = KernelCallback;
   od_kernel();
   od_kernel();
   CHECK(nKernelCallbackCount == 2);

   ODKrnlRequestChatToggle();
   ODKrnlDispatchPending(TRUE);
   CHECK(!od_control.od_chat_active);

   /* Two requests cancel one another before owner-thread dispatch. */
   od_control.od_chat_active = TRUE;
   ODKrnlRequestChatToggle();
   ODKrnlRequestChatToggle();
   ODKrnlDispatchPending(TRUE);
   CHECK(od_control.od_chat_active);

   od_control.od_chat_active = FALSE;
   od_control.od_disable_inactivity = TRUE;
   od_control.od_time_msg_func = TimeMessage;
   od_control.od_time_warning = (char *)"%d";
   od_control.user_timelimit = 2;
   nNextTimeDeductTime = time(NULL);
   od_control.od_ker_exec = NULL;
   ODKrnlRequestTimeUpdate();
   od_kernel();
   CHECK(bTimerCallbackOnOwner);
   CHECK(od_control.user_timelimit == 1);

   bODInitialized = FALSE;
   ODKrnlShutdown();
   ODInQueueFree(hODInputQueue);
   hODInputQueue = NULL;
   ODSyncSessionShutdown();
   return(0);
}
