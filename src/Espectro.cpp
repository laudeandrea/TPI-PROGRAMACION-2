#include "Espectro.h"

Espectro::Espectro(int _fila, int _columna) : Enemigo(_fila, _columna, VIDA_INICIAL, DANIO){
   textura.loadFromFile("Imagenes/espectro_64.png");
};

bool Espectro::puedePisar(TipoTerreno terreno) const {
	return terreno == TipoTerreno::SUELO
		|| terreno == TipoTerreno::COLUMNAROTA
		|| terreno == TipoTerreno::ABISMO;
};