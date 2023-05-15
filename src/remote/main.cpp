trivia_main {
    setup_ui ();

#ifdef _WIN32
    SOCKET s = INVALID_SOCKET
#else
    int s = -1
#endif
        ;
    for (;;) {
        if ((s = locate_server ()) ==
#ifdef _WIN32
            INVALID_SOCKET
#else
            -1
#endif
        ) {
            WINDOW *const restrict w = create_window (0, 0, 0, 0);
            if (!w)
                warning ("could not create the \"server not found\" window.");

            else {
                box (w, 0, 0);
                mvwprintw (w, 5, 2, "No se pudo encontrar el servidor.");
                mvwprintw (w, 6, 2, "Pulsa Intro para continuar.");

                for (int c; (c = wgetch (w)) != '\r' && c != '\n' && c != KEY_ENTER;)
                    ;
            }

            continue;
        }

        break;
    }

    return 0;
}