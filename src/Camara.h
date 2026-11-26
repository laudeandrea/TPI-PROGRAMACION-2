#pragma once
#include <SFML/Graphics.hpp>

/* Encuadra el tablero en la ventana: una sf::View que mira el mundo (el tablero isometrico) y lo dibuja en
*  la parte izquierda de la ventana. La derecha queda para el panel del HUD, que usa la vista por defecto. */
class Camara {
	private:
		// Que parte del ancho de la ventana (desde la izquierda) es para el tablero. El resto es el panel del HUD
		static constexpr float PARTE_TABLERO = 0.75f; // 960 de 1280 px
		sf::View vista;
		sf::Vector2f areaVisible; // tablero + margen: lo que siempre tiene que entrar en la ventana
	public:
		Camara(sf::Vector2f tamanioMundo, float margen);
		// Si la ventana cambia de tamanio, agranda la vista del lado que sobra (asi el tablero no se deforma).
		void ajustarAVentana(sf::Vector2u tamanioVentana);
		// Pone la vista de la camara en la ventana: lo que se dibuje despues queda encuadrado como el tablero.
		void aplicar(sf::RenderWindow& ventana) const;
		// La vista de la camara, para convertir posiciones del mundo a la pantalla (HUD).
		const sf::View& getVista() const;
};
