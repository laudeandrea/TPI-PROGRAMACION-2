#pragma once
#include <SFML/graphics.hpp>
#include <vector>
#include "Mago.h"
#include "Pergamino.h"
#include "Nivel.h"

/* Lo que se dibuja fijo en la pantalla, arriba del tablero: la vida, el mana, la oleada, la ayuda
*  de las habilidades y los numeros de danio que suben cuando se golpea a alguien.
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

		// Una fila del HUD: titulo a la izquierda, barra y "actual/maximo" a la derecha
		void dibujarBarra(sf::RenderWindow& ventana, float y, const std::string& titulo,
			int actual, int maximo, sf::Color color) const;
		void dibujarMana(sf::RenderWindow& ventana, const Mago& mago, int habilidadPreparada) const;
		void dibujarAyuda(sf::RenderWindow& ventana, const Mago& mago, int habilidadPreparada, bool magoSeleccionado) const;
		void dibujarNumeros(sf::RenderWindow& ventana, const sf::View& vistaCamara) const;
		// Crea un texto con la fuente y el estilo del juego.
		sf::Text crearTexto(const std::string& contenido, unsigned int tamanio, sf::Color color) const;

	public:
		Hud();
		// Muestra un numero de danio. La posicion es en el mundo (por ejemplo, arriba de la cabeza).
		void agregarDanio(sf::Vector2f posicionMundo, int cantidad);
		// Hace avanzar las animaciones. dt = segundos desde el cuadro anterior.
		void actualizar(float dt);
		// Borra los numeros que quedaron en pantalla (al empezar una partida nueva).
		void limpiar();
		// Dibuja todo. Cambia la vista de la ventana a la vista por defecto.
		// habilidadPreparada: el indice de la habilidad elegida, o -1 si no hay ninguna.
		void dibujar(sf::RenderWindow& ventana, const sf::View& vistaCamara, const Mago& mago,
			const Pergamino& codice, const Nivel& nivel, int habilidadPreparada, bool magoSeleccionado) const;
		// Oscurece la pantalla y muestra si se gano o se perdio. Se llama despues de dibujar().
		void dibujarFinDePartida(sf::RenderWindow& ventana, bool gano, const std::string& motivo) const;
};
