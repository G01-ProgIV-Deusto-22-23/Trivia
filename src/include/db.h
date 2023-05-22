#ifndef DB_H_
#define DB_H_

#ifdef __cplusplus
extern "C" {
#endif

    extern void                               print_db_err (sqlite3 *const restrict);
    extern __attribute__ ((nonnull (2))) void open_db (const char *const restrict, sqlite3 **const restrict);

    // funciones para la tabla Usuario
    Usuario *obtenerUsuarios (sqlite3 *db);
    size_t   numeroUsuarios (sqlite3 *db);
    uint32_t obtenerIDUsuario (sqlite3 *db);
    void     insertarUsuario (sqlite3 *db, Usuario usuario);
    void     eliminarUsuario (sqlite3 *db, uint32_t id);
    void     modifyNombreVisible (sqlite3 *db, char nombreVisible [], uint32_t id);
    void     modifyUsername (sqlite3 *db, char Username [], uint32_t id);
    void     modifyContrasena (sqlite3 *db, char Contrasena [], uint32_t id);
    void     modifyAciertosTotales (sqlite3 *db, uint32_t aciertosTotales, uint32_t id);
    void     modifyFallosTotales (sqlite3 *db, uint32_t fallosTotales, uint32_t id);
    void     modifyIDPresets (sqlite3 *db, uint32_t IDPresets, uint32_t id);

    // funciones para la tabla Configuracion
    size_t obtenerNMaxJugadores (sqlite3 *db);
    size_t obtenerNPartidas (sqlite3 *db);
    void   modifyConfig (sqlite3 *db, size_t NMaxJugadores, size_t NPartidas);

    // funciones para la tabla UsuarioCategoria
    uint32_t obtenerFallos (sqlite3 *db, uint32_t ID_Usuario, uint32_t ID_Categoria);
    uint32_t obtenerAciertos (sqlite3 *db, uint32_t ID_Usuario, uint32_t ID_Categoria);
    void     insertarFallosAciertos (
            sqlite3 *db, uint32_t ID_Categoria, uint32_t ID_Usuario, uint32_t fallos, uint32_t aciertos
        );
    void modifyFallos (sqlite3 *db, uint32_t ID_Categoria, uint32_t ID_Usuario, uint32_t fallos);
    void modifyAciertos (sqlite3 *db, uint32_t ID_Categoria, uint32_t ID_Usuario, uint32_t aciertos);

    // funciones para la tabla Categorias
    Categoria *obtenerCategorias (sqlite3 *db);
    size_t     numeroCategorias (sqlite3 *db);
    uint32_t   obtenerIDCategoria (sqlite3 *db);
    void       insertarCategoria (sqlite3 *db, char nombre []);
    void       eliminarCategoria (sqlite3 *db, uint32_t id);

    // funciones para la tabla Presets
    Presets *obtenerListaPresets (sqlite3 *db);
    size_t   numeroPresets (sqlite3 *db);
    uint32_t obtenerIDPresets (sqlite3 *db);
    void     insertarPresets (sqlite3 *db, Presets presets);
    void     eliminarPresets (sqlite3 *db, uint32_t id);
    void     modifyNJugadores (sqlite3 *db, uint32_t nJugadores, uint32_t id);
    void     modifyNRondas (sqlite3 *db, uint32_t nRondas, uint32_t id);
    void     modifyRoundTime (sqlite3 *db, uint32_t roundTime, uint32_t id);
    void     modifyMecanica1 (sqlite3 *db, char mecanica [], uint32_t id);
    void     modifyMecanica2 (sqlite3 *db, char mecanica [], uint32_t id);
    void     modifyMecanica3 (sqlite3 *db, char mecanica [], uint32_t id);
    void     modifyMecanica4 (sqlite3 *db, char mecanica [], uint32_t id);

    void imprimirUsuario (Usuario usuario);
    void imprimirCategoria (Categoria categoria);
    void imprimirPresets (Presets presets);
    void imprimirRespuesta (answer_t respuesta);
    void imprimirPregunta (question_t pregunta);

    // funcion para parsea JSON
    linkedlist_t get_questions (void);

#ifdef __cplusplus
}
#endif

#endif
