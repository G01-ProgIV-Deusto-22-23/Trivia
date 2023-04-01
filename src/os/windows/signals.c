#if defined(__cplusplus) || __STDC_VERSION__ > 201710L
static void sigsegv_handler (int [[unused]] signum) {
#else
static void sigsegv_handler (const int __attribute__ ((unused)) signum) {
#endif
    _Pragma ("GCC diagnostic push");
    _Pragma ("GCC diagnostic ignored \"-Wimplicit-function-declaration\"");
    error ("segmentation fault.");
    _Pragma ("GCC diagnostic pop");
}

void handle_signals (void) {
    signal (SIGSEGV, sigsegv_handler);
}