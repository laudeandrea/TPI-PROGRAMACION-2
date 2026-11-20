#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Mago.h"
#include "Pergamino.h"
#include "Nivel.h"

/* Lo que se dibuja fijo en la pantalla: el panel de la derecha (nivel, oleada, vida, mana, habilidades
*  y la ayuda de que hacer), los numeros de danio que suben cuando se golpea a alguien y los carteles.
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

		// El fondo del panel de la derecha y las lineas que separan sus partes.
		void dibujarPanel(sf::RenderWindow& ventana) const;
		void dibujarSeparador(sf::RenderWindow& ventana, float y) const;
		// Titulo y "actual/maximo" en una linea, y abajo la barra
		void dibujarBarra(sf::RenderWindow& ventana, float y, const std::string& titulo,
			int actual, int maximo, sf::Color color) const;
		void dibujarMana(sf::RenderWindow& ventana, const Mago& mago, HabilidadSeleccionada habilidadPreparada) const;
		// La lista de habilidades, una por linea, con su tecla y su costo.
		void dibujarHabilidades(sf::RenderWindow& ventana, const Mago& mago, HabilidadSeleccionada habilidadPreparada) const;
		// Que hacer ahora (cambia si el mago esta seleccionado o hay una habilidad preparada).
		void dibujarAyuda(sf::RenderWindow& ventana, const Mago& mago, HabilidadSeleccionada habilidadPreparada, bool magoSeleccionado) const;
		void dibujarNumeros(sf::RenderWindow& ventana, const sf::View& vistaCamara) const;
		// Oscurece la pantalla y muestra un panel en el medio con tres lineas: titulo (grande), detalle y que tecla apretar.
		void dibujarCartel(sf::RenderWindow& ventana, const std::string& titulo, sf::Color colorTitulo,
			const std::string& detalle, const std::string& instruccion) const;
		// Crea un texto con la fuente y el estilo del juego.
		sf::Text crearTexto(const std::string& contenido, unsigned int tamanio, sf::Color color) const;
		// Lo mismo, pero alineado a la derecha del panel (termina en xDerecha).
		void dibujarTextoDerecha(sf::RenderWindow& ventana, sf::Text texto, float xDerecha, float y) const;

	public:
		Hud();
		// Muestra un numero de danio. La posicion es en el mundo (por ejemplo, arriba de la cabeza).
		void agregarDanio(sf::Vector2f posicionMundo, int cantidad);
		// Hace avanzar las animaciones. dt = segundos desde el cuadro anterior.
		void actualizar(float dt);
		// Borra los numeros que quedaron en pantalla (al empezar una partida nueva).
		void limpiar();
		// Dibuja todo. Cambia la vista de la ventana a la vista por defecto.
		void dibujar(sf::RenderWindow& ventana, const sf::View& vistaCamara, const Mago& mago,
			const Pergamino& codice, const Nivel& nivel, HabilidadSeleccionada habilidadPreparada, bool magoSeleccionado) const;
		// Cartel de victoria o derrota. Se llama despues de dibujar().
		void dibujarFinDePartida(sf::RenderWindow& ventana, bool gano, const std::string& motivo) const;
		// Cartel entre un nivel y el siguiente. aviso: por ejemplo, la habilidad nueva que aprendio.
		void dibujarNivelSuperado(sf::RenderWindow& ventana, const std::string& aviso) const;
};
