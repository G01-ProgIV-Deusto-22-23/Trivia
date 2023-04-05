
void configurarServidor(void) {
	actionmenu (
	        0, 0, 0, 0,
	        ((const char *[]) {"editar Usuarios", "editar Presets", "editar Categorias"}),
	        ((choicefunc_t *const []) { usuarioMenu, NULL, NULL }), "Configuracion de servidor"
	    );

}
