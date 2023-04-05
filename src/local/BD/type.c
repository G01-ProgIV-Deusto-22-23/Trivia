#include "type.h"
#include <stdio.h>

void imprimirUsuario(Usuario usuario) {
	printf("ID: %d Name: %s username: %s contrasena: %s aciertosTotales: %i, fallosTotales: %i, ID_Presets: %i\n", usuario.ID_Usuario, usuario.nombreVisible, usuario.username, usuario.contrasena, usuario.aciertosTotales, usuario.fallosTotales, usuario.ID_Presets);
}
void imprimirCategoria(Categoria categoria) {
	printf("ID: %d, Nombre: %s\n", categoria.ID_Categoria, categoria.nombre);
}
void imprimirPresets(Presets presets) {
	printf("ID: %d, numero Jugadores: %i, numero de rondas: %i, Round time: %i, Categorias: %s, Mecanica1 : %s, Mecanica2 : %s, Mecanica3 : %s, Mecanica4: %s\n", presets.ID_Presets, presets.nJugadores, presets.nRondas, presets.RoundTime, presets.Categorias, presets.Mecanica1, presets.Mecanica2, presets.Mecanica3, presets.Mecanica4);
}
