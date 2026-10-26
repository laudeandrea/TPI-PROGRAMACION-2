#include "Casilla.h"

Casilla::Casilla(TipoTerreno t) {
	tipo = t;
};

TipoTerreno Casilla::getTipoTerreno() {
	return tipo;
};

void Casilla::setTipoTerreno(TipoTerreno _tipoTerreno) {
	tipo = _tipoTerreno;
}

bool Casilla::esTransitable() {
	return tipo == TipoTerreno::SUELO;
};