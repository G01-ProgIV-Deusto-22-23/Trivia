static void restart_server2 (void) {
    restart_server (get_server_port ());
}

static void stop_server2 (void) {
    stop_server (get_server_port ());
}

void server_status_menu (void) {
    size_t m = get_server_status () == server_off     ? ({
        const size_t _m = create_menu (
            actionmenu, 0, 0, 0, 0, ((const char *const []) { "Encender servidor" }),
            ((choicefunc_t *const []) { start_server })
        );
        loop_menu (_m, false);
        set_menu_title_color (_m, title_red);
        display_menu (_m, "Servidor apagado");
    })
               : get_server_status () == server_error ? ({
                     size_t _m = create_menu (
                         actionmenu, 0, 0, 0, 0, ((const char *const []) { "Reiniciar servidor" }),
                         ((choicefunc_t *const []) { restart_server2 })
                     );
                     set_menu_title_color (_m, title_red);
                     display_menu (_m, "Error en el servidor");
                 })
               : get_server_status () == server_on
                   ? actionmenu (
                         0, 0, 0, 0, ((const char *const []) { "Reiniciar servidor", "Apagar servidor" }),
                         ((choicefunc_t *const []) { restart_server2, stop_server2 }), "Servidor encendido"
                     )
                   : choicemenu (
                         0, 0, 0, 0, ((const char *const []) { "El servidor está siendo iniciado, espera." }),
                         "Servidor inciándose"
                     );

    if (!delete_menu (m))
        warning ("could not delete menu");

    if (!get_menu_ret (m) || *get_menu_ret (m) == (size_t) -1)
        return;

    server_status_menu ();
}
