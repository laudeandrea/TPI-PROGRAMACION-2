#pragma once
#include <SFML/graphics.hpp>

class Camara {
	private:
		sf::View vista;
		sf::Vector2f areaVisible; // tablero + margen: lo que siempre tiene que entrar en la ventana

	public:
		Camara(sf::Vector2f tamanioMundo, float margen);
		void ajustarAVentana(sf::Vector2u tamanioVentana);
		void aplicar(sf::RenderWindow& ventana) const;
};
