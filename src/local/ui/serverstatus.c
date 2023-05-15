static
#ifdef _WIN32
    DWORD
#else
    void *
#endif
    start_server3 (void __attribute__ ((unused)) * unused) {
    start_server ();

    return 0;
}

static void start_server2 (void) {
#ifdef _WIN32
    if (!CreateThread (
            &(SECURITY_ATTRIBUTES
            ) { .nLength = sizeof (SECURITY_ATTRIBUTES), .lpSecurityDescriptor = NULL, .bInheritHandle = TRUE },
            0, NULL, start_server3, 0, NULL
        ))
#else
    static pthread_t t;
    if (pthread_create (&t, NULL, start_server3, NULL) == -1)
#endif
        warning ("could not create the server start thread.");
}

static
#ifdef _WIN32
    DWORD
#else
    void *
#endif
    restart_server3 (void __attribute__ ((unused)) * unused) {
    restart_server (get_server_port ());

    return 0;
}

static void restart_server2 (void) {
#ifdef _WIN32
    if (!CreateThread (
            &(SECURITY_ATTRIBUTES
            ) { .nLength = sizeof (SECURITY_ATTRIBUTES), .lpSecurityDescriptor = NULL, .bInheritHandle = TRUE },
            0, NULL, restart_server3, 0, NULL
        ))
#else
    static pthread_t t;
    if (pthread_create (&t, NULL, restart_server3, NULL) == -1)
#endif
        warning ("could not create the server restart thread.");
}

static
#ifdef _WIN32
    DWORD
#else
    void *
#endif
    stop_server3 (void __attribute__ ((unused)) * unused) {
    stop_server (get_server_port ());

    return 0;
}

static void stop_server2 (void) {
#ifdef _WIN32
    if (!CreateThread (
            &(SECURITY_ATTRIBUTES
            ) { .nLength = sizeof (SECURITY_ATTRIBUTES), .lpSecurityDescriptor = NULL, .bInheritHandle = TRUE },
            0, NULL, stop_server3, 0, NULL
        ))
#else
    static pthread_t t;
    if (pthread_create (&t, NULL, stop_server3, NULL) == -1)
#endif
        warning ("could not create the server stop thread.");
}

void server_status_menu (void) {
    const server_status_t s = get_server_status ();

    size_t m =
        s == server_off     ? ({
            const size_t _m = create_menu (
                actionmenu, 0, 0, 0, 0, ((const char *const []) { "Encender servidor" }),
                ((choicefunc_t *const []) { start_server2 })
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
