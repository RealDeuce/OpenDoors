static void validates_paths(void)
{
   UT_ASSERT(!utt_ODBBSDevAbsolutePath(NULL));
   UT_ASSERT(!utt_ODBBSDevAbsolutePath(""));
#ifdef ODPLAT_NIX
   UT_ASSERT(utt_ODBBSDevAbsolutePath("/node/BBSDEV.DRP"));
   UT_ASSERT(!utt_ODBBSDevAbsolutePath("node/BBSDEV.DRP"));
#else
   UT_ASSERT(utt_ODBBSDevAbsolutePath("C:\\node\\BBSDEV.DRP"));
   UT_ASSERT(utt_ODBBSDevAbsolutePath("c:/node/BBSDEV.DRP"));
   UT_ASSERT(utt_ODBBSDevAbsolutePath("\\\\server\\BBSDEV.DRP"));
   UT_ASSERT(utt_ODBBSDevAbsolutePath("//server/BBSDEV.DRP"));
   UT_ASSERT(utt_ODBBSDevAbsolutePath("\\/server/BBSDEV.DRP"));
   UT_ASSERT(!utt_ODBBSDevAbsolutePath("C:BBSDEV.DRP"));
   UT_ASSERT(!utt_ODBBSDevAbsolutePath("1:\\BBSDEV.DRP"));
   UT_ASSERT(!utt_ODBBSDevAbsolutePath("/BBSDEV.DRP"));
   UT_ASSERT(!utt_ODBBSDevAbsolutePath("BBSDEV.DRP"));
#endif
}

static const UTTestCase ut_cases[] = {
   {"absolute paths", validates_paths}
};
