#ifndef OPENDOORS_ACCEPTANCE_TEST_SUPPORT_H
#define OPENDOORS_ACCEPTANCE_TEST_SUPPORT_H

#include <stdio.h>
#include <string.h>

#include <OpenDoor.h>

#define OD_TEST_CHECK(condition) do { if(!(condition)) { \
   fprintf(stderr, "%s:%d: check failed: %s\n", __FILE__, __LINE__, \
      #condition); \
   return(__LINE__); \
} } while(0)

static void ODTestConfigureLocal(void)
{
   memset(&od_control, 0, sizeof(od_control));
   od_control.od_force_local = TRUE;
   od_control.od_disable = DIS_INFOFILE | DIS_NAME_PROMPT |
      DIS_CARRIERDETECT | DIS_TIMEOUT | DIS_LOCAL_INPUT;
   od_control.od_silent_mode = TRUE;
   od_control.od_nocopyright = TRUE;
   od_control.od_noexit = TRUE;
}

#endif
