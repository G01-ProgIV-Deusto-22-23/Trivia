void user_menu (
    const
#ifdef _WIN32
    SOCKET
#else
    int
#endif
            s,
    Usuario u
) {
    static struct sockaddr_in addr;
    static socklen_t          addr_size = sizeof (struct sockaddr_in);
    if (getpeername (s, (struct sockaddr *) &addr, &addr_size) ==
#ifdef _WIN32
        SOCKET_ERROR
#else
        -1
#endif
    )
        error ("could not get an address from the socket.");

    static char ip [sizeof ("255.255.255.255")] = { 0 };
    memccpy (ip, inet_ntoa (addr.sin_addr), '\0', sizeof ("255.255.255.255") - 1);
    static const int port = ntohs (addr.sin_port);

    static size_t m;
main : {
    static char title [sizeof ("Hola, ") + sizeof (u.nombreVisible) + sizeof ("()") + sizeof (u.username) - 3] =
        "Hola, ";
    *(char *) mempcpy (
        mempcpy (mempcpy (title + sizeof ("Hola, "), u.nombreVisible, strlen (u.nombreVisible)), "(", sizeof ("(") - 1),
        u.username, strlen (u.username)
    )                                = ')';
    static const char *const opts [] = { "Jugar una partida", "Ver mis datos", "Cambiar mi nombre visible",
                                         "Cambiar mi contraseña", "Limpiar mis estadísticas" };
    m                                = choicemenu (0, 0, 0, 0, opts, title);
}

    static size_t r = get_menu_ret (m) ? *get_menu_ret (m) : (size_t) -1;
    if (!delete_menu (m))
        warning ("could not delete the menu properly.");

    if (r == (size_t) -1)
        return;

    if (!r) {
        static cmd_t game_list_cmd;
        if ((game_list_cmd = send_server (ip, port, game_list_command (), NULL, 0)).cmd == cmd_error) {
            WINDOW *const w = create_window (0, 0, 0, 0);
            if (!w)
                error ("could not create the window.");

            box (w, 0, 0);
            mvwprintw (w, 6, 2, "No se pudo obtener la lista de partidas del servidor.");
            mvwprintw (w, 6, 2, "Pulsa Intro para continuar.");
            for (int c; (c = wgetch (w)) != '\r' && c != '\n' && c != KEY_ENTER;)
                ;

            if (!delete_window (w))
                warning ("the window could not be deleted properly.");

            goto main;
        }

        static char ids [MAX_GAMES][sizeof ("XXXX")];
        game_list_parse (ids, game_list_cmd.info.pack.text);

        static const char *id;

    select_game : {
        static const char *const opts [] = { "Jugar una partida local", "Aleatoria", "Seleccionar ID manualmente" };
        m                                = choicemenu (0, 0, 0, 0, opts, "Escoger una partida");
    }

        r = get_menu_ret (m) ? *get_menu_ret (m) : (size_t) -1;

        if (!get_menu_ret (m))
            warning ("could not get the user's response.");

        if (!delete_menu (m))
            warning ("could not delete the menu properly.");

        if (r == (size_t) -1 || !r) {
            if (!r)
                local_game ();

            goto main;
        }

        if (r == 1)
            id =
                *(ids + (size_t) (srand ((unsigned) time (NULL)), rand ()) %
                            (size_t) ((double) get_games () * (double) get_pub_games_perc () / 100.0));

        else {
            static const size_t games = (size_t) ((double) get_games () * (double) get_pub_games_perc () / 100.0);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow=local"
#pragma GCC diagnostic ignored "-Wshadow=compatible-local"

            static const char *opts [6][2] = { { "", NULL }, { "", NULL }, { "", NULL },
                                               { "", NULL }, { "", NULL }, { ">", "Siguientes 5 partidas" } };

#pragma GCC diagnostic pop

            static const linkedlist_t game_list = create_linkedlist (*ids);
            if (!game_list)
                error ("could not create the list of games.");

            for (size_t i = 1; i < games;)
                if (!insert_linkedlist (game_list, *(ids + i++)))
                    warning ("game could not be inserted in the games list.");

            for (;;) {
                for (size_t i = 0; i < 5; i++)
                    if ((*(*(opts + i) + 1) = reinterpret_cast<const char *> (pop_linkedlist (game_list, 0))))
                        insert_linkedlist (game_list, const_cast<char *> (*(*(opts + i) + 1)));
                    else
                        *(*(opts + i) + 1) = "";

            menu:
                m = choicemenu (0, 0, 0, 0, opts, "Escoger una partida");
                r = get_menu_ret (m) ? *get_menu_ret (m) : (size_t) -1;

                if (!get_menu_ret (m))
                    warning ("could not get the user's response.");

                if (!delete_menu (m))
                    warning ("could not delete the menu properly.");

                if (r == (size_t) -1)
                    goto select_game;

                if (r == 5)
                    continue;

                if (!**(*(opts + r) + 1))
                    goto menu;

                break;
            }
        }

        static QuestionHandler qh (ip, port, u, id);
        qh.game ();
        qh.sendResults ();

        goto main;
    }

    if (r == 1) {
        WINDOW *const w = create_window (0, 0, 0, 0);
        if (!w)
            error ("could not create the user data window.");

        static char id_str [sizeof ("Nombre de usuario: ") + sizeof (stringify (UINT32_MAX)) - 1];
        static char visname_str [sizeof ("Nombre de usuario: ") + sizeof (u.nombreVisible) - 2];
        static char uname_str [sizeof ("Nombre de usuario: ") + sizeof (u.username) - 2];
        static char ok_str [sizeof ("Nombre de usuario: ") + sizeof (stringify (UINT32_MAX)) - 1];
        static char fail_str [sizeof ("Nombre de usuario: ") + sizeof (stringify (UINT32_MAX)) - 1];

        sprintf (
            id_str, "%-*s%" PRIu32, (int) (sizeof ("Nombre de usuario: ") - sizeof ("ID: ")), "ID: ", u.ID_Usuario
        );
        sprintf (
            visname_str, "%-*s%s", (int) (sizeof ("Nombre de usuario: ") - sizeof ("Nombre visible")), "Nombre visible",
            u.nombreVisible
        );
        sprintf (uname_str, "%s%s", "Nombre de usuario: ", u.username);
        sprintf (
            ok_str, "%-*s%" PRIu32, (int) (sizeof ("Nombre de usuario: ") - sizeof ("Aciertos totales")),
            "Aciertos totales", u.ID_Usuario
        );
        sprintf (
            fail_str, "%-*s%" PRIu32, (int) (sizeof ("Nombre de usuario: ") - sizeof ("Fallos totales")),
            "Fallos totales", u.fallosTotales
        );

        box (w, 0, 0);

        mvwprintw (w, 5, 2, "%s", id_str);
        mvwprintw (w, 6, 2, "%s", visname_str);
        mvwprintw (w, 7, 2, "%s", uname_str);
        mvwprintw (w, 8, 2, "%s", ok_str);
        mvwprintw (w, 9, 2, "%s", fail_str);

        mvwprintw (w, 10, 2, "Pulsa Intro para continuar.");
        for (int c; (c = wgetch (w)) != '\r' && c != '\n' && c != KEY_ENTER;)
            ;

        if (!delete_window (w))
            warning ("could not delete the user data window properly.");

        goto main;
    }

    if (r == 2) {
        static const field_attr_t fields [] = { alnum_field (20) };
        static const char *const  titles [] = { "Nombre visible:" };
        static const size_t       f =
            form (0, 0, 0, 0, fields, titles, "Elige un nombre visible (vació para usar el nombre de usuario)");

        static char prevname [sizeof (u.nombreVisible)];
        memccpy (prevname, u.nombreVisible, '\0', sizeof (u.nombreVisible) - 1);

        {
            static const char *name = *get_form_data (f);
            memccpy (u.nombreVisible, *name ? name : u.username, '\0', sizeof (u.nombreVisible));
            delete_form (f);
        }

        static const cmd_t resp = send_server (ip, port, update_user_command (u), NULL, 0);

        if (resp.cmd == cmd_error) {
            memccpy (u.nombreVisible, prevname, '\0', sizeof (u.nombreVisible) - 1);

            WINDOW *const w = create_window (0, 0, 0, 0);
            if (!w)
                error ("could not create the window.");

            box (w, 0, 0);
            mvwprintw (w, 5, 2, "No se pudo actualizar el nombre visible del usuario.");
            mvwprintw (w, 6, 2, "Pulsa Intro para continuar.");
            for (int c; (c = wgetch (w)) != '\r' && c != '\n' && c != KEY_ENTER;)
                ;

            if (!delete_window (w))
                warning ("the window could not be deleted properly.");
        }

        goto main;
    }

    if (r == 3) {
        static const field_attr_t fields [] = { passwd_field (sizeof (u.contrasena) - 1),
                                                passwd_field (sizeof (u.contrasena) - 1),
                                                passwd_field (sizeof (u.contrasena) - 1) };
        static const char *const  titles [] = { "Contraseña actual:", "Nueva contraseña", "Confirmar contraseña" };
        static const size_t       f         = create_form (0, 0, 0, 0, fields, titles);

    form:
        display_form (f, "Cambiar contraseña");

        if (!(**get_form_data (f) && *(*(get_form_data (f)) + 1) && *(*(get_form_data (f)) + 2))) {
            WINDOW *const w = create_window (0, 0, 0, 0);
            if (!w)
                error ("could not create the window.");

            box (w, 0, 0);
            mvwprintw (w, 5, 2, "Ninguno de los campos puede estar vacío.");
            mvwprintw (w, 6, 2, "Pulsa Intro para continuar.");
            for (int c; (c = wgetch (w)) != '\r' && c != '\n' && c != KEY_ENTER;)
                ;

            if (!delete_window (w))
                warning ("the window could not be deleted properly.");

            goto form;
        }

        if (strcmp (u.contrasena, *get_form_data (f))) {
            WINDOW *const w = create_window (0, 0, 0, 0);
            if (!w)
                error ("could not create the window.");

            box (w, 0, 0);
            mvwprintw (w, 5, 2, "La contraseña actual no es correcta.");
            mvwprintw (w, 6, 2, "Pulsa Intro para continuar.");
            for (int c; (c = wgetch (w)) != '\r' && c != '\n' && c != KEY_ENTER;)
                ;

            if (!delete_window (w))
                warning ("the window could not be deleted properly.");

            goto form;
        }

        if (strcmp (*(get_form_data (f) + 1), *(get_form_data (f) + 2))) {
            WINDOW *const w = create_window (0, 0, 0, 0);
            if (!w)
                error ("could not create the window.");

            box (w, 0, 0);
            mvwprintw (w, 5, 2, "Las contraseñas no coinciden.");
            mvwprintw (w, 6, 2, "Pulsa Intro para continuar.");
            for (int c; (c = wgetch (w)) != '\r' && c != '\n' && c != KEY_ENTER;)
                ;

            if (!delete_window (w))
                warning ("the window could not be deleted properly.");

            goto form;
        }

        static char prevpasswd [sizeof (u.nombreVisible)];
        memccpy (prevpasswd, u.nombreVisible, '\0', sizeof (u.contrasena) - 1);
        memccpy (u.contrasena, *(get_form_data (f) + 1), '\0', sizeof (u.contrasena));

        delete_form (f);

        static const cmd_t resp = send_server (ip, port, update_user_command (u), NULL, 0);

        if (resp.cmd == cmd_error) {
            memccpy (u.nombreVisible, prevpasswd, '\0', sizeof (u.contrasena) - 1);

            WINDOW *const w = create_window (0, 0, 0, 0);
            if (!w)
                error ("could not create the window.");

            box (w, 0, 0);
            mvwprintw (w, 5, 2, "No se pudo actualizar el nombre visible del usuario.");
            mvwprintw (w, 6, 2, "Pulsa Intro para continuar.");
            for (int c; (c = wgetch (w)) != '\r' && c != '\n' && c != KEY_ENTER;)
                ;

            if (!delete_window (w))
                warning ("the window could not be deleted properly.");
        }

        goto main;
    }

    static const char *const opts [][2] = { { "", "Sí" }, { "", "No" } };
    m                                   = choicemenu (0, 0, 0, 0, opts, "¿Estás seguro?");
    r                                   = get_menu_ret (m) ? *get_menu_ret (m) : (size_t) -1;

    if (!get_menu_ret (m))
        warning ("could not get the user's response.");

    if (!delete_menu (m))
        warning ("could not delete the menu properly.");

    if (r == (size_t) -1 || r == 1)
        goto main;

    const uint32_t prev [2] = { u.aciertosTotales, u.fallosTotales };
    u.aciertosTotales       = 0;
    u.fallosTotales         = 0;

    if (send_server (ip, port, update_user_command (u), NULL, 0).cmd == cmd_error) {
        u.aciertosTotales = *prev;
        u.fallosTotales   = *(prev + 1);

        WINDOW *const w = create_window (0, 0, 0, 0);
        if (!w)
            error ("could not create the window.");

        box (w, 0, 0);
        mvwprintw (w, 5, 2, "No se pudieron limpiar las estadísticas.");
        mvwprintw (w, 6, 2, "Pulsa Intro para continuar.");
        for (int c; (c = wgetch (w)) != '\r' && c != '\n' && c != KEY_ENTER;)
            ;

        if (!delete_window (w))
            warning ("the window could not be deleted properly.");
    }

    goto main;
}