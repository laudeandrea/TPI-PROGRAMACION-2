#pragma once
#include <SFML/graphics.hpp>

class Camara {
	private:
		// Que parte del ancho de la ventana (desde la izquierda) es para el tablero. El resto es el panel del HUD
		static constexpr float PARTE_TABLERO = 0.75f; // 960 de 1280 px
		sf::View vista;
		sf::Vector2f areaVisible; // tablero + margen: lo que siempre tiene que entrar en la ventana

	public:
		Camara(sf::Vector2f tamanioMundo, float margen);
		void ajustarAVentana(sf::Vector2u tamanioVentana);
		void aplicar(sf::RenderWindow& ventana) const;
		// La vista de la camara, para convertir posiciones del mundo a la pantalla (HUD).
		const sf::View& getVista() const;
};
