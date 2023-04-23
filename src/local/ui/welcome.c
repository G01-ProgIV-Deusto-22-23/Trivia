#if defined(__cpp_attributes) || __STDC_VERSION__ > 201710L
[[noreturn]]
#else
__attribute__ ((noreturn))
#endif
void welcome (void) {
    // actionmenu (
    //     0, 0, 0, 0,
    //     ((const char *[]) { "Administrar partidas", "Configurar el servidor", "Administrar el estado del servidor"
    //     }),
    //     ((choicefunc_t *const []) { NULL, configurarServidor, server_status_menu }), "Bienvenido, administrador"
    // );

    form (
        0, 0, 0, 0, ((field_attr_t []) { ipv4_field (), passwd_field (10), alnum_field (15) }),
        ((char *[]) { "IP", "Contraseña", "Nombre" }), "Ejemplo"
    );

    exit (0);
}
