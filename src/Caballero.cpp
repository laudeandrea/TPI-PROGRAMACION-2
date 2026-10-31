#include "Caballero.h"

Caballero::Caballero(int _fila, int _columna) : Enemigo(_fila, _columna, VIDA_INICIAL, DANIO) {
    textura.loadFromFile("Imagenes/caballero_64.png");
}
