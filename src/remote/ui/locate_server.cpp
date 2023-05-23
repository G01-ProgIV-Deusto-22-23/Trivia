#ifdef _WIN32
SOCKET
#else
int
#endif
locate_server (void) {
    size_t m;
    {
        const char *const opts [] = { "Este equipo", "Otro equipo (introducir IP)" };
        m                         = choicemenu (0, 0, 0, 0, opts, "Indica dónde se encuentra el servidor.");
    }

    if (!get_menu_ret (m))
        error ("no se pudo obtener la respuesta del usuario.");

    const size_t r = *get_menu_ret (m);
    delete_menu (m);

    if (r == (size_t) -1)
        exit (0);

    char ip [sizeof ("255.255.255.255")] = { 0 };
    int  port                            = 0;
    if (r) {
        field_attr_t      fields [] = { ipv4_field (), int_field (0, IANA_DYNAMIC_PORT_END) };
        const char *const titles [] = { "IP (vacío para este mismo equipo):",
                                        "Puerto (0 o vacío para puerto predeterminado):" };
        size_t            f         = form (0, 0, 0, 0, fields, titles);

        memccpy (ip, *get_form_data (f), '\0', sizeof (ip));
        if (strcmp ("0", *(get_form_data (f) + 1)) && !(port = (int) strtol (*(get_form_data (f) + 1), NULL, 10)))
            warning ("could not parse the port number.");

        delete_form (f);
    }

    return connect_server (*ip ? ip : NULL, port ? port : get_server_port ());
}