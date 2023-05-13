#ifdef __cplusplus
extern "C" {
#endif

#include "log_window.h"

#define LOG_FILENAME_TRIES    5
#define LOG_FILENAME_SUFFIX   ".log"
#define LOG_FILENAME_MOD      "XXXXXX"
#define LOG_FILENAME_TEMPLATE LOG_FILENAME_MOD LOG_FILENAME_SUFFIX

    WINDOW *LOG_WINDOW_VAR = NULL;

    static uint64_t LOG_WINDOW_DIMS_VAR = UINT64_C (-1);
    static int      LOG_FILE_FILENO     = -1;
    static int      STDERR_SAVE         = -1;
    static int      LAST_LOG_LINE       = 0;
    static char     LOG_FILENAME [sizeof (LOG_FILENAME_TEMPLATE)];

    int get_log_file (void) {
        return LOG_FILE_FILENO;
    }

    int impl_set_log_file (const int fd, const bool temp) {
        if (!temp)
            LOG_FILE_FILENO = fd;
        else
            temp_log_file ();

        setbuf (
            ({
                FILE *const f = fdopen (LOG_FILE_FILENO, "r+");
                if (!f)
                    error ("could not open the log file.");
                LOG_FILE_FILENO =
#ifdef _WIN32
                    _fileno (f);
#else
                fileno (f);
#endif
                f;
            }),
            NULL
        );

        return LOG_FILE_FILENO;
    }

    int temp_log_file (void) {
        size_t tries = 0;

    tempfile:
#ifdef _WIN32
        LOG_FILE_FILENO = mkstemp (memcpy (LOG_FILENAME, LOG_FILENAME_MOD, sizeof (LOG_FILENAME_MOD)));
#else
    LOG_FILE_FILENO = mkstemps (
        memcpy (LOG_FILENAME, LOG_FILENAME_TEMPLATE, sizeof (LOG_FILENAME_TEMPLATE)), sizeof (LOG_FILENAME_SUFFIX) - 1
    );
#endif

        if (LOG_FILE_FILENO == -1) {
            if (errno == EEXIST) {
                tries++;

                if (tries == LOG_FILENAME_TRIES &&
#ifdef _WIN32
                    (_sopen_s (
                         &LOG_FILE_FILENO, LOG_FILENAME, _O_CREAT | _O_RDWR | _O_SEQUENTIAL | _O_U8TEXT, _SH_DENYNO,
                         _S_IREAD | _S_IWRITE
                     ),
                     LOG_FILE_FILENO == -1)
#else
                ((LOG_FILE_FILENO = open (LOG_FILENAME, O_RDWR, S_IRUSR | S_IWUSR)) == -1)
#endif
                ) {
                    warning ("could not create a temporary log file.");

                    return LOG_FILE_FILENO;
                }

                goto tempfile;
            }

            else
                return LOG_FILE_FILENO;
        }

#ifdef _WIN32
        if (({
                char newname [sizeof (LOG_FILENAME_TEMPLATE)];
                int  res = -1;
                if (!_close (LOG_FILE_FILENO)) {
                    memcpy (
                        mempcpy (newname, LOG_FILENAME, sizeof (LOG_FILENAME_MOD) - 1), LOG_FILENAME_SUFFIX,
                        sizeof (LOG_FILENAME_SUFFIX)
                    );
                    if ((res = rename (LOG_FILENAME, newname)) == -1) {
                        res = remove (newname);
                    }
                    memcpy (LOG_FILENAME, newname, sizeof (LOG_FILENAME));
                }

                res;
            }))
            return LOG_FILE_FILENO;

        _sopen_s (
            &LOG_FILE_FILENO, LOG_FILENAME, _O_CREAT | _O_RDWR | _O_SEQUENTIAL | _O_U8TEXT, _SH_DENYNO,
            _S_IREAD | _S_IWRITE
        );
#endif

        return LOG_FILE_FILENO;
    }

    int open_log_file (void) {
        if (LOG_FILE_FILENO == -1)
            return -1;

        int ret;

        if (STDERR_SAVE == -1)
            if ((STDERR_SAVE =
#ifdef _WIN32
                     _dup (_fileno (stderr))
#else
                 dup (STDERR_FILENO)
#endif
                ) == -1)
                error ("could not save stderr.");

        if ((ret =
#ifdef _WIN32
                 _dup2 (LOG_FILE_FILENO, _fileno (stderr))
#else
             dup2 (LOG_FILE_FILENO, STDERR_FILENO)
#endif
            ) == -1)
            error ("could not redirect stderr.");

        return ret;
    }

    int close_log_file (void) {
        if (STDERR_SAVE == -1)
            return -1;

        fflush (stderr);

        int ret;

        if ((ret =
#ifdef _WIN32
                 _dup2 (STDERR_SAVE, _fileno (stderr))
#else
             dup2 (STDERR_SAVE, STDERR_FILENO)
#endif
            ) == -1)
            warning ("could not restore stderr.");

        return ret;
    }

    bool is_log_window (void) {
        return LOG_WINDOW_VAR;
    }

    WINDOW *get_log_window (void) {
        return LOG_WINDOW_VAR;
    }

    int refresh_log_window (void) {
        if (!LOG_WINDOW_VAR)
            return ERR;

        box (LOG_WINDOW_VAR, 0, 0);
        return wrefresh (LOG_WINDOW_VAR) | refresh ();
    }

    int clear_log_window (void) {
        fflush (stderr);

        if (wclear (LOG_WINDOW_VAR) == ERR) {
            WINDOW *temp   = LOG_WINDOW_VAR;
            LOG_WINDOW_VAR = NULL;
            warning ("could not clear the log window.");
            LOG_WINDOW_VAR = temp;

            return ERR;
        }
        clearok (LOG_WINDOW_VAR, TRUE);
        LAST_LOG_LINE = 0;

        return refresh_log_window ();
    }

    WINDOW *create_log_window (void) {
        if (LOG_WINDOW_VAR)
            delete_log_window ();

        nodelay (
            LOG_WINDOW_VAR = create_window (
                get_log_window_width (), get_log_window_height (), (uint32_t) getbegx (stdscr),
                (uint32_t) getmaxy (stdscr) - get_log_window_height ()
            ),
            TRUE
        );
        refresh_log_window ();

        open_log_file ();

        return LOG_WINDOW_VAR;
    }

    bool delete_log_window (void) {
        return delete_window (LOG_WINDOW_VAR) == OK;
    }

    uint64_t get_log_window_dims (void) {
        if (LOG_WINDOW_DIMS_VAR == UINT64_C (-1))
            LOG_WINDOW_DIMS_VAR = u64comp (DEFAULT_LOG_WINDOW_WIDTH, DEFAULT_LOG_WINDOW_HEIGHT);

        return LOG_WINDOW_DIMS_VAR;
    }

    uint64_t impl_set_log_window_dims (const uint32_t w, const uint32_t h) {
        LAST_LOG_LINE = 0;
        delwin (LOG_WINDOW_VAR);

        LOG_WINDOW_DIMS_VAR = u64comp (w, h);
        LOG_WINDOW_VAR      = create_window (
            get_log_window_width (), get_log_window_height (), (uint32_t) getbegx (stdscr),
            (uint32_t) getmaxy (stdscr) - get_log_window_height ()
        );

        return LOG_WINDOW_DIMS_VAR;
    }

    int inc_last_log_line (void) {
        return ++LAST_LOG_LINE;
    }

#ifdef __cplusplus
}
#endif