#include "Golem.h"

Golem::Golem(int _fila, int _columna) : Enemigo(_fila, _columna, VIDA_INICIAL, DANIO) {
	textura.loadFromFile("Imagenes/golem_64.png");
}
