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
   if(argc != 5)
   {
      int index;
      fprintf(stderr, "spawn child received %d arguments, expected 5\n", argc);
      for(index = 0; index < argc; ++index)
         fprintf(stderr, "spawn child argv[%d] is '%s'\n", index, argv[index]);
      return(17);
   }
   if(strcmp(argv[1], "argument one") != 0)
   {
      fprintf(stderr, "spawn child argv[1] is '%s'\n", argv[1]);
      return(17);
   }
   if(strcmp(argv[2], "argument-two") != 0)
   {
      fprintf(stderr, "spawn child argv[2] is '%s'\n", argv[2]);
      return(17);
   }
   if(strcmp(argv[3], "quote \"inside\"") != 0)
   {
      fprintf(stderr, "spawn child argv[3] is '%s'\n", argv[3]);
      return(17);
   }
   if(strcmp(argv[4], "trailing\\") != 0)
   {
      fprintf(stderr, "spawn child argv[4] is '%s'\n", argv[4]);
      return(17);
   }
   if(value == NULL || strcmp(value, "environment value") != 0)
   {
      fprintf(stderr, "spawn child environment value is '%s'\n",
         value == NULL ? "(unset)" : value);
      return(17);
   }
   file = fopen("ODCHILD.OK", "w");
   if(file == NULL)
      return(18);
   if(fputs("spawn child passed\n", file) < 0 || fclose(file) != 0)
      return(19);
   return(23);
}
