#define IS_BACKTRACE_SRC_FILE
#define BT_FILENAME_SUFFIX   ".backtrace"
#define BT_FILENAME_MOD      "XXXXXX"
#define BT_FILENAME_TEMPLATE BT_FILENAME_MOD BT_FILENAME_SUFFIX
#define BT_FILENAME_TRIES    5
#define BT_TIME_BUFSZ        100
#define UNDEFINED_TIME_STR   "could not get the backtrace initialization time."

#ifdef __cplusplus
extern "C" {
#endif
    static struct backtrace_state *BT_STATE               = NULL;
    static bool                    BT_INIT_ALREADY_CALLED = false;
    static int                     BT_FD                  = -1;
    static char                    BT_FILENAME [sizeof (BT_FILENAME_TEMPLATE)];

    static char   BT_INIT_TIME [BT_TIME_BUFSZ];
    static size_t BT_INIT_TIME_LEN;

    static
#ifdef _WIN32
        DWORD
#else
    pid_t
#endif
                BT_PID;
    static char BT_DIGITS [sizeof (STRINGIFY ((typeof (BT_PID)) -1)) - 1];

    int get_backtrace_fd (void) {
        return BT_FD;
    }

    static void close_backtrace_file (void) {
        if (BT_FD == -1)
            return;

        if (
#ifdef _WIN32
        _close (BT_FD)
#else
        close (BT_FD)
#endif
         == -1)
            warning ("could not close the backtrace file.");

        BT_FD = -1;
    }

#if defined(__cpp_attributes) || __STDC_VERSION__ >= 201710L
    static int bt_callback_func (
        void [[unused]] * unused1, uintptr_t [[unused]] unused2, const char *filename, int lineno, const char *function
    ) {
#else
static int bt_callback_func (
    void __attribute__ ((unused)) * unused1, uintptr_t __attribute__ ((unused)) unused2, const char *filename,
    int lineno, const char *function
) {
#endif
        if (!(filename && function))
            return 1;

        write (BT_FD, "\nFunction ", sizeof ("\nFunction ") - 1);
        write (
            BT_FD, function,
#ifdef _WIN32
            (unsigned int)
#endif
                strlen (function)
        );
        write (BT_FD, " (file ", sizeof (" (file ") - 1);
        write (
            BT_FD, filename,
#ifdef _WIN32
            (unsigned int)
#endif
                strlen (filename)
        );
        write (BT_FD, ", line ", sizeof (", line ") - 1);

        uint32_t line = (uint32_t) lineno;
        uint32_t dp   = (uint32_t) decplaces (line);
        for (uint32_t i = 1; line; line /= 10)
            *(BT_DIGITS + dp - i++) = (char) (line % 10) + '0';
        write (BT_FD, BT_DIGITS, dp);

        write (BT_FD, ").", sizeof (").") - 1);

        return 0;
    }

#if defined(__cpp_attributes) || __STDC_VERSION__ >= 201710L
    static bt_error_func (void *f, const char [[unused]] * unused2, int errnum) {
#else
static void bt_error_func (void *f, const char __attribute__ ((unused)) * unused2, int errnum) {
#endif
        if (errnum != -1) {
            if (!f) {
                error ("an error was encountered when trying to set up the backtrace sytem.");
                close_backtrace_file ();
            }

            else {
                write (
                    BT_FD, "An error was encountered when trying to print a backtrace.",
                    sizeof ("An error was encountered when trying to print a backtrace.") - 1
                );
                close_backtrace_file ();

                error ("an error was encountered when trying to print a backtrace.");
            }
        }

        warning ("there is not enough debug information to set up the backtrace system.");
    }

    void print_backtrace (void) {
        if (!BT_INIT_ALREADY_CALLED)
            init_backtrace ();

        if (!BT_STATE || BT_FD == -1)
            return;

        write (BT_FD, "Backtrace (", sizeof ("Backtrace (") - 1);
        write (
            BT_FD, BT_INIT_TIME,
#ifdef _WIN32
            (unsigned int)
#endif
                BT_INIT_TIME_LEN
        );
        write (BT_FD, ")\nProcess ID: ", sizeof (")\nProcess ID: ") - 1);

#ifdef _WIN32
        if (BT_PID) {
#endif
            auto dp = decplaces (BT_PID);
            for (typeof (BT_PID) i = 1; BT_PID; BT_PID /= 10)
                *(BT_DIGITS + dp - i++) = (char) (BT_PID % 10) + '0';
            write (BT_FD, BT_DIGITS, (unsigned) dp);
#ifdef _WIN32
        }

        else
            write (BT_FD, "unknown", sizeof ("unknown") - 1);
#endif
        write (BT_FD, "\n", sizeof ("\n") - 1);

        backtrace_full (BT_STATE, 1, bt_callback_func, bt_error_func, (void *) ((uintptr_t) NULL + 1));

        close_backtrace_file ();
    }

    void init_backtrace (void) {
        if (BT_INIT_ALREADY_CALLED)
            return;

        BT_INIT_ALREADY_CALLED = true;

        BT_PID =
#ifdef _WIN32
            GetProcessId (GetCurrentProcess ());
#else
        getpid ();
#endif
        ;
        size_t tries = 0;

    tempfile:
#ifdef _WIN32
        BT_FD = mkstemp (memcpy (BT_FILENAME, BT_FILENAME_MOD, sizeof (BT_FILENAME_MOD)));
#else
    BT_FD = mkstemps (
        memcpy (BT_FILENAME, BT_FILENAME_TEMPLATE, sizeof (BT_FILENAME_TEMPLATE)), sizeof (BT_FILENAME_SUFFIX) - 1
    );
#endif

        if (BT_FD == -1) {
            if (errno == EEXIST) {
                tries++;

                if (tries == BT_FILENAME_TRIES)

#ifdef _WIN32
                    (_sopen_s (&BT_FD, BT_FILENAME, _O_RDWR | _O_SEQUENTIAL, _SH_DENYNO, _S_IREAD | _S_IWRITE),
                     BT_FD == -1)
#else
                ((BT_FD = open (BT_FILENAME, O_RDWR, S_IRUSR | S_IWUSR)) == -1)
#endif
                        ? bt_error_func (NULL, NULL, 0)
                        : (void) 0;

                goto tempfile;
            }

            else
                bt_error_func (NULL, NULL, 0);
        }

#ifdef _WIN32
        if (({
                char newname [sizeof (BT_FILENAME_TEMPLATE)];
                int  res = -1;
                if (!_close (BT_FD)) {
                    memcpy (
                        mempcpy (newname, BT_FILENAME, sizeof (BT_FILENAME_MOD) - 1), BT_FILENAME_SUFFIX,
                        sizeof (BT_FILENAME_SUFFIX)
                    );
                    if ((res = rename (BT_FILENAME, newname)) == -1) {
                        res = remove (newname) | rename (BT_FILENAME, newname);
                    }
                    memcpy (BT_FILENAME, newname, sizeof (BT_FILENAME));
                }

                res;
            }) ||
            _sopen_s (&BT_FD, BT_FILENAME, _O_RDWR | _O_SEQUENTIAL, _SH_DENYNO, _S_IREAD | _S_IWRITE))
            bt_error_func (NULL, NULL, 0);
#endif

        BT_STATE = backtrace_create_state (NULL, 1, bt_error_func, NULL);

        struct tm *_time = NULL;
        if (!(({
                  const char *loc = setlocale (LC_TIME, NULL);
                  time_t      t   = time (NULL);
                  setlocale (LC_TIME, "es_ES.UTF-8");
                  if (t != (time_t) -1)
                      _time = localtime (&t);
                  setlocale (LC_TIME, loc);
                  _time;
              }) &&
              (BT_INIT_TIME_LEN = strftime (BT_INIT_TIME, sizeof (BT_INIT_TIME), "%c", _time)))) {
            warning (UNDEFINED_TIME_STR);
            memcpy (BT_INIT_TIME, UNDEFINED_TIME_STR, BT_INIT_TIME_LEN = sizeof (UNDEFINED_TIME_STR));
        }
    }

    void end_backtrace (void) {
        if (BT_FD == -1)
            return;

        close_backtrace_file ();

        if (
#ifdef _WIN32
            !DeleteFileA (BT_FILENAME)
#else
        remove (BT_FILENAME) == -1
#endif
        )
            warning ("could not remove the backtrace file.");
    }

#ifdef __cplusplus
}
#endif
