static int INIT_CURS_VAR;

#define TRIVIA_USAGE_ARG_SHORT       "-u"
#define TRIVIA_USAGE_ARG_SHORT_LEFT  TRIVIA_USAGE_ARG_SHORT
#define TRIVIA_USAGE_ARG_SHORT_RIGHT "give a short usage message"

#define TRIVIA_USAGE_ARG_LONG       "--usage"
#define TRIVIA_USAGE_ARG_LONG_LEFT  TRIVIA_USAGE_ARG_LONG
#define TRIVIA_USAGE_ARG_LONG_RIGHT "same as " TRIVIA_USAGE_ARG_SHORT

#define TRIVIA_HELP_ARG_SHORT1      "-h"
#define TRIVIA_HELP_ARG_SHORT2      "-?"
#define TRIVIA_HELP_ARG_SHORT_LEFT  TRIVIA_HELP_ARG_SHORT1 ", " TRIVIA_HELP_ARG_SHORT2
#define TRIVIA_HELP_ARG_SHORT_RIGHT "give this help list"

#define TRIVIA_HELP_ARG_LONG       "--help"
#define TRIVIA_HELP_ARG_LONG_LEFT  TRIVIA_HELP_ARG_LONG
#define TRIVIA_HELP_ARG_LONG_RIGHT "same as " TRIVIA_HELP_ARG_SHORT1

#define TRIVIA_NOLOG_ARG       "--no-log"
#define TRIVIA_NOLOG_ARG_LEFT  TRIVIA_NOLOG_ARG
#define TRIVIA_NOLOG_ARG_RIGHT "disable logging to a file, uses stderr instead."

#define TRIVIA_LOG_ARG       "--log"
#define TRIVIA_LOG_ARG_LEFT  TRIVIA_LOG_ARG
#define TRIVIA_LOG_ARG_RIGHT "enable logging to a file (default behavior)"

#define TRIVIA_LOGFILE_ARG       "--log-file"
#define TRIVIA_LOGFILE_ARG_LEFT  TRIVIA_LOGFILE_ARG " <file>"
#define TRIVIA_LOGFILE_ARG_RIGHT "use the given file name for the log file (\"\" for a random name)"

static bool TRIVIA_WANTS_USAGE        = false;
static bool TRIVIA_WANTS_HELP         = false;
static bool TRIVIA_WANTS_NOLOG        = false;
static bool TRIVIA_WANTS_LOG          = false;
static bool TRIVIA_EXPECTING_FILENAME = false;

#define WINDOWS_ILLEGAL_FILENAME_CHARS "/\\?%*:|\"<>,;="
static const char
#ifndef _WIN32
    #if defined(__cpp_attributes) || __STDC_VERSION__ >= 201710L
    [[unused]]
    #else
    __attribute__ ((unused))
    #endif
#endif
    *const         WINDOWS_ILLEGAL_FILENAMES [] = { "CON",      "CONIN$",  "CONOUT$",  "PRN",     "AUX",    "CLOCK$",
                                                    "NUL",      "COM0",    "COM1",     "COM2",    "COM3",   "COM4",
                                                    "COM5",     "COM6",    "COM7",     "COM8",    "COM9",   "LPT0",
                                                    "LPT1",     "LPT2",    "LPT3",     "LPT4",    "LPT5",   "LPT6",
                                                    "LPT7",     "LPT8",    "LPT9",     "KEYBD$",  "$Mft",   "$MftMirr",
                                                    "$LogFile", "$Volume", "$AttrDef", "$Bitmap", "$Boot",  "$BadClus",
                                                    "$Secure",  "$Upcase", "$Extend",  "$Quota",  "$ObjId", "$Reparse" };
static const char *LOG_FILENAME                 = NULL;

static void atexit_end_ui (void) {
    end_ui ();
}

static void atexit_close_log_file (void) {
    close_log_file ();
}

int impl_setup_ui (
    const int argc, const char *const *const restrict argv, const int cursor, const int en_cbreak, const int en_echo,
    const int en_keypad, const int en_halfdelay
) {
    for (int i = 0; i < argc; i++) {
        if (!(strcmp (*(argv + i), TRIVIA_USAGE_ARG_SHORT) && strcmp (*(argv + i), TRIVIA_USAGE_ARG_LONG))) {
            TRIVIA_WANTS_USAGE = true;

            continue;
        }

        if (!(strcmp (*(argv + i), TRIVIA_HELP_ARG_SHORT1) && strcmp (*(argv + i), TRIVIA_HELP_ARG_SHORT2) &&
              strcmp (*(argv + i), TRIVIA_HELP_ARG_LONG))) {
            TRIVIA_WANTS_HELP = true;

            continue;
        }

        if (!strcmp (*(argv + i), TRIVIA_NOLOG_ARG)) {
            TRIVIA_WANTS_NOLOG = true;

            goto log_ex;
        }

        if (!strcmp (*(argv + i), TRIVIA_LOG_ARG)) {
            TRIVIA_WANTS_LOG = true;

            goto log_ex;
        }

        if (!strcmp (*(argv + i), TRIVIA_LOGFILE_ARG)) {
            LOG_FILENAME              = NULL;
            TRIVIA_WANTS_LOG          = true;
            TRIVIA_EXPECTING_FILENAME = true;

            goto log_ex;
        }

        if (TRIVIA_EXPECTING_FILENAME) {
            LOG_FILENAME              = *(argv + i);
            TRIVIA_EXPECTING_FILENAME = false;

            continue;
        }

        error (
            "please, provide a valid argument list. Run the program with the --help (or -h) argument in order to see the list of valid arguments."
        );

    log_ex:
        if (TRIVIA_WANTS_NOLOG && TRIVIA_WANTS_LOG)
            error ("The " STRINGIFY (TRIVIA_NOLOG_ARG) " and " STRINGIFY (TRIVIA_LOG_ARG
            ) "/" STRINGIFY (TRIVIA_LOGFILE_ARG) " arguments are mutually exclusive.");
    }

    if (TRIVIA_EXPECTING_FILENAME)
        error ("a filename was expected.");

    if (LOG_FILENAME) {
#ifdef _WIN32
        if (strpbrk (LOG_FILENAME, WINDOWS_ILLEGAL_FILENAME_CHARS) || ({
                bool illegal = false;
                for (size_t i = 0;
                     i < sizeof (WINDOWS_ILLEGAL_FILENAME_CHARS) / sizeof (*WINDOWS_ILLEGAL_FILENAME_CHARS);)
                    if ((illegal = !strcmp (LOG_FILENAME, *(WINDOWS_ILLEGAL_FILENAMES + i++))))
                        break;
                illegal;
            }))
#else
        if (strchr (LOG_FILENAME, '/') || !strcmp (LOG_FILENAME, ".") || !strcmp (LOG_FILENAME, ".."))
#endif
            error ("please, provide a valid file name for the log file.");
    }

    if (TRIVIA_WANTS_USAGE || TRIVIA_WANTS_HELP) {
#ifdef _WIN32
        {
            char path [MAX_PATH + 1];
#endif

            fprintf (
                stderr, "Usage: %s [OPTION...]\n",
#ifdef _WIN32
                ({
                    if (!GetModuleFileNameA (NULL, path, MAX_PATH + 1))
                        memcpy (path, "program", sizeof ("program"));
                    basename (path);
                })
#else
            basename (program_invocation_short_name)
#endif
            );
#ifdef _WIN32
        }
#endif

        if (!TRIVIA_WANTS_HELP)
            exit (0);

        fprintf (
            stderr,
            "\n%-20s\t\t" TRIVIA_USAGE_ARG_SHORT_RIGHT "\n%-20s\t\t" TRIVIA_USAGE_ARG_LONG_RIGHT
            "\n%-20s\t\t" TRIVIA_HELP_ARG_SHORT_RIGHT "\n%-20s\t\t" TRIVIA_HELP_ARG_LONG_RIGHT
            "\n%-20s\t\t" TRIVIA_NOLOG_ARG_RIGHT "\n%-20s\t\t" TRIVIA_LOG_ARG_RIGHT
            "\n%-20s\t\t" TRIVIA_LOGFILE_ARG_RIGHT,
            TRIVIA_USAGE_ARG_SHORT_LEFT, TRIVIA_USAGE_ARG_LONG_LEFT, TRIVIA_HELP_ARG_SHORT_LEFT,
            TRIVIA_HELP_ARG_LONG_LEFT, TRIVIA_NOLOG_ARG_LEFT, TRIVIA_LOG_ARG_LEFT, TRIVIA_LOGFILE_ARG_LEFT
        );

        exit (0);
    }

    if (!TRIVIA_WANTS_NOLOG) {
        set_log_file (LOG_FILENAME);
        open_log_file ();
    }

#ifdef _WIN32
    {
        bool checked = false;
    checksz:
#endif
        if (get_term_width () < MIN_TERM_WIDTH || get_term_height () < MIN_TERM_HEIGHT) {
#ifdef _WIN32
            goto checked_lbl;

        error_lbl:
#endif

            error ("the terminal is not big enough to display a UI.");
        }

#ifdef _WIN32

        else
            goto pass_lbl;

    checked_lbl:
        if (!checked) {
            checked = set_term_dims (
                get_term_width () < MIN_TERM_WIDTH ? MIN_TERM_WIDTH : 0,
                get_term_height () < MIN_TERM_HEIGHT ? MIN_TERM_HEIGHT : 0
            );

            goto checksz;
        }

        else
            goto error_lbl;

    pass_lbl:;
    }

    HWND win;
    if (!((win = GetConsoleWindow ()) &&
          SetWindowLong (win, GWL_STYLE, GetWindowLong (win, GWL_STYLE) & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX) &&
          SetWindowPos (win, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED)))
        warning ("could not prevent users from resizing the window.");
#endif

    setlocale (LC_ALL, "es_ES.UTF-8");

    initscr ();
    if ((INIT_CURS_VAR = curs_set (0)))
        warning ("could not retrieve the initial state of the cursor.");

    if (has_colors ()) {
        use_default_colors ();
        start_color ();

        short term_bckgd;

        init_pair (log_message + 1, COLOR_GREEN, (pair_content (0, &(short) { 0 }, &term_bckgd), term_bckgd));
        init_pair (log_warning + 1, COLOR_YELLOW, term_bckgd);
        init_pair (log_error + 1, COLOR_RED, term_bckgd);
    }

    create_log_window ();

    int settings = set_ui_settings (cursor, en_cbreak, en_echo, en_keypad, en_halfdelay);
    atexit (atexit_close_log_file);
    atexit (atexit_end_ui);

    message ("UI set up completed.");

    return settings;
}

int initial_cursor_mode (void) {
    return INIT_CURS_VAR;
}