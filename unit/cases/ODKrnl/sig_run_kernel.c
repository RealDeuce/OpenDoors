#define UT_CUSTOM_MOCK_od_kernel
static unsigned ut_kernel_calls;
void ODCALL utm_od_kernel(void) { ++ut_kernel_calls; }
static void invokes_the_kernel_for_any_signal_number(void)
{
   ut_kernel_calls = 0;
   utt_sig_run_kernel(SIGALRM);
   utt_sig_run_kernel(0);
   UT_ASSERT_EQ_UINT(2, ut_kernel_calls);
}
static const UTTestCase ut_cases[] = {
   {"dispatch", invokes_the_kernel_for_any_signal_number}
};
