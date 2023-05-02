static HANDLE ABORT_THREAD = NULL;

#if defined(__cpp_attributes) || __STDC_VERSION__ > 201710L
[[__noreturn__]] static void impl_trivia_abort (void [[unused]] * unused) {
#else
__attribute__ ((__noreturn__)) static void impl_trivia_abort (void __attribute__ ((unused)) * unused) {
#endif
    unreachable ();
}

#if defined(__cpp_attributes) || __STDC_VERSION__ > 201710L
[[__noreturn__]] static void trivia_abort (void [[unused]] * unused) {
#else
__attribute__ ((__noreturn__)) static void trivia_abort (void __attribute__ ((unused)) * unused) {
#endif
    _Pragma ("GCC diagnostic push");
    _Pragma ("GCC diagnostic ignored \"-Wimplicit-function-declaration\"");
    error ("segmentation fault.", impl_trivia_abort);
    _Pragma ("GCC diagnostic pop");
}

#if defined(__cpp_attributes) || __STDC_VERSION__ > 201710L
static void sigsegv_handler (int [[unused]] signum) {
#else
static void                                sigsegv_handler (const int __attribute__ ((unused)) signum) {
#endif
    ResumeThread (ABORT_THREAD);
    Sleep (INFINITE);
}

void handle_signals (void) {
    if (!ABORT_THREAD &&
        !(ABORT_THREAD = CreateThread (
              &(SECURITY_ATTRIBUTES
              ) { .nLength = sizeof (SECURITY_ATTRIBUTES), .lpSecurityDescriptor = NULL, .bInheritHandle = TRUE },
              0, NULL, trivia_abort, CREATE_SUSPENDED, NULL
          )))
        error ("could not create the signal handling thread.", impl_trivia_abort);

    signal (SIGSEGV, sigsegv_handler);
}