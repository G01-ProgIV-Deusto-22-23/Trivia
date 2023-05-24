#ifdef _WIN32
    #define MAX_FILE_CHARS MAX_PATH
#else
    #define MAX_FILE_CHARS PATH_MAX
#endif

static char DATABASE_FILE [MAX_FILE_CHARS + 1]  = DEFAULT_DATABASE_FILE;
static char QUESTIONS_FILE [MAX_FILE_CHARS + 1] = DEFAULT_QUESTIONS_FILE;
static char CONFIG_FILE [MAX_FILE_CHARS + 1]    = DEFAULT_CONFIG_FILE;

const char *get_database_file (void) {
    static struct stat st = { 0 };
    static char        DATABASE_FILE2 [MAX_FILE_CHARS + 1];

    if (stat (
#ifdef _WIN32
            (_fullpath (DATABASE_FILE, DATABASE_FILE2, sizeof (DATABASE_FILE2) - 1),
             *(DATABASE_FILE2 + ({
                   size_t i = strlen (DATABASE_FILE2);
                   for (; i && *(DATABASE_FILE2 + i - 1) && *(DATABASE_FILE2 + i - 1) != '\\';)
                       ;
                   i;
               })) = '\0',
             DATABASE_FILE2)
#else
            (realpath (DATABASE_FILE, DATABASE_FILE2),
             *(DATABASE_FILE2 + strlen (DATABASE_FILE2) - strlen (basename (DATABASE_FILE2))) = '\0', DATABASE_FILE2)
#endif
                ,
            &st
        ) == -1)
        mkdir (
            DATABASE_FILE2
#ifndef _WIN32
            ,
            0700
#endif
        );

    return DATABASE_FILE;
}

const char *set_database_file (const char *const restrict f) {
    if (!f)
        return memcpy (DATABASE_FILE, DEFAULT_DATABASE_FILE, sizeof (DEFAULT_DATABASE_FILE));

    memset (CONFIG_FILE, 0, sizeof (CONFIG_FILE));
    if (!memccpy (DATABASE_FILE, f, '\0', sizeof (DATABASE_FILE))) {
        warning ("the filename was larger than the maximum permitted filename (" stringify (MAX_FILE_CHARS
        ) " chars not including the null terminator).");

        *(DATABASE_FILE + sizeof (DATABASE_FILE) - 1) = '\0';
    }

    return DATABASE_FILE;
}

const char *get_questions_file (void) {
    return QUESTIONS_FILE;
}

const char *set_questions_file (const char *const restrict f) {
    if (!f)
        return memcpy (QUESTIONS_FILE, DEFAULT_QUESTIONS_FILE, sizeof (DEFAULT_QUESTIONS_FILE));

    memset (CONFIG_FILE, 0, sizeof (CONFIG_FILE));
    if (!memccpy (QUESTIONS_FILE, f, '\0', sizeof (QUESTIONS_FILE))) {
        warning ("the filename was larger than the maximum permitted filename (" stringify (MAX_FILE_CHARS
        ) " chars not including the null terminator).");

        *(QUESTIONS_FILE + sizeof (QUESTIONS_FILE) - 1) = '\0';
    }

    return QUESTIONS_FILE;
}

const char *get_config_file (void) {
    return CONFIG_FILE;
}

const char *set_config_file (const char *const restrict f) {
    memset (CONFIG_FILE, 0, sizeof (CONFIG_FILE));
    if (!memccpy (CONFIG_FILE, f, '\0', sizeof (CONFIG_FILE))) {
        warning ("the filename was larger than the maximum permitted filename (" stringify (MAX_FILE_CHARS
        ) " chars not including the null terminator).");

        *(CONFIG_FILE + sizeof (CONFIG_FILE) - 1) = '\0';
    }

    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow=local"
#pragma GCC diagnostic ignored "-Wshadow=compatible-local"

        static FILE *f = NULL;
        if (!(f = fopen (CONFIG_FILE_LOCATOR, "w+")))
            error ("could not open the config locator.");

        setvbuf (f, NULL, _IONBF, 0);
        fwrite (CONFIG_FILE, 1, strlen (CONFIG_FILE), f);
        fclose (f);

#pragma GCC diagnostic pop
    }

    return CONFIG_FILE;
}

void write_config (const char *const restrict path) {
    static char  buf [ONE_MB]                 = { 0 };
    static char  fname [sizeof (CONFIG_FILE)] = { 0 };
    static FILE *f                            = NULL;

    memset (buf, 0, sizeof (buf));
    memset (fname, 0, sizeof (fname));

    if (!path) {
        if (!(f = fopen (CONFIG_FILE_LOCATOR, "r"))) {
            warning ("could not get the location of the config file, using the default file.");

            if (!(f = fopen (CONFIG_FILE_LOCATOR, "w+")))
                error ("could not create the config file locator.");

            setvbuf (f, NULL, _IONBF, 0);
            fwrite (CONFIG_FILE, 1, strlen (CONFIG_FILE), f);
        }

        fread (fname, 1, sizeof (CONFIG_FILE) - 1, f);
        fclose (f);
    }

    else
        memcpy (fname, path, strlen (path));

    if (!(f = fopen (fname, "w+")))
        error ("could not open the config file for writing.");

    cJSON *const restrict o = cJSON_CreateObject ();
    if (!o) {
        warning ("could not create the JSON object.");
        fclose (f);

        return;
    }

    cJSON *const restrict db = cJSON_CreateString (DATABASE_FILE);
    if (db)
        cJSON_AddItemToObject (o, "database", db);

    else
        warning ("could not add the \"database\" field to the object.");

    cJSON *const restrict qs = cJSON_CreateString (QUESTIONS_FILE);
    if (db)
        cJSON_AddItemToObject (o, "questions", qs);

    else
        warning ("could not add the \"questions\" field to the object.");

    cJSON *const restrict port = cJSON_CreateNumber ((double) get_server_port ());
    if (port)
        cJSON_AddItemToObject (o, "port", port);

    else
        warning ("could not add the \"port\" field to the object.");

    if (!cJSON_PrintPreallocated (o, buf, sizeof (buf), 1)) {
        warning ("could not stringify the configuration.");
        fclose (f);

        return;
    }

    setvbuf (f, NULL, _IONBF, 0);
    fwrite (buf, 1, strlen (buf), f);

    cJSON_Delete (o);
    fclose (f);
}

cJSON *get_config (const char *const restrict path) {
    static char   prebuf [1 << 13]             = { 0 };
    static char   fname [sizeof (CONFIG_FILE)] = { 0 };
    static FILE  *f                            = NULL;
    static size_t fsz                          = 0;

    memccpy (fname, CONFIG_FILE, '\0', sizeof (CONFIG_FILE));

    if (!path) {
        if (!(f = fopen (CONFIG_FILE_LOCATOR, "r"))) {
            warning ("could not get the location of the config file, using the default file.");

            if (!(f = fopen (CONFIG_FILE_LOCATOR, "w+")))
                error ("could not create the config file locator.");

            setvbuf (f, NULL, _IONBF, 0);
            fwrite (CONFIG_FILE, 1, strlen (CONFIG_FILE), f);
        }

        fread (fname, 1, sizeof (CONFIG_FILE) - 1, f);
        fclose (f);
    }

    else
        memcpy (fname, path, strlen (path));

    if (!(f = fopen (fname, "r"))) {
        warning ("could not open the config file, creating a default configuration.");

        write_config (fname);
        return get_config (fname);
    }

    if (fseek (f, 0L, SEEK_END))
        error ("could not jump to the end of the config file.");

    if ((fsz = (size_t) ftell (f)) == (size_t) -1L)
        error ("could not get the position of the cursor in the config file.");
    rewind (f);

    char *buf = prebuf;
    if (fsz >= sizeof (prebuf) && !(buf = calloc (1, fsz)))
        error ("could not allocate space for the buffer.");

    fread (buf, 1, fsz, f);

    cJSON *config = cJSON_ParseWithLength (buf, fsz);

    if (buf != prebuf)
        free (buf);

    return config;
}

char *get_config_database (const char *const restrict path) {
    static char db [sizeof (DATABASE_FILE) + 5] = { 0 };

    memset (db, 0, sizeof (db));
    cJSON *const config = get_config (path);
    if (!config)
        return NULL;

    cJSON *field = NULL;
    if (!(field = cJSON_GetObjectItemCaseSensitive (config, "database"))) {
        warning ("there is no \"database\" field.");
        cJSON_Delete (config);

        return NULL;
    }

    if (!cJSON_IsString (field)) {
        warning ("the \"database\" field is not a string field.");
        cJSON_Delete (config);

        return NULL;
    }

    if (!cJSON_PrintPreallocated (cJSON_GetObjectItemCaseSensitive (config, "database"), db, sizeof (db), 0))
        warning ("could not print the JSON object properly.");

    cJSON_Delete (config);

    return db;
}

char *get_config_questions (const char *const restrict path) {
    static char db [sizeof (QUESTIONS_FILE) + 5] = { 0 };

    memset (db, 0, sizeof (db));
    cJSON *const config = get_config (path);
    if (!config)
        return NULL;

    cJSON *field = NULL;
    if (!(field = cJSON_GetObjectItemCaseSensitive (config, "questions"))) {
        warning ("there is no \"questions\" field.");
        cJSON_Delete (config);

        return NULL;
    }

    if (!cJSON_IsString (field)) {
        warning ("the \"questions\" field is not a string field.");
        cJSON_Delete (config);

        return NULL;
    }

    if (!cJSON_PrintPreallocated (cJSON_GetObjectItemCaseSensitive (config, "questions"), db, sizeof (db), 0))
        warning ("could not print the JSON object properly.");

    cJSON_Delete (config);

    return db;
}

int get_config_port (const char *const restrict path) {
    static char port [sizeof (stringify (IANA_DYNAMIC_PORT_END)) + 5] = { 0 };

    memset (port, 0, sizeof (port));
    cJSON *const config = get_config (path);
    if (!config)
        return -1;

    cJSON *field = NULL;
    if (!(field = cJSON_GetObjectItemCaseSensitive (config, "port"))) {
        warning ("there is no \"port\" field.");
        cJSON_Delete (config);

        return -1;
    }

    if (!cJSON_IsNumber (field)) {
        warning ("the \"port\" field is not a numeric field.");
        cJSON_Delete (config);

        return -1;
    }

    if (!cJSON_PrintPreallocated (field, port, sizeof (port), 0))
        warning ("could not print the JSON object properly.");

    int r = field->valueint == INT_MIN || field->valueint == INT_MAX
                ? (warning ("the \"port\" field does not contain an int."), -1)
                : field->valueint;
    cJSON_Delete (config);

    return r;
}