static void not_implemented_yet_ui (void) {
    delete_menu (
        choicemenu (0, 0, 0, 0, ((const char *const []) { "Vale :(" }), "Funcionalidad no implementada todavía.")
    );
}

void configurarServidor (void) {
    actionmenu (
        0, 0, 0, 0, ((const char *const []) { "Editar Usuarios", "Editar Presets", "Editar Categorias" }),
        ((choicefunc_t *const []) { usuarioMenu, not_implemented_yet_ui, not_implemented_yet_ui }),
        "Configuracion de servidor"
    );
}
