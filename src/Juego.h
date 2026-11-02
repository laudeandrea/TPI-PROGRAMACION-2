#pragma once
#include <SFML/graphics.hpp>
#include "Camara.h"
#include "Mapa.h"
#include "Mago.h"
#include "Menu.h"
#include "Caballero.h"
#include "Golem.h"
#include "Espectro.h"
#include "Pergamino.h"
#include <memory>
#include <vector>

enum class EstadoJuego{
    MENU,
    JUGANDO
};

enum class FaseTurno {
	JUGADOR,
	ENEMIGOS
};

class Juego {
	private:
		static const int ANCHO_VENTANA = 800;
		static const int ALTO_VENTANA = 600;
		static constexpr float MARGEN_CAMARA = 64.f; // espacio alrededor del tablero para el HUD

		// El orden importa. C++ crea los atributos en el orden en que estan declarados,
		// y la camara necesita que el mapa ya exista (usa mapa.getTamanioMundo())
		sf::RenderWindow window;
		Mapa mapa;
		Mago mago;
		std::vector<std::unique_ptr<Enemigo>> enemigos;
		Pergamino codice;
		Camara camara;
		Menu menu;
		EstadoJuego estado;
		FaseTurno fase;

		void procesarEventos();
		void dibujar();
		/** @brief Si hay otro personaje parado en esa casilla. */
		bool casillaOcupada(int fila, int columna) const;
		/** @brief Si ese personaje puede moverse a esa casilla: terreno permitido y libre. */
		bool puedeMover(const Personaje& quien, int fila, int columna) const;
		void actualizar();

	public:
		Juego();
		void ejecutar();
};