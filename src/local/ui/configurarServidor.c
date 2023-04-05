
void configurarServidor (void) {
    actionmenu (
        0, 0, 0, 0, ((const char *[]) { "Editar Usuarios", "Editar Presets", "Editar Categorias" }),
        ((choicefunc_t *const []) { usuarioMenu, NULL, NULL }), "Configuracion de servidor"
    );
}
