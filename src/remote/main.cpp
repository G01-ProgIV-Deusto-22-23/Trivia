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

                if (!delete_window (w))
                    warning ("the window could not be deleted properly.");
            }

            continue;
        }

        break;
    }

    static cmd_t loginfo;

login:
    login_menu (s);
    if (loginfo.cmd == cmd_error) {
        if (*loginfo.info.arg == CMD_ERROR_NO_USER) {
            message ("Las credenciales de usuario elegidas no son correctas.");

            const char *const opts [] = { "Sí", "No" };
            const size_t      m       = choicemenu (0, 0, 0, 0, opts, "Volver a intentarlo?");
            const size_t      ret     = get_menu_ret (m) ? *get_menu_ret (m) : (size_t) -1;
            delete_menu (m);

            if (!ret)
                goto login;
        }

        else
            warning ("Hubo un error al intentar iniciar sesión.");
    }

    return 0;
}