#pragma once
#include <SFML/System/Vector2.hpp>
#include <vector>

/* Lo que hace un ataque: a que casillas les pega y cuanto. Lo usan las habilidades del mago (Mago::atacar,
*  Habilidad::usar), los enemigos (Enemigo::atacar) y las bolas cuando llegan (Proyectil::colisionar).
*  El que ataca no sabe quien esta parado ahi: el Juego lastima al que este (el mago, el codice o un enemigo).
*  Es un struct (solo datos, sin metodos), por eso no tiene .cpp. */
struct Ataque {
	std::vector<sf::Vector2i> casillas; // x = columna, y = fila
	int danio = 0;
};
