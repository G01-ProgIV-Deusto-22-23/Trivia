#ifdef __cplusplus
extern "C" {
#endif

    __attribute__ ((__noreturn__)) static void trivia_abort (void __attribute__ ((unused)) * unused) {
        trap ();
        unreachable ();
    }

    static void sigsegv_handler (const int __attribute__ ((unused)) unused) {
        print_backtrace ();
        _Pragma ("GCC diagnostic push");
        _Pragma ("GCC diagnostic ignored \"-Wimplicit-function-declaration\"");
        error ("segmentation fault.", trivia_abort);
        _Pragma ("GCC diagnostic pop");
    }

    void handle_signals (void) {
        sigaction (SIGSEGV, &(struct sigaction) { .sa_handler = sigsegv_handler }, NULL);
    }

#ifdef __cplusplus
}
#endif
