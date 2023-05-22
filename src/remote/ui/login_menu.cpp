cmd_t login_menu (const
#ifdef _WIN32
                  SOCKET
#else
                  int
#endif
                      socket) {
    struct sockaddr_in addr;
    socklen_t          addr_size = sizeof (struct sockaddr_in);
    if (getpeername (socket, (struct sockaddr *) &addr, &addr_size) ==
#ifdef _WIN32
        SOCKET_ERROR
#else
        -1
#endif
    )
        error ("could not get an address from the socket.");

    char ip [sizeof ("255.255.255.255")] = { 0 };
    memccpy (ip, inet_ntoa (addr.sin_addr), '\0', sizeof ("255.255.255.255") - 1);

    static const char *const choices [] = { "Iniciar sesión", "Registrarse" };
    const size_t             m          = choicemenu (0, 0, 0, 0, choices);
    if (get_menu_ret (m))
        error ("could not get the user's choice.");

    const size_t r = *get_menu_ret (m);

    delete_menu (m);

    if (r == (size_t) -1)
        exit (0);

    static const field_attr_t fields [] = { alnum_field (20), passwd_field (20) };
    static const char *const  titles [] = { "Usuario:", "Contraseña:" };
    static const size_t       f         = form (0, 0, 0, 0, fields, titles, r ? "Registrarse" : "Iniciar sesión");

    Usuario u;
    memccpy (&u.username, *get_form_data (f), '\0', sizeof (u.username));
    memccpy (&u.contrasena, *(get_form_data (f) + 1), '\0', sizeof (u.contrasena));
    delete_form (f);

    const int port = ntohs (addr.sin_port);

    if (r) {
        const cmd_t resp = send_server (ip, port, insert_user_command (u), NULL, 0);

        if (resp.cmd == cmd_error) {
            WINDOW *const w = create_window (0, 0, 0, 0);
            if (!w)
                error ("could not create the window.");

            box (w, 0, 0);

            if (resp.info.arg [0] == CMD_ERROR_SEND) {
                warning ("timeout for sending the user registration information failed.");
                mvwprintw (w, 5, 2, "Pasó el tiempo para enviar la información del usuario al servidor.");
            }

            else if (resp.info.arg [0] == CMD_ERROR_INSERT_USER) {
                warning ("the user already exists.");
                mvwprintw (w, 5, 2, "El usuario ya existe.");
            }

            else {
                warning ("an unspecified error occurred.");
                mvwprintw (w, 5, 2, "Ha occurrido un error no especificado.");
            }

            mvwprintw (w, 6, 2, "Pulsa Intro para continuar.");
            for (int c; (c = wgetch (w)) != '\r' && c != '\n' && c != KEY_ENTER;)
                ;

            if (!delete_window (w))
                warning ("the window could not be deleted properly.");

            return login_menu (socket);
        }
    }

    return send_server (ip, port, user_creds_command (u), NULL, 0);
}