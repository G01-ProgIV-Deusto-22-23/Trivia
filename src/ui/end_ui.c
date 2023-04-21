int end_ui (void) {
    if (!delete_windows ())
        warning ("could not delete all windows.");

    if (!isendwin ()) {
        noraw ();
        nocbreak ();
        if (curs_set (initial_cursor_mode ()) == ERR)
            warning ("could not restore the initial state of the cursor.");

        clear ();

        if (endwin () == ERR) {
            warning ("could not end the UI sucessfully.");

            return ERR;
        }
    }

    message ("the UI was ended successfully.");

    return OK;
}
