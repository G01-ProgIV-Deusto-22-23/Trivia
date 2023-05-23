trivia_main {
    setup_ui ();

    static const char *const opts [] = { "Jugar una partida como invitado", "Continuar como usuario" };

main_menu : {
    const size_t m = choicemenu (0, 0, 0, 0, opts, "Bienvenido");
    const size_t r = get_menu_ret (m) ? *get_menu_ret (m) : (size_t) -1;

    if (!get_menu_ret (m))
        error ("could not get the user's response.");

    if (!delete_menu (m))
        warning ("the menu could not be properly deleted.");

    if (r == (size_t) -1)
        return 0;

    if (!r) {
        local_game ();

        goto main_menu;
    }
}

    static
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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow=local"
#pragma GCC diagnostic ignored "-Wshadow=compatible-local"

            static const char *const opts [] = { "Sí", "No" };
            static const size_t      m       = choicemenu (0, 0, 0, 0, opts, "Volver a intentarlo?");
            static const size_t      r       = get_menu_ret (m) ? *get_menu_ret (m) : (size_t) -1;

#pragma GCC diagnostic pop

            if (!get_menu_ret (m))
                error ("could not get the user's response.");

            delete_menu (m);

            if (r)
                goto main_menu;

            goto login;
        }

        else {
            warning ("Hubo un error al intentar iniciar sesión.");

            goto main_menu;
        }
    }

    user_menu (s, loginfo.info.user);

    return 0;
}