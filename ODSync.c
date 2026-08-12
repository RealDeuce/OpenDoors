/* OpenDoors session ownership and synchronization. */

#define BUILDING_OPENDOORS

#include <stddef.h>

#include "OpenDoor.h"
#include "ODCore.h"
#include "ODGen.h"
#include "ODKrnl.h"
#include "ODSync.h"

#ifdef OD_THREAD_SUPPORT
typedef struct
{
   tODMutex state;
   HANDLE changed;
   DWORD owner;
   unsigned readers;
   unsigned waiting_writers;
   BOOL writer;
} tODControlLock;

static tODControlLock ControlLock;
#endif

static BOOL bSyncActive;
static unsigned nAPILevel;
static unsigned nPublicReadDepth;
static unsigned nPublicWriteDepth;
static BOOL bDispatching;
static BOOL bPublicLockPhysical;

static void ODReleasePublicLock(void);

tODResult ODMutexInitialize(tODMutex *pMutex)
{
#ifdef OD_THREAD_SUPPORT
   InitializeCriticalSection(&pMutex->cs);
   return(kODRCSuccess);
#else
   (void)pMutex;
   return(kODRCSuccess);
#endif
}

void ODMutexDestroy(tODMutex *pMutex)
{
#ifdef OD_THREAD_SUPPORT
   DeleteCriticalSection(&pMutex->cs);
#else
   (void)pMutex;
#endif
}

void ODMutexLock(tODMutex *pMutex)
{
#ifdef OD_THREAD_SUPPORT
   EnterCriticalSection(&pMutex->cs);
#else
   (void)pMutex;
#endif
}

void ODMutexUnlock(tODMutex *pMutex)
{
#ifdef OD_THREAD_SUPPORT
   LeaveCriticalSection(&pMutex->cs);
#else
   (void)pMutex;
#endif
}

#ifdef OD_THREAD_SUPPORT
static void ODControlWakeWaiters(void)
{
   SetEvent(ControlLock.changed);
}

static void ODControlReadAcquire(void)
{
   ODMutexLock(&ControlLock.state);
   while(ControlLock.writer || ControlLock.waiting_writers != 0)
   {
      ResetEvent(ControlLock.changed);
      ODMutexUnlock(&ControlLock.state);
      WaitForSingleObject(ControlLock.changed, INFINITE);
      ODMutexLock(&ControlLock.state);
   }
   ++ControlLock.readers;
   ODMutexUnlock(&ControlLock.state);
}

static void ODControlReadRelease(void)
{
   ODMutexLock(&ControlLock.state);
   ASSERT(ControlLock.readers != 0);
   --ControlLock.readers;
   if(ControlLock.readers == 0)
      ODControlWakeWaiters();
   ODMutexUnlock(&ControlLock.state);
}

static void ODControlWriteAcquire(void)
{
   ODMutexLock(&ControlLock.state);
   ++ControlLock.waiting_writers;
   while(ControlLock.writer || ControlLock.readers != 0)
   {
      ResetEvent(ControlLock.changed);
      ODMutexUnlock(&ControlLock.state);
      WaitForSingleObject(ControlLock.changed, INFINITE);
      ODMutexLock(&ControlLock.state);
   }
   --ControlLock.waiting_writers;
   ControlLock.writer = TRUE;
   ODMutexUnlock(&ControlLock.state);
}

static void ODControlWriteRelease(void)
{
   ODMutexLock(&ControlLock.state);
   ASSERT(ControlLock.writer);
   ControlLock.writer = FALSE;
   ODControlWakeWaiters();
   ODMutexUnlock(&ControlLock.state);
}
#endif

tODResult ODSyncSessionInitialize(void)
{
   if(bSyncActive)
   {
      if(!bODInitialized && bPublicLockPhysical)
         ODReleasePublicLock();
#ifdef OD_THREAD_SUPPORT
      if(!bODInitialized && nAPILevel == 0 && !bPublicLockPhysical
         && nPublicReadDepth == 0 && nPublicWriteDepth == 0)
         ControlLock.owner = GetCurrentThreadId();
#endif
      return(kODRCSuccess);
   }
#ifdef OD_THREAD_SUPPORT
   if(ODMutexInitialize(&ControlLock.state) != kODRCSuccess)
      return(kODRCGeneralFailure);
   ControlLock.changed = CreateEvent(NULL, TRUE, FALSE, NULL);
   if(ControlLock.changed == NULL)
   {
      ODMutexDestroy(&ControlLock.state);
      return(kODRCGeneralFailure);
   }
   ControlLock.owner = GetCurrentThreadId();
   ControlLock.readers = 0;
   ControlLock.waiting_writers = 0;
   ControlLock.writer = FALSE;
#endif
   nAPILevel = 0;
   bDispatching = FALSE;
   bPublicLockPhysical = FALSE;
   bSyncActive = TRUE;
   return(kODRCSuccess);
}

void ODSyncInitializationComplete(void)
{
   if(!bSyncActive || bPublicLockPhysical || nAPILevel != 0) return;
   if(nPublicWriteDepth != 0)
   {
      ODSyncControlWriteLock();
      bPublicLockPhysical = TRUE;
   }
   else if(nPublicReadDepth != 0)
   {
      ODSyncControlReadLock();
      bPublicLockPhysical = TRUE;
   }
}

void ODSyncSessionShutdown(void)
{
   if(!bSyncActive)
      return;
#ifdef OD_THREAD_SUPPORT
   CloseHandle(ControlLock.changed);
   ODMutexDestroy(&ControlLock.state);
#endif
   bSyncActive = FALSE;
   nAPILevel = 0;
   nPublicReadDepth = 0;
   nPublicWriteDepth = 0;
   bPublicLockPhysical = FALSE;
}

BOOL ODSyncIsOwnerThread(void)
{
   if(!bSyncActive)
      return(TRUE);
#ifdef OD_THREAD_SUPPORT
   return(ControlLock.owner == GetCurrentThreadId());
#else
   return(TRUE);
#endif
}

BOOL ODSyncSessionActive(void) { return(bSyncActive); }

BOOL ODSyncAPIWriterHeldByCurrentThread(void)
{
#ifdef OD_THREAD_SUPPORT
   return(bSyncActive && ODSyncIsOwnerThread() && nAPILevel != 0);
#else
   return(FALSE);
#endif
}

void ODSyncControlReadLock(void)
{
#ifdef OD_THREAD_SUPPORT
   if(bSyncActive && (nAPILevel == 0 || !ODSyncIsOwnerThread()))
      ODControlReadAcquire();
#endif
}

void ODSyncControlReadUnlock(void)
{
#ifdef OD_THREAD_SUPPORT
   if(bSyncActive && (nAPILevel == 0 || !ODSyncIsOwnerThread()))
      ODControlReadRelease();
#endif
}

void ODSyncControlWriteLock(void)
{
#ifdef OD_THREAD_SUPPORT
   if(bSyncActive && (nAPILevel == 0 || !ODSyncIsOwnerThread()))
      ODControlWriteAcquire();
#endif
}

void ODSyncControlWriteUnlock(void)
{
#ifdef OD_THREAD_SUPPORT
   if(bSyncActive && (nAPILevel == 0 || !ODSyncIsOwnerThread()))
      ODControlWriteRelease();
#endif
}

static void ODReleasePublicLock(void)
{
   if(!bPublicLockPhysical) return;
   if(nPublicWriteDepth != 0) ODSyncControlWriteUnlock();
   else if(nPublicReadDepth != 0) ODSyncControlReadUnlock();
   bPublicLockPhysical = FALSE;
}

static void ODAcquirePublicLock(void)
{
   if(nPublicWriteDepth != 0) ODSyncControlWriteLock();
   else if(nPublicReadDepth != 0) ODSyncControlReadLock();
   else return;
   bPublicLockPhysical = TRUE;
}

static void ODDispatch(BOOL bAllowApplicationCallbacks)
{
   if(!bDispatching && bSyncActive && ODSyncIsOwnerThread())
   {
      bDispatching = TRUE;
      ODSyncControlWriteLock();
      ++nAPILevel;
      ODKrnlDispatchPending(bAllowApplicationCallbacks);
      --nAPILevel;
      ODSyncControlWriteUnlock();
      bDispatching = FALSE;
   }
}

void ODSyncAPIEntry(void)
{
   BOOL bOutermost = nAPILevel == 0;

   ASSERT(ODSyncIsOwnerThread());
   if(bOutermost)
   {
      ODReleasePublicLock();
      ODSyncControlWriteLock();
   }
   ++nAPILevel;
   if(bOutermost)
      ODKrnlDispatchPending(TRUE);
}

void ODSyncAPIExit(void)
{
   ASSERT(nAPILevel != 0);
   if(nAPILevel == 0) return;
   if(--nAPILevel == 0)
   {
      ODSyncControlWriteUnlock();
      ODDispatch(TRUE);
      ODAcquirePublicLock();
   }
}

BOOL ODSyncAPICheckpoint(void)
{
   unsigned nSavedAPILevel;

   if(nAPILevel == 0 || !ODSyncIsOwnerThread())
      return(bODInitialized);
   nSavedAPILevel = ODSyncAPIRelease();
   ODSyncAPIReacquire(nSavedAPILevel);
   ODKrnlDispatchPending(TRUE);
   if(bODInitialized)
      od_kernel();
   return(bODInitialized);
}

unsigned ODSyncAPIRelease(void)
{
   unsigned nSavedAPILevel = nAPILevel;

   ASSERT(nSavedAPILevel != 0);
   nAPILevel = 0;
   ODSyncControlWriteUnlock();
   return(nSavedAPILevel);
}

void ODSyncAPIReacquire(unsigned nSavedAPILevel)
{
   ASSERT(nSavedAPILevel != 0);
   if(bSyncActive) ODSyncControlWriteLock();
   nAPILevel = nSavedAPILevel;
}

ODAPIDEF const tODControl * ODCALL od_control_read_lock(void)
{
   if(!bSyncActive && ODSyncSessionInitialize() != kODRCSuccess) return(NULL);
   if(bSyncActive && !ODSyncIsOwnerThread()) return(NULL);
   if(nPublicWriteDepth != 0) ++nPublicReadDepth;
   else if(nPublicReadDepth++ == 0 && nAPILevel == 0 && bODInitialized)
   {
      ODSyncControlReadLock();
      bPublicLockPhysical = TRUE;
   }
   return(&od_control);
}

ODAPIDEF void ODCALL od_control_read_unlock(void)
{
   ASSERT(nPublicReadDepth != 0);
   if(nPublicReadDepth == 0) return;
   if(--nPublicReadDepth == 0 && nPublicWriteDepth == 0 && nAPILevel == 0
      && bPublicLockPhysical)
   {
      ODSyncControlReadUnlock();
      bPublicLockPhysical = FALSE;
   }
}

ODAPIDEF tODControl * ODCALL od_control_write_lock(void)
{
   if(!bSyncActive && ODSyncSessionInitialize() != kODRCSuccess) return(NULL);
   if((bSyncActive && !ODSyncIsOwnerThread()) ||
      (nPublicReadDepth != 0 && nPublicWriteDepth == 0)) return(NULL);
   if(nPublicWriteDepth++ == 0 && nAPILevel == 0 && bODInitialized)
   {
      ODSyncControlWriteLock();
      bPublicLockPhysical = TRUE;
   }
   return(&od_control);
}

ODAPIDEF void ODCALL od_control_write_unlock(void)
{
   ASSERT(nPublicWriteDepth != 0);
   if(nPublicWriteDepth == 0) return;
   if(--nPublicWriteDepth == 0 && nAPILevel == 0 && bPublicLockPhysical)
   {
      ODSyncControlWriteUnlock();
      bPublicLockPhysical = FALSE;
      if(nPublicReadDepth != 0)
      {
         ODSyncControlReadLock();
         bPublicLockPhysical = TRUE;
      }
   }
}
