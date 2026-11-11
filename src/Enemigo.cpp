#include "Enemigo.h"
#include <cmath>

Enemigo::Enemigo(int _fila, int _columna, int _vida, int _danio)
	: Personaje(_fila, _columna, _vida), danio(_danio) {
}

int Enemigo::getDanio() const {
	return danio;
}

bool Enemigo::puedeIrA(const Mapa& mapa, int fila, int columna) const {
	if (!mapa.estaDentro(fila, columna)) {
		return false;   // fuera del tablero: nunca, porque getTipoTerreno no valida
	}
	return puedePisar(mapa.getTipoTerreno(fila, columna));
};

sf::Vector2i Enemigo::calcularPaso(const Mapa& mapa, int filaObjetivo, int columnaObjetivo) const {
	int dFila = filaObjetivo - getFila();
	int dColumna = columnaObjetivo - getColumna();

	sf::Vector2i actual(columna, fila);   // (x = columna, y = fila)

	if (puedeAtacar(filaObjetivo, columnaObjetivo)) return actual; // ya le llega, no se mueve

	int pasoFila = 0, pasoColumna = 0;
	if (dFila > 0) {
		pasoFila = 1;
	}
	else if (dFila < 0) {
		pasoFila = -1;
	}
	else {
		pasoFila = 0;
	}

	if (dColumna > 0) {
		pasoColumna = 1;
	}
	else if (dColumna < 0) {
		pasoColumna = -1;
	}
	else {
		pasoColumna = 0;
	}

	sf::Vector2i primero, segundo;

	if (std::abs(dFila) >= std::abs(dColumna)) {
		primero = sf::Vector2i(columna, fila + pasoFila);
		segundo = sf::Vector2i(columna + pasoColumna, fila);
	}
	else {
		primero = sf::Vector2i(columna + pasoColumna, fila);
		segundo = sf::Vector2i(columna, fila + pasoFila);
	}

	if (puedeIrA(mapa, primero.y, primero.x)) return primero;
	if (puedeIrA(mapa, segundo.y, segundo.x)) return segundo;

	return actual;
}

int Enemigo::getAlcanceAtaque() const {
	return 1;
}

bool Enemigo::puedeAtacar(int filaObjetivo, int columnaObjetivo) const{
	int distancia = std::abs(filaObjetivo - fila) + std::abs(columnaObjetivo - columna);
	return distancia >= 1 && distancia <= getAlcanceAtaque(); // virtual: cada enemigo dice su alcance
}

Ataque Enemigo::atacar(int filaObjetivo, int columnaObjetivo){
	Ataque ataque;
	ataque.casillas.push_back(sf::Vector2i(columnaObjetivo, filaObjetivo));
	ataque.danio = danio;
	return ataque;
}

bool Enemigo::lanzaProyectil() const {
	return false;
}
