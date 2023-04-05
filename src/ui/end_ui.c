int end_ui (void) {
    //delete_windows ();

    if (!isendwin ()) {
        noraw ();
        nocbreak ();
        if (curs_set (initial_cursor_mode ()) == ERR)
            warning ("could not restore the initial state of the cursor.");

        refresh_log_window ();
        clear ();

        if (endwin () == ERR) {
            warning ("could not end the UI sucessfully.");

            return ERR;
        }
    }

    message ("the UI was ended successfully.");

    return OK;
}
