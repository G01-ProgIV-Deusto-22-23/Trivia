#if defined(__cplusplus) || __STDC_VERSION__ > 201710L
[[__noreturn__]] static void trivia_abort (void [[unused]] * unused) {
#else
__attribute__ ((__noreturn__)) static void trivia_abort (void __attribute__ ((unused)) * unused) {
#endif
    unreachable ();
}

#if defined(__cplusplus) || __STDC_VERSION__ > 201710L
static void sigsegv_handler (const int [[unused]] unused) {
#else
static void                                sigsegv_handler (const int __attribute__ ((unused)) unused) {
#endif
    _Pragma ("GCC diagnostic push");
    _Pragma ("GCC diagnostic ignored \"-Wimplicit-function-declaration\"");
    error ("segmentation fault.", trivia_abort);
    _Pragma ("GCC diagnostic pop");
}

void handle_signals (void) {
    sigaction (SIGSEGV, &(struct sigaction) { .sa_handler = sigsegv_handler }, NULL);
    sigaction (SIGINT, NULL, NULL);
    sigaction (SIGTERM, NULL, NULL);
}
