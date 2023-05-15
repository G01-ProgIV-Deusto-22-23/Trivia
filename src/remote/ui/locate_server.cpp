void locate_server (void) {
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
    if (r) {
        field_attr_t fields [] = { ipv4_field () };
        size_t       f         = form (0, 0, 0, 0, fields, (const char *const []) { "IP del servidor:" });

        memccpy (ip, *get_form_data (f), '\0', sizeof (ip));

        delete_form (f);
    }
}