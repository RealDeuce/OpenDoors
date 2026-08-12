#include <stdlib.h>

#ifdef _WIN32 /* OpenDoor.h has not yet supplied ODPLAT_WIN32. */
#include <winsock2.h>
#endif

#define OD_ACCEPTANCE_NO_LOCAL_CONFIG
#include "test_support.h"

#ifdef ODPLAT_WIN32
#include <windows.h>
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

static void ODCALL TimeMessage(char *message)
{
   (void)message;
   od_disp("TIME", 4, FALSE);
}

static void PauseWithoutOpenDoors(void)
{
#ifdef ODPLAT_WIN32
   Sleep(6000);
#else
   sleep(6);
#endif
}

int main(int argc, char **argv)
{
   struct sockaddr_in address;
   tODInputEvent event;
#ifdef ODPLAT_WIN32
   WSADATA data;
   SOCKET connection;
#else
   int connection;
#endif

   OD_TEST_CHECK(argc == 2);
#ifdef ODPLAT_WIN32
   OD_TEST_CHECK(WSAStartup(MAKEWORD(2, 0), &data) == 0);
#endif
   connection = socket(AF_INET, SOCK_STREAM, 0);
#ifdef ODPLAT_WIN32
   OD_TEST_CHECK(connection != INVALID_SOCKET);
#else
   OD_TEST_CHECK(connection >= 0);
#endif
   memset(&address, 0, sizeof(address));
   address.sin_family = AF_INET;
   address.sin_addr.s_addr = inet_addr("127.0.0.1");
   address.sin_port = htons((unsigned short)strtoul(argv[1], NULL, 10));
   OD_TEST_CHECK(connect(connection, (struct sockaddr *)&address,
      sizeof(address)) == 0);

   memset(&od_control, 0, sizeof(od_control));
   od_control.baud = 38400;
   od_control.od_connect_speed = 38400;
   od_control.od_use_socket = TRUE;
   od_control.od_open_handle = (DWORD_PTR)connection;
   od_control.od_disable = DIS_INFOFILE | DIS_NAME_PROMPT | DIS_TIMEOUT |
      DIS_LOCAL_INPUT | DIS_SYSOP_KEYS;
   od_control.od_silent_mode = TRUE;
   od_control.od_nocopyright = TRUE;
   od_control.od_noexit = TRUE;
   od_init();
   OD_TEST_CHECK(od_control.od_com_method == COM_SOCKET);
   od_control.od_inactivity = 10;
   od_control.od_inactive_warning = 5;
   od_control.od_inactivity_warning = (char *)"warning";
   od_control.od_time_msg_func = TimeMessage;

   od_disp("ARMED", 5, FALSE);
   PauseWithoutOpenDoors();
   OD_TEST_CHECK(od_get_input(&event, OD_NO_TIMEOUT, GETIN_RAW));
   OD_TEST_CHECK(event.EventType == EVENT_CHARACTER);
   OD_TEST_CHECK(event.bFromRemote);
   OD_TEST_CHECK(event.chKeyPress == 'Z');
   od_disp("DONE", 4, FALSE);
   od_exit(0, FALSE);
#ifdef ODPLAT_WIN32
   WSACleanup();
#endif
   return(0);
}
