static void restart_server2 (void) {
    restart_server (get_server_port ());
}

static void stop_server2 (void) {
    stop_server (get_server_port ());
}

void server_status_menu (void) {
    const server_status_t s = get_server_status ();

    size_t m =
        s == server_off     ? ({
            const size_t _m = create_menu (
                actionmenu, 0, 0, 0, 0, ((const char *const []) { "Encender servidor" }),
                ((choicefunc_t *const []) { start_server })
            );
            loop_menu (_m, false);

            set_menu_title_color (_m, title_red);
            display_menu (_m, "Servidor apagado");
        })
        : s == server_error ? ({
              size_t _m = create_menu (
                  actionmenu, 0, 0, 0, 0, ((const char *const []) { "Reiniciar servidor" }),
                  ((choicefunc_t *const []) { restart_server2 })
              );

              loop_menu (_m, false);
              set_menu_title_color (_m, title_red);
              display_menu (_m, "Error en el servidor");
          })
        : s == server_on
            ? ({
                  size_t _m = create_menu (
                      actionmenu, 0, 0, 0, 0, ((const char *const []) { "Reiniciar servidor", "Apagar servidor" }),
                      ((choicefunc_t *const []) { restart_server2, stop_server2 })
                  );
                  loop_menu (_m, false);
                  set_menu_title_color (_m, title_green);
                  display_menu (_m, "Servidor encendido");
              })
            : ({
                  size_t _m = create_menu (choicemenu, 0, 0, 0, 0, ((const char *const []) { "Por favor, espera." }));

                  loop_menu (_m, false);
                  set_menu_title_color (_m, title_yellow);
                  display_menu (_m, s == server_starting ? "Servidor iniciándose" : "Servidor reiniciándose");
              });

    if (!delete_menu (m))
        warning ("could not delete menu");

    if (!get_menu_ret (m) || *get_menu_ret (m) == (size_t) -1)
        return;

    server_status_menu ();
}
