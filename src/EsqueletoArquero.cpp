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
	return darPaso(mapa, ocupadas, filaObjetivo, columnaObjetivo);
}

Ataque EsqueletoArquero::atacar() {
	return tomarAtaqueAnunciado();
}

int EsqueletoArquero::getAlcanceAtaque() const {
	return alcanceAtaque;
}

bool EsqueletoArquero::atacaADistancia() const {
	return true;
}