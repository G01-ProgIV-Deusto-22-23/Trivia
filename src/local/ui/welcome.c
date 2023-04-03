#if defined(__cpp_attributes) || __STDC_VERSION__ > 201710L
[[noreturn]]
#else
__attribute__ ((noreturn))
#endif
void welcome (void) {
    actionmenu (
        0, 0, 0, 0,
        ((const char *[]) { "Administrar partidas", "Configurar el servidor", "Administrar el estado del servidor" }),
        ((choicefunc_t *const []) { NULL, NULL, server_status_menu }), "Bienvenido, administrador"
    );

    exit (0);
}
