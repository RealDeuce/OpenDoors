#include <stdlib.h>

#ifdef _WIN32 /* OpenDoor.h has not yet supplied ODPLAT_WIN32. */
#include <winsock2.h>
#include <windows.h>
#elif defined(__MSDOS__) || defined(MSDOS) || defined(__DOS__)
#include <dos.h>
#else
#include <unistd.h>
#endif

#define OD_ACCEPTANCE_NO_LOCAL_CONFIG
#include "test_support.h"

#ifdef ODPLAT_DOS32
#define OD_ACCEPTANCE_CALLBACK ODCALL
#else
#define OD_ACCEPTANCE_CALLBACK
#endif

#define ARRAY_COUNT(array) (sizeof(array) / sizeof((array)[0]))

static int before_chat_calls;
static int after_chat_calls;
static int chat_kernel_calls;
static int time_message_calls;

static void Marker(const char *text)
{
   od_disp(text, (INT)strlen(text), FALSE);
}

static void OD_ACCEPTANCE_CALLBACK BeforeChat(void)
{
   ++before_chat_calls;
}

static void OD_ACCEPTANCE_CALLBACK AfterChat(void)
{
   ++after_chat_calls;
}

static void OD_ACCEPTANCE_CALLBACK StopChat(void)
{
   if(++chat_kernel_calls >= 1000)
      od_control.od_chat_active = FALSE;
}

static void OD_ACCEPTANCE_CALLBACK TimeMessage(char *message)
{
   (void)message;
   ++time_message_calls;
   Marker("TIME-MESSAGE");
}

static void PauseWithoutOpenDoors(unsigned seconds)
{
#ifdef ODPLAT_WIN32
   Sleep((DWORD)seconds * 1000UL);
#elif defined(ODPLAT_DOS) || defined(ODPLAT_DOS32)
   delay(seconds * 1000U);
#else
   sleep(seconds);
#endif
}

static int FinishScenario(const char *marker)
{
   Marker(marker);
   OD_TEST_CHECK(od_get_key(TRUE) == 'X');
   od_exit(0, FALSE);
   return(0);
}

static int RunInputScenario(void)
{
   static const unsigned char burst[] =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-";
   tODInputEvent event;
   size_t index;

   OD_TEST_CHECK(!od_get_input(&event, 0, GETIN_RAW));
   od_control.od_error = ERR_NONE;
   OD_TEST_CHECK(!od_get_input(NULL, 0, GETIN_RAW));
   OD_TEST_CHECK(od_control.od_error == ERR_PARAMETER);

   Marker("INPUT-CHAR");
   OD_TEST_CHECK(od_get_input(&event, OD_NO_TIMEOUT, GETIN_NORMAL));
   OD_TEST_CHECK(event.EventType == EVENT_CHARACTER);
   OD_TEST_CHECK(event.bFromRemote);
   OD_TEST_CHECK(event.chKeyPress == 'Q');

   Marker("INPUT-CTRL-NORMAL");
   OD_TEST_CHECK(od_get_input(&event, OD_NO_TIMEOUT, GETIN_NORMAL));
   OD_TEST_CHECK(event.EventType == EVENT_EXTENDED_KEY);
   OD_TEST_CHECK((unsigned char)event.chKeyPress == OD_KEY_UP);

   Marker("INPUT-CTRL-RAW");
   OD_TEST_CHECK(od_get_input(&event, OD_NO_TIMEOUT, GETIN_RAWCTRL));
   OD_TEST_CHECK(event.EventType == EVENT_CHARACTER);
   OD_TEST_CHECK((unsigned char)event.chKeyPress == 5U);

   Marker("INPUT-SEQUENCE");
   OD_TEST_CHECK(od_get_input(&event, OD_NO_TIMEOUT, GETIN_NORMAL));
   OD_TEST_CHECK(event.EventType == EVENT_EXTENDED_KEY);
   OD_TEST_CHECK((unsigned char)event.chKeyPress == OD_KEY_UP);

   Marker("INPUT-RAW-SEQUENCE");
   OD_TEST_CHECK(od_get_input(&event, OD_NO_TIMEOUT, GETIN_RAW));
   OD_TEST_CHECK(event.EventType == EVENT_CHARACTER && event.chKeyPress == 27);
   OD_TEST_CHECK(od_get_input(&event, OD_NO_TIMEOUT, GETIN_RAW));
   OD_TEST_CHECK(event.EventType == EVENT_CHARACTER && event.chKeyPress == '[');
   OD_TEST_CHECK(od_get_input(&event, OD_NO_TIMEOUT, GETIN_RAW));
   OD_TEST_CHECK(event.EventType == EVENT_CHARACTER && event.chKeyPress == 'A');

   Marker("INPUT-LINEFEED");
   OD_TEST_CHECK(od_get_key(TRUE) == 'R');
   OD_TEST_CHECK(od_control.od_last_input == 0);

   Marker("INPUT-BURST");
   od_sleep(250);
   for(index = 0; index < ARRAY_COUNT(burst) - 1; ++index)
   {
      OD_TEST_CHECK(od_get_input(&event, OD_NO_TIMEOUT, GETIN_RAW));
      OD_TEST_CHECK(event.EventType == EVENT_CHARACTER);
      OD_TEST_CHECK(event.bFromRemote);
      OD_TEST_CHECK((unsigned char)event.chKeyPress == burst[index]);
   }
   OD_TEST_CHECK(!od_key_pending());

   Marker("INPUT-CLEAR");
   od_sleep(250);
   OD_TEST_CHECK(od_key_pending());
   od_clear_keybuffer();
   OD_TEST_CHECK(!od_key_pending());
   OD_TEST_CHECK(od_get_key(FALSE) == 0);

   return(FinishScenario("INPUT-DONE"));
}

static int RunInteractiveScenario(void)
{
   char answer;
   char input[8];
   char edit_input[8];
   char multiline[64];
   WORD edit_result;
   INT menu_result;

   edit_input[0] = '\0';
   OD_TEST_CHECK(od_edit_str(edit_input, "A", 0, 0, L_WHITE,
      D_GREY, ' ', EDIT_FLAG_NORMAL) == EDIT_RETURN_ERROR);
   OD_TEST_CHECK(od_multiline_edit(NULL, 1, NULL) == OD_MULTIEDIT_ERROR);
   OD_TEST_CHECK(od_popup_menu("Bad", "One", 1, 1, 11, MENU_NORMAL) ==
      POPUP_ERROR);

   Marker("INTERACTIVE-ANSWER");
   answer = od_get_answer("YN");
   OD_TEST_CHECK(answer == 'Y');

   Marker("INTERACTIVE-STRING");
   memset(input, 0, sizeof(input));
   od_input_str(input, 4, 32, 126);
   OD_TEST_CHECK(strcmp(input, "aCDE") == 0);

   Marker("INTERACTIVE-HIGH");
   memset(input, 0, sizeof(input));
   od_input_str(input, 2, 0x80, 0xff);
   OD_TEST_CHECK((unsigned char)input[0] == 0x80U);
   OD_TEST_CHECK((unsigned char)input[1] == 0xffU);
   OD_TEST_CHECK(input[2] == '\0');

   Marker("INTERACTIVE-EDIT");
   edit_input[0] = '\0';
   edit_result = od_edit_str(edit_input, "UUU", 2, 2, L_WHITE,
      L_YELLOW, ' ', EDIT_FLAG_NORMAL);
   OD_TEST_CHECK(edit_result == EDIT_RETURN_ACCEPT);
   OD_TEST_CHECK(strcmp(edit_input, "ABC") == 0);

   Marker("INTERACTIVE-EDIT-CANCEL");
   strcpy(edit_input, "OLD");
   edit_result = od_edit_str(edit_input, "UUU", 2, 2, L_WHITE,
      L_YELLOW, ' ', EDIT_FLAG_EDIT_STRING | EDIT_FLAG_ALLOW_CANCEL);
   OD_TEST_CHECK(edit_result == EDIT_RETURN_CANCEL);
   OD_TEST_CHECK(strcmp(edit_input, "OLD") == 0);

   Marker("INTERACTIVE-MULTILINE");
   od_sleep(250);
   multiline[0] = '\0';
   OD_TEST_CHECK(od_multiline_edit(multiline, sizeof(multiline), NULL) ==
      OD_MULTIEDIT_SUCCESS);
   OD_TEST_CHECK(strstr(multiline, "Hi") != NULL);
   OD_TEST_CHECK(strstr(multiline, "There") != NULL);

   Marker("INTERACTIVE-POPUP-HOTKEY");
   menu_result = od_popup_menu("Menu", "^Save|^Load|E^xit", 2, 2, 0,
      MENU_NORMAL);
   OD_TEST_CHECK(menu_result == 1);

   Marker("INTERACTIVE-POPUP-CANCEL");
   menu_result = od_popup_menu("Menu", "^Save|^Load", 2, 2, 0,
      MENU_ALLOW_CANCEL);
   OD_TEST_CHECK(menu_result == POPUP_ESCAPE);

   Marker("INTERACTIVE-POPUP-KEEP");
   menu_result = od_popup_menu("Menu", "^Save|^Load", 2, 2, 0,
      MENU_KEEP);
   OD_TEST_CHECK(menu_result == 2);
   OD_TEST_CHECK(od_popup_menu(NULL, NULL, 0, 0, 0, MENU_DESTROY) ==
      POPUP_ESCAPE);

   od_control.od_okaytopage = PAGE_DISABLE;
   Marker("INTERACTIVE-PAGE-CANCEL");
   od_page();
   OD_TEST_CHECK(od_control.user_numpages == 0);

   Marker("INTERACTIVE-PAGE-DISABLED");
   od_page();
   OD_TEST_CHECK(od_control.user_numpages == 0);
   OD_TEST_CHECK(od_control.user_wantchat);

   od_control.od_cbefore_chat = BeforeChat;
   od_control.od_cafter_chat = AfterChat;
   od_control.od_ker_exec = StopChat;
   Marker("INTERACTIVE-CHAT");
   od_sleep(250);
   od_chat();
   od_control.od_ker_exec = NULL;
   OD_TEST_CHECK(before_chat_calls == 1);
   OD_TEST_CHECK(after_chat_calls == 1);
   OD_TEST_CHECK(!od_control.od_chat_active);

   return(FinishScenario("INTERACTIVE-DONE"));
}

static int RunDisplayScenario(void)
{
   char menu_choice;

   Marker("DISPLAY-OUTPUT");
   od_disp("DISP", 4, FALSE);
   od_disp_str("-STRING");
   od_putch('-');
   od_repeat('R', 3);
   od_printf("-%s-%d", "PRINTF", 7);
   od_disp_emu("\x1b[31m-EMU-\x1b[0m", TRUE);

   od_control.od_page_pausing = FALSE;
   OD_TEST_CHECK(od_send_file("ODFILE"));
   OD_TEST_CHECK(od_send_file_section("ODSECT.ASC", "SECOND"));
   OD_TEST_CHECK(od_list_files("FILES.BBS"));

   od_control.od_error = ERR_NONE;
   OD_TEST_CHECK(!od_send_file("MISSING.ASC"));
   OD_TEST_CHECK(od_control.od_error == ERR_FILEOPEN);
   od_control.od_error = ERR_NONE;
   OD_TEST_CHECK(!od_send_file_section("ODSECT.ASC", "MISSING"));
   OD_TEST_CHECK(od_control.od_error == ERR_NONE);

   Marker("DISPLAY-HOTKEY-WAIT");
   menu_choice = od_hotkey_menu("ODMENU.ASC", "12Q", TRUE);
   OD_TEST_CHECK(menu_choice == '2');

   Marker("DISPLAY-HOTKEY-EARLY");
   OD_TEST_CHECK(od_get_key(TRUE) == 'A');
   menu_choice = od_hotkey_menu("ODLONG.ASC", "Q", FALSE);
   OD_TEST_CHECK(menu_choice == 'Q');

   return(FinishScenario("DISPLAY-DONE"));
}

static int RunSessionScenario(void)
{
   tODInputEvent event;
   int count;

   od_control.user_ansi = FALSE;
   od_control.user_rip = FALSE;
   Marker("SESSION-AUTODETECT");
   od_autodetect(DETECT_NORMAL);
   OD_TEST_CHECK(od_control.user_ansi);
   OD_TEST_CHECK(od_control.user_rip);

   Marker("SESSION-AUTODETECT-KNOWN");
   od_autodetect(DETECT_NORMAL);
   OD_TEST_CHECK(od_control.user_ansi && od_control.user_rip);

   od_control.user_ansi = FALSE;
   od_control.user_rip = FALSE;
   Marker("SESSION-AUTODETECT-FAIL");
   od_autodetect(DETECT_NORMAL);
   OD_TEST_CHECK(!od_control.user_ansi);
   OD_TEST_CHECK(!od_control.user_rip);

   od_control.od_inactivity = 10;
   od_control.od_inactive_warning = 5;
   od_control.od_inactivity_warning = (char *)"warning";
   od_control.od_time_msg_func = TimeMessage;
   Marker("SESSION-TIMER-ARMED");
   PauseWithoutOpenDoors(6);
   OD_TEST_CHECK(od_get_input(&event, OD_NO_TIMEOUT, GETIN_RAW));
   OD_TEST_CHECK(event.EventType == EVENT_CHARACTER);
   OD_TEST_CHECK(event.bFromRemote && event.chKeyPress == 'Z');
   OD_TEST_CHECK(time_message_calls != 0);

   OD_TEST_CHECK(od_carrier());
   Marker("SESSION-DISCONNECT");
   for(count = 0; count < 100 && od_carrier(); ++count)
      od_sleep(50);
   OD_TEST_CHECK(count < 100);
   od_exit(0, FALSE);
   return(0);
}

int main(int argc, char **argv)
{
   const char *scenario;
#if !defined(ODPLAT_DOS) && !defined(ODPLAT_DOS32)
   DWORD_PTR handle;
#endif
#ifdef ODPLAT_WIN32
   struct sockaddr_in address;
   WSADATA data;
   SOCKET connection;
#endif

#if defined(ODPLAT_DOS) || defined(ODPLAT_DOS32)
   OD_TEST_CHECK(argc == 2);
   scenario = argv[1];
#else
   OD_TEST_CHECK(argc == 3);
   scenario = argv[1];
#ifdef ODPLAT_WIN32
   OD_TEST_CHECK(WSAStartup(MAKEWORD(2, 0), &data) == 0);
   connection = socket(AF_INET, SOCK_STREAM, 0);
   OD_TEST_CHECK(connection != INVALID_SOCKET);
   memset(&address, 0, sizeof(address));
   address.sin_family = AF_INET;
   address.sin_addr.s_addr = inet_addr("127.0.0.1");
   address.sin_port = htons((unsigned short)strtoul(argv[2], NULL, 10));
   OD_TEST_CHECK(connect(connection, (struct sockaddr *)&address,
      sizeof(address)) == 0);
   handle = (DWORD_PTR)connection;
#else
   handle = (DWORD_PTR)strtoul(argv[2], NULL, 10);
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
      DIS_LOCAL_INPUT | DIS_SYSOP_KEYS | DIS_CARRIERDETECT;
   od_control.od_silent_mode = TRUE;
   od_control.od_nocopyright = TRUE;
   od_control.od_noexit = TRUE;
   od_control.user_ansi = TRUE;
   if(strcmp(scenario, "input") == 0)
      od_control.od_in_buf_size = 8;
   od_init();

#if defined(ODPLAT_DOS) || defined(ODPLAT_DOS32)
#ifdef OD_ACCEPTANCE_DOS_FOSSIL
   OD_TEST_CHECK(od_control.od_com_method == COM_FOSSIL);
#else
   OD_TEST_CHECK(od_control.od_com_method == COM_INTERNAL);
#endif
#else
   OD_TEST_CHECK(od_control.od_com_method == COM_SOCKET);
#endif
   OD_TEST_CHECK(od_carrier());
   /* DOSBox's usedtr null-modem connects asynchronously after OpenDoors
    * raises DTR. Give that transport time to attach before the first marker;
    * hosted sockets are already connected and merely yield here. */
   od_sleep(500);

   if(strcmp(scenario, "input") == 0)
      return(RunInputScenario());
   if(strcmp(scenario, "interactive") == 0)
      return(RunInteractiveScenario());
   if(strcmp(scenario, "display") == 0)
      return(RunDisplayScenario());
   if(strcmp(scenario, "session") == 0)
      return(RunSessionScenario());

   fprintf(stderr, "unknown transport acceptance scenario: %s\n", scenario);
   return(2);
}
