#include "Enemigo.h"

Enemigo::Enemigo(int _fila, int _columna, int _vida, int _danio)
	: Personaje(_fila, _columna, _vida), danio(_danio) {
}

int Enemigo::getDanio() const {
	return danio;
}
