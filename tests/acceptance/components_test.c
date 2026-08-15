#include "test_support.h"

#define CONFIG_FILE "ODACCEPT.CFG"
#define LOG_FILE "ODACCEPT.LOG"

static int custom_calls;
static int personality_initialize_calls;

static void ConfigComplete(void)
{
}

static void CustomConfig(char *keyword, char *options)
{
   if(strcmp(keyword, "ACCEPTANCEOPTION") == 0 &&
      strcmp(options, "recognized value") == 0)
      ++custom_calls;
}

static OD_PERSONALITY_PROC TestPersonality;

static void TestPersonality(BYTE operation)
{
   if(operation == PEROP_INITIALIZE)
      ++personality_initialize_calls;
}

int main(void)
{
   FILE *file;
   char log_contents[1024];
   size_t length;

   remove(CONFIG_FILE);
   remove(LOG_FILE);
   file = fopen(CONFIG_FILE, "w");
   OD_TEST_CHECK(file != NULL);
   OD_TEST_CHECK(fputs("MaximumDoorTime 37\n", file) >= 0);
   OD_TEST_CHECK(fputs("AcceptanceOption recognized value\n", file) >= 0);
   OD_TEST_CHECK(fclose(file) == 0);

   ODTestConfigureLocal();
   od_control.od_config_file = INCLUDE_CONFIG_FILE;
   od_control.od_config_filename = CONFIG_FILE;
   od_control.od_config_function = CustomConfig;
   od_control.od_config_callback = ConfigComplete;
   od_control.od_logfile = INCLUDE_LOGFILE;
   strcpy(od_control.od_logfile_name, LOG_FILE);
   od_control.od_mps = INCLUDE_MPS;
   od_init();

   OD_TEST_CHECK(od_control.od_maxtime == 37);
   OD_TEST_CHECK(custom_calls == 1);
   OD_TEST_CHECK(od_control.od_config_callback == ConfigComplete);
   od_control.od_logfile_disable = TRUE;
   OD_TEST_CHECK(od_log_write("disabled acceptance log record"));
   od_control.od_logfile_disable = FALSE;
   OD_TEST_CHECK(od_log_open());
   OD_TEST_CHECK(od_log_write("acceptance log record"));
#if defined(ODPLAT_DOS) || defined(ODPLAT_DOS32) || defined(ODPLAT_WIN32)
   OD_TEST_CHECK(od_add_personality("Acceptance", 1, 1, TestPersonality));
   OD_TEST_CHECK(od_set_personality("Acceptance"));
   OD_TEST_CHECK(personality_initialize_calls == 1);
   OD_TEST_CHECK(!od_set_personality("Missing"));
   OD_TEST_CHECK(od_control.od_error == ERR_LIMIT);
#else
   OD_TEST_CHECK(!od_add_personality("Acceptance", 1, 1, TestPersonality));
   OD_TEST_CHECK(od_control.od_error == ERR_UNSUPPORTED);
#endif

   od_exit(0, FALSE);
   file = fopen(LOG_FILE, "r");
   OD_TEST_CHECK(file != NULL);
   length = fread(log_contents, 1, sizeof(log_contents) - 1, file);
   OD_TEST_CHECK(!ferror(file));
   log_contents[length] = '\0';
   OD_TEST_CHECK(fclose(file) == 0);
   OD_TEST_CHECK(strstr(log_contents, "acceptance log record") != NULL);
   pdef_opendoors(PEROP_INITIALIZE);
   pdef_opendoors(PEROP_DEINITIALIZE);
   pdef_pcboard(PEROP_INITIALIZE);
   pdef_pcboard(PEROP_DEINITIALIZE);
   pdef_ra(PEROP_INITIALIZE);
   pdef_ra(PEROP_DEINITIALIZE);
   pdef_wildcat(PEROP_INITIALIZE);
   pdef_wildcat(PEROP_DEINITIALIZE);
   remove(CONFIG_FILE);
   remove(LOG_FILE);
   return(0);
}
