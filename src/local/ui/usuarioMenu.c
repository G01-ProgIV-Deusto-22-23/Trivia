static void insert_user_ui (void) {
    static Usuario u;
    memset (&u, 0, sizeof (Usuario));

form : {
    const field_attr_t fields [] = { alnum_field (sizeof (u.username) - 1), alnum_field (sizeof (u.nombreVisible) - 1),
                                     passwd_field (sizeof (u.contrasena) - 1) };
    const char *const  titles [] = { "Nombre de usuario:", "Nombre visible (vacío para usar el nombre de usuario):",
                                     "Contraseña:" };
    const size_t       f         = form (0, 0, 0, 0, fields, titles, "Insertar usuario");

    if (!(**get_form_data (f) && **(get_form_data (f) + 2))) {
        WINDOW *const w = create_window (0, 0, 0, 0);
        if (!w)
            error ("could not create the \"no empty fields\" window.");

        box (w, 0, 0);

        if (!**get_form_data (f))
            mvwprintw (w, 5, 2, "El campo de nombre usuario no puede estar vacío.");

        if (!**(get_form_data (f) + 1))
            mvwprintw (w, 5 + !**get_form_data (f), 2, "El campo de contraseña no puede estar vacío.");

        mvwprintw (w, 6 + !**get_form_data (f), 2, "Pulsa Intro para continuar.");
        for (int c; (c = wgetch (w)) != '\r' && c != '\n' && c != KEY_ENTER;)
            ;

        if (!delete_window (w))
            warning ("could not delete the window properly.");

        delete_form (f);

        goto form;
    }

    memccpy (u.username, *get_form_data (f), '\0', sizeof (u.username) - 1);
    memccpy (
        u.nombreVisible, **(get_form_data (f) + 1) ? *(get_form_data (f) + 1) : *get_form_data (f), '\0',
        sizeof (u.nombreVisible) - 1
    );
    memccpy (u.contrasena, *(get_form_data (f) + 2), '\0', sizeof (u.contrasena) - 1);

    delete_form (f);

    const cmd_t resp = send_server (NULL, 0, insert_user_command (u), NULL, 0);
    if (resp.cmd == cmd_error) {
        warning ("could not insert the user.");

        WINDOW *const w = create_window (0, 0, 0, 0);
        if (!w)
            error ("could not create the \"insert user error\" window.");

        box (w, 0, 0);
        mvwprintw (
            w, 5, 2,
            *resp.info.arg == CMD_ERROR_INSERT_USER ? "No se pudo insertar el usuario porque ya existe."
                                                    : "Hubo un error no especificado a la hora de insertar el usuario."
        );
        mvwprintw (w, 6, 2, "Pulsa Intro para continuar.");
        for (int c; (c = wgetch (w)) != '\r' && c != '\n' && c != KEY_ENTER;)
            ;

        if (!delete_window (w))
            warning ("could not delete the window properly.");
    }

    else
        message ("the user was inserted successfully.");
}
}

static void modify_user_ui (void) {
    static Usuario u;
    memset (&u, 0, sizeof (Usuario));

form : {
    const field_attr_t fields [] = { alnum_field (sizeof (u.username) - 1) };
    const char *const  titles [] = { "Nombre de usuario:" };
    const size_t       f = form (0, 0, 0, 0, fields, titles, "Modificar usuario (deja el campo vacío para salir)");

    memccpy (u.username, *get_form_data (f), '\0', sizeof (u.username) - 1);

    if (!delete_form (f))
        warning ("the form could not be deleted properly.");
}

    {
        const cmd_t resp = send_server (NULL, 0, user_fetch_command (u), NULL, 0);
        if (resp.cmd == cmd_error) {
            warning ("could not fetch the user.");

            WINDOW *const w = create_window (0, 0, 0, 0);
            if (!w)
                error ("could not create the \"user creds fetch error\" window.");

            box (w, 0, 0);
            mvwprintw (
                w, 5, 2,
                *resp.info.arg == CMD_ERROR_NO_USER ? "El usuario no existe."
                                                    : "Hubo un error no especificado a la hora de eliminar el usuario."
            );
            mvwprintw (w, 6, 2, "Pulsa Intro para continuar.");
            for (int c; (c = wgetch (w)) != '\r' && c != '\n' && c != KEY_ENTER;)
                ;

            if (!delete_window (w))
                warning ("could not delete the window properly.");

            goto form;
        }

        u = resp.info.user;
    }

    {
        const field_attr_t fields [] = { alnum_field (sizeof (u.nombreVisible) - 1),
                                         passwd_field (sizeof (u.contrasena) - 1), int_field (0, (UINT32_MAX)),
                                         int_field (0, (UINT32_MAX)) };
        const char *const  titles [] = { "Nombre visible:", "Contraseña:", "Aciertos totales:", "Fallos totales:" };
        const size_t       f         = create_form (0, 0, 0, 0, fields, titles);

        static char
            data [4]
                 [ct_max (
                      ct_max (ct_max (sizeof (u.nombreVisible) - 1, sizeof (u.contrasena) - 1), decplaces (UINT32_MAX)),
                      decplaces (UINT32_MAX)
                  ) +
                  1];

        memccpy (*data, u.nombreVisible, '\0', sizeof (u.nombreVisible) - 1);
        sprintf (*(data + 2), "%" PRIu32, u.aciertosTotales);
        sprintf (*(data + 3), "%" PRIu32, u.fallosTotales);

        set_form_data (f, (void *) data);
        display_form (f, "Modificar usuario");

        const cmd_t resp = send_server (NULL, 0, update_user_command (u), NULL, 0);
        if (resp.cmd == cmd_error) {
            warning ("could not update the user.");

            WINDOW *const w = create_window (0, 0, 0, 0);
            if (!w)
                error ("could not create the \"update user error\" window.");

            box (w, 0, 0);
            mvwprintw (
                w, 5, 2,
                *resp.info.arg == CMD_ERROR_NO_USER
                    ? "No se pudo actualizar el usuario porque no existe."
                    : "Hubo un error no especificado a la hora de actualizar el usuario."
            );
            mvwprintw (w, 6, 2, "Pulsa Intro para continuar.");
            for (int c; (c = wgetch (w)) != '\r' && c != '\n' && c != KEY_ENTER;)
                ;

            if (!delete_window (w))
                warning ("could not delete the window properly.");
        }

        else
            message ("the user was updated successfully.");
    }
}

static void delete_user_ui (void) {
    static Usuario u;
    memset (&u, 0, sizeof (Usuario));

    const field_attr_t fields [] = { alnum_field (sizeof (u.username) - 1) };
    const char *const  titles [] = { "Nombre de usuario:" };
    const size_t       f = form (0, 0, 0, 0, fields, titles, "Eliminar usuario (deja el campo vacío para salir)");

    memccpy (u.username, *get_form_data (f), '\0', sizeof (u.username) - 1);

    if (!delete_form (f))
        warning ("the form could not be deleted properly.");

    const cmd_t resp = send_server (NULL, 0, delete_user_command (u), NULL, 0);
    if (resp.cmd == cmd_error) {
        warning ("could not delete the user.");

        WINDOW *const w = create_window (0, 0, 0, 0);
        if (!w)
            error ("could not create the \"delete user error\" window.");

        box (w, 0, 0);
        mvwprintw (
            w, 5, 2,
            *resp.info.arg == CMD_ERROR_NO_USER ? "No se pudo eliminar el usuario porque no existe."
                                                : "Hubo un error no especificado a la hora de eliminar el usuario"
        );
        mvwprintw (w, 6, 2, "Pulsa Intro para continuar.");
        for (int c; (c = wgetch (w)) != '\r' && c != '\n' && c != KEY_ENTER;)
            ;

        if (!delete_window (w))
            warning ("could not delete the window properly.");
    }

    else
        message ("the user was deleted successfully.");
}

void usuarioMenu (void) {
    const size_t m = actionmenu (
        0, 0, 0, 0, ((const char *[]) { "Insertar usuario", "Modificar usuario", "Eliminar usuario" }),
        ((choicefunc_t *const []) { insert_user_ui, modify_user_ui, delete_user_ui }), "Administración de usuarios"
    );

    delete_menu (m);
}
