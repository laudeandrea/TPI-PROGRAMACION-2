#include "Espectro.h"

Espectro::Espectro(int _fila, int _columna, int _vida, int _danio, int _alcanceAtaque)
	: Enemigo(_fila, _columna, _vida, _danio), alcanceAtaque(_alcanceAtaque) {
	textura.loadFromFile("Imagenes/espectro_64.png");
}

TipoEnemigo Espectro::getTipo() const {
	return TipoEnemigo::ESPECTRO;
}

bool Espectro::puedePisar(TipoTerreno terreno) const {
	// Tambien cruza el muro de fuego, pero se quema (el danio lo hace Juego::quemarSiEstaEnFuego)
	return terreno == TipoTerreno::SUELO
		|| terreno == TipoTerreno::COLUMNAROTA
		|| terreno == TipoTerreno::ABISMO
		|| terreno == TipoTerreno::MURO;
}

int Espectro::getAlcanceAtaque() const {
	return alcanceAtaque;
}

bool Espectro::atacaADistancia() const {
	return true;
}

bool Espectro::mover(const Mapa& mapa, const std::vector<std::vector<bool>>& ocupadas, int filaObjetivo, int columnaObjetivo) {
	// Vuela: el mismo camino mas corto que los demas, pero como puedePisar deja pasar pozos, columnas y fuego,
	// va mas derecho. Para apenas le llega su ataque (desde 3 casillas)
	return darPaso(mapa, ocupadas, filaObjetivo, columnaObjetivo);
}

Ataque Espectro::atacar() {
	return lanzarProyectil();
}

Ataque Espectro::lanzarProyectil() {
	// La bola va a la casilla que marco; el Juego crea el Proyectil y el danio se aplica cuando llega
	return tomarAtaqueAnunciado();
}
