#include <stdlib.h>

#include "test_support.h"

static int before_chat_calls;

static void EndChat(void)
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
   DWORD_PTR handle;

   OD_TEST_CHECK(argc == 2);
#ifdef ODPLAT_WIN32
   handle = (DWORD_PTR)_strtoui64(argv[1], NULL, 10);
#else
   handle = (DWORD_PTR)strtoul(argv[1], NULL, 10);
#endif
   OD_TEST_CHECK(handle != 0);
   memset(&od_control, 0, sizeof(od_control));
   od_control.baud = 38400;
   od_control.od_connect_speed = 38400;
   od_control.od_use_socket = TRUE;
   od_control.od_open_handle = handle;
   od_control.od_disable = DIS_INFOFILE | DIS_NAME_PROMPT | DIS_TIMEOUT |
      DIS_LOCAL_INPUT | DIS_SYSOP_KEYS;
   od_control.od_silent_mode = TRUE;
   od_control.od_nocopyright = TRUE;
   od_control.od_noexit = TRUE;
   od_control.user_ansi = TRUE;
   od_init();
   OD_TEST_CHECK(od_control.od_com_method == COM_SOCKET);
   OD_TEST_CHECK(od_carrier());

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
   od_sleep(50);
   od_kernel();
   OD_TEST_CHECK(od_key_pending());
   OD_TEST_CHECK(od_get_key(FALSE) == 'K');
   OD_TEST_CHECK(od_control.od_last_input == 0);

   od_disp("WAITKEY", 7, FALSE);
   OD_TEST_CHECK(od_get_key(TRUE) == 'W');

   od_disp("CLEAR", 5, FALSE);
   od_sleep(50);
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
   fprintf(stderr, "socket: shutting down\n");
   od_exit(0, FALSE);
   fprintf(stderr, "socket: shut down\n");
   return(0);
}
