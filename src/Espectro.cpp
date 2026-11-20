#include "Espectro.h"

Espectro::Espectro(int _fila, int _columna) : Enemigo(_fila, _columna, VIDA_INICIAL, DANIO){
   textura.loadFromFile("Imagenes/espectro_64.png");
};

bool Espectro::puedePisar(TipoTerreno terreno) const {
	// Tambien cruza el muro de fuego, pero se quema (el danio lo hace Juego::quemarSiEstaEnFuego)
	return terreno == TipoTerreno::SUELO
		|| terreno == TipoTerreno::COLUMNAROTA
		|| terreno == TipoTerreno::ABISMO
		|| terreno == TipoTerreno::MURO;
};

int Espectro::getAlcanceAtaque() const {
	return ALCANCE_ATAQUE;
};

bool Espectro::lanzaProyectil() const {
	return true;
};
