#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "OpenDoor.h"
#include "ODInEx.h"

int main(void)
{
   FILE *pFile = NULL;
   tExitInfoRecord InputRecord;
   tExitInfoRecord OutputRecord;
   DWORD dwMinutes;
   int nHour;
   int nDay;
   int nResult = 0;

#define CHECK(condition) do { if(!(condition)) { nResult = __LINE__; goto cleanup; } } while(0)

   CHECK(!ODGetElapsedMinutes(&dwMinutes, (time_t)-1, (time_t)100));
   CHECK(!ODGetElapsedMinutes(&dwMinutes, (time_t)100, (time_t)-1));
   CHECK(!ODGetElapsedMinutes(&dwMinutes, (time_t)101, (time_t)100));
   CHECK(ODGetElapsedMinutes(&dwMinutes, (time_t)100, (time_t)220));
   CHECK(dwMinutes == 2);

   memset(&InputRecord, 0, sizeof(InputRecord));
   InputRecord.timelimit = 100;
   for(nHour = 0; nHour < 24; ++nHour)
   {
      InputRecord.busyperhour[nHour] = (WORD)(0x100 + nHour);
   }
   for(nDay = 0; nDay < 7; ++nDay)
   {
      InputRecord.busyperday[nDay] = (WORD)(0x200 + nDay);
   }

   pFile = tmpfile();
   CHECK(pFile != NULL);
   CHECK(fwrite(&InputRecord, 1, sizeof(InputRecord) - 1, pFile)
      == sizeof(InputRecord) - 1);
   rewind(pFile);

   CHECK(!ODReadExitInfoPrimitive(pFile, (INT)sizeof(InputRecord)));
   CHECK(pExitInfoRecord == NULL);

   fclose(pFile);
   pFile = NULL;

   pFile = tmpfile();
   CHECK(pFile != NULL);
   CHECK(fwrite(&InputRecord, 1, sizeof(InputRecord), pFile)
      == sizeof(InputRecord));
   rewind(pFile);

   CHECK(ODReadExitInfoPrimitive(pFile, (INT)sizeof(InputRecord)));
   for(nHour = 0; nHour < 24; ++nHour)
   {
      CHECK(od_control.timelog_busyperhour[nHour]
         == (INT16)(0x100 + nHour));
      od_control.timelog_busyperhour[nHour] = (INT16)(0x300 + nHour);
   }
   for(nDay = 0; nDay < 7; ++nDay)
   {
      CHECK(od_control.timelog_busyperday[nDay] == (INT16)(0x200 + nDay));
      od_control.timelog_busyperday[nDay] = (INT16)(0x400 + nDay);
   }

   nStartupUnixTime = (time_t)-1;
   nInitialRemaining = 60;
   od_control.user_timelimit = 55;
   rewind(pFile);
   CHECK(ODWriteExitInfoPrimitive(pFile, (INT)sizeof(OutputRecord)));
   rewind(pFile);
   CHECK(fread(&OutputRecord, 1, sizeof(OutputRecord), pFile)
      == sizeof(OutputRecord));
   CHECK(OutputRecord.timelimit == 100);

   nStartupUnixTime = time(NULL);
   CHECK(nStartupUnixTime != (time_t)-1);
   rewind(pFile);
   CHECK(ODWriteExitInfoPrimitive(pFile, (INT)sizeof(OutputRecord)));
   rewind(pFile);
   CHECK(fread(&OutputRecord, 1, sizeof(OutputRecord), pFile)
      == sizeof(OutputRecord));
   CHECK(OutputRecord.timelimit == 95);

   for(nHour = 0; nHour < 24; ++nHour)
   {
      CHECK(OutputRecord.busyperhour[nHour] == (WORD)(0x300 + nHour));
   }
   for(nDay = 0; nDay < 7; ++nDay)
   {
      CHECK(OutputRecord.busyperday[nDay] == (WORD)(0x400 + nDay));
   }

cleanup:
   if(pFile != NULL)
      fclose(pFile);
   free(pExitInfoRecord);
   pExitInfoRecord = NULL;
   return(nResult);
}
