#ifndef BD_H_
#define BD_H_

#define DEFAULT_DATABASE_FILE "BD.sqlite"

extern void                               print_db_err (sqlite3 *const restrict);
extern __attribute__ ((nonnull (2))) void open_db (const char *const restrict, sqlite3 **const restrict);

// funciones para la tabla Usuario
Usuario *obtenerUsuarios (sqlite3 *db);
int      numeroUsuarios (sqlite3 *db);
int      obtenerIDUsuario (sqlite3 *db);
void     insertarUsuario (sqlite3 *db, Usuario usuario);
void     eliminarUsuario (sqlite3 *db, int id);
void     modifyNombreVisible (sqlite3 *db, char nombreVisible [], int id);
void     modifyUsername (sqlite3 *db, char Username [], int id);
void     modifyContrasena (sqlite3 *db, char Contrasena [], int id);
void     modifyAciertosTotales (sqlite3 *db, int aciertosTotales, int id);
void     modifyFallosTotales (sqlite3 *db, int fallosTotales, int id);
void     modifyIDPresets (sqlite3 *db, int IDPresets, int id);

// funciones para la tabla Configuracion
int  obtenerNMaxJugadores (sqlite3 *db);
int  obtenerNPartidas (sqlite3 *db);
void modifyConfig (sqlite3 *db, int NMaxJugadores, int NPartidas);

// funciones para la tabla UsuarioCategoria
int  obtenerFallos (sqlite3 *db, int ID_Usuario, int ID_Categoria);
int  obtenerAciertos (sqlite3 *db, int ID_Usuario, int ID_Categoria);
void insertarFallosAciertos (sqlite3 *db, int ID_Categoria, int ID_Usuario, int fallos, int aciertos);
void modifyFallos (sqlite3 *db, int ID_Categoria, int ID_Usuario, int fallos);
void modifyAciertos (sqlite3 *db, int ID_Categoria, int ID_Usuario, int aciertos);

// funciones para la tabla Categorias
Categoria *obtenerCategorias (sqlite3 *db);
int        numeroCategorias (sqlite3 *db);
int        obtenerIDCategoria (sqlite3 *db);
void       insertarCategoria (sqlite3 *db, char nombre []);
void       eliminarCategoria (sqlite3 *db, int id);

// funciones para la tabla Presets
Presets *obtenerListaPresets (sqlite3 *db);
int      numeroPresets (sqlite3 *db);
int      obtenerIDPresets (sqlite3 *db);
void     insertarPresets (sqlite3 *db, Presets presets);
void     eliminarPresets (sqlite3 *db, int id);
void     modifyNJugadores (sqlite3 *db, int nJugadores, int id);
void     modifyNRondas (sqlite3 *db, int nRondas, int id);
void     modifyRoundTime (sqlite3 *db, int roundTime, int id);
void     modifyMecanica1 (sqlite3 *db, char mecanica [], int id);
void     modifyMecanica2 (sqlite3 *db, char mecanica [], int id);
void     modifyMecanica3 (sqlite3 *db, char mecanica [], int id);
void     modifyMecanica4 (sqlite3 *db, char mecanica [], int id);

void imprimirUsuario (Usuario usuario);
void imprimirCategoria (Categoria categoria);
void imprimirPresets (Presets presets);

#endif
