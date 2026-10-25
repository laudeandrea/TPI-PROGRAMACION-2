#include "Mago.h"

Mago::Mago(int _fila, int _columna) : Personaje(_fila, _columna, VIDA_INICIAL) {
	mana = MANA_INICIAL;
	textura.loadFromFile("Imagenes/mago_64.png");
}
