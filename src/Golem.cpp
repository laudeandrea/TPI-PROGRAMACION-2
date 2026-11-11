#include "Golem.h"

Golem::Golem(int _fila, int _columna) : Enemigo(_fila, _columna, VIDA_INICIAL, DANIO) {
	textura.loadFromFile("Imagenes/golem_64.png");
}

Ataque Golem::atacar(int filaObjetivo, int columnaObjetivo) {
	Ataque ataque;
	ataque.danio = danio;

	// Las 8 casillas que lo rodean (las diagonales tambien). No importa a quien apuntaba:
	// la explosion le pega a todo lo que este cerca. Si alguna cae fuera del tablero, el Juego la ignora
	for (int f = fila - 1; f <= fila + 1; f++) {
		for (int c = columna - 1; c <= columna + 1; c++) {
			if (f == fila && c == columna) continue; // su propia casilla no
			ataque.casillas.push_back(sf::Vector2i(c, f));
		}
	}

	vida = 0; // se destruye: el Juego lo saca del tablero como a cualquier enemigo sin vida
	return ataque;
}
