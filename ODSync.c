/* OpenDoors session ownership and synchronization. */

#define BUILDING_OPENDOORS

#include "OpenDoor.h"
#include "ODCore.h"
#include "ODGen.h"
#include "ODKrnl.h"
#include "ODSync.h"

#ifdef OD_MULTITHREADED
typedef struct
{
   tODMutex state;
#ifdef ODPLAT_WIN32
   HANDLE changed;
   DWORD owner;
#else
   pthread_cond_t changed;
   pthread_t owner;
#endif
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
#ifdef OD_MULTITHREADED
#ifdef ODPLAT_WIN32
   InitializeCriticalSection(&pMutex->cs);
   return(kODRCSuccess);
#else
   return(pthread_mutex_init(&pMutex->mutex, NULL) == 0
      ? kODRCSuccess : kODRCGeneralFailure);
#endif
#else
   (void)pMutex;
   return(kODRCSuccess);
#endif
}

void ODMutexDestroy(tODMutex *pMutex)
{
#ifdef OD_MULTITHREADED
#ifdef ODPLAT_WIN32
   DeleteCriticalSection(&pMutex->cs);
#else
   pthread_mutex_destroy(&pMutex->mutex);
#endif
#else
   (void)pMutex;
#endif
}

void ODMutexLock(tODMutex *pMutex)
{
#ifdef OD_MULTITHREADED
#ifdef ODPLAT_WIN32
   EnterCriticalSection(&pMutex->cs);
#else
   pthread_mutex_lock(&pMutex->mutex);
#endif
#else
   (void)pMutex;
#endif
}

void ODMutexUnlock(tODMutex *pMutex)
{
#ifdef OD_MULTITHREADED
#ifdef ODPLAT_WIN32
   LeaveCriticalSection(&pMutex->cs);
#else
   pthread_mutex_unlock(&pMutex->mutex);
#endif
#else
   (void)pMutex;
#endif
}

#ifdef OD_MULTITHREADED
static void ODControlWakeWaiters(void)
{
#ifdef ODPLAT_WIN32
   SetEvent(ControlLock.changed);
#else
   pthread_cond_broadcast(&ControlLock.changed);
#endif
}

static void ODControlReadAcquire(void)
{
   ODMutexLock(&ControlLock.state);
   while(ControlLock.writer || ControlLock.waiting_writers != 0)
   {
#ifdef ODPLAT_WIN32
      ResetEvent(ControlLock.changed);
      ODMutexUnlock(&ControlLock.state);
      WaitForSingleObject(ControlLock.changed, INFINITE);
      ODMutexLock(&ControlLock.state);
#else
      pthread_cond_wait(&ControlLock.changed, &ControlLock.state.mutex);
#endif
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
#ifdef ODPLAT_WIN32
      ResetEvent(ControlLock.changed);
      ODMutexUnlock(&ControlLock.state);
      WaitForSingleObject(ControlLock.changed, INFINITE);
      ODMutexLock(&ControlLock.state);
#else
      pthread_cond_wait(&ControlLock.changed, &ControlLock.state.mutex);
#endif
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
#ifdef OD_MULTITHREADED
      if(!bODInitialized && nAPILevel == 0 && !bPublicLockPhysical
         && nPublicReadDepth == 0 && nPublicWriteDepth == 0)
      {
#ifdef ODPLAT_WIN32
         ControlLock.owner = GetCurrentThreadId();
#else
         ControlLock.owner = pthread_self();
#endif
      }
#endif
      return(kODRCSuccess);
   }
#ifdef OD_MULTITHREADED
   if(ODMutexInitialize(&ControlLock.state) != kODRCSuccess)
      return(kODRCGeneralFailure);
#ifdef ODPLAT_WIN32
   ControlLock.changed = CreateEvent(NULL, TRUE, FALSE, NULL);
   if(ControlLock.changed == NULL)
   {
      ODMutexDestroy(&ControlLock.state);
      return(kODRCGeneralFailure);
   }
   ControlLock.owner = GetCurrentThreadId();
#else
   if(pthread_cond_init(&ControlLock.changed, NULL) != 0)
   {
      ODMutexDestroy(&ControlLock.state);
      return(kODRCGeneralFailure);
   }
   ControlLock.owner = pthread_self();
#endif
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
#ifdef OD_MULTITHREADED
#ifdef ODPLAT_WIN32
   CloseHandle(ControlLock.changed);
#else
   pthread_cond_destroy(&ControlLock.changed);
#endif
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
#ifdef OD_MULTITHREADED
#ifdef ODPLAT_WIN32
   return(ControlLock.owner == GetCurrentThreadId());
#else
   return(pthread_equal(ControlLock.owner, pthread_self()) != 0);
#endif
#else
   return(TRUE);
#endif
}

BOOL ODSyncSessionActive(void) { return(bSyncActive); }

BOOL ODSyncAPIWriterHeldByCurrentThread(void)
{
#ifdef OD_MULTITHREADED
   return(bSyncActive && ODSyncIsOwnerThread() && nAPILevel != 0);
#else
   return(FALSE);
#endif
}

void ODSyncControlReadLock(void)
{
#ifdef OD_MULTITHREADED
   if(bSyncActive && !(nAPILevel != 0 && ODSyncIsOwnerThread()))
      ODControlReadAcquire();
#endif
}

void ODSyncControlReadUnlock(void)
{
#ifdef OD_MULTITHREADED
   if(bSyncActive && !(nAPILevel != 0 && ODSyncIsOwnerThread()))
      ODControlReadRelease();
#endif
}

void ODSyncControlWriteLock(void)
{
#ifdef OD_MULTITHREADED
   if(bSyncActive && !(nAPILevel != 0 && ODSyncIsOwnerThread()))
      ODControlWriteAcquire();
#endif
}

void ODSyncControlWriteUnlock(void)
{
#ifdef OD_MULTITHREADED
   if(bSyncActive && !(nAPILevel != 0 && ODSyncIsOwnerThread()))
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
   ASSERT(ODSyncIsOwnerThread());
   if(nAPILevel == 0)
   {
      ODReleasePublicLock();
      ODSyncControlWriteLock();
   }
   ++nAPILevel;
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
   ODDispatch(FALSE);
   ODSyncAPIReacquire(nSavedAPILevel);
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
