#include "Caballero.h"

Caballero::Caballero(int _fila, int _columna, int _vida, int _danio) : Enemigo(_fila, _columna, _vida, _danio) {
	textura.loadFromFile("Imagenes/caballero_64.png");
}

TipoEnemigo Caballero::getTipo() const {
	return TipoEnemigo::CABALLERO;
}

bool Caballero::mover(const Mapa& mapa, const std::vector<std::vector<bool>>& ocupadas, int filaObjetivo, int columnaObjetivo) {
	// Camina por el camino mas corto: solo pisa el suelo (ver puedePisar en Personaje)
	return darPaso(mapa, ocupadas, filaObjetivo, columnaObjetivo);
}

Ataque Caballero::atacar() {
	// Le pega a la casilla que marco, este quien este ahi
	return tomarAtaqueAnunciado();
}
