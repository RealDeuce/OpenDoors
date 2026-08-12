static void owns_no_resources_that_require_shutdown(void)
{
   utt_ODKrnlShutdown();
   UT_ASSERT(TRUE);
}

static const UTTestCase ut_cases[] = {
   {"no resources", owns_no_resources_that_require_shutdown}
};
