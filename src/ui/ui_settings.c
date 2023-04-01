static int TRIVIA_UI_SETTINGS_VAR = 0;

int get_ui_settings (void) {
    return TRIVIA_UI_SETTINGS_VAR;
}

int impl_set_ui_settings (
    const int cursor, const int en_cbreak, const int en_echo, const int en_keypad, const int en_halfdelay
) {
    if (curs_set (cursor & 3) == ERR) {
        if (!cursor)
            warning ("could not hide the cursor.");

        else if (cursor & 1)
            warning ("could not make the cursor visible.");

        else
            warning ("could not make the cursor very visible.");
    }

    if (en_cbreak)
        cbreak ();
    else
        raw ();

    nonl ();

    if (en_echo)
        echo ();
    else
        noecho ();

    keypad (stdscr, !!en_keypad);

    halfdelay (get_halfdelay_secs () * !!en_halfdelay);

    refresh ();

    return TRIVIA_UI_SETTINGS_VAR = compact_ui_settings (cursor, en_cbreak, en_echo, en_keypad, en_halfdelay);
}