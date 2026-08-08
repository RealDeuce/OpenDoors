#include <string.h>

#include "OpenDoor.h"
#include "ODGen.h"
#include "ODKrnl.h"

#define CHECK(condition) do { if(!(condition)) return(__LINE__); } while(0)

int main(void)
{
   memset(&od_control, 0, sizeof(od_control));
   od_control.od_chat_active = TRUE;
   od_control.od_error = ERR_NONE;

   CHECK(!ODKrnlChatThreadStartSucceeded(kODRCGeneralFailure));
   CHECK(!od_control.od_chat_active);
   CHECK(od_control.od_error == ERR_GENERALFAILURE);

   od_control.od_chat_active = TRUE;
   od_control.od_error = ERR_NONE;
   CHECK(ODKrnlChatThreadStartSucceeded(kODRCSuccess));
   CHECK(od_control.od_chat_active);
   CHECK(od_control.od_error == ERR_NONE);

   return(0);
}
