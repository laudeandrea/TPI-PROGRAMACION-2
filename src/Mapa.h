#pragma once
#include <SFML/graphics.hpp>
#include "Casilla.h"

class Mapa {
	private:
		static const int CANT_CASILLAS = 8; // static const = La constante es una sola para toda la clase, en vez de una copia por cada objeto Mapa
		static constexpr float ANCHO_CASILLAS = 64.f; // (constexpr: porque C++ solo permite inicializar static const SOLO si es entero)
		static constexpr float ALTO_CASILLAS = 32.f;
		static constexpr float CENTRO_ROMBO_DESDE_ABAJO = 34.f; // La distancia desde el fondo de la imagen hasta el centro de la cara de arriba
		Casilla casillas[CANT_CASILLAS][CANT_CASILLAS];
		sf::Texture texturaSuelo;
		sf::Texture texturaColumna;
		sf::Texture texturaAbismo;
	public:
		Mapa();
		void mostrar(sf::RenderWindow& ventana);
		sf::Vector2f casillaAmundo(int fila, int columna) const;
		sf::Vector2f getTamanioMundo() const;
		void mover();
		bool esValida();
};