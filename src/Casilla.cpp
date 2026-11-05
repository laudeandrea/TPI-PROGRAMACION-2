#include "Casilla.h"

Casilla::Casilla(TipoTerreno t) {
	tipo = t;
};

TipoTerreno Casilla::getTipoTerreno() const {
	return tipo;
};

void Casilla::setTipoTerreno(TipoTerreno _tipoTerreno) {
	tipo = _tipoTerreno;
}

