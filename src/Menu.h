#pragma once
#include <SFML/Graphics.hpp>

/* El menu principal: Continuar, Jugar, Como jugar, Creditos y Salir. Se elige con las flechas o el mouse.
*  Usa su propia vista fija de 1280x720 (no la de la camara del tablero). "Continuar" solo se puede elegir
*  si hay una partida guardada. */
class Menu {
	private:
		static const int CANT_OPCIONES = 5;
		static constexpr float Y_PRIMERA_OPCION = 500.f;   // centro de "Continuar" (con 5 opciones, la ultima queda en 660)
		static constexpr float SEPARACION_OPCIONES = 40.f; // distancia entre opciones

		sf::Texture texturaFondo, texturaTitulo, texturaSelector;
		sf::Sprite fondo, titulo, selector;
		sf::Font fuente;
		// 0 = Continuar, 1 = Jugar, 2 = Como jugar, 3 = Creditos, 4 = Salir (el Juego usa estos numeros)
		sf::Text opciones[CANT_OPCIONES];
		bool hayPartida;      // si hay una partida guardada: se puede elegir "Continuar"
		sf::View vista;       // vista fija de 1280x720
		int seleccionada;     // la opcion marcada (0 a CANT_OPCIONES - 1)
	public:
		Menu();
		// Las flechas o el mouse marcan una opcion. Enter o click devuelven la opcion elegida (0 a 4); si no, -1.
		int procesarEvento(const sf::Event& evento, const sf::RenderWindow& ventana);
		// Dibuja el fondo, el titulo, las opciones y el selector.
		void dibujar(sf::RenderWindow& ventana);
		// Si hay partida guardada, se puede elegir "Continuar" (y queda marcada).
		void setHayPartida(bool _hayPartida);
};
