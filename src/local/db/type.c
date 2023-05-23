#include "fallback_questions.h"

void imprimirUsuario (Usuario usuario) {
    fprintf (
        stderr,
        "ID: %" PRIu32 " Name: %s username: %s contrasena: %s aciertosTotales: %" PRIu32 ", fallosTotales: %" PRIu32
        ", ID_Presets: %" PRIu32 "\n",
        usuario.ID_Usuario, usuario.nombreVisible, usuario.username, usuario.contrasena, usuario.aciertosTotales,
        usuario.fallosTotales, usuario.ID_Presets
    );
}

void imprimirCategoria (Categoria categoria) {
    fprintf (stderr, "ID: %" PRIu32 ", Nombre: %s\n", categoria.ID_Categoria, categoria.nombre);
}

void imprimirPresets (Presets presets) {
    fprintf (
        stderr,
        "ID: %" PRIu32 ", numero Jugadores: %" PRIu32 ", numero de rondas: %" PRIu32 ", Round time: %" PRIu32
        ", Categorias: %s, Mecanica1 : %s, Mecanica2 : %s, Mecanica3 : %s, Mecanica4: %s\n",
        presets.ID_Presets, presets.nJugadores, presets.nRondas, presets.RoundTime, presets.Categorias,
        presets.Mecanica1, presets.Mecanica2, presets.Mecanica3, presets.Mecanica4
    );
}

void imprimirRespuesta (answer_t respuesta) {
    fprintf (stderr, "%s (%sorrecta)\n", respuesta.text, respuesta.correct ? "C" : "Inc");
}

void imprimirPregunta (question_t pregunta) {
    fprintf (stderr, "Pregunta\n\tTipo: %s\nEnunciado: %s\n", pregunta.type, pregunta.text);
    for (size_t i = 0; i < sizeof (pregunta.ans) / sizeof (*pregunta.ans);
         fprintf (stderr, "\t"), imprimirRespuesta (*(pregunta.ans + i++)))
        ;
}

linkedlist_t get_questions (void) {
    static question_t qs [MAX_QUESTIONS];
    static char       prealloc_buf [ONE_MB];
    question_t        q;

    const char *const filename = get_questions_file ();
    char             *buf;
    size_t            len;
    FILE             *f;
    if (!(f = fopen (filename, "r"))) {
        warning (
            "could not open questions file, the fallback questions will be used and (if possible) written to the file."
        );

        buf = (char *) FALLBACK_QUESTIONS;

        if (!(f = fopen (filename, "w+")))
            warning ("could not create the questions file.");

        else {
            fwrite (FALLBACK_QUESTIONS, 1, len = sizeof (FALLBACK_QUESTIONS), f);
            if (fclose (f) == EOF)
                warning ("could not close the file properly.");
        }
    }

    else if (fseek (f, 0, SEEK_END) || (len = (size_t) ftell (f)) == (size_t) -1L) {
        warning ("could not get the size of the file.");
        if (fclose (f) == EOF)
            warning ("could not close the file properly.");

        buf = (char *) FALLBACK_QUESTIONS;
        len = sizeof (FALLBACK_QUESTIONS);
    }

    else {
        rewind (f);

        if (len < sizeof (prealloc_buf))
            buf = prealloc_buf;

        else if (!(buf = malloc (len)))
            error ("could not allocate space for the buffer.");

        if (fread (buf, 1, len, f) < len)
            warning ("could not read all the bytes in the file.");

        if (ferror (f))
            warning ("the file could not be properly read.");

        if (fclose (f) == EOF)
            warning ("could not close the file properly.");
    }

    cJSON *const json = cJSON_ParseWithLength (buf, len);

    if (!(buf == (char *) FALLBACK_QUESTIONS || buf == prealloc_buf))
        free (buf);

    if (!json) {
        warning ("the contents of the buffer could not be properly parsed.");

        return NULL;
    }

    if (!cJSON_IsArray (json)) {
        warning ("the object is not a JSON array.");
        cJSON_Delete (json);

        return NULL;
    }

    cJSON  *o, *o2, *field, *field2, *field3;
    size_t  rem;
    uint8_t count = 0;

    if (cJSON_GetArraySize (json) > MAX_QUESTIONS)
        warning ("only the first " stringify (MAX_QUESTIONS) " valid questions will be fetched from the array.");

    cJSON_ArrayForEach (o, json) {
        if (count >= MAX_QUESTIONS)
            break;

        memset (&q, 0, sizeof (question_t));

        if (!cJSON_IsObject (o)) {
            warning ("the current item is not a JSON object.");

            continue;
        }

        if (!(cJSON_HasObjectItem (o, "tipo") && (field = cJSON_GetObjectItemCaseSensitive (o, "tipo")) &&
              cJSON_IsString (field))) {
            warning ("the object must have a string field named \"tipo\".");

            continue;
        }

        len = strlen (buf = cJSON_GetStringValue (field));
        if (!len) {
            warning ("the question type cannot be an empty string.");

            continue;
        }

        if (len >= sizeof (q.type)) {
            warning ("only the first " stringify (MAX_QUESTION_TYPE_TEXT
            ) "(including the null terminator) characters will be copied.");
            len = sizeof (q.type) - 1;
        }

        *(char *) mempcpy (q.type, buf, len) = '\0';

        if (!(cJSON_HasObjectItem (o, "pregunta") && (field = cJSON_GetObjectItemCaseSensitive (o, "pregunta")) &&
              cJSON_IsString (field))) {
            warning ("the object must have a string field named \"pregunta\".");

            continue;
        }

        len = strlen (buf = cJSON_GetStringValue (field));
        if (!len) {
            warning ("the question text cannot be an empty string.");

            continue;
        }

        if (len >= sizeof (q.text)) {
            warning ("only the first " stringify (MAX_QUESTION_TEXT
            ) "(including the null terminator) characters will be copied.");
            len = sizeof (q.text) - 1;
        }

        *(char *) mempcpy (q.text, buf, len) = '\0';

        if (!(cJSON_HasObjectItem (o, "opciones") && (field = cJSON_GetObjectItemCaseSensitive (o, "opciones")) &&
              cJSON_IsArray (field))) {
            warning ("the object must have an array field named \"opciones\".");

            continue;
        }

        if (!(rem = (size_t) cJSON_GetArraySize (field))) {
            warning ("the choices array must be have at least one item");

            continue;
        }

        if (rem > 4) {
            warning ("only the first four items will be fetched, given they are all deemed valid.");
            rem = 4;
        }

        q.n = (uint8_t) rem;

        cJSON_ArrayForEach (o2, field) {
            if (!rem)
                break;

            if (!cJSON_IsObject (o2)) {
                warning ("the current item is not a JSON object.");
                rem = (size_t) -1;

                break;
            }

            if (!(cJSON_HasObjectItem (o2, "respuesta") &&
                  (field2 = cJSON_GetObjectItemCaseSensitive (o2, "respuesta")) && cJSON_IsString (field2))) {
                warning ("the object must have a string field named \"respuesta\".");
                rem = (size_t) -1;

                break;
            }

            len = strlen (buf = cJSON_GetStringValue (field2));
            if (!len) {
                warning ("the answer cannot be an empty string.");
                rem = (size_t) -1;

                break;
            }

            if (len >= sizeof (q.ans->text)) {
                warning ("only the first " stringify (MAX_ANSWER_TEXT
                ) "(including the null terminator) characters will be copied.");
                len = sizeof (q.ans->text) - 1;
            }

            if (!(cJSON_HasObjectItem (o2, "correcta") &&
                  (field3 = cJSON_GetObjectItemCaseSensitive (o2, "correcta")) && cJSON_IsBool (field3))) {
                warning ("the object must have a boolean field named \"correcta\".");
                rem = (size_t) -1;

                break;
            }

            *(char *) mempcpy ((q.ans + (q.n - (uint8_t) rem))->text, buf, len) = '\0';
            (q.ans + (q.n - (uint8_t) rem--))->correct                          = (uint8_t) cJSON_IsTrue (field3);
        }

        if (rem == (size_t) -1)
            continue;

        { // Bubble sort de forma totalmente no irónica
            answer_t aux;
            int      cmp;
            uint8_t  i, j;
        order:
            for (i = 0; i < q.n - 1; i++)
                for (j = 0; j < q.n - i - 1; j++) {
                    if ((cmp = strcmp ((q.ans + j)->text, (q.ans + j + 1)->text)) < 0)
                        continue;

                    if (!cmp) {
                        if (j + 2 < q.n) {
                            memcpy (q.ans + j + 1, q.ans + j + 2, sizeof (answer_t));
                            memset (q.ans + j + 2, 0, sizeof (answer_t));
                        }

                        else
                            memset (q.ans + j + 1, 0, sizeof (answer_t));

                        q.n--;
                        goto order;
                    }

                    memcpy (&aux, q.ans + j, sizeof (answer_t));
                    memcpy (q.ans + j, q.ans + j + 1, sizeof (answer_t));
                    memcpy (q.ans + j + 1, &aux, sizeof (answer_t));
                }
        }

        *(qs + count++) = q;
    }

    linkedlist_t l = NULL;
    if (count) {
        if (!(l = create_linkedlist (qs)))
            warning ("could not create the question list.");

        else
            for (size_t i = 1; i < count; i++)
                if (!insert_linkedlist (l, qs + i))
                    warning ("could not insert the question in the list.");
    }

    if (buf == prealloc_buf)
#ifdef _WIN32
        SecureZeroMemory (prealloc_buf, sizeof (prealloc_buf));
#else
        explicit_bzero (prealloc_buf, sizeof (prealloc_buf));
#endif

    return l;
}