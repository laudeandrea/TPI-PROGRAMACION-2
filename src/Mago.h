#pragma once
#include <vector>
#include "Personaje.h"
#include "Habilidad.h"

/* El personaje que maneja el jugador (Tomas, el aprendiz). Ademas de lo de Personaje tiene mana y las
*  habilidades que aprendio: en cada nivel el Juego le ensenia las que corresponden (ver Habilidades.dat). */
class Mago : public Personaje {
	private:
		// 130: en Normal aguanta 10 golpes del caballero (12) o 4 explosiones del golem (30)
		static const int VIDA_INICIAL = 130;
		static const int MANA_INICIAL = 10;
		static const int MANA_MAXIMO = 10;
		static const int MOVIMIENTO = 3; // cuantos pasos puede caminar por turno (cambiar aca para probar otros)

		int mana;
		std::vector<Habilidad> habilidades;
	public:
		Mago(int _fila, int _columna);
		int getMana() const;
		int getManaMaximo() const;
		int getMovimiento() const;
		// Resta mana. Si no alcanza, no resta nada y devuelve false.
		bool gastarMana(int _cantidad);
		// Para cargar una partida guardada (sin pasarse del maximo).
		void setMana(int _mana);
		// Suma mana sin pasarse del maximo.
		void recuperarMana(int _cantidad);
		// Hay que llamar antes a tieneHabilidad(): si no la aprendio, devuelve la primera que tenga.
		const Habilidad& getHabilidad(HabilidadSeleccionada _habilidad) const;
		int getCantidadHabilidades() const;
		// Si ya aprendio esa habilidad (cada una se aprende en un nivel, ver Habilidades.dat).
		bool tieneHabilidad(HabilidadSeleccionada _habilidad) const;
		// Usa esa habilidad sobre esa casilla: gasta el mana y devuelve el ataque (casillas y danio).
		// Si no le alcanza el mana, no gasta nada y devuelve un ataque sin casillas.
		Ataque atacar(HabilidadSeleccionada _habilidad, int filaObjetivo, int columnaObjetivo);
		// Agrega una habilidad. El orden no importa: se buscan por su tipo.
		void aprenderHabilidad(const Habilidad& _habilidad);
		// Se olvida de todas (al empezar un nivel se le vuelven a ensenar las que corresponden).
		void olvidarHabilidades();
};
