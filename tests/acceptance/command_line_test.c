#include <stdlib.h>

#include "test_support.h"

static char custom_keyword[32];
static char custom_options[80];
static INT custom_calls;

static void CustomHandler(char *keyword, char *options)
{
   strncpy(custom_keyword, keyword, sizeof(custom_keyword) - 1);
   custom_keyword[sizeof(custom_keyword) - 1] = '\0';
   strncpy(custom_options, options, sizeof(custom_options) - 1);
   custom_options[sizeof(custom_options) - 1] = '\0';
   ++custom_calls;
}

int main(void)
{
   INT count;
   char **arguments;

   memset(&od_control, 0, sizeof(od_control));
   od_control.od_cmd_line_handler = CustomHandler;
#ifdef ODPLAT_WIN32
   {
      char command[] = "-USERNAME Jane Smith -LOCATION Test Lab "
         "-BBSNAME Acceptance BBS -CUSTOM alpha beta -NODE 7";
      od_parse_cmd_line(command);
   }
#else
   {
      char *argv[] = { "door", "-USERNAME", "Jane", "Smith",
         "-LOCATION", "Test", "Lab", "-BBSNAME", "Acceptance", "BBS",
         "-CUSTOM", "alpha", "beta", "-NODE", "7" };
      od_parse_cmd_line((INT)(sizeof(argv) / sizeof(argv[0])), argv);
   }
#endif
   OD_TEST_CHECK(strcmp(od_control.user_name, "Jane Smith") == 0);
   OD_TEST_CHECK(strcmp(od_control.user_location, "Test Lab") == 0);
   OD_TEST_CHECK(strcmp(od_control.system_name, "Acceptance BBS") == 0);
   OD_TEST_CHECK(od_control.od_node == 7);
   OD_TEST_CHECK(custom_calls == 1);
   OD_TEST_CHECK(strcmp(custom_keyword, "-CUSTOM") == 0);
   OD_TEST_CHECK(strcmp(custom_options, "alpha beta") == 0);

   arguments = od_split_cmd_line("-NAME \"Jane Smith\" -LOCAL", &count);
   OD_TEST_CHECK(arguments != NULL);
   OD_TEST_CHECK(count == 5);
   OD_TEST_CHECK(strcmp(arguments[1], "-NAME") == 0);
   OD_TEST_CHECK(strcmp(arguments[2], "\"Jane") == 0);
   OD_TEST_CHECK(strcmp(arguments[3], "Smith\"") == 0);
   OD_TEST_CHECK(arguments[5] == NULL);
   od_free_split_cmd_line(arguments);
   return(0);
}
