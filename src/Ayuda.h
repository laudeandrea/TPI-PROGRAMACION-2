#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "Catalogo.h"

// Pantalla de "Como jugar" (se entra desde el menu). Son varias paginas que se pasan con las flechas.
// Cada pagina tiene un titulo arriba, un dibujo a la izquierda y el texto a la derecha.
// Igual que el Menu, usa su propia vista fija de 1280x720.
class Ayuda {
	private:
		struct Pagina {
			std::string titulo;
			std::vector<std::string> lineas; // las que empiezan con '#' son subtitulos (van en Gema, sin el '#')
			sf::Texture dibujo;              // Imagenes/ayuda_N.png
		};

		static const int CANT_PAGINAS = 6;

		sf::Texture texturaFondo, texturaFlecha;
		sf::Sprite fondo;
		sf::Sprite flechaAnterior, flechaSiguiente; // la de la izquierda es la misma imagen, espejada
		sf::Font fuente;
		Pagina paginas[CANT_PAGINAS];
		int paginaActual;  // de 0 a CANT_PAGINAS - 1
		sf::View vista;    // vista fija de 1280x720

		// Crea un texto con la fuente del juego, en tamanio 22 (el unico en el que se ve nitida).
		sf::Text crearTexto(const std::string& contenido, sf::Color color) const;
		// Como el texto "[Esc] Volver", para saber si se le hizo click.
		sf::Text textoVolver() const;
		void pasarPagina(int cuanto); // +1 siguiente, -1 anterior (no se pasa de la primera ni de la ultima)

	public:
		// Los numeros de las reglas (vida, danio, mana...) los saca del catalogo: si cambian los .dat, la ayuda se entera sola.
		Ayuda(const Catalogo& catalogo);
		// Flechas (o click en las flechas) cambian de pagina.
		// Devuelve true cuando hay que volver al menu: Escape, click en "[Esc] Volver" o Enter en la ultima pagina.
		bool procesarEvento(const sf::Event& evento, const sf::RenderWindow& ventana);
		void dibujar(sf::RenderWindow& ventana);
		// Vuelve a la primera pagina (se llama cada vez que se entra desde el menu).
		void reiniciar();
};
