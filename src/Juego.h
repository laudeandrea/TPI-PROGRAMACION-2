#pragma once
#include <SFML/graphics.hpp>
#include "Mapa.h"
#include "Mago.h"
#include "Camara.h"

class Juego {
	private:
		static const int ANCHO_VENTANA = 800;
		static const int ALTO_VENTANA = 600;
		static constexpr float MARGEN_CAMARA = 64.f; // espacio alrededor del tablero para el HUD

		// IMPORTANTE: el orden importa. C++ crea los atributos en el orden en que estan declarados,
		// y la camara necesita que el mapa ya exista (usa mapa.getTamanioMundo())
		sf::RenderWindow window;
		Mapa mapa;
		Mago mago;
		Camara camara;

		void procesarEventos();
		void dibujar();

	public:
		Juego();
		void ejecutar();
};
