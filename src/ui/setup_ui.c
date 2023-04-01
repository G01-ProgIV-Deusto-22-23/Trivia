static int INIT_CURS_VAR;

int impl_setup_ui (
    const int cursor, const int en_cbreak, const int en_echo, const int en_keypad, const int en_halfdelay
) {
    if (get_term_width () < MIN_TERM_WIDTH || get_term_height () < MIN_TERM_HEIGHT)
        error ("the terminal is not big enough to display a UI.");

#ifdef _WIN32
    SetConsoleCP (65001);
    setlocale (LC_ALL, "es_ES.UTF-8");
#endif

    initscr ();
    if ((INIT_CURS_VAR = curs_set (0)))
        warning ("could not retrieve the initial state of the cursor.");

    if (has_colors ()) {
        use_default_colors ();
        start_color ();

        short term_bckgd;

        init_pair (
            log_message + 1, COLOR_GREEN,
            (pair_content (
                 0,
#ifdef _WIN32
                 &(short) { 0 }, &term_bckgd
#else
                 &term_bckgd, &(short) { 0 }
#endif
             ),
             term_bckgd)
        );
        init_pair (log_warning + 1, COLOR_YELLOW, term_bckgd);
        init_pair (log_error + 1, COLOR_RED, term_bckgd);
    }

    create_log_window ();
    message ("UI set up completed.");

    return set_ui_settings (cursor, en_cbreak, en_echo, en_keypad, en_halfdelay);
}

int initial_cursor_mode (void) {
    return INIT_CURS_VAR;
}