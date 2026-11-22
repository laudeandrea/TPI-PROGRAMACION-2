#define _CRT_SECURE_NO_WARNINGS
#include "PartidaArchivo.h"
#include <cstdio>

Partida::Partida(int _nivel) : nivel(_nivel) {
}

int Partida::getNivel() const {
	return nivel;
}

void Partida::setNivel(int _nivel) {
	nivel = _nivel;
}