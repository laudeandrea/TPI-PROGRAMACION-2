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
	public:
		// Una casilla a la que va a pegar algun enemigo en su proximo turno, y cuanto (si son varios, la suma)
		struct Amenaza {
			sf::Vector2f posicionMundo; // el centro de la casilla, en pixeles del MUNDO
			int danio;
		};
		// Una casilla de fuego y cuantos turnos de los enemigos le quedan
		struct TurnosFuego {
			sf::Vector2f posicionMundo; // donde va el numero, en pixeles del MUNDO
			int turnos;
		};
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
		// Si es el turno del jugador, la linea que esta debajo del mouse se resalta (se puede hacer click).
		void dibujarHabilidades(sf::RenderWindow& ventana, const Mago& mago, HabilidadSeleccionada habilidadPreparada,
			bool turnoJugador, sf::Vector2f mouse) const;
		// Las habilidades que sabe el mago, en el orden de sus teclas (1, 2, 3).
		std::vector<HabilidadSeleccionada> habilidadesDelMago(const Mago& mago) const;
		// Donde esta (para el mouse) la linea numero "posicion" de la lista de habilidades (0 = la primera).
		sf::FloatRect cajaHabilidad(int posicion) const;
		// Donde esta el boton "Terminar turno" (va abajo de la ultima habilidad).
		sf::FloatRect cajaTerminarTurno(const Mago& mago) const;
		// Que hacer ahora (cambia si el mago esta seleccionado o hay una habilidad preparada).
		void dibujarAyuda(sf::RenderWindow& ventana, const Mago& mago, HabilidadSeleccionada habilidadPreparada, bool magoSeleccionado) const;
		void dibujarNumeros(sf::RenderWindow& ventana, const sf::View& vistaCamara) const;
		// Oscurece todo lo de atras (el tablero se sigue viendo, apagado).
		void dibujarVelo(sf::RenderWindow& ventana) const;
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
		// turnoJugador: si se pueden usar los botones. mouse: donde esta, en coordenadas de la vista por defecto.
		// nombreDificultad: se muestra abajo a la derecha.
		void dibujar(sf::RenderWindow& ventana, const sf::View& vistaCamara, const Mago& mago,
			const Pergamino& codice, const Nivel& nivel, HabilidadSeleccionada habilidadPreparada, bool magoSeleccionado,
			bool turnoJugador, sf::Vector2f mouse, const std::string& nombreDificultad) const;
		// El danio que va a llegar a cada casilla marcada y un "!" arriba de cada enemigo que va a atacar
		// (atacantes: arriba de su cabeza, en pixeles del mundo). Cambia a la vista por defecto.
		void dibujarAmenazas(sf::RenderWindow& ventana, const sf::View& vistaCamara,
			const std::vector<Amenaza>& amenazas, const std::vector<sf::Vector2f>& atacantes) const;
		// El numero de turnos que le quedan a cada casilla de fuego, en el color del fuego. Cambia a la vista por defecto.
		void dibujarTurnosFuego(sf::RenderWindow& ventana, const sf::View& vistaCamara, const std::vector<TurnosFuego>& fuegos) const;
		// "Musica: 7" abajo a la izquierda, despues de cambiar el volumen. opacidad: 1 = se ve entero, 0 = invisible.
		void dibujarVolumen(sf::RenderWindow& ventana, int volumen, float opacidad) const;
		// Que habilidad hay en ese punto del panel (NINGUNA si no hay ninguna). El punto, en la vista por defecto.
		HabilidadSeleccionada habilidadEn(sf::Vector2f punto, const Mago& mago) const;
		// Si ese punto cae sobre el boton "Terminar turno".
		bool estaSobreTerminarTurno(sf::Vector2f punto, const Mago& mago) const;
		// Cartel de victoria o derrota. Se llama despues de dibujar().
		void dibujarFinDePartida(sf::RenderWindow& ventana, bool gano, const std::string& motivo) const;
		// Cartel grande con la historia del piso: titulo, un aviso (en Gema, puede ir vacio), las lineas de la
		// historia y que tecla apretar. Se usa antes de cada piso, entre un nivel y el siguiente y en los creditos.
		void dibujarHistoria(sf::RenderWindow& ventana, const std::string& titulo, const std::string& aviso,
			const std::vector<std::string>& lineas, const std::string& instruccion) const;
};
