static void copies_text(void)
{
   char output[8];

   memset(output, 'X', sizeof(output));
   utt_ODBBSDevCopyText(output, sizeof(output), "text");
   UT_ASSERT(strcmp(output, "text") == 0);

   utt_ODBBSDevCopyText(output, sizeof(output), "");
   UT_ASSERT(strcmp(output, "") == 0);

   output[0] = 'X';
   utt_ODBBSDevCopyText(output, 0, "text");
   UT_ASSERT_EQ_INT('X', output[0]);
}

static void truncates_at_character_boundary(void)
{
   char output[8];

   utt_ODBBSDevCopyText(output, 5, "abcdef");
   UT_ASSERT(strcmp(output, "abcd") == 0);

   utt_ODBBSDevCopyText(output, 5, "ab\xc2\xa1z");
   UT_ASSERT(strcmp(output, "ab\xc2\xa1") == 0);

   utt_ODBBSDevCopyText(output, 4, "ab\xc2\xa1z");
   UT_ASSERT(strcmp(output, "ab") == 0);

   utt_ODBBSDevCopyText(output, 2, "\xc2\xa1z");
   UT_ASSERT(strcmp(output, "") == 0);
}

static const UTTestCase ut_cases[] = {
   {"copy", copies_text},
   {"UTF-8 truncation", truncates_at_character_boundary}
};
