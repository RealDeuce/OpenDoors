#define UT_CUSTOM_MOCK_fclose
#define UT_CUSTOM_MOCK_ferror
#define UT_CUSTOM_MOCK_fgetc
#define UT_CUSTOM_MOCK_fopen
#define UT_CUSTOM_MOCK_free
#define UT_CUSTOM_MOCK_malloc
#define UT_CUSTOM_MOCK_realloc
#define UT_CUSTOM_MOCK_strcasecmp
#define UT_CUSTOM_MOCK_strcmp
#define UT_CUSTOM_MOCK_stricmp
#define UT_CUSTOM_MOCK_ODBBSDevAbsolutePath
#define UT_CUSTOM_MOCK_ODBBSDevCopyText
#define UT_CUSTOM_MOCK_ODBBSDevLanguageTagValid
#define UT_CUSTOM_MOCK_od_set_user_8bit

#define UT_FILE_CAPACITY 8192
#ifdef ODPLAT_NIX
#define UT_BBSDEV_PATH "/node/BBSDEV.DRP"
#else
#define UT_BBSDEV_PATH "C:\\node\\BBSDEV.DRP"
#endif

static char ut_file[UT_FILE_CAPACITY];
static char ut_memory[UT_FILE_CAPACITY];
static size_t ut_file_size;
static size_t ut_file_pos;
static BOOL ut_open_fails;
static BOOL ut_read_fails;
static BOOL ut_malloc_fails;
static BOOL ut_realloc_fails;
static unsigned ut_close_calls;
static unsigned ut_free_calls;
static unsigned ut_user_8bit_calls;
static BOOL ut_user_8bit;
static BOOL ut_language_valid;
static BOOL ut_path_valid;
static const char *ut_fields[19];

int utm_strcmp(const char *left, const char *right);

static int compare_no_case(const char *left, const char *right)
{
   unsigned char left_char;
   unsigned char right_char;
   do
   {
      left_char = (unsigned char)*left++;
      right_char = (unsigned char)*right++;
      if(left_char >= 'a' && left_char <= 'z')
         left_char = (unsigned char)(left_char - 'a' + 'A');
      if(right_char >= 'a' && right_char <= 'z')
         right_char = (unsigned char)(right_char - 'a' + 'A');
   } while(left_char != '\0' && left_char == right_char);
   return (int)left_char - (int)right_char;
}

int utm_strcasecmp(const char *left, const char *right)
{
   return compare_no_case(left, right);
}

int utm_stricmp(const char *left, const char *right)
{
   return compare_no_case(left, right);
}

static BOOL utm_ODBBSDevLanguageTagValid(const char *language)
{
   UT_ASSERT(utm_strcmp(ut_fields[12], language) == 0);
   return ut_language_valid;
}

static void utm_ODBBSDevCopyText(char *destination, size_t size,
   const char *source)
{
   size_t index = 0;
   UT_ASSERT(size != 0);
   while(source[index] != '\0' && index + 1 < size)
   {
      destination[index] = source[index];
      ++index;
   }
   destination[index] = '\0';
}

static BOOL utm_ODBBSDevAbsolutePath(const char *path)
{
   return ut_path_valid && path != NULL && path[0] != '\0';
}

FILE *utm_fopen(const char *path, const char *mode)
{
   UT_ASSERT(utm_strcmp(UT_BBSDEV_PATH, path) == 0);
   UT_ASSERT(utm_strcmp("rb", mode) == 0);
   return ut_open_fails ? NULL : (FILE *)ut_file;
}

int utm_strcmp(const char *left, const char *right)
{
   while(*left != '\0' && *left == *right)
   {
      ++left;
      ++right;
   }
   return (unsigned char)*left - (unsigned char)*right;
}

int utm_fgetc(FILE *file)
{
   UT_ASSERT_EQ_PTR((FILE *)ut_file, file);
   if(ut_file_pos == ut_file_size)
      return EOF;
   return (unsigned char)ut_file[ut_file_pos++];
}

int utm_ferror(FILE *file)
{
   UT_ASSERT_EQ_PTR((FILE *)ut_file, file);
   return ut_read_fails;
}

int utm_fclose(FILE *file)
{
   UT_ASSERT_EQ_PTR((FILE *)ut_file, file);
   ++ut_close_calls;
   return 0;
}

void *utm_malloc(size_t size)
{
   UT_ASSERT(size <= sizeof(ut_memory));
   return ut_malloc_fails ? NULL : ut_memory;
}

void *utm_realloc(void *memory, size_t size)
{
   UT_ASSERT_EQ_PTR(ut_memory, memory);
   UT_ASSERT(size <= sizeof(ut_memory));
   return ut_realloc_fails ? NULL : ut_memory;
}

void utm_free(void *memory)
{
   UT_ASSERT_EQ_PTR(ut_memory, memory);
   ++ut_free_calls;
}

BOOL ODCALL utm_od_set_user_8bit(BOOL enabled)
{
   ++ut_user_8bit_calls;
   ut_user_8bit = enabled;
   return TRUE;
}

static void append_text(const char *text)
{
   while(*text != '\0')
   {
      UT_ASSERT(ut_file_size < sizeof(ut_file));
      ut_file[ut_file_size++] = *text++;
   }
}

static void reset_input(void)
{
   memset(&od_control, 0, sizeof(od_control));
   memset(ut_file, 0, sizeof(ut_file));
   memset(ut_memory, 0, sizeof(ut_memory));
   ut_file_size = ut_file_pos = 0;
   ut_open_fails = ut_read_fails = FALSE;
   ut_malloc_fails = ut_realloc_fails = FALSE;
   ut_close_calls = ut_free_calls = 0;
   ut_user_8bit_calls = 0;
   ut_user_8bit = FALSE;
   ut_language_valid = TRUE;
   ut_path_valid = TRUE;
   bBBSDevDeadlineSet = FALSE;
   bBBSDevSession = FALSE;
   nBBSDevComMethod = kComMethodUnspecified;
   pszBBSDevStorage = NULL;
   pszBBSDevUserID = NULL;
}

static void build_fields(void)
{
   unsigned index;
   for(index = 0; index < DIM(ut_fields); ++index)
   {
      append_text(ut_fields[index]);
      append_text("\n");
   }
}

static void set_fields(void)
{
   reset_input();
   ut_fields[0] = "1.0";
   ut_fields[1] = "local";
   ut_fields[2] = "";
   ut_fields[3] = "Alias";
   ut_fields[4] = "user-key";
   ut_fields[5] = "80";
   ut_fields[6] = "24";
   ut_fields[7] = "Y";
   ut_fields[8] = "N";
   ut_fields[9] = "1.332";
   ut_fields[10] = "";
   ut_fields[11] = "IBM437";
   ut_fields[12] = "en-US";
   ut_fields[13] = "Example BBS 1.0";
   ut_fields[14] = "Example Board";
   ut_fields[15] = "Sysop";
   ut_fields[16] = "42";
   ut_fields[17] = "7";
   ut_fields[18] = "Y";
}

static void set_field(unsigned field, const char *value)
{
   set_fields();
   ut_fields[field] = value;
   build_fields();
}

static void set_core(const char *mode, const char *parameters,
   const char *deadline)
{
   set_fields();
   ut_fields[1] = mode;
   ut_fields[2] = parameters;
   ut_fields[10] = deadline;
   build_fields();
}

static BOOL parse(void)
{
   return utt_ODInitReadBBSDevDropFile(UT_BBSDEV_PATH);
}

static void reject_field(unsigned field, const char *value)
{
   set_field(field, value);
   UT_ASSERT(!parse());
}

static void accept_field(unsigned field, const char *value)
{
   set_field(field, value);
   UT_ASSERT(parse());
}

static void accepts_core(void)
{
   set_core("local", "", "2030-02-28T23:59:59Z");
   UT_ASSERT(parse());
   UT_ASSERT_EQ_INT(BBSDEVDRP, od_control.od_info_type);
   UT_ASSERT(utm_strcmp("Alias", od_control.user_name) == 0);
   UT_ASSERT(utm_strcmp("Alias", od_control.user_handle) == 0);
   UT_ASSERT(utm_strcmp("Example Board", od_control.system_name) == 0);
   UT_ASSERT(utm_strcmp("Sysop", od_control.sysop_name) == 0);
   UT_ASSERT_EQ_INT(80, od_control.user_screenwidth);
   UT_ASSERT_EQ_INT(24, od_control.user_screen_length);
   UT_ASSERT_EQ_INT(TRUE, od_control.user_ansi);
   UT_ASSERT_EQ_INT(FALSE, od_control.user_rip);
   UT_ASSERT_EQ_INT(42, od_control.user_security);
   UT_ASSERT_EQ_INT(7, od_control.od_node);
   UT_ASSERT_EQ_INT(TRUE, od_control.od_force_local);
   UT_ASSERT_EQ_INT(TRUE, bBBSDevSession);
   UT_ASSERT_EQ_INT(TRUE, bBBSDevDeadlineSet);
   UT_ASSERT_EQ_INT((time_t)1898553599L, nBBSDevDeadline);
   UT_ASSERT_EQ_UINT(1, ut_user_8bit_calls);
   UT_ASSERT_EQ_INT(TRUE, ut_user_8bit);
   UT_ASSERT_EQ_UINT(1, ut_close_calls);
   UT_ASSERT_EQ_UINT(0, ut_free_calls);

   ut_file_pos = 0;
   UT_ASSERT(parse());
   UT_ASSERT_EQ_UINT(1, ut_free_calls);

   set_core("local", "", "");
   UT_ASSERT(parse());
   UT_ASSERT_EQ_INT(FALSE, bBBSDevDeadlineSet);
   UT_ASSERT_EQ_INT(0, od_control.user_timelimit);
}

static void accepts_extensions(void)
{
   set_core("local", "", "");
   ut_file[1] = '.';
   ut_file[2] = '7';
   append_text("extension\n");
   UT_ASSERT(parse());

   set_core("local", "", "");
   {
      size_t in_pos;
      size_t out_pos = 0;
      for(in_pos = 0; in_pos < ut_file_size; ++in_pos)
      {
         if(ut_file[in_pos] == '\n')
            ut_memory[out_pos++] = '\r';
         ut_memory[out_pos++] = ut_file[in_pos];
      }
      memcpy(ut_file, ut_memory, out_pos);
      memset(ut_memory, 0, sizeof(ut_memory));
      ut_file_size = out_pos;
   }
   UT_ASSERT(parse());
}

static void rejects_structure(void)
{
   set_core("local", "", "");
   UT_ASSERT(!utt_ODInitReadBBSDevDropFile(NULL));
   UT_ASSERT(!utt_ODInitReadBBSDevDropFile(""));
#ifdef ODPLAT_NIX
   ut_path_valid = FALSE;
   UT_ASSERT(!utt_ODInitReadBBSDevDropFile("node/BBSDEV.DRP"));
#else
   ut_path_valid = FALSE;
   UT_ASSERT(!utt_ODInitReadBBSDevDropFile("BBSDEV.DRP"));
#endif
   ut_path_valid = TRUE;

   set_core("local", "", "");
   ut_file_size = 5;
   UT_ASSERT(!parse());

   set_core("local", "", "");
   ut_file[0] = '\n';
   ut_file_size = 1;
   UT_ASSERT(!parse());

   set_core("local", "", "");
   ut_file_size = 0;
   UT_ASSERT(!parse());

   set_core("local", "", "");
   --ut_file_size;
   UT_ASSERT(!parse());

   set_core("local", "", "");
   ut_file[3] = '\r';
   UT_ASSERT(!parse());

   set_core("local", "", "");
   ut_file[21] = '\0';
   UT_ASSERT(!parse());

   set_core("local", "", "");
   ut_file[ut_file_size - 1] = '\r';
   UT_ASSERT(!parse());

   set_core("local", "", "");
   ut_file[0] = (char)0xef;
   ut_file[1] = (char)0xbb;
   ut_file[2] = (char)0xbf;
   UT_ASSERT(!parse());

   set_core("local", "", "");
   ut_file[0] = (char)0xef;
   UT_ASSERT(!parse());
   set_core("local", "", "");
   ut_file[0] = (char)0xef;
   ut_file[1] = (char)0xbb;
   UT_ASSERT(!parse());

   set_core("local", "", "");
   append_text("extra\n");
   UT_ASSERT(!parse());

   set_core("local", "", "");
   ut_read_fails = TRUE;
   UT_ASSERT(!parse());

   set_core("local", "", "");
   ut_open_fails = TRUE;
   UT_ASSERT(!parse());

   set_core("local", "", "");
   ut_malloc_fails = TRUE;
   UT_ASSERT(!parse());

   set_core("local", "", "");
   {
      unsigned lines = 0;
      size_t index;
      for(index = 0; index < ut_file_size; ++index)
      {
         if(ut_file[index] == '\n' && ++lines == 18)
         {
            ut_file_size = index + 1;
            break;
         }
      }
   }
   UT_ASSERT(!parse());

   {
      static char long_alias[600];
      unsigned index;
      for(index = 0; index < sizeof(long_alias) - 1; ++index)
         long_alias[index] = 'A';
      long_alias[sizeof(long_alias) - 1] = '\0';
      set_field(3, long_alias);
      UT_ASSERT(parse());
      set_field(3, long_alias);
      ut_realloc_fails = TRUE;
      UT_ASSERT(!parse());
   }
   {
      static char huge_alias[4500];
      unsigned index;
      for(index = 0; index < sizeof(huge_alias) - 1; ++index)
         huge_alias[index] = 'A';
      huge_alias[sizeof(huge_alias) - 1] = '\0';
      set_field(3, huge_alias);
      UT_ASSERT(!parse());
   }
}

static void rejects_fields(void)
{
   static const char *bad_versions[] = {"2.0", "01.0", "1.00"};
   unsigned index;
   for(index = 0; index < DIM(bad_versions); ++index)
   {
      set_field(0, bad_versions[index]);
      UT_ASSERT(!parse());
   }

   set_core("local", "x", "");
   UT_ASSERT(!parse());
   set_core("local", "", "2023-02-29T12:00:00Z");
   UT_ASSERT(!parse());
   set_core("local", "", "2030-01-01T24:00:00Z");
   UT_ASSERT(!parse());
   set_core("local", "", "2030-01-01T12:00:60Z");
   UT_ASSERT(!parse());

   set_field(5, "0");
   UT_ASSERT(!parse());

   set_field(7, "y");
   UT_ASSERT(!parse());

   set_field(3, "\xc0");
   UT_ASSERT(!parse());
}

static void validates_text_and_dimensions(void)
{
   static const unsigned required[] = {3,4,11,12,13,14,15};
   static const char *bad_widths[] = {"", "x", "65536", "0", "01"};
   static const char *bad_flags[] = {"", "YY", "y"};
   unsigned index;

   for(index = 0; index < DIM(required); ++index)
      reject_field(required[index], "");
   for(index = 0; index < DIM(bad_widths); ++index)
   {
      reject_field(5, bad_widths[index]);
      reject_field(6, bad_widths[index]);
   }
   reject_field(5, "/");
   reject_field(6, "/");
   accept_field(5, "65535");
   UT_ASSERT_EQ_INT(255, od_control.user_screenwidth);
   accept_field(6, "65535");
   for(index = 0; index < DIM(bad_flags); ++index)
   {
      reject_field(7, bad_flags[index]);
      reject_field(8, bad_flags[index]);
      reject_field(18, bad_flags[index]);
   }
   accept_field(7, "N");
   accept_field(8, "Y");
   accept_field(18, "N");
   UT_ASSERT(od_control.od_silent_mode);
}

static void validates_versions_and_cterm(void)
{
   static const char *bad_versions[] = {
      "", "0.0", "2.0", "10.0", "1", "1.", "1.a", "1./", "1.00"
   };
   static const char *bad_cterm[] = {
      ".", "1.", ".1", "1..2", "01", "a", "/"
   };
   unsigned index;
   for(index = 0; index < DIM(bad_versions); ++index)
      reject_field(0, bad_versions[index]);
   accept_field(0, "1.1");
   for(index = 0; index < DIM(bad_cterm); ++index)
      reject_field(9, bad_cterm[index]);
   accept_field(9, "");
   accept_field(9, "0");
   accept_field(9, "1.0.23");
}

static void validates_deadlines(void)
{
   static const char *bad[] = {
      "2030/01-01T12:00:00Z", "2030-01/01T12:00:00Z",
      "2030-01-01 12:00:00Z", "2030-01-01T12.00:00Z",
      "2030-01-01T12:00.00Z", "2030-01-01T12:00:00X",
      "2030-01-01T12:00:00ZZ", "203x-01-01T12:00:00Z",
      "/030-01-01T12:00:00Z",
      "2030-00-01T12:00:00Z", "2030-13-01T12:00:00Z",
      "2030-01-00T12:00:00Z", "2030-04-31T12:00:00Z",
      "2030-01-01T24:00:00Z", "2030-01-01T12:60:00Z",
      "2030-01-01T12:00:60Z", "1900-02-29T00:00:00Z"
   };
   unsigned index;
   for(index = 0; index < DIM(bad); ++index)
      reject_field(10, bad[index]);
   accept_field(10, "1969-12-31T23:59:59Z");
   UT_ASSERT_EQ_INT((time_t)0, nBBSDevDeadline);
   accept_field(10, "1996-02-29T00:00:00Z");
   accept_field(10, "2000-02-29T00:00:00Z");
   accept_field(10, "2000-03-01T00:00:00Z");
   accept_field(10, "2030-03-01T00:00:00Z");
   accept_field(10, "2030-01-31T00:00:00Z");
   accept_field(10, "2030-04-30T00:00:00Z");
   accept_field(10, "2030-06-30T00:00:00Z");
   accept_field(10, "2030-09-30T00:00:00Z");
   accept_field(10, "2030-11-30T00:00:00Z");
   accept_field(10, "2039-01-01T00:00:00Z");
   accept_field(10, "9999-12-31T23:59:59Z");
}

static void validates_tokens_and_numbers(void)
{
   static const char *bad_numbers[] = {
      "", "x", "01", "18446744073709551616",
      "111111111111111111111"
   };
   unsigned index;
   reject_field(11, "UTF 8");
   reject_field(11, "{");
   reject_field(11, "Aa0-_");
   accept_field(11, "UTF-8");
   UT_ASSERT(od_control.od_cp437_to_utf8_out);
   accept_field(11, "uTf-8");
   UT_ASSERT(od_control.od_cp437_to_utf8_out);
   accept_field(11, "IBM437");
   accept_field(11, "ibm437");
   reject_field(11, "XTF-8");
   reject_field(11, "UTF_8");
   reject_field(11, "UTF-9");
   reject_field(11, "UTF-80");
   set_field(12, "invalid");
   ut_language_valid = FALSE;
   UT_ASSERT(!parse());
   for(index = 0; index < DIM(bad_numbers); ++index)
   {
      reject_field(16, bad_numbers[index]);
      reject_field(17, bad_numbers[index]);
   }
   accept_field(16, "sysop");
   UT_ASSERT(bIsSysop);
   accept_field(16, "cosysop");
   UT_ASSERT(bIsCoSysop);
   accept_field(16, "18446744073709551615");
   accept_field(16, "0");
   accept_field(16, "99999");
   UT_ASSERT_EQ_INT(65535, od_control.user_security);
   accept_field(17, "18446744073709551615");
   UT_ASSERT_EQ_INT(65535, od_control.od_node);
}

static void validates_utf8(void)
{
   static const char *valid[] = {
      "A\xc2\xa1", "A\xe0\xa0\x80", "A\xed\x9f\xbf",
      "A\xe2\x82\xac", "A\xf0\x90\x80\x80", "A\xf4\x8f\xbf\xbf",
      "A\xf1\x80\x80\x80"
   };
   static const char *invalid[] = {
      "\xc0", "\xc2", "\xc2" "A", "\xe0", "\xe0\x9f\x80",
      "\xed\xa0\x80", "\xe1" "A\x80", "\xe1\x80",
      "\xe1\x80" "A", "\xf0", "\xf1" "A\x80\x80",
      "\xf1\x80", "\xf1\x80" "A\x80", "\xf0\x8f\x80\x80",
      "\xf4\x90\x80\x80", "\xf1\x80\x80",
      "\xf1\x80\x80" "A", "\xf5\x80\x80\x80", "\x1f", "\x7f",
      "\xc2\x80", " leading", "trailing "
   };
   static const char *spaces[] = {
      " ", "\xc2\xa0", "\xe1\x9a\x80", "\xe2\x80\x80",
      "\xe2\x80\xa8", "\xe2\x80\xa9", "\xe2\x80\xaf",
      "\xe2\x81\x9f", "\xe3\x80\x80"
   };
   unsigned index;
   char edge[8];
   for(index = 0; index < DIM(valid); ++index)
      accept_field(3, valid[index]);
   for(index = 0; index < DIM(invalid); ++index)
      reject_field(3, invalid[index]);
   for(index = 0; index < DIM(spaces); ++index)
   {
      unsigned position = 0;
      reject_field(3, spaces[index]);
      edge[position++] = 'A';
      while(spaces[index][position - 1] != '\0')
      {
         edge[position] = spaces[index][position - 1];
         ++position;
      }
      edge[position] = '\0';
      reject_field(3, edge);
   }
   accept_field(3, "\xe1\xbf\xbf");
   accept_field(3, "\xe2\x80\x8b");
}

static void accepts_modes(void)
{
#ifdef ODPLAT_NIX
   set_core("stdio", "", "");
   UT_ASSERT(parse());
   UT_ASSERT_EQ_INT(kComMethodStdIO, nBBSDevComMethod);
   set_core("serial", "9", "");
   UT_ASSERT(parse());
   UT_ASSERT_EQ_INT(9, od_control.od_open_handle);
#endif
#if defined(ODPLAT_NIX) || defined(ODPLAT_WIN32)
   set_core("socket", "12", "");
   UT_ASSERT(parse());
   UT_ASSERT_EQ_INT(kComMethodSocket, nBBSDevComMethod);
   UT_ASSERT_EQ_INT(TRUE, od_control.od_use_socket);
#endif
#ifdef ODPLAT_WIN32
   set_core("winserial", "15", "");
   UT_ASSERT(parse());
   UT_ASSERT_EQ_INT(kComMethodWin32, nBBSDevComMethod);
#endif
#if defined(ODPLAT_DOS) || defined(ODPLAT_DOS32)
   set_core("uart", "03F8,4", "");
   UT_ASSERT(parse());
   UT_ASSERT_EQ_INT(kComMethodUART, nBBSDevComMethod);
   UT_ASSERT_EQ_INT(0x03f8, od_control.od_com_address);
   UT_ASSERT_EQ_INT(4, od_control.od_com_irq);
   set_core("uart", "03F8,15", "");
   UT_ASSERT(parse());
   UT_ASSERT_EQ_INT(15, od_control.od_com_irq);
   set_core("fossil", "0", "");
   UT_ASSERT(parse());
   UT_ASSERT_EQ_INT(kComMethodFOSSIL, nBBSDevComMethod);
#endif
}

static void rejects_socket_modes(void)
{
   set_core("unknown", "1", "");
   UT_ASSERT(!parse());
   set_core("socket", "01", "");
   UT_ASSERT(!parse());
   set_core("socket", "", "");
   UT_ASSERT(!parse());
   set_core("socket", "x", "");
   UT_ASSERT(!parse());
   set_core("socket", "/", "");
   UT_ASSERT(!parse());
   set_core("socket", "0", "");
#if defined(ODPLAT_NIX) || defined(ODPLAT_WIN32)
   UT_ASSERT(parse());
#else
   UT_ASSERT(!parse());
#endif
   set_core("socket", "2147483647", "");
#if defined(ODPLAT_NIX) || defined(ODPLAT_WIN32)
   UT_ASSERT(parse());
#else
   UT_ASSERT(!parse());
#endif
   set_core("socket", "18446744073709551616", "");
   UT_ASSERT(!parse());
}

static void rejects_uart_modes(void)
{
   set_core("uart", "03f8,4", "");
   UT_ASSERT(!parse());
   set_core("uart", "", "");
   UT_ASSERT(!parse());
   set_core("uart", "03F84", "");
   UT_ASSERT(!parse());
   set_core("uart", "03F8,x", "");
   UT_ASSERT(!parse());
   set_core("uart", "03F8,/", "");
   UT_ASSERT(!parse());
   set_core("uart", "03F8,1x", "");
   UT_ASSERT(!parse());
   set_core("uart", "03F8,1/", "");
   UT_ASSERT(!parse());
   set_core("uart", "03F8,123", "");
   UT_ASSERT(!parse());
   set_core("uart", "G3F8,4", "");
   UT_ASSERT(!parse());
   set_core("uart", "/3F8,4", "");
   UT_ASSERT(!parse());
   set_core("uart", "03F8,16", "");
   UT_ASSERT(!parse());
}

static void rejects_fossil_modes(void)
{
   set_core("fossil", "255", "");
   UT_ASSERT(!parse());
   set_core("fossil", "", "");
   UT_ASSERT(!parse());
   set_core("fossil", "x", "");
   UT_ASSERT(!parse());
   set_core("fossil", "/", "");
   UT_ASSERT(!parse());
   set_core("fossil", "01", "");
   UT_ASSERT(!parse());
}

static void rejects_platform_modes(void)
{
   set_core("stdio", "x", "");
   UT_ASSERT(!parse());

#ifndef ODPLAT_NIX
   set_core("stdio", "", "");
   UT_ASSERT(!parse());
   set_core("serial", "9", "");
   UT_ASSERT(!parse());
#endif
#ifndef ODPLAT_WIN32
   set_core("winserial", "9", "");
   UT_ASSERT(!parse());
#endif
#if !defined(ODPLAT_DOS) && !defined(ODPLAT_DOS32)
   set_core("uart", "03F8,4", "");
   UT_ASSERT(!parse());
   set_core("fossil", "0", "");
   UT_ASSERT(!parse());
#endif
}

static const UTTestCase ut_cases[] = {
   {"core", accepts_core},
   {"extensions", accepts_extensions},
   {"structure", rejects_structure},
   {"fields", rejects_fields},
   {"text and dimensions", validates_text_and_dimensions},
   {"versions and CTerm", validates_versions_and_cterm},
   {"deadlines", validates_deadlines},
   {"tokens and numbers", validates_tokens_and_numbers},
   {"UTF-8", validates_utf8},
   {"modes", accepts_modes},
   {"bad socket modes", rejects_socket_modes},
   {"bad UART modes", rejects_uart_modes},
   {"bad FOSSIL modes", rejects_fossil_modes},
   {"unsupported platform modes", rejects_platform_modes}
};
