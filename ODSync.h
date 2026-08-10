/* Internal synchronization and session-owner services. */

#ifndef _INC_ODSYNC
#define _INC_ODSYNC

#include "ODTypes.h"

#ifdef OD_MULTITHREADED
#ifdef ODPLAT_WIN32
#include <windows.h>
typedef struct { CRITICAL_SECTION cs; } tODMutex;
#else
#include <pthread.h>
typedef struct { pthread_mutex_t mutex; } tODMutex;
#endif
#else
typedef struct { int unused; } tODMutex;
#endif

tODResult ODMutexInitialize(tODMutex *pMutex);
void ODMutexDestroy(tODMutex *pMutex);
void ODMutexLock(tODMutex *pMutex);
void ODMutexUnlock(tODMutex *pMutex);

tODResult ODSyncSessionInitialize(void);
void ODSyncInitializationComplete(void);
void ODSyncSessionShutdown(void);
BOOL ODSyncIsOwnerThread(void);
BOOL ODSyncSessionActive(void);
BOOL ODSyncAPIWriterHeldByCurrentThread(void);
void ODSyncAPIEntry(void);
void ODSyncAPIExit(void);
BOOL ODSyncAPICheckpoint(void);
unsigned ODSyncAPIRelease(void);
void ODSyncAPIReacquire(unsigned nSavedAPILevel);

void ODSyncControlReadLock(void);
void ODSyncControlReadUnlock(void);
void ODSyncControlWriteLock(void);
void ODSyncControlWriteUnlock(void);

#endif /* _INC_ODSYNC */
