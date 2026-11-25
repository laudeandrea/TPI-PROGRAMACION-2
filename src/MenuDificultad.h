#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "Catalogo.h"

/* La pantalla que aparece al elegir "Nueva partida": Facil, Normal o Dificil.
*  Funciona igual que el Menu (flechas o mouse para marcar, Enter o click para elegir) y usa su mismo arte.
*  Abajo de las opciones explica la dificultad marcada: que es y que cambia (sale de Dificultades.dat). */
class MenuDificultad {
	public:
		static const int NADA = -1;   // todavia no se eligio nada
		static const int VOLVER = -2; // se apreto Escape: volver al menu
	private:
		static constexpr float Y_PRIMERA_OPCION = 520.f;   // como en el Menu
		static constexpr float SEPARACION_OPCIONES = 40.f;

		sf::Texture texturaFondo, texturaTitulo, texturaSelector;
		sf::Sprite fondo, titulo, selector;
		sf::Font fuente;
		sf::View vista;                            // vista fija de 1280x720, como el Menu
		std::vector<DatosDificultad> dificultades; // una por opcion, en el orden de Dificultades.dat
		std::vector<sf::Text> opciones;
		int seleccionada;                          // posicion en "dificultades" (empieza en 0)

		// Texto con la fuente del juego, tamanio 22 y borde oscuro (se lee sobre el fondo del menu).
		sf::Text crearTexto(const std::string& contenido, sf::Color color) const;
		// Dibuja el texto centrado en x = 640, con su centro en esa y.
		void dibujarCentrado(sf::RenderWindow& ventana, sf::Text texto, float y) const;
	public:
		MenuDificultad(const Catalogo& catalogo);
		// Flechas o mouse marcan una opcion. Enter o click devuelven el NUMERO de esa dificultad (1, 2, 3).
		// Escape devuelve VOLVER y, si no paso nada, NADA.
		int procesarEvento(const sf::Event& evento, const sf::RenderWindow& ventana);
		void dibujar(sf::RenderWindow& ventana);
		// Deja marcada la dificultad con ese numero (al entrar se marca la Normal).
		void setSeleccionada(int numero);
};
