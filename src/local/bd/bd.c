void print_db_err (sqlite3 *const restrict db) {
    fprintf (stderr, "Error code %d: %s\n", sqlite3_errcode (db), sqlite3_errmsg (db));
}

__attribute__ ((nonnull (2))) void open_db (const char *const restrict f, sqlite3 **const restrict db) {
    bool new = false;
    if (sqlite3_open_v2 (set_database_file (f), db, SQLITE_OPEN_READWRITE, NULL) != SQLITE_OK) {
        new = true;
        if (sqlite3_open (get_database_file (), db) != SQLITE_OK) {
            print_db_err (*db);
            error ("could not open the database.");
        }
    }

    if (sqlite3_db_readonly (*db, NULL)) {
        print_db_err (*db);

        error ("the database is in read-only mode.");
    }

    if (!new)
        return;

    static const char *const sql [] = {
        "CREATE TABLE \"Categorias\"(\"ID_Categoria\" INTEGER NOT NULL, \"Nombre\" VARCHAR (100), PRIMARY KEY (\"ID_Categoria\"))",
        "CREATE TABLE [Presets]([ID_Presets] INTEGER NULL PRIMARY KEY, [nJugadores] INTEGER NULL, [nRondas] INTEGER NULL,[RoundTime] INTEGER NULL, [Categorias] VARCHAR (20) NULL, [Mecanica1] VARCHAR (3) NULL,[Mecanica2] VARCHAR (3) NULL, [Mecanica3] VARCHAR (3) NULL, [mecanica4] VARCHAR (3) NULL)",
        "CREATE TABLE \"UsuarioCategoria\"(\"ID_Categoria\" INTEGER NOT NULL, \"ID_Usuario\" INTEGER NOT NULL, \"Fallos\" INTEGER, \"Aciertos\" INTEGER,PRIMARY KEY (\"ID_Categoria\", \"ID_Usuario\"))",
        "CREATE TABLE [configuracion]([nPartidas] INTEGER NULL, [nMaxJugadores] INTEGER NULL, [ID_configuracion] INTEGER NOT NULL PRIMARY KEY)",
        "CREATE TABLE \"usuario\"(\"ID_Usuario\" INTEGER NOT NULL, \"NombreVisible\" VARCHAR (20), \"Username\" VARCHAR (20),\"Contrasena\" VARCHAR (20), \"AciertosTotales\" INTEGER, \"FallosTotales\" INTEGER, \"ID_Presets\" INTEGER,PRIMARY KEY (\"ID_Usuario\"))"
    };

    for (size_t i = 0; i < sizeof (sql) / sizeof (*sql);) {
        if (sqlite3_exec (*db, *(sql + i++), NULL, NULL, NULL) != SQLITE_OK) {
            print_db_err (*db);
            error ("could not create the table.");
        }
    }
}

Usuario *obtenerUsuarios (sqlite3 *db) {
    sqlite3_stmt *stmt;
    int           tamanyo  = numeroUsuarios (db);
    Usuario      *usuarios = malloc (sizeof (Usuario) * (size_t) (size_t) tamanyo);

    char sql [] = "Select * from usuario";
    int  result = sqlite3_prepare_v2 (db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        error ("could not prepare select statement.");
        print_db_err (db);
    }

    message ("select query prepared.");
    Usuario usuario;
    int     i = 0;
    do {
        result = sqlite3_step (stmt);
        if (result == SQLITE_ROW) {
            usuario.ID_Usuario = sqlite3_column_int (stmt, 0);
            strcpy (usuario.nombreVisible, (char *) sqlite3_column_text (stmt, 1));
            strcpy (usuario.username, (char *) sqlite3_column_text (stmt, 2));
            strcpy (usuario.contrasena, (char *) sqlite3_column_text (stmt, 3));
            usuario.aciertosTotales = sqlite3_column_int (stmt, 4);
            usuario.fallosTotales   = sqlite3_column_int (stmt, 5);
            usuario.ID_Presets      = sqlite3_column_int (stmt, 6);
            usuarios [i]            = usuario;
            i++;
        }
    } while (result == SQLITE_ROW);

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        warning ("could not finalize the select statement.");
        print_db_err (db);
    }

    message ("select statement finalized.");

    return usuarios;
}

int numeroUsuarios (sqlite3 *db) {
    int           resultado = 0;
    sqlite3_stmt *stmt;
    char          sql [] = "Select * from usuario";
    int           result = sqlite3_prepare_v2 (db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        error ("could not prepare select statement.");
        print_db_err (db);
    }

    message ("count query prepared.");

    do {
        result = sqlite3_step (stmt);
        if (result == SQLITE_ROW)
            resultado++;
    } while (result == SQLITE_ROW);

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        warning ("could not finalize the count statement.");
        print_db_err (db);
    }

    else
        message ("count statement finalized.");

    return resultado;
}

int obtenerIDUsuario (sqlite3 *db) {
    int           resultado = 1;
    int           idActual  = 0;
    sqlite3_stmt *stmt;
    char          sql [] = "Select ID_Usuario from usuario";
    int           result = sqlite3_prepare_v2 (db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        error ("could not prepare select statement.");
        print_db_err (db);
    }

    message ("count query prepared.");

    do {
        result = sqlite3_step (stmt);
        if (result == SQLITE_ROW) {
            idActual = sqlite3_column_int (stmt, 0);
            if (idActual != resultado)
                break;
            resultado++;
        }
    } while (result == SQLITE_ROW);

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        warning ("could not finalize the count statement.");
        print_db_err (db);
    }

    else
        message ("count statement finalized.");

    return resultado;
}

void insertarUsuario (sqlite3 *db, Usuario usuario) {
    sqlite3_stmt *stmt;
    int           id = obtenerIDUsuario (db);
    char          sql [] =
        "Insert into usuario (ID_Usuario, NombreVisible, Username, Contrasena, AciertosTotales, FallosTotales, ID_Presets) values(?, ?, ?, ?, ?, ?, ?)";
    int result = sqlite3_prepare_v2 (db, sql, (int) strlen (sql) + 7, &stmt, NULL);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("could not prepare the insert statement.");
    }

    message ("insert statement prepared.");

    result = sqlite3_bind_int (stmt, 1, id);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_bind_text (stmt, 2, usuario.nombreVisible, (int) strlen (usuario.nombreVisible), SQLITE_STATIC);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_bind_text (stmt, 3, usuario.username, (int) strlen (usuario.username), SQLITE_STATIC);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_bind_text (stmt, 4, usuario.contrasena, (int) strlen (usuario.contrasena), SQLITE_STATIC);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_bind_int (stmt, 5, usuario.aciertosTotales);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_bind_int (stmt, 6, usuario.aciertosTotales);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_bind_int (stmt, 7, usuario.ID_Presets);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_step (stmt);
    if (result != SQLITE_DONE) {
        print_db_err (db);
        error ("error updating table.");
    }

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        warning ("could not finalize the insert statement.");
        print_db_err (db);
    }

    else
        message ("Prepared statement finalized (INSERT)");
}

void eliminarUsuario (sqlite3 *db, int id) {
    sqlite3_stmt *stmt;
    char          sql [] = "Delete from usuario where ID_Usuario = ?";
    int           result = sqlite3_prepare_v2 (db, sql, (int) strlen (sql), &stmt, NULL);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("could not prepare the delete statement.");
    }

    message ("delete statement prepared.");

    result = sqlite3_bind_int (stmt, 1, id);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_step (stmt);
    if (result != SQLITE_DONE) {
        print_db_err (db);
        error ("error updating table.");
    }

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        warning ("could not finalize the delete statement.");
        print_db_err (db);
    }

    else
        message ("delete statement finalized.");
}

void modifyNombreVisible (sqlite3 *db, char nombreVisible [], int id) {
    sqlite3_stmt *stmt;

    char sql [] = "Update usuario set NombreVisible = ? where ID_Usuario = ?";
    int  result = sqlite3_prepare_v2 (db, sql, (int) strlen (sql) + 2, &stmt, NULL);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("could not prepare the update statement.");
    }

    message ("update statement prepared.");

    result = sqlite3_bind_text (stmt, 1, nombreVisible, (int) strlen (nombreVisible), SQLITE_STATIC);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_bind_int (stmt, 2, id);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_step (stmt);
    if (result != SQLITE_DONE) {
        print_db_err (db);
        error ("error updating table.");
    }

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        warning ("could not finalize update statement");
        print_db_err (db);
    }

    else
        message ("update statement finalized.");
}

void modifyUsername (sqlite3 *db, char Username [], int id) {
    sqlite3_stmt *stmt;

    char sql [] = "Update usuario set Username = ? where ID_Usuario = ?";
    int  result = sqlite3_prepare_v2 (db, sql, (int) strlen (sql) + 2, &stmt, NULL);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("could not prepare the update statement.");
    }

    message ("update statement prepared.");

    result = sqlite3_bind_text (stmt, 1, Username, (int) strlen (Username), SQLITE_STATIC);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_bind_int (stmt, 2, id);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_step (stmt);
    if (result != SQLITE_DONE) {
        print_db_err (db);
        error ("error updating table.");
    }

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        warning ("could not finalize update statement");
        print_db_err (db);
    }

    else
        message ("update statement finalized.");
}

void modifyContrasena (sqlite3 *db, char Contrasena [], int id) {
    sqlite3_stmt *stmt;

    char sql [] = "Update usuario set Contrasena = ? where ID_Usuario = ?";
    int  result = sqlite3_prepare_v2 (db, sql, (int) strlen (sql) + 2, &stmt, NULL);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("could not prepare the update statement.");
    }

    message ("update statement prepared.");

    result = sqlite3_bind_text (stmt, 1, Contrasena, (int) strlen (Contrasena), SQLITE_STATIC);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_bind_int (stmt, 2, id);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_step (stmt);
    if (result != SQLITE_DONE) {
        print_db_err (db);
        error ("error updating table.");
    }

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        warning ("could not finalize update statement");
        print_db_err (db);
    }

    else
        message ("update statement finalized.");
}

void modifyAciertosTotales (sqlite3 *db, int aciertosTotales, int id) {
    sqlite3_stmt *stmt;

    char sql [] = "Update usuario set AciertosTotales = ? where ID_Usuario = ?";
    int  result = sqlite3_prepare_v2 (db, sql, (int) strlen (sql) + 2, &stmt, NULL);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("could not prepare the update statement.");
    }

    message ("update statement prepared.");

    result = sqlite3_bind_int (stmt, 1, aciertosTotales);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_bind_int (stmt, 2, id);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_step (stmt);
    if (result != SQLITE_DONE) {
        print_db_err (db);
        error ("error updating table.");
    }

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        warning ("could not finalize update statement");
        print_db_err (db);
    }

    else
        message ("update statement finalized.");
}

void modifyFallosTotales (sqlite3 *db, int fallosTotales, int id) {
    sqlite3_stmt *stmt;

    char sql [] = "Update usuario set FallosTotales = ? where ID_Usuario = ?";
    int  result = sqlite3_prepare_v2 (db, sql, (int) strlen (sql) + 2, &stmt, NULL);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("could not prepare the update statement.");
    }

    message ("update statement prepared.");

    result = sqlite3_bind_int (stmt, 1, fallosTotales);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_bind_int (stmt, 2, id);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_step (stmt);
    if (result != SQLITE_DONE) {
        print_db_err (db);
        error ("error updating table.");
    }

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        warning ("could not finalize update statement");
        print_db_err (db);
    }

    else
        message ("update statement finalized.");
}

void modifyIDPresets (sqlite3 *db, int IDPresets, int id) {
    sqlite3_stmt *stmt;

    char sql [] = "Update usuario set ID_Presets = ? where ID_Usuario = ?";
    int  result = sqlite3_prepare_v2 (db, sql, (int) strlen (sql) + 2, &stmt, NULL);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("could not prepare the update statement.");
    }

    message ("update statement prepared.");

    result = sqlite3_bind_int (stmt, 1, IDPresets);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_bind_int (stmt, 2, id);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_step (stmt);
    if (result != SQLITE_DONE) {
        print_db_err (db);
        error ("error updating table.");
    }

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        warning ("could not finalize update statement");
        print_db_err (db);
    }

    else
        message ("update statement finalized.");
}

int obtenerNPartidas (sqlite3 *db) {
    int           resultado = 0;
    sqlite3_stmt *stmt;

    char sql [] = "Select * from configuracion";
    int  result = sqlite3_prepare_v2 (db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        error ("could not prepare select statement.");
        print_db_err (db);
    }

    message ("select query prepared.");
    result = sqlite3_step (stmt);
    if (result == SQLITE_ROW)
        resultado = sqlite3_column_int (stmt, 0);

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        warning ("could not finalize the select statement.");
        print_db_err (db);
    }

    message ("select statement finalized.");

    return resultado;
}

int obtenerNMaxJugadores (sqlite3 *db) {
    int           resultado = 0;
    sqlite3_stmt *stmt;

    char sql [] = "Select * from configuracion";
    int  result = sqlite3_prepare_v2 (db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        error ("could not prepare select statement.");
        print_db_err (db);
    }

    message ("select query prepared.");
    do {
        result = sqlite3_step (stmt);
        if (result == SQLITE_ROW)
            resultado = sqlite3_column_int (stmt, 1);
    } while (result == SQLITE_ROW);

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        warning ("could not finalize the select statement.");
        print_db_err (db);
    }

    message ("select statement finalized.");

    return resultado;
}

void modifyConfig (sqlite3 *db, int NPartidas, int NMaxJugadores) {
    sqlite3_stmt *stmt;

    char sql [] = "Update configuracion set NPartidas = ?, NMaxJugadores = ?";
    int  result = sqlite3_prepare_v2 (db, sql, (int) strlen (sql), &stmt, NULL);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("could not prepare the update statement.");
    }

    message ("update statement prepared.");

    result = sqlite3_bind_int (stmt, 1, NPartidas);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_bind_int (stmt, 2, NMaxJugadores);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_step (stmt);
    if (result != SQLITE_DONE) {
        print_db_err (db);
        error ("error updating table.");
    }

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        warning ("could not finalize update statement");
        print_db_err (db);
    }

    else
        message ("update statement finalized.");
}

int obtenerFallos (sqlite3 *db, int ID_Categoria, int ID_Usuario) {
    int           resultado = 0;
    sqlite3_stmt *stmt;

    char sql [] = "Select Fallos from UsuarioCategoria where ID_Categoria = ? and ID_Usuario = ?";
    int  result = sqlite3_prepare_v2 (db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        error ("could not prepare select statement.");
        print_db_err (db);
    }

    message ("select query prepared.");
    result = sqlite3_bind_int (stmt, 1, ID_Categoria);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_bind_int (stmt, 2, ID_Usuario);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }

    do {
        result = sqlite3_step (stmt);
        if (result == SQLITE_ROW)
            resultado = sqlite3_column_int (stmt, 0);
    } while (result == SQLITE_ROW);

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        warning ("could not finalize the select statement.");
        print_db_err (db);
    }

    message ("select statement finalized.");

    return resultado;
}

int obtenerAciertos (sqlite3 *db, int ID_Categoria, int ID_Usuario) {
    int           resultado = 0;
    sqlite3_stmt *stmt;

    char sql [] = "Select Aciertos from UsuarioCategoria where ID_Categoria = ? and ID_Usuario = ?";
    int  result = sqlite3_prepare_v2 (db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        error ("could not prepare select statement.");
        print_db_err (db);
    }

    message ("select query prepared.");
    result = sqlite3_bind_int (stmt, 1, ID_Categoria);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_bind_int (stmt, 2, ID_Usuario);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }

    do {
        result = sqlite3_step (stmt);
        if (result == SQLITE_ROW)
            resultado = sqlite3_column_int (stmt, 0);
    } while (result == SQLITE_ROW);

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        warning ("could not finalize the select statement.");
        print_db_err (db);
    }

    message ("select statement finalized.");

    return resultado;
}

void insertarFallosAciertos (sqlite3 *db, int ID_Categoria, int ID_Usuario, int fallos, int aciertos) {
    sqlite3_stmt *stmt;
    char sql [] = "Insert into UsuarioCategoria (ID_Categoria, ID_Usuario, Fallos, Aciertos) values(?, ?, ?, ?)";
    int  result = sqlite3_prepare_v2 (db, sql, (int) strlen (sql) + 4, &stmt, NULL);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("could not prepare the insert statement.");
    }

    message ("insert statement prepared.");

    result = sqlite3_bind_int (stmt, 1, ID_Categoria);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_bind_int (stmt, 2, ID_Usuario);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_bind_int (stmt, 3, fallos);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_bind_int (stmt, 4, aciertos);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_step (stmt);
    if (result != SQLITE_DONE) {
        print_db_err (db);
        error ("error updating table.");
    }

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        warning ("could not finalize the insert statement.");
        print_db_err (db);
    }

    else
        message ("Prepared statement finalized (INSERT)");
}

void modifyFallos (sqlite3 *db, int ID_Categoria, int ID_Usuario, int fallos) {
    sqlite3_stmt *stmt;

    char sql [] = "Update UsuarioCategoria set Fallos = ? where ID_Categoria = ? and ID_Usuario = ?";
    int  result = sqlite3_prepare_v2 (db, sql, (int) strlen (sql) + 2, &stmt, NULL);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("could not prepare the update statement.");
    }

    message ("update statement prepared.");

    result = sqlite3_bind_int (stmt, 1, fallos);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_bind_int (stmt, 2, ID_Categoria);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_bind_int (stmt, 3, ID_Usuario);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_step (stmt);
    if (result != SQLITE_DONE) {
        print_db_err (db);
        error ("error updating table.");
    }

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        warning ("could not finalize update statement");
        print_db_err (db);
    }

    else
        message ("update statement finalized.");
}

void modifyAciertos (sqlite3 *db, int ID_Categoria, int ID_Usuario, int aciertos) {
    sqlite3_stmt *stmt;

    char sql [] = "Update UsuarioCategoria set Aciertos = ? where ID_Categoria = ? and ID_Usuario = ?";
    int  result = sqlite3_prepare_v2 (db, sql, (int) strlen (sql) + 2, &stmt, NULL);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("could not prepare the update statement.");
    }

    message ("update statement prepared.");

    result = sqlite3_bind_int (stmt, 1, aciertos);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_bind_int (stmt, 2, ID_Categoria);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_bind_int (stmt, 3, ID_Usuario);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_step (stmt);
    if (result != SQLITE_DONE) {
        print_db_err (db);
        error ("error updating table.");
    }

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        warning ("could not finalize update statement");
        print_db_err (db);
    }

    else
        message ("update statement finalized.");
}

Categoria *obtenerCategorias (sqlite3 *db) {
    int           tamanyo    = numeroCategorias (db);
    Categoria    *categorias = malloc (sizeof (Categoria) * (size_t) tamanyo);
    sqlite3_stmt *stmt;

    char sql [] = "Select * from categorias";
    int  result = sqlite3_prepare_v2 (db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        error ("could not prepare select statement.");
        print_db_err (db);
    }

    message ("select query prepared.");
    Categoria categoria;
    int       i = 0;
    do {
        result = sqlite3_step (stmt);
        if (result == SQLITE_ROW) {
            categoria.ID_Categoria = sqlite3_column_int (stmt, 0);
            strcpy (categoria.nombre, (char *) sqlite3_column_text (stmt, 1));
            categorias [i] = categoria;
            i++;
        }
    } while (result == SQLITE_ROW);

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        warning ("could not finalize the select statement.");
        print_db_err (db);
    }

    message ("select statement finalized.");

    return categorias;
}

int numeroCategorias (sqlite3 *db) {
    int           resultado = 0;
    sqlite3_stmt *stmt;
    char          sql [] = "Select * from Categorias";
    int           result = sqlite3_prepare_v2 (db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        error ("could not prepare select statement.");
        print_db_err (db);
    }

    message ("count query prepared.");

    do {
        result = sqlite3_step (stmt);
        if (result == SQLITE_ROW)
            resultado++;
    } while (result == SQLITE_ROW);

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        warning ("could not finalize the select statement.");
        print_db_err (db);
    }

    message ("select statement finalized.");

    return resultado;
}

int obtenerIDCategoria (sqlite3 *db) {
    int           resultado = 1;
    int           idActual  = 0;
    sqlite3_stmt *stmt;
    char          sql [] = "Select ID_Categoria from Categorias";
    int           result = sqlite3_prepare_v2 (db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        error ("could not prepare select statement.");
        print_db_err (db);
    }

    message ("count query prepared.");

    do {
        result = sqlite3_step (stmt);
        if (result == SQLITE_ROW) {
            idActual = sqlite3_column_int (stmt, 0);
            if (idActual != resultado)
                break;
            resultado++;
        }
    } while (result == SQLITE_ROW);

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        warning ("could not finalize the count statement.");
        print_db_err (db);
    }

    else
        message ("count statement finalized.");

    return resultado;
}

void insertarCategoria (sqlite3 *db, char nombre []) {
    sqlite3_stmt *stmt;
    int           id     = obtenerIDCategoria (db);
    char          sql [] = "Insert into Categorias (ID_Categoria, nombre) values(?, ?)";
    int           result = sqlite3_prepare_v2 (db, sql, (int) strlen (sql) + 20, &stmt, NULL);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("could not prepare the insert statement.");
    }

    message ("insert statement prepared.");

    result = sqlite3_bind_int (stmt, 1, id);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_bind_text (stmt, 2, nombre, (int) strlen (nombre), SQLITE_STATIC);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_step (stmt);
    if (result != SQLITE_DONE) {
        print_db_err (db);
        error ("error updating table.");
    }

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        warning ("could not finalize the insert statement.");
        print_db_err (db);
    }

    else
        message ("Prepared statement finalized (INSERT)");
}

void eliminarCategoria (sqlite3 *db, int id) {
    sqlite3_stmt *stmt;
    char          sql [] = "Delete from categorias where ID_Categoria = ?";
    int           result = sqlite3_prepare_v2 (db, sql, (int) strlen (sql) + 1, &stmt, NULL);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("could not prepare the delete statement.");
    }

    message ("delete statement prepared.");

    result = sqlite3_bind_int (stmt, 1, id);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_step (stmt);
    if (result != SQLITE_DONE) {
        print_db_err (db);
        error ("error updating table.");
    }

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        warning ("could not finalize the delete statement.");
        print_db_err (db);
    }

    else
        message ("delete statement finalized.");
}

Presets *obtenerListaPresets (sqlite3 *db) {
    sqlite3_stmt *stmt;
    int           tamanyo      = numeroPresets (db);
    Presets      *listaPresets = malloc (sizeof (Presets) * (size_t) tamanyo);

    char sql [] = "Select * from Presets";
    int  result = sqlite3_prepare_v2 (db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        error ("could not prepare select statement.");
        print_db_err (db);
    }

    message ("select query prepared.");
    Presets presets;
    int     i = 0;
    do {
        result = sqlite3_step (stmt);
        if (result == SQLITE_ROW) {
            presets.ID_Presets = sqlite3_column_int (stmt, 0);
            presets.nJugadores = sqlite3_column_int (stmt, 1);
            presets.nRondas    = sqlite3_column_int (stmt, 2);
            presets.RoundTime  = sqlite3_column_int (stmt, 3);
            strcpy (presets.Categorias, (char *) sqlite3_column_text (stmt, 4));
            strcpy (presets.Mecanica1, (char *) sqlite3_column_text (stmt, 5));
            strcpy (presets.Mecanica2, (char *) sqlite3_column_text (stmt, 6));
            strcpy (presets.Mecanica3, (char *) sqlite3_column_text (stmt, 7));
            strcpy (presets.Mecanica4, (char *) sqlite3_column_text (stmt, 8));
            listaPresets [i] = presets;
            i++;
        }
    } while (result == SQLITE_ROW);

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        warning ("could not finalize the select statement.");
        print_db_err (db);
    }

    message ("select statement finalized.");

    return listaPresets;
}

int numeroPresets (sqlite3 *db) {
    int           resultado = 0;
    sqlite3_stmt *stmt;
    char          sql [] = "Select * from Presets";
    int           result = sqlite3_prepare_v2 (db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("could not prepare the count statement.");
    }

    message ("count query prepared.");

    do {
        result = sqlite3_step (stmt);
        if (result == SQLITE_ROW)
            resultado++;
    } while (result == SQLITE_ROW);

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        warning ("could not finalize the count statement.");
        print_db_err (db);
    }

    else
        message ("count statement finalized.");

    return resultado;
}

int obtenerIDPresets (sqlite3 *db) {
    int           resultado = 1;
    int           idActual  = 0;
    sqlite3_stmt *stmt;
    char          sql [] = "Select ID_Presets from Presets";
    int           result = sqlite3_prepare_v2 (db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        error ("could not prepare select statement.");
        print_db_err (db);
    }

    message ("count query prepared.");

    do {
        result = sqlite3_step (stmt);
        if (result == SQLITE_ROW) {
            idActual = sqlite3_column_int (stmt, 0);
            if (idActual != resultado)
                break;
            resultado++;
        }
    } while (result == SQLITE_ROW);

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        warning ("could not finalize the count statement.");
        print_db_err (db);
    }

    else
        message ("count statement finalized.");

    return resultado;
}

void insertarPresets (sqlite3 *db, Presets presets) {
    sqlite3_stmt *stmt;
    int           id = obtenerIDPresets (db);
    char          sql [] =
        "Insert into Presets (ID_Presets, nJugadores, nRondas, RoundTime, Categorias, Mecanica1, Mecanica2, Mecanica3, Mecanica4) values(?, ?, ?, ?, ?, ?, ?, ?, ?)";
    int result = sqlite3_prepare_v2 (db, sql, (int) strlen (sql) + 10, &stmt, NULL);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("could not prepare the insert statement.");
    }
    result = sqlite3_bind_int (stmt, 1, id);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_bind_int (stmt, 2, presets.nJugadores);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_bind_int (stmt, 3, presets.nRondas);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_bind_int (stmt, 4, presets.RoundTime);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_bind_text (stmt, 5, presets.Categorias, (int) strlen (presets.Categorias), SQLITE_STATIC);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_bind_text (stmt, 6, presets.Mecanica1, (int) strlen (presets.Mecanica1), SQLITE_STATIC);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_bind_text (stmt, 7, presets.Mecanica2, (int) strlen (presets.Mecanica2), SQLITE_STATIC);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_bind_text (stmt, 8, presets.Mecanica3, (int) strlen (presets.Mecanica3), SQLITE_STATIC);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_bind_text (stmt, 9, presets.Mecanica4, (int) strlen (presets.Mecanica4), SQLITE_STATIC);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_step (stmt);
    if (result != SQLITE_DONE) {
        print_db_err (db);
        error ("error updating table.");
    }

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        warning ("could not finalize the insert statement.");
        print_db_err (db);
    }
}

void eliminarPresets (sqlite3 *db, int id) {
    sqlite3_stmt *stmt;
    char          sql [] = "Delete from Presets where ID_Presets = ?";
    int           result = sqlite3_prepare_v2 (db, sql, (int) strlen (sql) + 1, &stmt, NULL);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("could not prepare the delete statement.");
    }

    message ("delete statement prepared.");

    result = sqlite3_bind_int (stmt, 1, id);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_step (stmt);
    if (result != SQLITE_DONE) {
        print_db_err (db);
        error ("error updating table.");
    }

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        warning ("could not finalize the delete statement.");
        print_db_err (db);
    }

    else
        message ("delete statement finalized.");
}

void modifyNJugadores (sqlite3 *db, int nJugadores, int id) {
    sqlite3_stmt *stmt;

    char sql [] = "Update Presets set nJugadores = ? where ID_Presets = ?";
    int  result = sqlite3_prepare_v2 (db, sql, (int) strlen (sql) + 2, &stmt, NULL);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("could not prepare the update statement.");
    }

    message ("update statement prepared.");

    result = sqlite3_bind_int (stmt, 1, nJugadores);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_bind_int (stmt, 2, id);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_step (stmt);
    if (result != SQLITE_DONE) {
        print_db_err (db);
        error ("error updating table.");
    }

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        warning ("could not finalize update statement");
        print_db_err (db);
    }

    else
        message ("update statement finalized.");
}

void modifyNRondas (sqlite3 *db, int nRondas, int id) {
    sqlite3_stmt *stmt;

    char sql [] = "Update Presets set nRondas = ? where ID_Presets = ?";
    int  result = sqlite3_prepare_v2 (db, sql, (int) strlen (sql) + 2, &stmt, NULL);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("could not prepare the update statement.");
    }

    message ("update statement prepared.");

    result = sqlite3_bind_int (stmt, 1, nRondas);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_bind_int (stmt, 2, id);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_step (stmt);
    if (result != SQLITE_DONE) {
        print_db_err (db);
        error ("error updating table.");
    }

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        warning ("could not finalize update statement");
        print_db_err (db);
    }

    else
        message ("update statement finalized.");
}

void modifyRoundTime (sqlite3 *db, int roundTime, int id) {
    sqlite3_stmt *stmt;

    char sql [] = "Update Presets set nRondas = ? where ID_Presets = ?";
    int  result = sqlite3_prepare_v2 (db, sql, (int) strlen (sql) + 2, &stmt, NULL);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("could not prepare the update statement.");
    }

    message ("update statement prepared.");

    result = sqlite3_bind_int (stmt, 1, roundTime);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_bind_int (stmt, 2, id);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_step (stmt);
    if (result != SQLITE_DONE) {
        print_db_err (db);
        error ("error updating table.");
    }

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        warning ("could not finalize update statement");
        print_db_err (db);
    }

    else
        message ("update statement finalized.");
}

void modifyMecanica1 (sqlite3 *db, char mecanica [], int id) {
    sqlite3_stmt *stmt;

    char sql [] = "Update Presets set Mecanica1 = ? where ID_Presets = ?";
    int  result = sqlite3_prepare_v2 (db, sql, (int) strlen (sql) + 2, &stmt, NULL);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("could not prepare the update statement.");
    }

    message ("update statement prepared.");

    result = sqlite3_bind_text (stmt, 1, mecanica, (int) strlen (mecanica), SQLITE_STATIC);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_bind_int (stmt, 2, id);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_step (stmt);
    if (result != SQLITE_DONE) {
        print_db_err (db);
        error ("error updating table.");
    }

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        warning ("could not finalize update statement");
        print_db_err (db);
    }

    else
        message ("update statement finalized.");
}

void modifyMecanica2 (sqlite3 *db, char mecanica [], int id) {
    sqlite3_stmt *stmt;

    char sql [] = "Update Presets set Mecanica2 = ? where ID_Presets = ?";
    int  result = sqlite3_prepare_v2 (db, sql, (int) strlen (sql) + 2, &stmt, NULL);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("could not prepare the update statement.");
    }

    message ("update statement prepared.");

    result = sqlite3_bind_text (stmt, 1, mecanica, (int) strlen (mecanica), SQLITE_STATIC);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_bind_int (stmt, 2, id);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_step (stmt);
    if (result != SQLITE_DONE) {
        print_db_err (db);
        error ("error updating table.");
    }

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        warning ("could not finalize update statement");
        print_db_err (db);
    }

    else
        message ("update statement finalized.");
}

void modifyMecanica3 (sqlite3 *db, char mecanica [], int id) {
    sqlite3_stmt *stmt;

    char sql [] = "Update Presets set Mecanica3 = ? where ID_Presets = ?";
    int  result = sqlite3_prepare_v2 (db, sql, (int) strlen (sql) + 2, &stmt, NULL);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("could not prepare the update statement.");
    }

    message ("update statement prepared.");

    result = sqlite3_bind_text (stmt, 1, mecanica, (int) strlen (mecanica), SQLITE_STATIC);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_bind_int (stmt, 2, id);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_step (stmt);
    if (result != SQLITE_DONE) {
        print_db_err (db);
        error ("error updating table.");
    }

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        warning ("could not finalize update statement");
        print_db_err (db);
    }

    else
        message ("update statement finalized.");
}

void modifyMecanica4 (sqlite3 *db, char mecanica [], int id) {
    sqlite3_stmt *stmt;

    char sql [] = "Update Presets set Mecanica4 = ? where ID_Presets = ?";
    int  result = sqlite3_prepare_v2 (db, sql, (int) strlen (sql) + 2, &stmt, NULL);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("could not prepare the update statement.");
    }

    message ("update statement prepared.");

    result = sqlite3_bind_text (stmt, 1, mecanica, (int) strlen (mecanica), SQLITE_STATIC);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_bind_int (stmt, 2, id);
    if (result != SQLITE_OK) {
        print_db_err (db);
        error ("error binding parameters");
    }
    result = sqlite3_step (stmt);
    if (result != SQLITE_DONE) {
        print_db_err (db);
        error ("error updating table.");
    }

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        warning ("could not finalize update statement");
        print_db_err (db);
    }

    else
        message ("update statement finalized.");
}
