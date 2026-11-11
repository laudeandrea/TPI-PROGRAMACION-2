#pragma once

#include <SFML/graphics.hpp>
#include "Camara.h"
#include "Mapa.h"
#include "Mago.h"
#include "Menu.h"
#include "Nivel.h"
#include "Pergamino.h"
#include "Proyectil.h"
#include "Hud.h"
#include <memory>
#include <vector>

enum class EstadoJuego{
    MENU,
    JUGANDO,
    GANASTE,  // se mato a todas las oleadas: se muestra el cartel de victoria
    PERDISTE  // murio el mago o destruyeron el codice
};

enum class FaseTurno {
	JUGADOR,
	DISPARANDO,          // la bola del mago esta volando: nadie se mueve hasta que llegue
	ENEMIGOS,
	ENEMIGOS_DISPARANDO  // las bolas de los espectros estan volando: despues le toca al jugador
};

class Juego {
	private:
		static const int ANCHO_VENTANA = 1280;
		static const int ALTO_VENTANA = 720;
		static constexpr float MARGEN_CAMARA = 64.f; // espacio alrededor del tablero para el HUD
		static const int MANA_POR_TURNO = 1;         // lo que recupera el mago al empezar cada turno suyo
		static constexpr float ALTURA_PECHO = 30.f;  // desde los pies hasta el pecho: de ahi sale y ahi pega la bola
		static constexpr float ALTURA_CABEZA = 72.f; // desde los pies hasta arriba de la cabeza: ahi aparece el numero de danio
		static constexpr float ALTURA_CODICE = 54.f; // lo mismo para el codice (su imagen sobresale 46 px del centro de la casilla)
		static constexpr float DURACION_EXPLOSION = 0.5f; // segundos que se ven las casillas rojas de un golpe
		static const int SIN_HABILIDAD = -1;         // valor de habilidadPreparada cuando no hay ninguna elegida

		// Donde arranca cada partida
		static const int FILA_MAGO = 4, COLUMNA_MAGO = 1;
		static const int FILA_CODICE = 5, COLUMNA_CODICE = 0;

		// Una casilla que se marca en rojo un momento porque le pegaron (golpe o explosion)
		struct Explosion {
			int fila, columna;
			float tiempo; // segundos desde que aparecio
		};

		// El orden importa. C++ crea los atributos en el orden en que estan declarados,
		// y la camara necesita que el mapa ya exista (usa mapa.getTamanioMundo())
		sf::RenderWindow window;
		sf::Texture texturaFondo;
		sf::Sprite fondo;

		Mapa mapa;
		Mago mago;
		std::vector<std::unique_ptr<Enemigo>> enemigos;
		Pergamino codice;
		Nivel nivel;
		Camara camara;
		Menu menu;
		EstadoJuego estado;
		FaseTurno fase;
		int habilidadPreparada; // indice de la habilidad elegida (0 = Q, 1 = W), o SIN_HABILIDAD
		bool magoSeleccionado;  // se hizo click en el mago: se muestran las casillas a las que puede ir

		// Marcadores de casillas (movimiento y alcance de la habilidad) y las bolas que vuelan
		sf::Texture texturaMovimiento, texturaAlcance, texturaObjetivo, texturaBola, texturaBolaEspectro;
		std::vector<std::unique_ptr<Proyectil>> proyectiles; // vacio cuando no hay ninguna bola volando
		std::vector<Explosion> explosiones;
		Hud hud;
		sf::Clock reloj; // mide el tiempo entre cuadros, para las animaciones

		void procesarEventos();
		// Avanza el juego. dt = segundos que pasaron desde el cuadro anterior.
		void actualizar(float dt);
		void dibujar();
		// Marca las casillas a las que llega la habilidad (en rojo si hay un enemigo).
		void dibujarAlcance();
		// Marca las casillas a las que puede caminar el mago este turno.
		void dibujarMovimiento();
		// Dibuja un marcador (rombo de 64x40) sobre una casilla.
		void dibujarMarcador(const sf::Texture& textura, int fila, int columna);
		// Las casillas que recibieron un golpe hace poco, en rojo y desvaneciendose.
		void dibujarExplosiones();
		// Casillas a las que puede llegar caminando como mucho pasosMaximos pasos (sin contar la suya).
		// Rodea lo que no puede pisar (abismo, columnas, otros personajes). Cada casilla: x = columna, y = fila.
		std::vector<sf::Vector2i> casillasAlcanzables(const Personaje& quien, int pasosMaximos) const;
		// Si el mago puede caminar hasta esa casilla este turno.
		bool magoPuedeIrA(int fila, int columna) const;
		// El primer paso del camino mas corto hasta una casilla desde la que el enemigo pueda atacar al objetivo.
		// Rodea pozos, columnas y otros personajes. Devuelve (x = columna, y = fila); si es su casilla, no se mueve.
		sf::Vector2i pasoHacia(const Enemigo& enemigo, int filaObjetivo, int columnaObjetivo) const;
		// Si hay otro personaje (vivo) parado en esa casilla.
		bool casillaOcupada(int fila, int columna) const;
		// Si ese personaje puede moverse a esa casilla: terreno permitido y libre.
		bool puedeMover(const Personaje& quien, int fila, int columna) const;
		// El enemigo vivo que esta en esa casilla, o nullptr si no hay ninguno.
		Enemigo* buscarEnemigoEn(int fila, int columna) const;
		// Distancia en casillas caminando en fila + en columna (distancia "Manhattan").
		int distancia(int fila1, int columna1, int fila2, int columna2) const;
		int distanciaAlMago(int fila, int columna) const;

		// --- Partida ---
		// Deja todo como al principio: mago, codice, nivel y la primera oleada.
		void reiniciarPartida();
		// Pone en el tablero a los enemigos de la oleada actual del nivel.
		void empezarOleada();
		// Si se perdio, se gano o hay que pasar a la proxima oleada.
		void revisarFinDePartida();

		// --- Turno del jugador ---
		// Usa la habilidad preparada sobre esa casilla (si se puede: enemigo al alcance y mana suficiente).
		void usarHabilidad(int fila, int columna);
		// Empuja al enemigo una casilla, alejandolo del mago. Si cae al abismo y no vuela, muere.
		void empujar(Enemigo& enemigo, int danio);
		// Despues de que el mago actua: saca a los muertos y le pasa el turno a los enemigos.
		void terminarTurnoJugador();

		// --- Turno de los enemigos ---
		// Cada enemigo ataca o camina hacia lo que tenga mas cerca (el mago o el codice).
		void turnoEnemigos();
		// Vuelve el turno del jugador: recupera mana.
		void empezarTurnoJugador();

		// --- Danio ---
		// Lastima a quien este en esa casilla (mago, codice o enemigo) y muestra el numero.
		void daniarCasilla(int fila, int columna, int danio);
		// Mueve las bolas que estan volando. Las que llegan hacen su danio y se borran.
		void moverProyectiles(float dt);
		// Saca del vector a los enemigos que quedaron sin vida.
		void sacarMuertos();
	public:
		Juego();
		void ejecutar();
};
