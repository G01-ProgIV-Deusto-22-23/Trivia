void server_status_menu (void) {
    delete_menu (
        get_server_status () == server_off ? actionmenu (
                                                 0, 0, 0, 0, ((const char *const []) { "Encender servidor" }),
                                                 ((choicefunc_t *const []) { start_server }), "Servidor apagado"
                                             )
        : get_server_status () == server_on
            ? actionmenu (
                  0, 0, 0, 0, ((const char *const []) { "Reiniciar servidor", "Apagar servidor" }),
                  ((choicefunc_t *const []) { restart_server, stop_server }), "Servidor encendido"
              )
            : choicemenu (
                  0, 0, 0, 0, ((const char *const []) { "El servidor está siendo iniciado, espera." }),
                  "Servidor reinciándose."
              )
    );
}
