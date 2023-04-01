#if defined(__cpp_attributes) || __STDC_VERSION__ > 201710L
[[__noreturn__]] static void trivia_abort (void [[unused]] * unused) {
#else
__attribute__ ((__noreturn__)) static void trivia_abort (void __attribute__ ((unused)) * unused) {
#endif
    unreachable ();
}

#if defined(__cpp_attributes) || __STDC_VERSION__ > 201710L
static void sigsegv_handler (int [[unused]] signum) {
#else
static void                                sigsegv_handler (const int __attribute__ ((unused)) signum) {
#endif
    _Pragma ("GCC diagnostic push");
    _Pragma ("GCC diagnostic ignored \"-Wimplicit-function-declaration\"");
    error ("segmentation fault.", trivia_abort);
    _Pragma ("GCC diagnostic pop");
}

void handle_signals (void) {
    signal (SIGSEGV, sigsegv_handler);
}