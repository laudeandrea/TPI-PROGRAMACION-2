#include "EsqueletoArquero.h"

EsqueletoArquero::EsqueletoArquero(int _fila, int _columna, int _vida, int _danio, int _alcanceAtaque)
	: Enemigo(_fila, _columna, _vida, _danio), alcanceAtaque(_alcanceAtaque) {
	textura.loadFromFile("Imagenes/esqueleto_64.png");
}

TipoEnemigo EsqueletoArquero::getTipo() const {
	return TipoEnemigo::ESQUELETO;
}

bool EsqueletoArquero::mover(const Mapa& mapa, const std::vector<std::vector<bool>>& ocupadas,
	int filaObjetivo, int columnaObjetivo) {
	// El mismo camino mas corto que el caballero; la diferencia es que frena antes (desde 4 casillas ya le llega)
	return darPaso(mapa, ocupadas, filaObjetivo, columnaObjetivo);
}

Ataque EsqueletoArquero::atacar() {
	// La flecha va a la casilla que marco; el Juego crea el Proyectil y el danio se aplica cuando llega
	return tomarAtaqueAnunciado();
}

int EsqueletoArquero::getAlcanceAtaque() const {
	return alcanceAtaque;
}

bool EsqueletoArquero::atacaADistancia() const {
	return true;
}
