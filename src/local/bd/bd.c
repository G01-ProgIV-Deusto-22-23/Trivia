Usuario *obtenerUsuarios (sqlite3 *db) {
    sqlite3_stmt *stmt;
    int           tamanyo  = numeroUsuarios (db);
    Usuario      *usuarios = malloc (sizeof (Usuario) * tamanyo);

    char sql [] = "Select * from usuario";
    int  result = sqlite3_prepare_v2 (db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        printf ("Error preparing statement (SELECT)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }

    printf ("SQL query prepared (SELECT)\n");
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
    printf ("\n");
    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        printf ("Error finalizing statement (SELECT)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }

    printf ("Prepared statement finalized (SELECT)\n");
    printf ("\n");

    return usuarios;
}

int numeroUsuarios (sqlite3 *db) {
    int           resultado = 0;
    sqlite3_stmt *stmt;
    char          sql [] = "Select * from usuario";
    int           result = sqlite3_prepare_v2 (db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        printf ("Error preparing statement (SELECT)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    printf ("SQL query prepared (COUNT)\n");

    do {
        result = sqlite3_step (stmt);
        if (result == SQLITE_ROW)
            resultado++;
    } while (result == SQLITE_ROW);
    printf ("\n");

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        printf ("Error finalizing statement (COUNT)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    printf ("Prepared statement finalized (COUNT)\n");
    printf ("\n");
    return resultado;
}

int obtenerIDUsuario (sqlite3 *db) {
    int           resultado = 1;
    int           idActual  = 0;
    sqlite3_stmt *stmt;
    char          sql [] = "Select ID_Usuario from usuario";
    int           result = sqlite3_prepare_v2 (db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        printf ("Error preparing statement (SELECT)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    printf ("SQL query prepared (COUNT)\n");

    do {
        result = sqlite3_step (stmt);
        if (result == SQLITE_ROW) {
            idActual = sqlite3_column_int (stmt, 0);
            if (idActual != resultado)
                break;
            resultado++;
        }
    } while (result == SQLITE_ROW);
    printf ("\n");

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        printf ("Error finalizing statement (COUNT)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    printf ("Prepared statement finalized (COUNT)\n");
    printf ("\n");
    return resultado;
}

void insertarUsuario (sqlite3 *db, Usuario usuario) {
    sqlite3_stmt *stmt;
    int           id = obtenerIDUsuario (db);
    char          sql [] =
        "Insert into usuario (ID_Usuario, NombreVisible, Username, Contrasena, AciertosTotales, FallosTotales, ID_Presets) values(?, ?, ?, ?, ?, ?, ?)";
    int result = sqlite3_prepare_v2 (db, sql, strlen (sql) + 7, &stmt, NULL);
    if (result != SQLITE_OK) {
        printf ("Error preparing statement (INSERT)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    printf ("SQL query prepared (INSERT)\n");

    result = sqlite3_bind_int (stmt, 1, id);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_bind_text (stmt, 2, usuario.nombreVisible, strlen (usuario.nombreVisible), SQLITE_STATIC);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_bind_text (stmt, 3, usuario.username, strlen (usuario.username), SQLITE_STATIC);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_bind_text (stmt, 4, usuario.contrasena, strlen (usuario.contrasena), SQLITE_STATIC);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_bind_int (stmt, 5, usuario.aciertosTotales);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_bind_int (stmt, 6, usuario.aciertosTotales);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_bind_int (stmt, 7, usuario.ID_Presets);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_step (stmt);
    if (result != SQLITE_DONE)
        printf ("Error updating table usuario\n");

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        printf ("Error finalizing statement (INSERT)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }

    printf ("Prepared statement finalized (INSERT)\n");
}

void eliminarUsuario (sqlite3 *db, int id) {
    sqlite3_stmt *stmt;
    char          sql [] = "Delete from usuario where ID_Usuario = ?";
    int           result = sqlite3_prepare_v2 (db, sql, strlen (sql), &stmt, NULL);
    if (result != SQLITE_OK) {
        printf ("Error preparing statement (DELETE)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    printf ("SQL query prepared (DELETE)\n");

    result = sqlite3_bind_int (stmt, 1, id);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_step (stmt);
    if (result != SQLITE_DONE)
        printf ("Error updating table Usuario\n");

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        printf ("Error finalizing statement (DELETE)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }

    printf ("Prepared statement finalized (DELETE)\n");
}

void modifyNombreVisible (sqlite3 *db, char nombreVisible [], int id) {
    sqlite3_stmt *stmt;

    char sql [] = "Update usuario set NombreVisible = ? where ID_Usuario = ?";
    int  result = sqlite3_prepare_v2 (db, sql, strlen (sql) + 2, &stmt, NULL);
    if (result != SQLITE_OK) {
        printf ("Error preparing statement (UPDATE)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    printf ("SQL query prepared (UPDATE)\n");

    result = sqlite3_bind_text (stmt, 1, nombreVisible, strlen (nombreVisible), SQLITE_STATIC);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_bind_int (stmt, 2, id);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_step (stmt);
    if (result != SQLITE_DONE)
        printf ("Error updating table Usuario\n");

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        printf ("Error finalizing statement (UPDATE)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }

    printf ("Prepared statement finalized (UPDATE)\n");
}

void modifyUsername (sqlite3 *db, char Username [], int id) {
    sqlite3_stmt *stmt;

    char sql [] = "Update usuario set Username = ? where ID_Usuario = ?";
    int  result = sqlite3_prepare_v2 (db, sql, strlen (sql) + 2, &stmt, NULL);
    if (result != SQLITE_OK) {
        printf ("Error preparing statement (UPDATE)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    printf ("SQL query prepared (UPDATE)\n");

    result = sqlite3_bind_text (stmt, 1, Username, strlen (Username), SQLITE_STATIC);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_bind_int (stmt, 2, id);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_step (stmt);
    if (result != SQLITE_DONE)
        printf ("Error updating table Usuario\n");

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        printf ("Error finalizing statement (UPDATE)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }

    printf ("Prepared statement finalized (UPDATE)\n");
}

void modifyContrasena (sqlite3 *db, char Contrasena [], int id) {
    sqlite3_stmt *stmt;

    char sql [] = "Update usuario set Contrasena = ? where ID_Usuario = ?";
    int  result = sqlite3_prepare_v2 (db, sql, strlen (sql) + 2, &stmt, NULL);
    if (result != SQLITE_OK) {
        printf ("Error preparing statement (UPDATE)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    printf ("SQL query prepared (UPDATE)\n");

    result = sqlite3_bind_text (stmt, 1, Contrasena, strlen (Contrasena), SQLITE_STATIC);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_bind_int (stmt, 2, id);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_step (stmt);
    if (result != SQLITE_DONE)
        printf ("Error updating table Usuario\n");

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        printf ("Error finalizing statement (UPDATE)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }

    printf ("Prepared statement finalized (UPDATE)\n");
}

void modifyAciertosTotales (sqlite3 *db, int aciertosTotales, int id) {
    sqlite3_stmt *stmt;

    char sql [] = "Update usuario set AciertosTotales = ? where ID_Usuario = ?";
    int  result = sqlite3_prepare_v2 (db, sql, strlen (sql) + 2, &stmt, NULL);
    if (result != SQLITE_OK) {
        printf ("Error preparing statement (UPDATE)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    printf ("SQL query prepared (UPDATE)\n");

    result = sqlite3_bind_int (stmt, 1, aciertosTotales);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_bind_int (stmt, 2, id);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_step (stmt);
    if (result != SQLITE_DONE)
        printf ("Error updating table Usuario\n");

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        printf ("Error finalizing statement (UPDATE)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }

    printf ("Prepared statement finalized (UPDATE)\n");
}

void modifyFallosTotales (sqlite3 *db, int fallosTotales, int id) {
    sqlite3_stmt *stmt;

    char sql [] = "Update usuario set FallosTotales = ? where ID_Usuario = ?";
    int  result = sqlite3_prepare_v2 (db, sql, strlen (sql) + 2, &stmt, NULL);
    if (result != SQLITE_OK) {
        printf ("Error preparing statement (UPDATE)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    printf ("SQL query prepared (UPDATE)\n");

    result = sqlite3_bind_int (stmt, 1, fallosTotales);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_bind_int (stmt, 2, id);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_step (stmt);
    if (result != SQLITE_DONE)
        printf ("Error updating table Usuario\n");

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        printf ("Error finalizing statement (UPDATE)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }

    printf ("Prepared statement finalized (UPDATE)\n");
}

void modifyIDPresets (sqlite3 *db, int IDPresets, int id) {
    sqlite3_stmt *stmt;

    char sql [] = "Update usuario set ID_Presets = ? where ID_Usuario = ?";
    int  result = sqlite3_prepare_v2 (db, sql, strlen (sql) + 2, &stmt, NULL);
    if (result != SQLITE_OK) {
        printf ("Error preparing statement (UPDATE)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    printf ("SQL query prepared (UPDATE)\n");

    result = sqlite3_bind_int (stmt, 1, IDPresets);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_bind_int (stmt, 2, id);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_step (stmt);
    if (result != SQLITE_DONE)
        printf ("Error updating table Usuario\n");

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        printf ("Error finalizing statement (UPDATE)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }

    printf ("Prepared statement finalized (UPDATE)\n");
}

int obtenerNPartidas (sqlite3 *db) {
    int           resultado;
    sqlite3_stmt *stmt;

    char sql [] = "Select * from configuracion";
    int  result = sqlite3_prepare_v2 (db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        printf ("Error preparing statement (SELECT)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }

    printf ("SQL query prepared (SELECT)\n");
    result = sqlite3_step (stmt);
    if (result == SQLITE_ROW)
        resultado = sqlite3_column_int (stmt, 0);
    printf ("\n");

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        printf ("Error finalizing statement (SELECT)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    printf ("Prepared statement finalized (SELECT)\n");
    printf ("\n");
    return resultado;
}

int obtenerNMaxJugadores (sqlite3 *db) {
    int           resultado;
    sqlite3_stmt *stmt;

    char sql [] = "Select * from configuracion";
    int  result = sqlite3_prepare_v2 (db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        printf ("Error preparing statement (SELECT)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }

    printf ("SQL query prepared (SELECT)\n");
    do {
        result = sqlite3_step (stmt);
        if (result == SQLITE_ROW)
            resultado = sqlite3_column_int (stmt, 1);
    } while (result == SQLITE_ROW);
    printf ("\n");

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        printf ("Error finalizing statement (SELECT)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    printf ("Prepared statement finalized (SELECT)\n");
    printf ("\n");
    return resultado;
}

void modifyConfig (sqlite3 *db, int NPartidas, int NMaxJugadores) {
    sqlite3_stmt *stmt;

    char sql [] = "Update configuracion set NPartidas = ?, NMaxJugadores = ?";
    int  result = sqlite3_prepare_v2 (db, sql, strlen (sql), &stmt, NULL);
    if (result != SQLITE_OK) {
        printf ("Error preparing statement (UPDATE)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    printf ("SQL query prepared (UPDATE)\n");

    result = sqlite3_bind_int (stmt, 1, NPartidas);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_bind_int (stmt, 2, NMaxJugadores);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_step (stmt);
    if (result != SQLITE_DONE)
        printf ("Error updating table config\n");

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        printf ("Error finalizing statement (UPDATE)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }

    printf ("Prepared statement finalized (UPDATE)\n");
}

int obtenerFallos (sqlite3 *db, int ID_Categoria, int ID_Usuario) {
    int           resultado = 0;
    sqlite3_stmt *stmt;

    char sql [] = "Select Fallos from UsuarioCategoria where ID_Categoria = ? and ID_Usuario = ?";
    int  result = sqlite3_prepare_v2 (db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        printf ("Error preparing statement (SELECT)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }

    printf ("SQL query prepared (SELECT)\n");
    result = sqlite3_bind_int (stmt, 1, ID_Categoria);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_bind_int (stmt, 2, ID_Usuario);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }

    do {
        result = sqlite3_step (stmt);
        if (result == SQLITE_ROW)
            resultado = sqlite3_column_int (stmt, 0);
    } while (result == SQLITE_ROW);
    printf ("\n");
    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        printf ("Error finalizing statement (SELECT)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }

    printf ("Prepared statement finalized (SELECT)\n");
    printf ("\n");

    return resultado;
}

int obtenerAciertos (sqlite3 *db, int ID_Categoria, int ID_Usuario) {
    int           resultado = 0;
    sqlite3_stmt *stmt;

    char sql [] = "Select Aciertos from UsuarioCategoria where ID_Categoria = ? and ID_Usuario = ?";
    int  result = sqlite3_prepare_v2 (db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        printf ("Error preparing statement (SELECT)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }

    printf ("SQL query prepared (SELECT)\n");
    result = sqlite3_bind_int (stmt, 1, ID_Categoria);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_bind_int (stmt, 2, ID_Usuario);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }

    do {
        result = sqlite3_step (stmt);
        if (result == SQLITE_ROW)
            resultado = sqlite3_column_int (stmt, 0);
    } while (result == SQLITE_ROW);
    printf ("\n");
    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        printf ("Error finalizing statement (SELECT)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }

    printf ("Prepared statement finalized (SELECT)\n");
    printf ("\n");

    return resultado;
}

void insertarFallosAciertos (sqlite3 *db, int ID_Categoria, int ID_Usuario, int fallos, int aciertos) {
    sqlite3_stmt *stmt;
    char sql [] = "Insert into UsuarioCategoria (ID_Categoria, ID_Usuario, Fallos, Aciertos) values(?, ?, ?, ?)";
    int  result = sqlite3_prepare_v2 (db, sql, strlen (sql) + 4, &stmt, NULL);
    if (result != SQLITE_OK) {
        printf ("Error preparing statement (INSERT)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    printf ("SQL query prepared (INSERT)\n");

    result = sqlite3_bind_int (stmt, 1, ID_Categoria);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_bind_int (stmt, 2, ID_Usuario);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_bind_int (stmt, 3, fallos);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_bind_int (stmt, 4, aciertos);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_step (stmt);
    if (result != SQLITE_DONE)
        printf ("Error updating table Categorias\n");

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        printf ("Error finalizing statement (INSERT)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }

    printf ("Prepared statement finalized (INSERT)\n");
}

void modifyFallos (sqlite3 *db, int ID_Categoria, int ID_Usuario, int fallos) {
    sqlite3_stmt *stmt;

    char sql [] = "Update UsuarioCategoria set Fallos = ? where ID_Categoria = ? and ID_Usuario = ?";
    int  result = sqlite3_prepare_v2 (db, sql, strlen (sql) + 2, &stmt, NULL);
    if (result != SQLITE_OK) {
        printf ("Error preparing statement (UPDATE)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    printf ("SQL query prepared (UPDATE)\n");

    result = sqlite3_bind_int (stmt, 1, fallos);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_bind_int (stmt, 2, ID_Categoria);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_bind_int (stmt, 3, ID_Usuario);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_step (stmt);
    if (result != SQLITE_DONE)
        printf ("Error updating table UsuarioCategoria\n");

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        printf ("Error finalizing statement (UPDATE)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }

    printf ("Prepared statement finalized (UPDATE)\n");
}

void modifyAciertos (sqlite3 *db, int ID_Categoria, int ID_Usuario, int aciertos) {
    sqlite3_stmt *stmt;

    char sql [] = "Update UsuarioCategoria set Aciertos = ? where ID_Categoria = ? and ID_Usuario = ?";
    int  result = sqlite3_prepare_v2 (db, sql, strlen (sql) + 2, &stmt, NULL);
    if (result != SQLITE_OK) {
        printf ("Error preparing statement (UPDATE)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    printf ("SQL query prepared (UPDATE)\n");

    result = sqlite3_bind_int (stmt, 1, aciertos);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_bind_int (stmt, 2, ID_Categoria);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_bind_int (stmt, 3, ID_Usuario);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_step (stmt);
    if (result != SQLITE_DONE)
        printf ("Error updating table UsuarioCategoria\n");

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        printf ("Error finalizing statement (UPDATE)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }

    printf ("Prepared statement finalized (UPDATE)\n");
}

Categoria *obtenerCategorias (sqlite3 *db) {
    int           tamanyo    = numeroCategorias (db);
    Categoria    *categorias = malloc (sizeof (Categoria) * tamanyo);
    sqlite3_stmt *stmt;

    char sql [] = "Select * from categorias";
    int  result = sqlite3_prepare_v2 (db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        printf ("Error preparing statement (SELECT)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }

    printf ("SQL query prepared (SELECT)\n");
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
    printf ("\n");

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        printf ("Error finalizing statement (SELECT)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }

    printf ("Prepared statement finalized (SELECT)\n");
    printf ("\n");
    return categorias;
}

int numeroCategorias (sqlite3 *db) {
    int           resultado = 0;
    sqlite3_stmt *stmt;
    char          sql [] = "Select * from Categorias";
    int           result = sqlite3_prepare_v2 (db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        printf ("Error preparing statement (SELECT)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    printf ("SQL query prepared (COUNT)\n");

    do {
        result = sqlite3_step (stmt);
        if (result == SQLITE_ROW)
            resultado++;
    } while (result == SQLITE_ROW);
    printf ("\n");

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        printf ("Error finalizing statement (SELECT)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    printf ("Prepared statement finalized (SELECT)\n");
    printf ("\n");
    return resultado;
}

int obtenerIDCategoria (sqlite3 *db) {
    int           resultado = 1;
    int           idActual  = 0;
    sqlite3_stmt *stmt;
    char          sql [] = "Select ID_Categoria from Categorias";
    int           result = sqlite3_prepare_v2 (db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        printf ("Error preparing statement (SELECT)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    printf ("SQL query prepared (COUNT)\n");

    do {
        result = sqlite3_step (stmt);
        if (result == SQLITE_ROW) {
            idActual = sqlite3_column_int (stmt, 0);
            if (idActual != resultado)
                break;
            resultado++;
        }
    } while (result == SQLITE_ROW);
    printf ("\n");

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        printf ("Error finalizing statement (COUNT)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    printf ("Prepared statement finalized (COUNT)\n");
    printf ("\n");
    return resultado;
}

void insertarCategoria (sqlite3 *db, char nombre []) {
    sqlite3_stmt *stmt;
    int           id     = obtenerIDCategoria (db);
    char          sql [] = "Insert into Categorias (ID_Categoria, nombre) values(?, ?)";
    int           result = sqlite3_prepare_v2 (db, sql, strlen (sql) + 20, &stmt, NULL);
    if (result != SQLITE_OK) {
        printf ("Error preparing statement (INSERT)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    printf ("SQL query prepared (INSERT)\n");

    result = sqlite3_bind_int (stmt, 1, id);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_bind_text (stmt, 2, nombre, strlen (nombre), SQLITE_STATIC);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_step (stmt);
    if (result != SQLITE_DONE)
        printf ("Error updating table Categorias\n");

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        printf ("Error finalizing statement (INSERT)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }

    printf ("Prepared statement finalized (INSERT)\n");
}

void eliminarCategoria (sqlite3 *db, int id) {
    sqlite3_stmt *stmt;
    char          sql [] = "Delete from categorias where ID_Categoria = ?";
    int           result = sqlite3_prepare_v2 (db, sql, strlen (sql) + 1, &stmt, NULL);
    if (result != SQLITE_OK) {
        printf ("Error preparing statement (DELETE)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    printf ("SQL query prepared (DELETE)\n");

    result = sqlite3_bind_int (stmt, 1, id);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_step (stmt);
    if (result != SQLITE_DONE)
        printf ("Error updating table Categorias\n");

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        printf ("Error finalizing statement (DELETE)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }

    printf ("Prepared statement finalized (DELETE)\n");
}

Presets *obtenerListaPresets (sqlite3 *db) {
    sqlite3_stmt *stmt;
    int           tamanyo      = numeroPresets (db);
    Presets      *listaPresets = malloc (sizeof (Presets) * tamanyo);

    char sql [] = "Select * from Presets";
    int  result = sqlite3_prepare_v2 (db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        printf ("Error preparing statement (SELECT)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }

    printf ("SQL query prepared (SELECT)\n");
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
    printf ("\n");
    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        printf ("Error finalizing statement (SELECT)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }

    printf ("Prepared statement finalized (SELECT)\n");
    printf ("\n");

    return listaPresets;
}

int numeroPresets (sqlite3 *db) {
    int           resultado = 0;
    sqlite3_stmt *stmt;
    char          sql [] = "Select * from Presets";
    int           result = sqlite3_prepare_v2 (db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        printf ("Error preparing statement (COUNT)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    printf ("SQL query prepared (COUNT)\n");

    do {
        result = sqlite3_step (stmt);
        if (result == SQLITE_ROW)
            resultado++;
    } while (result == SQLITE_ROW);
    printf ("\n");

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        printf ("Error finalizing statement (COUNT)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    printf ("Prepared statement finalized (COUNT)\n");
    printf ("\n");
    return resultado;
}

int obtenerIDPresets (sqlite3 *db) {
    int           resultado = 1;
    int           idActual  = 0;
    sqlite3_stmt *stmt;
    char          sql [] = "Select ID_Presets from Presets";
    int           result = sqlite3_prepare_v2 (db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        printf ("Error preparing statement (SELECT)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    printf ("SQL query prepared (COUNT)\n");

    do {
        result = sqlite3_step (stmt);
        if (result == SQLITE_ROW) {
            idActual = sqlite3_column_int (stmt, 0);
            if (idActual != resultado)
                break;
            resultado++;
        }
    } while (result == SQLITE_ROW);
    printf ("\n");

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        printf ("Error finalizing statement (COUNT)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    printf ("Prepared statement finalized (COUNT)\n");
    printf ("\n");
    return resultado;
}

void insertarPresets (sqlite3 *db, Presets presets) {
    sqlite3_stmt *stmt;
    int           id = obtenerIDPresets (db);
    printf ("%i\n", id);
    char sql [] =
        "Insert into Presets (ID_Presets, nJugadores, nRondas, RoundTime, Categorias, Mecanica1, Mecanica2, Mecanica3, Mecanica4) values(?, ?, ?, ?, ?, ?, ?, ?, ?)";
    int result = sqlite3_prepare_v2 (db, sql, strlen (sql) + 10, &stmt, NULL);
    if (result != SQLITE_OK) {
        printf ("Error preparing statement (INSERT)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_bind_int (stmt, 1, id);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_bind_int (stmt, 2, presets.nJugadores);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_bind_int (stmt, 3, presets.nRondas);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_bind_int (stmt, 4, presets.RoundTime);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_bind_text (stmt, 5, presets.Categorias, strlen (presets.Categorias), SQLITE_STATIC);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_bind_text (stmt, 6, presets.Mecanica1, strlen (presets.Mecanica1), SQLITE_STATIC);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_bind_text (stmt, 7, presets.Mecanica2, strlen (presets.Mecanica2), SQLITE_STATIC);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_bind_text (stmt, 8, presets.Mecanica3, strlen (presets.Mecanica3), SQLITE_STATIC);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_bind_text (stmt, 9, presets.Mecanica4, strlen (presets.Mecanica4), SQLITE_STATIC);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_step (stmt);
    if (result != SQLITE_DONE)
        printf ("Error updating table Presets\n");

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        printf ("Error finalizing statement (INSERT)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
}

void eliminarPresets (sqlite3 *db, int id) {
    sqlite3_stmt *stmt;
    char          sql [] = "Delete from Presets where ID_Presets = ?";
    int           result = sqlite3_prepare_v2 (db, sql, strlen (sql) + 1, &stmt, NULL);
    if (result != SQLITE_OK) {
        printf ("Error preparing statement (DELETE)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    printf ("SQL query prepared (DELETE)\n");

    result = sqlite3_bind_int (stmt, 1, id);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_step (stmt);
    if (result != SQLITE_DONE)
        printf ("Error updating table Presets\n");

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        printf ("Error finalizing statement (DELETE)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }

    printf ("Prepared statement finalized (DELETE)\n");
}

void modifyNJugadores (sqlite3 *db, int nJugadores, int id) {
    sqlite3_stmt *stmt;

    char sql [] = "Update Presets set nJugadores = ? where ID_Presets = ?";
    int  result = sqlite3_prepare_v2 (db, sql, strlen (sql) + 2, &stmt, NULL);
    if (result != SQLITE_OK) {
        printf ("Error preparing statement (UPDATE)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    printf ("SQL query prepared (UPDATE)\n");

    result = sqlite3_bind_int (stmt, 1, nJugadores);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_bind_int (stmt, 2, id);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_step (stmt);
    if (result != SQLITE_DONE)
        printf ("Error updating table Presets\n");

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        printf ("Error finalizing statement (UPDATE)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }

    printf ("Prepared statement finalized (UPDATE)\n");
}

void modifyNRondas (sqlite3 *db, int nRondas, int id) {
    sqlite3_stmt *stmt;

    char sql [] = "Update Presets set nRondas = ? where ID_Presets = ?";
    int  result = sqlite3_prepare_v2 (db, sql, strlen (sql) + 2, &stmt, NULL);
    if (result != SQLITE_OK) {
        printf ("Error preparing statement (UPDATE)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    printf ("SQL query prepared (UPDATE)\n");

    result = sqlite3_bind_int (stmt, 1, nRondas);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_bind_int (stmt, 2, id);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_step (stmt);
    if (result != SQLITE_DONE)
        printf ("Error updating table Presets\n");

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        printf ("Error finalizing statement (UPDATE)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }

    printf ("Prepared statement finalized (UPDATE)\n");
}

void modifyRoundTime (sqlite3 *db, int roundTime, int id) {
    sqlite3_stmt *stmt;

    char sql [] = "Update Presets set nRondas = ? where ID_Presets = ?";
    int  result = sqlite3_prepare_v2 (db, sql, strlen (sql) + 2, &stmt, NULL);
    if (result != SQLITE_OK) {
        printf ("Error preparing statement (UPDATE)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    printf ("SQL query prepared (UPDATE)\n");

    result = sqlite3_bind_int (stmt, 1, roundTime);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_bind_int (stmt, 2, id);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_step (stmt);
    if (result != SQLITE_DONE)
        printf ("Error updating table Presets\n");

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        printf ("Error finalizing statement (UPDATE)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }

    printf ("Prepared statement finalized (UPDATE)\n");
}

void modifyMecanica1 (sqlite3 *db, char mecanica [], int id) {
    sqlite3_stmt *stmt;

    char sql [] = "Update Presets set Mecanica1 = ? where ID_Presets = ?";
    int  result = sqlite3_prepare_v2 (db, sql, strlen (sql) + 2, &stmt, NULL);
    if (result != SQLITE_OK) {
        printf ("Error preparing statement (UPDATE)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    printf ("SQL query prepared (UPDATE)\n");

    result = sqlite3_bind_text (stmt, 1, mecanica, strlen (mecanica), SQLITE_STATIC);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_bind_int (stmt, 2, id);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_step (stmt);
    if (result != SQLITE_DONE)
        printf ("Error updating table Presets\n");

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        printf ("Error finalizing statement (UPDATE)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }

    printf ("Prepared statement finalized (UPDATE)\n");
}

void modifyMecanica2 (sqlite3 *db, char mecanica [], int id) {
    sqlite3_stmt *stmt;

    char sql [] = "Update Presets set Mecanica2 = ? where ID_Presets = ?";
    int  result = sqlite3_prepare_v2 (db, sql, strlen (sql) + 2, &stmt, NULL);
    if (result != SQLITE_OK) {
        printf ("Error preparing statement (UPDATE)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    printf ("SQL query prepared (UPDATE)\n");

    result = sqlite3_bind_text (stmt, 1, mecanica, strlen (mecanica), SQLITE_STATIC);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_bind_int (stmt, 2, id);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_step (stmt);
    if (result != SQLITE_DONE)
        printf ("Error updating table Presets\n");

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        printf ("Error finalizing statement (UPDATE)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }

    printf ("Prepared statement finalized (UPDATE)\n");
}

void modifyMecanica3 (sqlite3 *db, char mecanica [], int id) {
    sqlite3_stmt *stmt;

    char sql [] = "Update Presets set Mecanica3 = ? where ID_Presets = ?";
    int  result = sqlite3_prepare_v2 (db, sql, strlen (sql) + 2, &stmt, NULL);
    if (result != SQLITE_OK) {
        printf ("Error preparing statement (UPDATE)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    printf ("SQL query prepared (UPDATE)\n");

    result = sqlite3_bind_text (stmt, 1, mecanica, strlen (mecanica), SQLITE_STATIC);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_bind_int (stmt, 2, id);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_step (stmt);
    if (result != SQLITE_DONE)
        printf ("Error updating table Presets\n");

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        printf ("Error finalizing statement (UPDATE)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }

    printf ("Prepared statement finalized (UPDATE)\n");
}

void modifyMecanica4 (sqlite3 *db, char mecanica [], int id) {
    sqlite3_stmt *stmt;

    char sql [] = "Update Presets set Mecanica4 = ? where ID_Presets = ?";
    int  result = sqlite3_prepare_v2 (db, sql, strlen (sql) + 2, &stmt, NULL);
    if (result != SQLITE_OK) {
        printf ("Error preparing statement (UPDATE)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    printf ("SQL query prepared (UPDATE)\n");

    result = sqlite3_bind_text (stmt, 1, mecanica, strlen (mecanica), SQLITE_STATIC);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_bind_int (stmt, 2, id);
    if (result != SQLITE_OK) {
        printf ("Error binding parameters\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }
    result = sqlite3_step (stmt);
    if (result != SQLITE_DONE)
        printf ("Error updating table Presets\n");

    result = sqlite3_finalize (stmt);
    if (result != SQLITE_OK) {
        printf ("Error finalizing statement (UPDATE)\n");
        printf ("%s\n", sqlite3_errmsg (db));
    }

    printf ("Prepared statement finalized (UPDATE)\n");
}
