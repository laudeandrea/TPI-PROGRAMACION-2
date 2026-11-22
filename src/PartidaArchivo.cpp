#define _CRT_SECURE_NO_WARNINGS
#include "PartidaArchivo.h"
#include <cstdio>

bool PartidaArchivo::existe() const {
	FILE* archivo = fopen(NOMBRE_ARCHIVO, "rb");
	if (archivo == NULL) {
		return false;
	}
	fclose(archivo);
	return true;
}

bool PartidaArchivo::guardar(const Partida& partida) const {
	FILE* archivo = fopen(NOMBRE_ARCHIVO, "wb"); // crea o pisa el archivo
	if (archivo == NULL) {
		return false;
	}

	size_t escritos = fwrite(&partida, sizeof(Partida), 1, archivo);
	fclose(archivo); // siempre cerrar
	return escritos == 1;
}

Partida PartidaArchivo::leer() const {
	Partida porDefecto; // nivel 1

	FILE* archivo = fopen(NOMBRE_ARCHIVO, "rb");
	if (archivo == NULL) {
		return porDefecto;
	}

	Partida leida;
	size_t leidos = fread(&leida, sizeof(Partida), 1, archivo);
	fclose(archivo);

	if (leidos != 1) {
		return porDefecto;
	}
	return leida;
}

int PartidaArchivo::getCantidadRegistros() const {
	FILE* archivo = fopen(NOMBRE_ARCHIVO, "rb");
	if (archivo == NULL) {
		return 0;
	}
	fseek(archivo, 0, SEEK_END);      
	long bytes = ftell(archivo);       
	fclose(archivo);
	return (int)(bytes / sizeof(Partida));
}

bool PartidaArchivo::borrar() const {
	return remove(NOMBRE_ARCHIVO) == 0;
}