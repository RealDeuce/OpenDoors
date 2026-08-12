#define UT_CUSTOM_MOCK_ODReleasePublicLock
#ifdef OD_MULTITHREADED
#define UT_CUSTOM_MOCK_ODMutexDestroy
#define UT_CUSTOM_MOCK_ODMutexInitialize
#ifdef ODPLAT_WIN32
#define UT_CUSTOM_MOCK_CreateEventA
#define UT_CUSTOM_MOCK_GetCurrentThreadId
#else
#define UT_CUSTOM_MOCK_pthread_cond_init
#define UT_CUSTOM_MOCK_pthread_self
#endif
#endif

static unsigned ut_releases;
static BOOL ut_release_clears;
#ifdef OD_MULTITHREADED
static tODResult ut_mutex_result;
static unsigned ut_mutex_initializes;
static unsigned ut_mutex_destroys;
#ifdef ODPLAT_WIN32
static HANDLE ut_event;
static unsigned ut_event_calls;
static DWORD ut_thread;
static unsigned ut_self_calls;
#else
static int ut_condition_result;
static unsigned ut_condition_calls;
static pthread_t ut_thread;
static unsigned ut_self_calls;
#endif
#endif

void utm_ODReleasePublicLock(void)
{
   ++ut_releases;
   if(ut_release_clears) bPublicLockPhysical = FALSE;
}

#ifdef OD_MULTITHREADED
tODResult utm_ODMutexInitialize(tODMutex *mutex)
{
   UT_ASSERT_EQ_PTR(&ControlLock.state, mutex);
   ++ut_mutex_initializes;
   return ut_mutex_result;
}

void utm_ODMutexDestroy(tODMutex *mutex)
{
   UT_ASSERT_EQ_PTR(&ControlLock.state, mutex);
   ++ut_mutex_destroys;
}

#ifdef ODPLAT_WIN32
HANDLE WINAPI utm_CreateEventA(LPSECURITY_ATTRIBUTES attributes,
   BOOL manual_reset, BOOL initial_state, LPCSTR name)
{
   UT_ASSERT(attributes == NULL);
   UT_ASSERT_EQ_INT(TRUE, manual_reset);
   UT_ASSERT_EQ_INT(FALSE, initial_state);
   UT_ASSERT(name == NULL);
   ++ut_event_calls;
   return ut_event;
}

DWORD WINAPI utm_GetCurrentThreadId(void)
{
   ++ut_self_calls;
   return ut_thread;
}
#else
int utm_pthread_cond_init(pthread_cond_t *condition,
   const pthread_condattr_t *attributes)
{
   UT_ASSERT_EQ_PTR(&ControlLock.changed, condition);
   UT_ASSERT(attributes == NULL);
   ++ut_condition_calls;
   return ut_condition_result;
}

pthread_t utm_pthread_self(void)
{
   ++ut_self_calls;
   return ut_thread;
}
#endif
#endif

static void reset_initialization(void)
{
   bSyncActive = FALSE;
   bODInitialized = FALSE;
   bPublicLockPhysical = FALSE;
   nAPILevel = 7;
   nPublicReadDepth = 0;
   nPublicWriteDepth = 0;
   bDispatching = TRUE;
   ut_releases = 0;
   ut_release_clears = TRUE;
#ifdef OD_MULTITHREADED
   ut_mutex_result = kODRCSuccess;
   ut_mutex_initializes = 0;
   ut_mutex_destroys = 0;
   ut_self_calls = 0;
#ifdef ODPLAT_WIN32
   ut_event = (HANDLE)1;
   ut_event_calls = 0;
   ut_thread = 42;
#else
   ut_condition_result = 0;
   ut_condition_calls = 0;
#endif
#endif
}

static void initializes_a_fresh_session_and_unwinds_failures(void)
{
   reset_initialization();
#ifdef OD_MULTITHREADED
   ut_mutex_result = kODRCGeneralFailure;
   UT_ASSERT_EQ_INT(kODRCGeneralFailure, utt_ODSyncSessionInitialize());
   UT_ASSERT_EQ_INT(FALSE, bSyncActive);

   reset_initialization();
#ifdef ODPLAT_WIN32
   ut_event = NULL;
#else
   ut_condition_result = 1;
#endif
   UT_ASSERT_EQ_INT(kODRCGeneralFailure, utt_ODSyncSessionInitialize());
   UT_ASSERT_EQ_UINT(1, ut_mutex_destroys);
   UT_ASSERT_EQ_INT(FALSE, bSyncActive);
   reset_initialization();
#endif
   UT_ASSERT_EQ_INT(kODRCSuccess, utt_ODSyncSessionInitialize());
   UT_ASSERT_EQ_INT(TRUE, bSyncActive);
   UT_ASSERT_EQ_UINT(0, nAPILevel);
   UT_ASSERT_EQ_INT(FALSE, bDispatching);
   UT_ASSERT_EQ_INT(FALSE, bPublicLockPhysical);
#ifdef OD_MULTITHREADED
   UT_ASSERT_EQ_UINT(1, ut_self_calls);
   UT_ASSERT_EQ_UINT(0, ControlLock.readers);
   UT_ASSERT_EQ_UINT(0, ControlLock.waiting_writers);
   UT_ASSERT_EQ_INT(FALSE, ControlLock.writer);
#ifdef ODPLAT_WIN32
   UT_ASSERT_EQ_UINT(42, ControlLock.owner);
   UT_ASSERT_EQ_PTR(ut_event, ControlLock.changed);
#endif
#endif
}

static void use_active_session(BOOL initialized, unsigned api_level,
   BOOL physical, unsigned read_depth, unsigned write_depth,
   BOOL release_clears)
{
   reset_initialization();
   bSyncActive = TRUE;
   bODInitialized = initialized;
   nAPILevel = api_level;
   bPublicLockPhysical = physical;
   nPublicReadDepth = read_depth;
   nPublicWriteDepth = write_depth;
   ut_release_clears = release_clears;
   UT_ASSERT_EQ_INT(kODRCSuccess, utt_ODSyncSessionInitialize());
}

static void releases_a_stale_public_lock_on_reuse(void)
{
   use_active_session(TRUE, 0, TRUE, 0, 0, TRUE);
   UT_ASSERT_EQ_UINT(0, ut_releases);
   use_active_session(FALSE, 0, FALSE, 0, 0, TRUE);
   UT_ASSERT_EQ_UINT(0, ut_releases);
   use_active_session(FALSE, 0, TRUE, 0, 0, TRUE);
   UT_ASSERT_EQ_UINT(1, ut_releases);
}

#ifdef OD_MULTITHREADED
static void rebinds_only_a_completely_idle_uninitialized_session(void)
{
   use_active_session(FALSE, 0, FALSE, 0, 0, TRUE);
   UT_ASSERT_EQ_UINT(1, ut_self_calls);
   use_active_session(TRUE, 0, FALSE, 0, 0, TRUE);
   UT_ASSERT_EQ_UINT(0, ut_self_calls);
   use_active_session(FALSE, 1, FALSE, 0, 0, TRUE);
   UT_ASSERT_EQ_UINT(0, ut_self_calls);
   use_active_session(FALSE, 0, TRUE, 0, 0, FALSE);
   UT_ASSERT_EQ_UINT(0, ut_self_calls);
   use_active_session(FALSE, 0, FALSE, 1, 0, TRUE);
   UT_ASSERT_EQ_UINT(0, ut_self_calls);
   use_active_session(FALSE, 0, FALSE, 0, 1, TRUE);
   UT_ASSERT_EQ_UINT(0, ut_self_calls);
}
#endif

static const UTTestCase ut_cases[] = {
   {"fresh session", initializes_a_fresh_session_and_unwinds_failures},
   {"stale public lock", releases_a_stale_public_lock_on_reuse},
#ifdef OD_MULTITHREADED
   {"owner rebind", rebinds_only_a_completely_idle_uninitialized_session},
#endif
};
