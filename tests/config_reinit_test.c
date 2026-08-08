#include <stdio.h>
#include <string.h>

#include "OpenDoor.h"
#include "ODCore.h"

#define CONFIG_ONE_FILENAME "ODCFGR1.TMP"
#define CONFIG_TWO_FILENAME "ODCFGR2.TMP"
#define DROP_ONE_FILENAME "ODDRPR1.TMP"
#define DROP_TWO_FILENAME "ODDRPR2.TMP"

static const char *const apszDayNames[] =
{
   "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday",
   "Saturday"
};

static int WriteDropFile(const char *pszFilename, const char *pszUserName)
{
   FILE *pFile;

   pFile = fopen(pszFilename, "w");
   if(pFile == NULL)
      return(0);
   if(fprintf(pFile, "%s\n", pszUserName) < 0)
   {
      fclose(pFile);
      return(0);
   }
   return(fclose(pFile) == 0);
}

static int WriteFirstConfig(void)
{
   FILE *pFile;
   int nDay;
   int nResult = 1;

   pFile = fopen(CONFIG_ONE_FILENAME, "w");
   if(pFile == NULL)
      return(0);
   for(nDay = 0; nDay < 7; ++nDay)
   {
      if(fprintf(pFile, "%sPagingHours 01:02 03:04\n",
         apszDayNames[nDay]) < 0)
      {
         nResult = 0;
      }
   }
   if(fputs("InactivityTimeout 123\n", pFile) < 0
      || fputs("PageDuration 13\n", pFile) < 0
      || fputs("SysopName First Config Sysop\n", pFile) < 0
      || fputs("SystemName First Config System\n", pFile) < 0
      || fputs("Personality FirstPersonality\n", pFile) < 0
      || fputs("CustomFileName " DROP_ONE_FILENAME "\n", pFile) < 0
      || fputs("CustomFileLine UserName\n", pFile) < 0)
   {
      nResult = 0;
   }
   if(fclose(pFile) != 0)
      nResult = 0;
   return(nResult);
}

static int WriteSecondConfig(void)
{
   FILE *pFile;
   int nResult = 1;

   pFile = fopen(CONFIG_TWO_FILENAME, "w");
   if(pFile == NULL)
      return(0);
   if(fputs("CustomFileName " DROP_TWO_FILENAME "\n", pFile) < 0
      || fputs("CustomFileLine UserName\n", pFile) < 0)
   {
      nResult = 0;
   }
   if(fclose(pFile) != 0)
      nResult = 0;
   return(nResult);
}

static void PrepareControl(const char *pszConfigFilename)
{
   memset(&od_control, 0, sizeof(od_control));
   od_control.od_config_file = INCLUDE_CONFIG_FILE;
   od_control.od_config_filename = pszConfigFilename;
   od_control.od_disable = DIS_NAME_PROMPT;
   od_control.od_silent_mode = TRUE;
   od_control.od_noexit = TRUE;
}

int main(void)
{
   int nResult = 0;

#define CHECK(condition) do { if(!(condition)) { nResult = __LINE__; goto cleanup; } } while(0)

   remove(CONFIG_ONE_FILENAME);
   remove(CONFIG_TWO_FILENAME);
   remove(DROP_ONE_FILENAME);
   remove(DROP_TWO_FILENAME);

   CHECK(WriteDropFile(DROP_ONE_FILENAME, "First Caller"));
   CHECK(WriteDropFile(DROP_TWO_FILENAME, "Second Caller"));
   CHECK(WriteFirstConfig());
   CHECK(WriteSecondConfig());

   PrepareControl(CONFIG_ONE_FILENAME);
   od_init();
   CHECK(strcmp(od_control.user_name, "First Caller") == 0);
   CHECK(od_control.od_pagestartmin == 62);
   CHECK(od_control.od_pageendmin == 184);
   CHECK(od_control.od_inactivity == 123);
   CHECK(od_control.od_page_len == 13);
   CHECK(strcmp(od_control.sysop_name, "First Config Sysop") == 0);
   CHECK(strcmp(od_control.system_name, "First Config System") == 0);
   CHECK(strcmp(szDesiredPersonality, "FirstPersonality") == 0);
   od_exit(0, FALSE);

   PrepareControl(CONFIG_TWO_FILENAME);
   od_init();
   CHECK(strcmp(od_control.user_name, "Second Caller") == 0);
   CHECK(od_control.od_pagestartmin == 480);
   CHECK(od_control.od_pageendmin == 1320);
   CHECK(od_control.od_inactivity == 200);
   CHECK(od_control.od_page_len == 15);
   CHECK(strcmp(od_control.sysop_name, "First Config Sysop") != 0);
   CHECK(strcmp(od_control.system_name, "First Config System") != 0);
   CHECK(szDesiredPersonality[0] == '\0');

cleanup:
   if(bODInitialized)
      od_exit(0, FALSE);
   remove(CONFIG_ONE_FILENAME);
   remove(CONFIG_TWO_FILENAME);
   remove(DROP_ONE_FILENAME);
   remove(DROP_TWO_FILENAME);
   return(nResult);
}
