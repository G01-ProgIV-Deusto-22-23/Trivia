#include <curses.h>
#ifdef __cplusplus
extern "C" {
#endif

    static WINDOW           *LOG_WINDOW_VAR      = NULL;
    static volatile uint64_t LOG_WINDOW_DIMS_VAR = UINT64_C (-1);
    static volatile int      LOG_FILE_FILENO     = -1;
    static volatile int      STDERR_SAVE         = -1;
    static volatile int      LAST_LOG_LINE       = 0;

#ifdef _WIN32
#else
#endif

    int get_log_file (void) {
        return LOG_FILE_FILENO;
    }

    int impl_set_log_file (const int fd) {
        return LOG_FILE_FILENO = fd;
    }

    int open_log_file (void) {
        if (LOG_FILE_FILENO == -1)
            return -1;

        if (STDERR_SAVE == -1)
            STDERR_SAVE = dup (STDERR_FILENO);

        return dup2 (LOG_FILE_FILENO, STDERR_FILENO);
    }

    int close_log_file (void) {
        if (STDERR_SAVE == -1)
            return -1;

        return dup2 (STDERR_SAVE, STDERR_FILENO);
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

        refresh ();
        box (LOG_WINDOW_VAR, 0, 0);
        refresh ();
        return wrefresh (LOG_WINDOW_VAR);
    }

    int clear_log_window (void) {
        if (wclear (LOG_WINDOW_VAR) == ERR) {
            warning ("could not clear the log window.");

            return ERR;
        }

        LAST_LOG_LINE = 0;

        return refresh_log_window ();
    }

    WINDOW *create_log_window (void) {
        delete_log_window ();
        LOG_WINDOW_VAR = create_window (
            get_log_window_width (), get_log_window_height (), (uint32_t) getbegx (stdscr),
            (uint32_t) getmaxy (stdscr) - get_log_window_height ()
        );

        refresh_log_window ();

        open_log_file ();

        return LOG_WINDOW_VAR;
    }

    bool delete_log_window (void) {
        bool del       = delete_window (LOG_WINDOW_VAR);
        LOG_WINDOW_VAR = NULL;

        close_log_file ();

        return del;
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