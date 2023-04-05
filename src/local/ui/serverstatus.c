static void wait_server_start (void) {
    WINDOW *win = create_window (0, 0, 0, 0);
    box (win, 0, 0);

    mvwprintw (win, 2, 2, "Reiniciando el servidor, espera.");

    wrefresh (win);

    restart_server ();

    delete_window (win);
}

void server_status_menu (void) {
    delete_menu (get_server_status () == server_off ? actionmenu (
                                             0, 0, 0, 0, ((const char *const []) { "Encender servidor" }),
                                             ((choicefunc_t *const []) { start_server }), "Servidor apagado"
                                         )
    : get_server_status () == server_on
        ? actionmenu (
              0, 0, 0, 0, ((const char *const []) { "Reiniciar servidor", "Apagar servidor" }),
              ((choicefunc_t *const []) { restart_server, stop_server }), "Servidor encendido"
          )
        : actionmenu (
              0, 0, 0, 0, ((const char *const []) { "El servidor está siendo iniciado, espera." }),
              ((choicefunc_t *const []) { wait_server_start }), "Servidor encendido"
          ));
}
