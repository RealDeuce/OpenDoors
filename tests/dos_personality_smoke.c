#include <stdio.h>
#include <string.h>

#include "OpenDoor.h"
#include "ODStat.h"

static int nInitializeCount;
static int nDisplayCount;
static int nUpdateCount;
static int nDeinitializeCount;
static int nInitialKeyCount;
static int nCallbackFailure;

static void Progress(int line, int operation)
{
   FILE *progress;

   progress = fopen("PERSSTEP.TXT", "w");
   if(progress != NULL)
   {
      fprintf(progress, "line %d, operation %d\n", line, operation);
      fclose(progress);
   }
}

static int Fail(int line)
{
   FILE *failure;

   failure = fopen("PERSFAIL.TXT", "w");
   if(failure != NULL)
   {
      fprintf(failure, "DOS personality test failed at line %d\n", line);
      fclose(failure);
   }
   return(line);
}

#define CHECK(condition) do { if(!(condition)) return(Fail(__LINE__)); } while(0)

static void TestPersonality(BYTE btOperation)
{
   char abtActual[2];
   char abtExpected[2];

   Progress(__LINE__, btOperation);
   switch(btOperation)
   {
      case PEROP_INITIALIZE:
         ++nInitializeCount;
         nInitialKeyCount = od_control.od_num_keys;
         ODStatAddKey(0x6200);
         break;

      case PEROP_DISPLAY1:
         ++nDisplayCount;
         strcpy(szStatusText, "SDK");
         abtExpected[0] = szStatusText[0];
         abtExpected[1] = 0x1e;
         if(!ODScrnPutText(80, 25, 80, 25, abtExpected)
            || !ODScrnGetText(80, 25, 80, 25, abtActual)
            || memcmp(abtActual, abtExpected, sizeof(abtActual)) != 0)
         {
            nCallbackFailure = TRUE;
         }
         break;

      case PEROP_UPDATE1:
         ++nUpdateCount;
         ODScrnSetAttribute(0x1e);
         ODScrnSetCursorPos(1, 24);
         ODScrnDisplayBuffer(szStatusText, 3);
         ODScrnDisplayChar(' ');
         ODScrnDisplayString("personality");
         ODScrnPrintf(" %u", od_control.user_security);
         break;

      case PEROP_DEINITIALIZE:
         ++nDeinitializeCount;
         ODStatRemoveKey(0x6200);
         break;
   }
}

int main(void)
{
   FILE *sentinel;

   Progress(__LINE__, -1);
   od_control.od_force_local = TRUE;
   od_control.od_disable |= DIS_INFOFILE | DIS_NAME_PROMPT;
   od_control.od_mps = INCLUDE_MPS;

   CHECK(od_add_personality("SDKTEST", 1, 23, TestPersonality));
   Progress(__LINE__, -1);
   od_init();
   Progress(__LINE__, -1);
   CHECK(od_set_personality("sdktest"));
   Progress(__LINE__, -1);
   CHECK(nInitializeCount == 1);
   CHECK(nDisplayCount >= 1);
   CHECK(!nCallbackFailure);
   CHECK(od_control.od_num_keys == nInitialKeyCount + 1);

   ODStatForceStatusUpdate();
   Progress(__LINE__, -1);
   CHECK(nUpdateCount >= 1);

   CHECK(od_set_personality("STANDARD"));
   Progress(__LINE__, -1);
   CHECK(nDeinitializeCount == 1);
   CHECK(od_control.od_num_keys == nInitialKeyCount);

   od_control.od_noexit = TRUE;
   od_exit(0, FALSE);
   Progress(__LINE__, -1);

   sentinel = fopen("PERSPASS.OK", "w");
   CHECK(sentinel != NULL);
   CHECK(fputs("OpenDoors DOS personality tests passed\n", sentinel) >= 0);
   CHECK(fclose(sentinel) == 0);
   return(0);
}
