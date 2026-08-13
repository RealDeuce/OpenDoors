#define UT_CUSTOM_MOCK_GetCommandLineA
#define UT_CUSTOM_MOCK_calloc
#define UT_CUSTOM_MOCK_free
#if defined(ODPLAT_NIX) || defined(ODPLAT_WIN32)
#define UT_CUSTOM_MOCK_isspace
#endif
#define UT_CUSTOM_MOCK_malloc
#define UT_CUSTOM_MOCK_memcpy
#define UT_CUSTOM_MOCK_realloc
#define UT_CUSTOM_MOCK_strdup
#define UT_CUSTOM_MOCK_strstr
#define UT_CUSTOM_MOCK_ODSyncPublicCallAllowed

static char *ut_arguments[4097];
static char ut_program[128];
static char ut_command[9000];
static char ut_long_command[9000];
static unsigned ut_strdup_calls;
static unsigned ut_free_calls;
static int ut_strdup_fail_call;
static BOOL ut_calloc_fails;
static BOOL ut_malloc_fails;
static BOOL ut_realloc_fails;
static BOOL ut_public_call_allowed;

BOOL utm_ODSyncPublicCallAllowed(void)
{
   if(!ut_public_call_allowed)
      od_control.od_error = ERR_GENERALFAILURE;
   return ut_public_call_allowed;
}
#ifdef ODPLAT_WIN32
static LPSTR ut_full_command;
static BOOL ut_program_was_allocated;
#endif

void *utm_calloc(size_t count, size_t size)
{
   UT_ASSERT_EQ_UINT(4097, count);
   UT_ASSERT_EQ_UINT(sizeof(char *), size);
   if(ut_calloc_fails)
      return NULL;
   memset(ut_arguments, 0, sizeof(ut_arguments));
   return ut_arguments;
}

void utm_free(void *memory)
{
   UT_ASSERT_NOT_NULL(memory);
   ++ut_free_calls;
}

void *utm_realloc(void *memory, size_t size)
{
   UT_ASSERT_EQ_PTR(ut_arguments, memory);
   UT_ASSERT(size >= 2 * sizeof(char *));
   if(ut_realloc_fails)
      return NULL;
   return memory;
}

char *utm_strdup(const char *text)
{
   char *destination;
   ++ut_strdup_calls;
   if((int)ut_strdup_calls == ut_strdup_fail_call)
      return NULL;
   destination = ut_strdup_calls == 1
#ifdef ODPLAT_WIN32
      && !ut_program_was_allocated
#endif
      ? ut_program : ut_command;
   UT_ASSERT(strlen(text) < (ut_strdup_calls == 1 ? sizeof(ut_program) :
      sizeof(ut_command)));
   strcpy(destination, text);
   return destination;
}

#if defined(ODPLAT_NIX) || defined(ODPLAT_WIN32)
int utm_isspace(int value)
{
   return value == ' ' || value == '\t' || value == '\n' ||
      value == '\r' || value == '\f' || value == '\v';
}
#endif

#ifdef ODPLAT_WIN32
LPSTR WINAPI utm_GetCommandLineA(void)
{
   return ut_full_command;
}

void *utm_malloc(size_t size)
{
   UT_ASSERT(size <= sizeof(ut_program));
   ut_program_was_allocated = !ut_malloc_fails;
   return ut_malloc_fails ? NULL : ut_program;
}

void *utm_memcpy(void *destination, const void *source, size_t size)
{
   unsigned char *out = (unsigned char *)destination;
   const unsigned char *in = (const unsigned char *)source;
   size_t index;
   for(index = 0; index < size; ++index) out[index] = in[index];
   return destination;
}

char *utm_strstr(const char *haystack, const char *needle)
{
   size_t length = strlen(needle);
   while(*haystack != '\0')
   {
      if(strncmp(haystack, needle, length) == 0) return (char *)haystack;
      ++haystack;
   }
   return length == 0 ? (char *)haystack : NULL;
}
#endif

static void reset_fixture(void)
{
   memset(&od_control, 0, sizeof(od_control));
   memset(ut_program, 0, sizeof(ut_program));
   memset(ut_command, 0, sizeof(ut_command));
   ut_strdup_calls = 0;
   ut_free_calls = 0;
   ut_strdup_fail_call = 0;
   ut_calloc_fails = FALSE;
   ut_malloc_fails = FALSE;
   ut_realloc_fails = FALSE;
   ut_public_call_allowed = TRUE;
#ifdef ODPLAT_WIN32
   ut_full_command = "door.exe";
   ut_program_was_allocated = FALSE;
#endif
}

static void rejects_invalid_parameters_and_allocation_failures(void)
{
   INT count = 99;
   char **result;
   reset_fixture();
   result = utt_od_split_cmd_line(NULL, &count);
   UT_ASSERT_NULL(result);
   UT_ASSERT_EQ_INT(0, count);
   UT_ASSERT_EQ_INT(ERR_PARAMETER, od_control.od_error);

   reset_fixture();
   result = utt_od_split_cmd_line("value", NULL);
   UT_ASSERT_NULL(result);
   UT_ASSERT_EQ_INT(ERR_PARAMETER, od_control.od_error);

   reset_fixture();
   ut_calloc_fails = TRUE;
   result = utt_od_split_cmd_line("value", &count);
   UT_ASSERT_NULL(result);
   UT_ASSERT_EQ_INT(ERR_MEMORY, od_control.od_error);

   reset_fixture();
   ut_strdup_fail_call = 1;
   result = utt_od_split_cmd_line("value", &count);
   UT_ASSERT_NULL(result);
   UT_ASSERT_EQ_INT(0, count);
   UT_ASSERT_EQ_INT(ERR_MEMORY, od_control.od_error);

   reset_fixture();
   ut_strdup_fail_call = 2;
   result = utt_od_split_cmd_line("value", &count);
   UT_ASSERT_NULL(result);
   UT_ASSERT_EQ_INT(0, count);
   UT_ASSERT_EQ_INT(ERR_MEMORY, od_control.od_error);
   UT_ASSERT_EQ_UINT(2, ut_free_calls);

#ifdef ODPLAT_WIN32
   reset_fixture();
   ut_full_command = NULL;
   ut_strdup_fail_call = 1;
   result = utt_od_split_cmd_line("value", &count);
   UT_ASSERT_NULL(result);
   UT_ASSERT_EQ_INT(0, count);
   UT_ASSERT_EQ_INT(ERR_MEMORY, od_control.od_error);
   UT_ASSERT_EQ_UINT(1, ut_free_calls);
#endif
}

static void rejects_a_terminal_session(void)
{
   INT count = 99;
   reset_fixture();
   ut_public_call_allowed = FALSE;
   UT_ASSERT_NULL(utt_od_split_cmd_line("value", &count));
   UT_ASSERT_EQ_INT(ERR_GENERALFAILURE, od_control.od_error);
   UT_ASSERT_EQ_INT(99, count);
}

static void splits_whitespace_and_handles_realloc_failure(void)
{
   INT count;
   char **result;
   reset_fixture();
   result = utt_od_split_cmd_line("one \t two", &count);
   UT_ASSERT_EQ_PTR(ut_arguments, result);
   UT_ASSERT_EQ_INT(3, count);
   UT_ASSERT_EQ_INT(0, strcmp("one", result[1]));
   UT_ASSERT_EQ_INT(0, strcmp("two", result[2]));

   reset_fixture();
   ut_realloc_fails = TRUE;
   result = utt_od_split_cmd_line("", &count);
   UT_ASSERT_EQ_PTR(ut_arguments, result);
   UT_ASSERT_EQ_INT(1, count);
}

static void stops_at_the_documented_argument_limit(void)
{
   INT count;
   char **result;
   unsigned position = 0;
   reset_fixture();
   while(position + 2 < sizeof(ut_long_command))
   {
      ut_long_command[position++] = 'x';
      ut_long_command[position++] = ' ';
   }
   ut_long_command[position] = '\0';
   result = utt_od_split_cmd_line(ut_long_command, &count);
   UT_ASSERT_NOT_NULL(result);
   UT_ASSERT_EQ_INT(4096, count);
}

#ifdef ODPLAT_WIN32
static void obtains_program_name_from_the_full_command_line(void)
{
   INT count;
   char **result;
   reset_fixture();
   ut_full_command = "door.exe one two";
   result = utt_od_split_cmd_line("one two", &count);
   UT_ASSERT_NOT_NULL(result);
   UT_ASSERT_EQ_INT(0, strcmp("door.exe", result[0]));

   reset_fixture();
   ut_full_command = NULL;
   result = utt_od_split_cmd_line("one", &count);
   UT_ASSERT_EQ_INT(0, strcmp("", result[0]));

   reset_fixture();
   ut_full_command = "one";
   result = utt_od_split_cmd_line("one", &count);
   UT_ASSERT_EQ_INT(0, strcmp("", result[0]));

   reset_fixture();
   ut_full_command = "door.exe unrelated";
   result = utt_od_split_cmd_line("missing", &count);
   UT_ASSERT_EQ_INT(0, strcmp("", result[0]));
}

static void reports_program_name_allocation_failure(void)
{
   INT count = 99;
   reset_fixture();
   ut_full_command = "door.exe one";
   ut_malloc_fails = TRUE;
   UT_ASSERT_NULL(utt_od_split_cmd_line("one", &count));
   UT_ASSERT_EQ_INT(0, count);
   UT_ASSERT_EQ_INT(ERR_MEMORY, od_control.od_error);
}
#endif

static const UTTestCase ut_cases[] = {
   {"terminal session", rejects_a_terminal_session},
   {"invalid parameters and allocations", rejects_invalid_parameters_and_allocation_failures},
   {"split whitespace", splits_whitespace_and_handles_realloc_failure},
   {"argument limit", stops_at_the_documented_argument_limit},
#ifdef ODPLAT_WIN32
   {"program name", obtains_program_name_from_the_full_command_line},
   {"program allocation", reports_program_name_allocation_failure},
#endif
};
