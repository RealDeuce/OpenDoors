#include <limits.h>

#include "OpenDoor.h"
#include "ODCore.h"
#include "ODGen.h"
#include "ODKrnl.h"
#include "ODPlat.h"

#define CHECK(condition) do { if(!(condition)) return(__LINE__); } while(0)

static int RejectsRectangle(BYTE btLeft, BYTE btTop, BYTE btRight,
   BYTE btBottom)
{
   od_control.od_error = ERR_NONE;
   return(!od_draw_box(btLeft, btTop, btRight, btBottom)
      && od_control.od_error == ERR_PARAMETER);
}

int main(void)
{
#ifdef OD_MULTITHREADED
   CHECK(ODSemaphoreAlloc(&hODActiveSemaphore, 0, INT_MAX) == kODRCSuccess);
#endif

   bODInitialized = TRUE;
   od_control.user_ansi = TRUE;
   od_control.user_avatar = FALSE;

   CHECK(RejectsRectangle(5, 2, 5, 4));
   CHECK(RejectsRectangle(5, 2, 4, 4));
   CHECK(RejectsRectangle(2, 5, 4, 5));
   CHECK(RejectsRectangle(2, 5, 4, 4));

   bODInitialized = FALSE;

#ifdef OD_MULTITHREADED
   ODSemaphoreFree(hODActiveSemaphore);
   hODActiveSemaphore = NULL;
#endif

   return(0);
}
