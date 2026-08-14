#include <stdlib.h>

#ifdef _WIN32 /* OpenDoor.h has not yet supplied ODPLAT_WIN32. */
#include <winsock2.h>
#endif

#define OD_ACCEPTANCE_NO_LOCAL_CONFIG
#include "test_support.h"

#ifdef ODPLAT_DOS32
#define OD_ACCEPTANCE_CALLBACK ODCALL
#else
#define OD_ACCEPTANCE_CALLBACK
#endif

static int before_chat_calls;

static void OD_ACCEPTANCE_CALLBACK EndChat(void)
{
   ++before_chat_calls;
   od_control.od_chat_active = FALSE;
}

int main(int argc, char **argv)
{
   tODInputEvent event;
   char answer;
   char input[16];
   char edit_input[4];
   char edit_format[2];
   char menu_choice;
   const char *burst;
   int burst_index;
#if !defined(ODPLAT_DOS) && !defined(ODPLAT_DOS32)
   DWORD_PTR handle;
#endif
#ifdef ODPLAT_WIN32
   struct sockaddr_in address;
   WSADATA data;
   SOCKET connection;
#endif

#if defined(ODPLAT_DOS) || defined(ODPLAT_DOS32)
   (void)argc;
   (void)argv;
#else
   OD_TEST_CHECK(argc == 2);
#ifdef ODPLAT_WIN32
   OD_TEST_CHECK(WSAStartup(MAKEWORD(2, 0), &data) == 0);
   connection = socket(AF_INET, SOCK_STREAM, 0);
   OD_TEST_CHECK(connection != INVALID_SOCKET);
   memset(&address, 0, sizeof(address));
   address.sin_family = AF_INET;
   address.sin_addr.s_addr = inet_addr("127.0.0.1");
   address.sin_port = htons((unsigned short)strtoul(argv[1], NULL, 10));
   OD_TEST_CHECK(connect(connection, (struct sockaddr *)&address,
      sizeof(address)) == 0);
   handle = (DWORD_PTR)connection;
#else
   handle = (DWORD_PTR)strtoul(argv[1], NULL, 10);
#endif
   OD_TEST_CHECK(handle != 0);
#endif
   memset(&od_control, 0, sizeof(od_control));
#if defined(ODPLAT_DOS) || defined(ODPLAT_DOS32)
   OD_TEST_CHECK(od_set_port(0));
#endif
   od_control.baud = 38400;
   od_control.od_connect_speed = 38400;
#if defined(ODPLAT_DOS) || defined(ODPLAT_DOS32)
#ifndef OD_ACCEPTANCE_DOS_FOSSIL
   od_control.od_no_fossil = TRUE;
#endif
   od_control.od_com_flow_control = COM_NO_FLOW;
#else
   od_control.od_use_socket = TRUE;
   od_control.od_open_handle = handle;
#endif
   od_control.od_disable = DIS_INFOFILE | DIS_NAME_PROMPT | DIS_TIMEOUT |
      DIS_LOCAL_INPUT | DIS_SYSOP_KEYS;
#if defined(ODPLAT_DOS) || defined(ODPLAT_DOS32)
   od_control.od_disable |= DIS_CARRIERDETECT;
#endif
   od_control.od_silent_mode = TRUE;
   od_control.od_nocopyright = TRUE;
   od_control.od_noexit = TRUE;
   od_control.od_in_buf_size = 4;
   od_control.user_ansi = TRUE;
   od_init();
#if defined(ODPLAT_DOS) || defined(ODPLAT_DOS32)
#ifdef OD_ACCEPTANCE_DOS_FOSSIL
   OD_TEST_CHECK(od_control.od_com_method == COM_FOSSIL);
#else
   OD_TEST_CHECK(od_control.od_com_method == COM_INTERNAL);
#endif
#else
   OD_TEST_CHECK(od_control.od_com_method == COM_SOCKET);
   OD_TEST_CHECK(od_carrier());
#endif

   od_disp("READY", 5, FALSE);
   OD_TEST_CHECK(od_get_input(&event, 5000, GETIN_NORMAL));
   OD_TEST_CHECK(event.EventType == EVENT_CHARACTER);
   OD_TEST_CHECK(event.bFromRemote);
   OD_TEST_CHECK(event.chKeyPress == 'Q');

   od_disp("ANSWER", 6, FALSE);
   answer = od_get_answer("YN");
   OD_TEST_CHECK(answer == 'Y');

   od_disp("STRING", 6, FALSE);
   memset(input, 0, sizeof(input));
   od_input_str(input, 8, 32, 126);
   OD_TEST_CHECK(strcmp(input, "socket") == 0);

   od_disp("KEY", 3, FALSE);
   od_sleep(250);
   od_kernel();
   burst = "ABCDE";
   for(burst_index = 0; burst[burst_index] != '\0'; ++burst_index)
   {
      OD_TEST_CHECK(od_key_pending());
      OD_TEST_CHECK(od_get_key(FALSE) == burst[burst_index]);
   }
   OD_TEST_CHECK(!od_key_pending());
   OD_TEST_CHECK(od_control.od_last_input == 0);

   od_disp("WAITKEY", 7, FALSE);
   OD_TEST_CHECK(od_get_key(TRUE) == 'W');

   od_disp("CLEAR", 5, FALSE);
   od_sleep(250);
   od_clear_keybuffer();
   OD_TEST_CHECK(!od_key_pending());
   OD_TEST_CHECK(od_get_key(FALSE) == 0);

   od_control.user_ansi = FALSE;
   od_control.user_rip = FALSE;
   od_disp("AUTODETECT", 10, FALSE);
   od_autodetect(DETECT_NORMAL);
   OD_TEST_CHECK(od_control.user_ansi);
   OD_TEST_CHECK(od_control.user_rip);

   od_control.od_page_pausing = FALSE;
   OD_TEST_CHECK(od_send_file("ODFILE.ASC"));
   OD_TEST_CHECK(od_send_file_section("ODSECT.ASC", "SECOND"));
   OD_TEST_CHECK(od_list_files("FILES.BBS"));
   od_disp("HOTMENU", 7, FALSE);
   menu_choice = od_hotkey_menu("ODMENU.ASC", "12Q", TRUE);
   OD_TEST_CHECK(menu_choice == '2');

   edit_input[0] = '\0';
   strcpy(edit_format, "A");
   OD_TEST_CHECK(od_edit_str(edit_input, edit_format, 0, 0, L_WHITE,
      D_GREY, ' ', EDIT_FLAG_NORMAL) == EDIT_RETURN_ERROR);
   OD_TEST_CHECK(od_multiline_edit(NULL, 1, NULL) ==
      OD_MULTIEDIT_ERROR);
   OD_TEST_CHECK(od_popup_menu("Bad", "One", 1, 1, 11, MENU_NORMAL) ==
      POPUP_ERROR);

   od_control.od_okaytopage = PAGE_DISABLE;
   od_page();
   od_control.od_cbefore_chat = EndChat;
   od_chat();
   OD_TEST_CHECK(before_chat_calls == 1);

   od_printf("RESULT %s %d", input, 7);
   od_disp_emu("\x1b[31mEMU\x1b[0m", TRUE);
   od_disp("DONE", 4, FALSE);
#if defined(ODPLAT_DOS) || defined(ODPLAT_DOS32)
   OD_TEST_CHECK(od_get_key(TRUE) == 'X');
#else
   fprintf(stderr, "socket: shutting down\n");
#endif
   od_exit(0, FALSE);
#ifdef ODPLAT_WIN32
   WSACleanup();
#endif
#if !defined(ODPLAT_DOS) && !defined(ODPLAT_DOS32)
   fprintf(stderr, "socket: shut down\n");
#endif
   return(0);
}
