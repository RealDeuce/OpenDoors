#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
   static const char *expected[] = {
      "argument one",
      "",
      "quote\"inside",
      "slashes\\\\\"quote",
      "ends with slash\\",
      "tab\targument",
      "argument-two"
   };
   const char *value = getenv("OD_ACCEPT_VALUE");
   FILE *file;
   int index;
   if(argc == 2 && strcmp(argv[1], "--simple") == 0)
   {
      file = fopen("ODSIMPLE.OK", "w");
      if(file == NULL)
         return(20);
      return(fputs("simple spawn passed\n", file) < 0 || fclose(file) != 0
         ? 21 : 0);
   }
   if(argc != 8)
   {
      fprintf(stderr, "spawn child received %d arguments, expected 8\n", argc);
      for(index = 0; index < argc; ++index)
         fprintf(stderr, "spawn child argv[%d] is '%s'\n", index, argv[index]);
      return(17);
   }
   for(index = 0; index < 7; ++index)
   {
      if(strcmp(argv[index + 1], expected[index]) != 0)
      {
         fprintf(stderr, "spawn child argv[%d] is '%s', expected '%s'\n",
            index + 1, argv[index + 1], expected[index]);
         return(17);
      }
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
