#include "Golem.h"

Golem::Golem(int _fila, int _columna, int _vida, int _danio) : Enemigo(_fila, _columna, _vida, _danio) {
	textura.loadFromFile("Imagenes/golem_64.png");
}

TipoEnemigo Golem::getTipo() const {
	return TipoEnemigo::GOLEM;
}

Ataque Golem::explosion() const {
	Ataque ataque;
	ataque.danio = danio;
	// Si alguna casilla cae fuera del tablero, el Juego la ignora
	for (int f = fila - 1; f <= fila + 1; f++) {
		for (int c = columna - 1; c <= columna + 1; c++) {
			if (f == fila && c == columna) continue; // su propia casilla no
			ataque.casillas.push_back(sf::Vector2i(c, f));
		}
	}
	return ataque;
}

bool Golem::mover(const Mapa& mapa, const std::vector<std::vector<bool>>& ocupadas, int filaObjetivo, int columnaObjetivo) {
	// Camina por el camino mas corto: solo pisa el suelo (ver puedePisar en Personaje)
	return darPaso(mapa, ocupadas, filaObjetivo, columnaObjetivo);
}

Ataque Golem::planearAtaque(int filaObjetivo, int columnaObjetivo) const {
	// No importa a quien apuntaba: explota alrededor suyo
	return explosion();
}

Ataque Golem::atacar() {
	tomarAtaqueAnunciado(); // borra el anuncio: la explosion marcada era esta misma
	return explotar();
}

Ataque Golem::explotar() {
	vida = 0; // se destruye: el Juego lo saca del tablero como a cualquier enemigo sin vida
	return explosion();
}

Ataque Golem::ataqueAlMorir() const {
	return explosion();
}
