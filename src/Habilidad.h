#pragma once
#include <string>
#include "Ataque.h"

// Que hace la habilidad. El Juego mira esto para saber si tira una bola, empuja o prende fuego.
// El numero de cada una (0, 1, 2) es el que se guarda en Habilidades.dat
enum class TipoHabilidad { BOLADEENERGIA, EMPUJE, MURO };

/* Cual de las habilidades del mago esta en la mano.
 * Los valores coinciden con la tecla: 1 = bola, 2 = empuje, 3 = muro de fuego (se aprende en el nivel 2).
 * Siempre es el TipoHabilidad + 1 (NINGUNA es el 0). */
enum class HabilidadSeleccionada {
	NINGUNA,
	BOLA_DE_ENERGIA,
	EMPUJE,
	MURO
};

/* Un hechizo del mago: nombre, danio, cuanto mana cuesta y hasta donde llega. Los valores salen de
*  Habilidades.dat (ver Catalogo::crearHabilidad). */
class Habilidad {
	private:
		TipoHabilidad tipo;
		std::string nombre;
		int danio, costoMana, alcance;
		int duracion; // turnos que dura lo que deja en el tablero (el muro de fuego); las demas, 0
	public:
		Habilidad(TipoHabilidad _tipo, std::string _nombre, int _danio, int _costoMana, int _alcance, int _duracion = 0);
		TipoHabilidad getTipo() const;
		std::string getNombre() const;
		void setNombre(std::string _nombre);
		int getDanio() const;
		void setDanio(int _danio);
		int getCostoMana() const;
		void setCostoMana(int _costoMana);
		int getAlcance() const;
		void setAlcance(int _alcance);
		int getDuracion() const;
		// Lo que hace sobre esa casilla: le pega con su danio. Lo que pasa despues (la bola que vuela, el
		// empujon, el fuego) lo resuelve el Juego segun el tipo.
		Ataque usar(int filaObjetivo, int columnaObjetivo) const;
};
