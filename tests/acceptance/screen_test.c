#include <stdlib.h>

#include "test_support.h"

#define LEGACY_SCREEN_BUFFER_SIZE (4U + 80U * 25U * 2U)

int main(void)
{
   unsigned char cells[12];
   unsigned char readback[12];
   unsigned char *snapshot;
   unsigned char *legacy_snapshot;
   void *window;
   DWORD snapshot_size;
   INT row;
   INT column;
   int index;

   ODTestConfigureLocal();
   od_control.user_ansi = TRUE;
   od_control.user_screenwidth = 100;
   od_control.user_screen_length = 30;
   od_control.od_info_type = CUSTOM;
   od_control.baud = 1;
   od_init();

   for(index = 0; index < 6; ++index)
   {
      cells[index * 2] = (unsigned char)('A' + index);
      cells[index * 2 + 1] = (unsigned char)(L_WHITE | (D_BLUE << 4));
   }
   OD_TEST_CHECK(od_puttext(94, 2, 99, 2, cells));
   memset(readback, 0, sizeof(readback));
   OD_TEST_CHECK(od_gettext(94, 2, 99, 2, readback));
   OD_TEST_CHECK(memcmp(cells, readback, sizeof(cells)) == 0);

   od_set_cursor(5, 100);
   od_get_cursor(&row, &column);
   OD_TEST_CHECK(row == 5 && column == 100);
   od_set_color(L_YELLOW, D_BLUE);
   OD_TEST_CHECK(od_control.od_cur_attrib == (L_YELLOW | (D_BLUE << 4)));
   od_set_attrib(L_CYAN | (D_RED << 4));
   OD_TEST_CHECK(od_control.od_cur_attrib == (L_CYAN | (D_RED << 4)));
   od_disp("XY", 2, TRUE);
   od_disp_str("Z");
   od_putch('!');
   od_repeat('.', 3);
   od_printf("%s %d", "value", 7);
   od_disp_emu("plain", TRUE);
   od_emulate('E');
   od_clr_line();

   OD_TEST_CHECK(od_draw_box(2, 3, 20, 8));
   window = od_window_create(25, 3, 45, 8, "Title", L_WHITE, L_YELLOW,
      D_BLUE, 0);
   OD_TEST_CHECK(window != NULL);
   OD_TEST_CHECK(od_window_remove(window));
   OD_TEST_CHECK(od_scroll(2, 3, 20, 8, 1, SCROLL_NORMAL));

   snapshot_size = od_save_screen_size();
   OD_TEST_CHECK(snapshot_size != 0);
   snapshot = (unsigned char *)malloc((size_t)snapshot_size);
   OD_TEST_CHECK(snapshot != NULL);
   OD_TEST_CHECK(!od_save_screen_ex(snapshot, snapshot_size - 1));
   OD_TEST_CHECK(od_control.od_error == ERR_PARAMETER);
   OD_TEST_CHECK(od_save_screen_ex(snapshot, snapshot_size));
   od_clr_scr();
   OD_TEST_CHECK(od_restore_screen_ex(snapshot, snapshot_size));
   free(snapshot);

   legacy_snapshot = (unsigned char *)malloc(LEGACY_SCREEN_BUFFER_SIZE);
   OD_TEST_CHECK(legacy_snapshot != NULL);
   OD_TEST_CHECK(od_save_screen(legacy_snapshot));
   od_clr_scr();
   OD_TEST_CHECK(od_restore_screen(legacy_snapshot));
   free(legacy_snapshot);

   od_exit(0, FALSE);
   return(0);
}
