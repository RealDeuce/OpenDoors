#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
   const char *value = getenv("OD_ACCEPT_VALUE");
   FILE *file;
   if(argc == 2 && strcmp(argv[1], "--simple") == 0)
   {
      file = fopen("ODSIMPLE.OK", "w");
      if(file == NULL)
         return(20);
      return(fputs("simple spawn passed\n", file) < 0 || fclose(file) != 0
         ? 21 : 0);
   }
   if(argc != 3 || strcmp(argv[1], "argument one") != 0 ||
      strcmp(argv[2], "argument-two") != 0 || value == NULL ||
      strcmp(value, "environment value") != 0)
      return(17);
   file = fopen("ODCHILD.OK", "w");
   if(file == NULL)
      return(18);
   if(fputs("spawn child passed\n", file) < 0 || fclose(file) != 0)
      return(19);
   return(23);
}
