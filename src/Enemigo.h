#pragma once
#include "Personaje.h"

/* Clase base de los enemigos: lo que comparten todos (vida, danio, dibujarse) */
class Enemigo : public Personaje {
	protected:
		int danio;
		bool puedeIrA(const Mapa& mapa, int fila, int columna) const;
	public:
		Enemigo(int _fila, int _columna, int _vida, int _danio);
		int getDanio() const;
		/* Casilla a la que quiere moverse este turno, como (x = columna, y = fila).
		*  Si devuelve su propia casilla, significa que no se mueve. */
		sf::Vector2i calcularPaso(const Mapa& mapa, const Personaje& objetivo) const;
};
