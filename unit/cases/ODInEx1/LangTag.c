#define UT_CUSTOM_MOCK_strcasecmp
#define UT_CUSTOM_MOCK_stricmp

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

static void accepts_well_formed_tags(void)
{
   static const char * const valid[] = {
      "en", "abc", "abcd", "abcdefgh", "EN-us",
      "zh-cmn-Hans-CN", "zh-aaa-bbb-ccc", "en-Latn-US",
      "en-Latn-419", "en-419", "sl-rozaj-biske-1994",
      "en-a-foo", "en-0-foo", "en-a-foo-b-bar",
      "de-CH-x-phonebk", "de-X-private", "x-whatever-more", "X-private",
      "art-lojban", "i-klingon", "en-GB-oed", "zh-min-nan",
      "zh-xiang"
   };
   unsigned index;
   for(index = 0; index < DIM(valid); ++index)
      UT_ASSERT(utt_ODBBSDevLanguageTagValid(valid[index]));
}

static void rejects_malformed_tags(void)
{
   static const char * const invalid[] = {
      "", "-en", "en-", "en--US", "abcdefghi", "en-abcdefghi",
      "en_Us", "en-{", "en-\xc3\xa9", "q", "12", "e1", "en-12",
      "x", "en-a", "en-a-b-foo", "en-x", "en-abc1",
      "zh-aaa-bbb-ccc-ddd", "sl-rozaj-ROZAJ",
      "en-a-foo-A-bar"
   };
   unsigned index;
   UT_ASSERT(!utt_ODBBSDevLanguageTagValid(NULL));
   for(index = 0; index < DIM(invalid); ++index)
      UT_ASSERT(!utt_ODBBSDevLanguageTagValid(invalid[index]));
}

static const UTTestCase ut_cases[] = {
   {"well-formed tags", accepts_well_formed_tags},
   {"malformed tags", rejects_malformed_tags}
};
