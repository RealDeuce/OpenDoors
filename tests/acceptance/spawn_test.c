#include <errno.h>
#include <stdlib.h>

#include "test_support.h"

int main(int argc, char **argv)
{
   const char *child_args[4];
   const char *child_environment[2];
   FILE *file;
   INT16 result;
   char command[512];

   OD_TEST_CHECK(argc == 2);
   remove("ODCHILD.OK");
   remove("ODSIMPLE.OK");
   ODTestConfigureLocal();
   od_init();

   child_args[0] = argv[1];
   child_args[1] = "argument one";
   child_args[2] = "argument-two";
   child_args[3] = NULL;
   child_environment[0] = "OD_ACCEPT_VALUE=environment value";
   child_environment[1] = NULL;
   result = od_spawnvpe(P_WAIT, argv[1], child_args, child_environment);
   if(result != 23)
      fprintf(stderr, "od_spawnvpe returned %d (errno %d)\n",
         (int)result, errno);
   OD_TEST_CHECK(result == 23);
   file = fopen("ODCHILD.OK", "r");
   OD_TEST_CHECK(file != NULL);
   OD_TEST_CHECK(fclose(file) == 0);
   remove("ODCHILD.OK");

   OD_TEST_CHECK(strlen(argv[1]) + sizeof(" --simple") < sizeof(command));
   strcpy(command, argv[1]);
   strcat(command, " --simple");
   OD_TEST_CHECK(od_spawn(command));
   file = fopen("ODSIMPLE.OK", "r");
   OD_TEST_CHECK(file != NULL);
   OD_TEST_CHECK(fclose(file) == 0);
   remove("ODSIMPLE.OK");

   errno = 0;
   result = od_spawnvpe(P_WAIT, "opendoors-acceptance-missing-program",
      child_args, child_environment);
   OD_TEST_CHECK(result == -1);
   od_exit(0, FALSE);
   return(0);
}
