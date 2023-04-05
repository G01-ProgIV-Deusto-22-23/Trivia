#ifndef TYPE_H_
#define TYPE_H_

typedef struct {
	int ID_Usuario;
	char nombreVisible[21];
	char username[21];
	char contrasena[21];
	int aciertosTotales;
	int fallosTotales;
	int ID_Presets;
}Usuario;
typedef struct {
	char nombre[21];
	int ID_Categoria;
}Categoria;
typedef struct{
	int ID_Presets;
	int nJugadores;
	int nRondas;
	int RoundTime;
	char Categorias[101];
	char Mecanica1[4];
	char Mecanica2[4];
	char Mecanica3[4];
	char Mecanica4[4];
}Presets;

void imprimirUsuario(Usuario usuario);
void imprimirCategoria(Categoria categoria);
void imprimirPresets(Presets presets);

#endif
