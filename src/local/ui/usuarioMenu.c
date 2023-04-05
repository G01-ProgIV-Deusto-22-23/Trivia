
void usuarioMenu(void) {
	actionmenu (
	        0, 0, 0, 0,
	        ((const char *[]) {"Insertar Usuario", "eliminar Usuario", "modificar Usuario"}),
	        ((choicefunc_t *const []) { NULL , NULL, NULL }), "Configuracion de servidor"
	    );
}
