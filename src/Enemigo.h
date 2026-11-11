#pragma once
#include <vector>
#include "Personaje.h"

// Lo que hace un enemigo cuando ataca: a que casillas les pega y cuanto.
// El enemigo no sabe quien esta parado ahi: el Juego se encarga de lastimar al que este
// (el mago, el codice o incluso otro enemigo).
struct Ataque {
	std::vector<sf::Vector2i> casillas; // x = columna, y = fila
	int danio;
};

/* Clase base de los enemigos: lo que comparten todos (vida, danio, dibujarse) */
class Enemigo : public Personaje {
	protected:
		int danio;
		bool puedeIrA(const Mapa& mapa, int fila, int columna) const;
	public:
		Enemigo(int _fila, int _columna, int _vida, int _danio);
		int getDanio() const;
		// Casilla a la que quiere moverse para acercarse al objetivo, como (x = columna, y = fila).
		// Si devuelve su propia casilla, significa que no se mueve.
		sf::Vector2i calcularPaso(const Mapa& mapa, int filaObjetivo, int columnaObjetivo) const;

		// Desde cuantas casillas puede atacar. Por defecto 1: tiene que estar pegado.
		virtual int getAlcanceAtaque() const;
		bool puedeAtacar(int filaObjetivo, int columnaObjetivo) const;
		// Por defecto le pega solo a la casilla del objetivo. Las hijas lo redefinen (ej: el golem explota).
		virtual Ataque atacar(int filaObjetivo, int columnaObjetivo);
		// Si el ataque viaja como un proyectil: el danio se aplica cuando llega, no en el momento.
		virtual bool lanzaProyectil() const;
};
