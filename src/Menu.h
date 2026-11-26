#pragma once
#include <SFML/Graphics.hpp>

/* El menu principal: Continuar, Nueva partida, Como jugar y Salir. Se elige con las flechas o el mouse.
*  Usa su propia vista fija de 1280x720 (no la de la camara del tablero). "Continuar" solo se puede elegir
*  si hay una partida guardada. */
class Menu {
	private:
		static constexpr float Y_PRIMERA_OPCION = 520.f;   // centro de "Continuar"
		static constexpr float SEPARACION_OPCIONES = 40.f; // distancia entre opciones

		sf::Texture texturaFondo, texturaTitulo, texturaSelector;
		sf::Sprite fondo, titulo, selector;
		sf::Font fuente;
		sf::Text opciones[4]; // 0 = Continuar, 1 = Nueva partida, 2 = Como jugar, 3 = Salir (el Juego usa estos numeros)
		bool hayPartida;      // si hay una partida guardada: se puede elegir "Continuar"
		sf::View vista;       // vista fija de 1280x720
		int seleccionada;     // la opcion marcada (0 a 3)
	public:
		Menu();
		// Las flechas o el mouse marcan una opcion. Enter o click devuelven la opcion elegida (0 a 3); si no, -1.
		int procesarEvento(const sf::Event& evento, const sf::RenderWindow& ventana);
		// Dibuja el fondo, el titulo, las opciones y el selector.
		void dibujar(sf::RenderWindow& ventana);
		// Si hay partida guardada, se puede elegir "Continuar" (y queda marcada).
		void setHayPartida(bool _hayPartida);
};
