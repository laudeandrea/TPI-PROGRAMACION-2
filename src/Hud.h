#pragma once
#include <SFML/graphics.hpp>
#include <vector>
#include "Mago.h"

/* Lo que se dibuja fijo en la pantalla, arriba del tablero: el mana, la ayuda de la habilidad
*  y los numeros de danio que suben cuando se golpea a alguien.
*  Usa la vista por defecto de la ventana (1280x720), no la de la camara. */
class Hud {
	private:
		// Un "-10" que sube y se desvanece arriba de quien recibio el golpe
		struct NumeroDanio {
			sf::Vector2f posicionMundo; // donde aparecio, en pixeles del MUNDO (se mueve con el tablero)
			int cantidad;
			float tiempo;               // segundos desde que aparecio
		};

		static constexpr float DURACION_NUMERO = 0.9f; // segundos que dura en pantalla
		static constexpr float SUBIDA_NUMERO = 22.f;   // cuantos pixeles del mundo sube en ese tiempo

		sf::Font fuente;
		std::vector<NumeroDanio> numeros;

		void dibujarMana(sf::RenderWindow& ventana, const Mago& mago, bool habilidadPreparada, bool magoSeleccionado) const;
		void dibujarNumeros(sf::RenderWindow& ventana, const sf::View& vistaCamara) const;
		/** @brief Crea un texto con la fuente y el estilo del juego. */
		sf::Text crearTexto(const std::string& contenido, unsigned int tamanio, sf::Color color) const;

	public:
		Hud();
		/** @brief Muestra un numero de danio. La posicion es en el mundo (por ejemplo, arriba de la cabeza). */
		void agregarDanio(sf::Vector2f posicionMundo, int cantidad);
		/** @brief Hace avanzar las animaciones. dt = segundos desde el cuadro anterior. */
		void actualizar(float dt);
		/** @brief Dibuja todo. Cambia la vista de la ventana a la vista por defecto. */
		void dibujar(sf::RenderWindow& ventana, const sf::View& vistaCamara, const Mago& mago,
			bool habilidadPreparada, bool magoSeleccionado) const;
};
