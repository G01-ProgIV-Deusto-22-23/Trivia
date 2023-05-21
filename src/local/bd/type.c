void imprimirUsuario (Usuario usuario) {
    printf (
        "ID: %d Name: %s username: %s contrasena: %s aciertosTotales: %i, fallosTotales: %i, ID_Presets: %i\n",
        usuario.ID_Usuario, usuario.nombreVisible, usuario.username, usuario.contrasena, usuario.aciertosTotales,
        usuario.fallosTotales, usuario.ID_Presets
    );
}

void imprimirCategoria (Categoria categoria) {
    printf ("ID: %d, Nombre: %s\n", categoria.ID_Categoria, categoria.nombre);
}

void imprimirPresets (Presets presets) {
    printf (
        "ID: %d, numero Jugadores: %i, numero de rondas: %i, Round time: %i, Categorias: %s, Mecanica1 : %s, Mecanica2 : %s, Mecanica3 : %s, Mecanica4: %s\n",
        presets.ID_Presets, presets.nJugadores, presets.nRondas, presets.RoundTime, presets.Categorias,
        presets.Mecanica1, presets.Mecanica2, presets.Mecanica3, presets.Mecanica4
    );
}
void imprimirRespuesta (Respuesta respuesta) {
	if (respuesta.correcta) {
		printf("respuesta; %s, correcta = true \n", respuesta.respuesta);
	} else {
		printf("respuesta; %s, correcta = false \n", respuesta.respuesta);
	}
}

void imprimirPregunta (Pregunta pregunta) {
	printf("tipo = %s enunciado = %s \n", pregunta.tipo, pregunta.enunciado);
	for (int i = 0; i < 4; i++) {
		imprimirRespuesta(pregunta.opciones[i]);
	}
	printf("\n");
}
Pregunta* Parsear_JSON() {

	Pregunta* preguntas = (Pregunta*)malloc(sizeof(Pregunta) * 200);
	Pregunta preguntaR;
	Respuesta opcion;
	int cantidad = 0;
	int i = 0;

	const char *filename = "TriviaCuestiones.json";
	FILE *ft;
	char* value = malloc(sizeof(char) * 1000000);
	struct stat sb;

	if (stat(filename, &sb) == -1) {
		perror("stat");
		exit(EXIT_FAILURE);
	}

	ft = fopen(filename, "r");
	while (!feof(ft))
	{
	    fread(value, sizeof(char), sb.st_size, ft);
	}
	fclose(ft);

	const cJSON *tipo = NULL;
	const cJSON *pregunta = NULL;
	const cJSON *opciones = NULL;
	const cJSON *respuesta = NULL;

	cJSON *CuestionesTrivias = cJSON_Parse(value);
	cJSON *CuestionesTrivia = NULL;

		if (CuestionesTrivias == NULL)
		{
			const char *error_ptr = cJSON_GetErrorPtr();
		    if (error_ptr != NULL)
		    {
		    	fprintf(stderr, "Error before: %s\n", error_ptr);
		    }
		    cJSON_Delete(CuestionesTrivias);
		    return preguntas;
		}
		cJSON_ArrayForEach(CuestionesTrivia, CuestionesTrivias) {
			tipo = cJSON_GetObjectItemCaseSensitive(CuestionesTrivia, "tipo");
			pregunta = cJSON_GetObjectItemCaseSensitive(CuestionesTrivia, "pregunta");
			opciones = cJSON_GetObjectItemCaseSensitive(CuestionesTrivia, "opciones");
			preguntaR.tipo = malloc(sizeof(char) * 200);
			preguntaR.enunciado = malloc(sizeof(char) * 200);
			strcpy(preguntaR.tipo, cJSON_GetStringValue(tipo));
			strcpy(preguntaR.enunciado, cJSON_GetStringValue(pregunta));
			cJSON_ArrayForEach(respuesta, opciones) {
				cJSON *respuestaE = cJSON_GetObjectItemCaseSensitive(respuesta, "respuesta");
				cJSON *correcta = cJSON_GetObjectItemCaseSensitive(respuesta, "correcta");
				opcion.respuesta = malloc(sizeof(char) * 200);
				strcpy(opcion.respuesta, cJSON_GetStringValue(respuestaE));
				if (cJSON_IsFalse(correcta)) {
					opcion.correcta = false;
				} else {
					opcion.correcta = true;
				}
				preguntaR.opciones[i] = opcion;
				i++;
			}
			preguntas[cantidad] = preguntaR;
			i = 0;
			cantidad++;
		}
		for (int j = 0; j < cantidad; j++) {
			imprimirPregunta(preguntas[j]);
		}
		return preguntas;
}
