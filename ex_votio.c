#include "ex_votio.h"

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#if defined(ODPLAT_DOS) || defined(ODPLAT_DOS32)
#include <dos.h>
#include <io.h>
#elif defined(ODPLAT_WIN32)
#include <io.h>
#include <windows.h>
#else
#include <unistd.h>
#endif

#ifndef O_BINARY
#define O_BINARY 0
#endif

#ifndef S_IREAD
#define S_IREAD S_IRUSR
#endif
#ifndef S_IWRITE
#define S_IWRITE S_IWUSR
#endif

typedef enum
{
   kVoteLockError = -1,
   kVoteLockHeld = 0,
   kVoteLockAcquired = 1
} tVoteLockResult;

static void VotePutDWORD(unsigned char *pachBuffer, DWORD uValue)
{
   pachBuffer[0] = (unsigned char)(uValue & 0xffUL);
   pachBuffer[1] = (unsigned char)((uValue >> 8) & 0xffUL);
   pachBuffer[2] = (unsigned char)((uValue >> 16) & 0xffUL);
   pachBuffer[3] = (unsigned char)((uValue >> 24) & 0xffUL);
}

static DWORD VoteGetDWORD(const unsigned char *pachBuffer)
{
   return((DWORD)pachBuffer[0]
      | ((DWORD)pachBuffer[1] << 8)
      | ((DWORD)pachBuffer[2] << 16)
      | ((DWORD)pachBuffer[3] << 24));
}

static BOOL VoteStringTerminated(const char *pszString, size_t nSize)
{
   return(memchr(pszString, '\0', nSize) != NULL);
}

BOOL VoteUserRecordEncode(unsigned char *pachRecord,
   const tUserRecord *pUser)
{
   int nQuestion;

   if(pachRecord == NULL || pUser == NULL
      || !VoteStringTerminated(pUser->szUserName,
         sizeof(pUser->szUserName)))
   {
      return(FALSE);
   }

   for(nQuestion = 0; nQuestion < MAX_QUESTIONS; ++nQuestion)
   {
      if(pUser->bVotedOnQuestion[nQuestion] > 1)
      {
         return(FALSE);
      }
   }

   memcpy(pachRecord, pUser->szUserName, sizeof(pUser->szUserName));
   memcpy(pachRecord + sizeof(pUser->szUserName),
      pUser->bVotedOnQuestion, sizeof(pUser->bVotedOnQuestion));
   return(TRUE);
}

tVoteIOResult VoteUserRecordDecode(tUserRecord *pUser,
   const unsigned char *pachRecord)
{
   int nQuestion;

   if(pUser == NULL || pachRecord == NULL)
   {
      return(kVoteIOError);
   }
   if(memchr(pachRecord, '\0', sizeof(pUser->szUserName)) == NULL)
   {
      return(kVoteIOInvalid);
   }
   for(nQuestion = 0; nQuestion < MAX_QUESTIONS; ++nQuestion)
   {
      if(pachRecord[sizeof(pUser->szUserName) + nQuestion] > 1)
      {
         return(kVoteIOInvalid);
      }
   }

   memcpy(pUser->szUserName, pachRecord, sizeof(pUser->szUserName));
   memcpy(pUser->bVotedOnQuestion,
      pachRecord + sizeof(pUser->szUserName),
      sizeof(pUser->bVotedOnQuestion));
   return(kVoteIOSuccess);
}

BOOL VoteQuestionRecordEncode(unsigned char *pachRecord,
   const tQuestionRecord *pQuestion)
{
   int nAnswer;

   if(pachRecord == NULL || pQuestion == NULL
      || pQuestion->nTotalAnswers < 0
      || pQuestion->nTotalAnswers > MAX_ANSWERS
      || pQuestion->bCanAddAnswers > 1
      || !VoteStringTerminated(pQuestion->szQuestion,
         sizeof(pQuestion->szQuestion))
      || !VoteStringTerminated(pQuestion->szCreatorName,
         sizeof(pQuestion->szCreatorName)))
   {
      return(FALSE);
   }
   for(nAnswer = 0; nAnswer < pQuestion->nTotalAnswers; ++nAnswer)
   {
      if(!VoteStringTerminated(pQuestion->aszAnswer[nAnswer],
         sizeof(pQuestion->aszAnswer[nAnswer])))
      {
         return(FALSE);
      }
   }

   memset(pachRecord, 0, VOTE_QUESTION_RECORD_SIZE);
   memcpy(pachRecord, pQuestion->szQuestion,
      sizeof(pQuestion->szQuestion));
   for(nAnswer = 0; nAnswer < pQuestion->nTotalAnswers; ++nAnswer)
   {
      memcpy(pachRecord + 72 + nAnswer * 32,
         pQuestion->aszAnswer[nAnswer],
         sizeof(pQuestion->aszAnswer[nAnswer]));
   }
   VotePutDWORD(pachRecord + 552, (DWORD)pQuestion->nTotalAnswers);
   for(nAnswer = 0; nAnswer < pQuestion->nTotalAnswers; ++nAnswer)
   {
      VotePutDWORD(pachRecord + 556 + nAnswer * 4,
         pQuestion->auVotesForAnswer[nAnswer]);
   }
   VotePutDWORD(pachRecord + 616, pQuestion->uTotalVotes);
   VotePutDWORD(pachRecord + 620, pQuestion->bCanAddAnswers);
   memcpy(pachRecord + 624, pQuestion->szCreatorName,
      sizeof(pQuestion->szCreatorName));
   VotePutDWORD(pachRecord + 660, pQuestion->uCreationTime);
   return(TRUE);
}

tVoteIOResult VoteQuestionRecordDecode(tQuestionRecord *pQuestion,
   const unsigned char *pachRecord)
{
   DWORD uAnswers;
   DWORD bCanAddAnswers;
   int nAnswer;

   if(pQuestion == NULL || pachRecord == NULL)
   {
      return(kVoteIOError);
   }
   uAnswers = VoteGetDWORD(pachRecord + 552);
   bCanAddAnswers = VoteGetDWORD(pachRecord + 620);
   if(uAnswers > MAX_ANSWERS || bCanAddAnswers > 1
      || memchr(pachRecord, '\0', 72) == NULL
      || memchr(pachRecord + 624, '\0', 36) == NULL)
   {
      return(kVoteIOInvalid);
   }
   for(nAnswer = 0; nAnswer < (int)uAnswers; ++nAnswer)
   {
      if(memchr(pachRecord + 72 + nAnswer * 32, '\0', 32) == NULL)
      {
         return(kVoteIOInvalid);
      }
   }

   memset(pQuestion, 0, sizeof(*pQuestion));
   memcpy(pQuestion->szQuestion, pachRecord, 72);
   for(nAnswer = 0; nAnswer < (int)uAnswers; ++nAnswer)
   {
      memcpy(pQuestion->aszAnswer[nAnswer],
         pachRecord + 72 + nAnswer * 32,
         sizeof(pQuestion->aszAnswer[nAnswer]));
   }
   pQuestion->nTotalAnswers = (INT32)uAnswers;
   for(nAnswer = 0; nAnswer < (int)uAnswers; ++nAnswer)
   {
      pQuestion->auVotesForAnswer[nAnswer]
         = VoteGetDWORD(pachRecord + 556 + nAnswer * 4);
   }
   pQuestion->uTotalVotes = VoteGetDWORD(pachRecord + 616);
   pQuestion->bCanAddAnswers = bCanAddAnswers;
   memcpy(pQuestion->szCreatorName, pachRecord + 624, 36);
   pQuestion->uCreationTime = VoteGetDWORD(pachRecord + 660);
   return(kVoteIOSuccess);
}

static tVoteIOResult VoteRecordRead(FILE *pfFile,
   unsigned char *pachRecord, size_t nSize)
{
   size_t nRead;

   nRead = fread(pachRecord, 1, nSize, pfFile);
   if(nRead == nSize)
   {
      return(kVoteIOSuccess);
   }
   if(nRead == 0 && feof(pfFile))
   {
      return(kVoteIOEnd);
   }
   return(ferror(pfFile) ? kVoteIOError : kVoteIOInvalid);
}

tVoteIOResult VoteUserRecordRead(FILE *pfFile, tUserRecord *pUser)
{
   unsigned char achRecord[VOTE_USER_RECORD_SIZE];
   tVoteIOResult Result;

   Result = VoteRecordRead(pfFile, achRecord, sizeof(achRecord));
   return(Result == kVoteIOSuccess
      ? VoteUserRecordDecode(pUser, achRecord) : Result);
}

BOOL VoteUserRecordWrite(FILE *pfFile, const tUserRecord *pUser)
{
   unsigned char achRecord[VOTE_USER_RECORD_SIZE];

   return(VoteUserRecordEncode(achRecord, pUser)
      && fwrite(achRecord, sizeof(achRecord), 1, pfFile) == 1);
}

BOOL VoteUserRecordSeek(FILE *pfFile, long nRecord)
{
   return(nRecord >= 0
      && fseek(pfFile, nRecord * VOTE_USER_RECORD_SIZE, SEEK_SET) == 0);
}

tVoteIOResult VoteQuestionRecordRead(FILE *pfFile,
   tQuestionRecord *pQuestion)
{
   unsigned char achRecord[VOTE_QUESTION_RECORD_SIZE];
   tVoteIOResult Result;

   Result = VoteRecordRead(pfFile, achRecord, sizeof(achRecord));
   return(Result == kVoteIOSuccess
      ? VoteQuestionRecordDecode(pQuestion, achRecord) : Result);
}

BOOL VoteQuestionRecordWrite(FILE *pfFile,
   const tQuestionRecord *pQuestion)
{
   unsigned char achRecord[VOTE_QUESTION_RECORD_SIZE];

   return(VoteQuestionRecordEncode(achRecord, pQuestion)
      && fwrite(achRecord, sizeof(achRecord), 1, pfFile) == 1);
}

BOOL VoteQuestionRecordSeek(FILE *pfFile, long nRecord)
{
   return(nRecord >= 0
      && fseek(pfFile, nRecord * VOTE_QUESTION_RECORD_SIZE, SEEK_SET) == 0);
}

static BOOL VoteFileValidationStart(FILE *pfFile, size_t nRecordSize,
   long nMaximumRecords, long *pnOriginal)
{
   long nLength;

   *pnOriginal = ftell(pfFile);
   if(*pnOriginal < 0 || fseek(pfFile, 0, SEEK_END) != 0
      || (nLength = ftell(pfFile)) < 0
      || nLength % (long)nRecordSize != 0
      || nLength / (long)nRecordSize > nMaximumRecords
      || fseek(pfFile, 0, SEEK_SET) != 0)
   {
      return(FALSE);
   }
   return(TRUE);
}

BOOL VoteUserFileValidate(FILE *pfFile)
{
   tUserRecord User;
   tVoteIOResult Result;
   long nOriginal;
   BOOL bValid;

   if(!VoteFileValidationStart(pfFile, VOTE_USER_RECORD_SIZE, MAX_USERS,
      &nOriginal))
   {
      return(FALSE);
   }
   while((Result = VoteUserRecordRead(pfFile, &User)) == kVoteIOSuccess)
   {
   }
   bValid = Result == kVoteIOEnd;
   clearerr(pfFile);
   return(fseek(pfFile, nOriginal, SEEK_SET) == 0 && bValid);
}

BOOL VoteQuestionFileValidate(FILE *pfFile)
{
   tQuestionRecord Question;
   tVoteIOResult Result;
   long nOriginal;
   BOOL bValid;

   if(!VoteFileValidationStart(pfFile, VOTE_QUESTION_RECORD_SIZE,
      MAX_QUESTIONS, &nOriginal))
   {
      return(FALSE);
   }
   while((Result = VoteQuestionRecordRead(pfFile, &Question))
      == kVoteIOSuccess)
   {
   }
   bValid = Result == kVoteIOEnd;
   clearerr(pfFile);
   return(fseek(pfFile, nOriginal, SEEK_SET) == 0 && bValid);
}

void VoteLockOwnerInitialize(tVoteLockOwner *pOwner, const char *pszLabel)
{
   memset(pOwner, 0, sizeof(*pOwner));
   strncpy(pOwner->szLabel, pszLabel, sizeof(pOwner->szLabel) - 1);
#if defined(ODPLAT_DOS) || defined(ODPLAT_DOS32)
   strcpy(pOwner->szProcessKind, "PSP");
   pOwner->uProcess = (DWORD)_psp;
#elif defined(ODPLAT_WIN32)
   strcpy(pOwner->szProcessKind, "PID");
   pOwner->uProcess = (DWORD)GetCurrentProcessId();
#else
   strcpy(pOwner->szProcessKind, "PID");
   pOwner->uProcess = (DWORD)getpid();
#endif
   pOwner->uStarted = (DWORD)time(NULL);
   pOwner->uNonce = (DWORD)clock()
      ^ (DWORD)(DWORD_PTR)(const void *)pOwner;
}

#if defined(MULTINODE_AWARE) && !defined(ODPLAT_WIN32)
static BOOL VoteWriteAll(int hFile, const char *pszText)
{
   size_t nLeft = strlen(pszText);
   const char *pszCurrent = pszText;

   while(nLeft > 0)
   {
      int nWritten = write(hFile, pszCurrent, (unsigned int)nLeft);
      if(nWritten <= 0)
      {
         return(FALSE);
      }
      pszCurrent += nWritten;
      nLeft -= (size_t)nWritten;
   }
   return(TRUE);
}
#endif

#ifdef MULTINODE_AWARE
static BOOL VoteReadOwner(const char *pszLockName, char *pszOwner,
   size_t nOwnerSize)
{
   FILE *pfLock;
   size_t nRead;
   size_t nIndex;

   pfLock = fopen(pszLockName, "rb");
   if(pfLock == NULL)
   {
      return(FALSE);
   }
   nRead = fread(pszOwner, 1, nOwnerSize - 1, pfLock);
   fclose(pfLock);
   pszOwner[nRead] = '\0';
   for(nIndex = 0; nIndex < nRead; ++nIndex)
   {
      if(pszOwner[nIndex] == '\r' || pszOwner[nIndex] == '\n')
      {
         pszOwner[nIndex] = '\0';
         break;
      }
      if(!isprint((unsigned char)pszOwner[nIndex]))
      {
         pszOwner[nIndex] = '?';
      }
   }
   return(nRead > 0);
}

static BOOL VoteReleaseLock(tVoteFile *pFile)
{
   char szCurrentOwner[128];

   if(!VoteReadOwner(pFile->szLockName, szCurrentOwner,
      sizeof(szCurrentOwner))
      || strcmp(szCurrentOwner, pFile->szOwner) != 0)
   {
      strcpy(pFile->szError, "Lock ownership changed before release");
      return(FALSE);
   }
   if(remove(pFile->szLockName) != 0)
   {
      strcpy(pFile->szError, "Unable to remove owned lock");
      return(FALSE);
   }
   pFile->bLocked = FALSE;
   return(TRUE);
}

static tVoteLockResult VoteCreateLock(const char *pszLockName,
   const char *pszOwner)
{
#ifdef ODPLAT_WIN32
   HANDLE hFile;
   DWORD dwWritten;
   DWORD dwLength = (DWORD)strlen(pszOwner);

   hFile = CreateFileA(pszLockName, GENERIC_WRITE, 0, NULL, CREATE_NEW,
      FILE_ATTRIBUTE_NORMAL, NULL);
   if(hFile == INVALID_HANDLE_VALUE)
   {
      return(GetLastError() == ERROR_FILE_EXISTS
         || GetLastError() == ERROR_ALREADY_EXISTS
         ? kVoteLockHeld : kVoteLockError);
   }
   if(!WriteFile(hFile, pszOwner, dwLength, &dwWritten, NULL)
      || dwWritten != dwLength)
   {
      CloseHandle(hFile);
      DeleteFileA(pszLockName);
      return(kVoteLockError);
   }
   CloseHandle(hFile);
   return(kVoteLockAcquired);
#else
   int hFile;

   hFile = open(pszLockName, O_WRONLY | O_CREAT | O_EXCL | O_BINARY,
      S_IREAD | S_IWRITE);
   if(hFile == -1)
   {
      return(errno == EEXIST ? kVoteLockHeld : kVoteLockError);
   }
   if(!VoteWriteAll(hFile, pszOwner))
   {
      close(hFile);
      remove(pszLockName);
      return(kVoteLockError);
   }
   close(hFile);
   return(kVoteLockAcquired);
#endif
}

#if defined(ODPLAT_NIX)
static BOOL VoteLinkUnsupported(int nError)
{
   return(nError == EPERM || nError == EACCES || nError == EXDEV
#ifdef EOPNOTSUPP
      || nError == EOPNOTSUPP
#endif
#ifdef ENOTSUP
      || nError == ENOTSUP
#endif
      );
}

static tVoteLockResult VoteLinkLock(const char *pszLockName,
   const char *pszOwner, tVoteLockOwner *pOwner)
{
   char szTemporary[256];
   char szExisting[128];
   struct stat Status;
   int hFile;
   int nLinkError;

   sprintf(szTemporary, "%s.%s.%lu.%08lX.%lu", pszLockName,
      pOwner->szLabel,
      (unsigned long)pOwner->uProcess,
      (unsigned long)pOwner->uNonce,
      (unsigned long)pOwner->uSequence);
   hFile = open(szTemporary, O_WRONLY | O_CREAT | O_EXCL | O_BINARY,
      S_IREAD | S_IWRITE);
   if(hFile == -1)
   {
      return(kVoteLockError);
   }
   if(!VoteWriteAll(hFile, pszOwner))
   {
      close(hFile);
      remove(szTemporary);
      return(kVoteLockError);
   }
   (void)fsync(hFile);
   close(hFile);

   if(link(szTemporary, pszLockName) == 0)
   {
      remove(szTemporary);
      return(kVoteLockAcquired);
   }
   nLinkError = errno;
   if(stat(szTemporary, &Status) == 0 && Status.st_nlink >= 2
      && VoteReadOwner(pszLockName, szExisting, sizeof(szExisting))
      && strcmp(szExisting, pszOwner) == 0)
   {
      remove(szTemporary);
      return(kVoteLockAcquired);
   }
   remove(szTemporary);
   if(nLinkError == EEXIST)
   {
      return(kVoteLockHeld);
   }
   if(VoteLinkUnsupported(nLinkError))
   {
      return(VoteCreateLock(pszLockName, pszOwner));
   }
   return(kVoteLockError);
}
#endif

static tVoteLockResult VoteTryLock(const char *pszLockName,
   const char *pszOwner, tVoteLockOwner *pOwner)
{
#if defined(ODPLAT_NIX)
   return(VoteLinkLock(pszLockName, pszOwner, pOwner));
#else
   (void)pOwner;
   return(VoteCreateLock(pszLockName, pszOwner));
#endif
}
#endif

BOOL VoteFileOpen(tVoteFile *pFile, const char *pszDataName,
   const char *pszLockName, const char *pszMode, tVoteLockOwner *pOwner,
   unsigned int nWaitSeconds, tVoteIdleCallback pfIdle)
{
#ifdef MULTINODE_AWARE
   time_t StartTime;
   tVoteLockResult LockResult;
   char szCurrentOwner[128];
#endif

   memset(pFile, 0, sizeof(*pFile));
#ifdef MULTINODE_AWARE
   ++pOwner->uSequence;
   sprintf(pFile->szOwner,
      "%s %s=%lu ODVOTELOCK=1 TOKEN=%08lX-%08lX-%08lX-%08lX",
      pOwner->szLabel, pOwner->szProcessKind,
      (unsigned long)pOwner->uProcess,
      (unsigned long)pOwner->uProcess,
      (unsigned long)pOwner->uStarted,
      (unsigned long)pOwner->uNonce,
      (unsigned long)pOwner->uSequence);
   strncpy(pFile->szLockName, pszLockName,
      sizeof(pFile->szLockName) - 1);
   StartTime = time(NULL);
   for(;;)
   {
      LockResult = VoteTryLock(pszLockName, pFile->szOwner, pOwner);
      if(LockResult == kVoteLockAcquired)
      {
         pFile->bLocked = TRUE;
         break;
      }
      if(LockResult == kVoteLockError)
      {
         sprintf(pFile->szError, "Unable to create lock %s", pszLockName);
         return(FALSE);
      }
      if(nWaitSeconds == 0
         || difftime(time(NULL), StartTime) >= nWaitSeconds)
      {
         if(VoteReadOwner(pszLockName, szCurrentOwner,
            sizeof(szCurrentOwner)))
         {
            sprintf(pFile->szError, "%s is locked by %.120s",
               pszDataName, szCurrentOwner);
         }
         else
         {
            sprintf(pFile->szError, "%s has an unreadable lock",
               pszDataName);
         }
         return(FALSE);
      }
      if(pfIdle != NULL)
      {
         (*pfIdle)();
      }
   }
#else
   (void)pszLockName;
   (void)pOwner;
   (void)nWaitSeconds;
   (void)pfIdle;
#endif

   pFile->pfFile = fopen(pszDataName, pszMode);
   if(pFile->pfFile == NULL)
   {
#ifdef MULTINODE_AWARE
      if(pFile->bLocked)
      {
         if(!VoteReleaseLock(pFile))
         {
            return(FALSE);
         }
      }
#endif
      sprintf(pFile->szError, "Unable to open %s", pszDataName);
      return(FALSE);
   }
   return(TRUE);
}

BOOL VoteFileClose(tVoteFile *pFile)
{
   BOOL bSuccess = TRUE;

   if(pFile->pfFile != NULL)
   {
      if(fclose(pFile->pfFile) != 0)
      {
         bSuccess = FALSE;
      }
      pFile->pfFile = NULL;
   }
#ifdef MULTINODE_AWARE
   if(pFile->bLocked && !VoteReleaseLock(pFile))
   {
      bSuccess = FALSE;
   }
#endif
   return(bSuccess);
}

const char *VoteFileError(const tVoteFile *pFile)
{
   return(pFile->szError);
}
