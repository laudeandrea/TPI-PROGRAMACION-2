#include <cstdlib> // std::srand
#include <ctime>   // std::time
#include "Juego.h"

// Solo crea el juego y lo arranca: toda la logica esta en la clase Juego
int main() {
	// Semilla para std::rand (con la hora, cambia en cada partida): Mapa::cargar sortea el dibujo de cada baldosa
	std::srand((unsigned)std::time(nullptr));

	Juego juego;
	juego.iniciar();

	return 0;
}
