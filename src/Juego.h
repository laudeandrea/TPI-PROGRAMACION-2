#pragma once

#include "PartidaArchivo.h"
#include <SFML/Graphics.hpp>
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
    NIVEL_SUPERADO, // se gano un nivel y hay otro: cartel con "Enter para seguir"
    GANASTE,  // se gano el ultimo nivel: se muestra el cartel de victoria
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
		static const int MANA_POR_TURNO = 2;         // lo que recupera el mago al empezar cada turno suyo
		static constexpr float ALTURA_PECHO = 30.f;  // desde los pies hasta el pecho: de ahi sale y ahi pega la bola
		static constexpr float ALTURA_CABEZA = 72.f; // desde los pies hasta arriba de la cabeza: ahi aparece el numero de danio
		static constexpr float ALTURA_CODICE = 54.f; // lo mismo para el codice (su imagen sobresale 46 px del centro de la casilla)
		static constexpr float DURACION_EXPLOSION = 0.5f; // segundos que se ven las casillas rojas de un golpe

		// Muro de fuego (se aprende en el nivel 2). Donde arrancan el mago y el codice lo dice el Nivel
		static const int NIVEL_MURO = 2;    // desde que nivel lo tiene el mago
		static const int COSTO_MURO = 4;    // mana
		static const int ALCANCE_MURO = 3;  // a cuantas casillas del mago se puede poner (el centro)
		static const int TURNOS_MURO = 3;   // cuantos turnos de los enemigos dura
		static const int DANIO_FUEGO = 10;  // lo que pierde el espectro cada vez que entra (o empieza su turno) en el fuego

		// Una casilla que se marca en rojo un momento porque le pegaron (golpe o explosion)
		struct Explosion {
			int fila, columna;
			float tiempo; // segundos desde que aparecio
		};

		// Una casilla del muro de fuego. Mientras dura, el mapa la tiene como TipoTerreno::MURO
		struct Muro {
			int fila, columna;
			int turnosRestantes;
		};

		sf::RenderWindow window;
		sf::Texture texturaFondo;
		sf::Sprite fondo;

		Mapa mapa;
		Mago mago;
		std::vector<std::unique_ptr<Enemigo>> enemigos;
		Pergamino codice;
		Nivel nivel;
		PartidaArchivo partidaArchivo;
		Camara camara;
		Menu menu;
		EstadoJuego estado;
		FaseTurno fase;
		HabilidadSeleccionada habilidadPreparada;
		bool magoSeleccionado;  // se hizo click en el mago: se muestran las casillas a las que puede ir
		bool magoSeMovio;       // ya camino este turno: puede tirar una habilidad o pasar, pero no volver a moverse
		Enemigo* enemigoEmpujado; // el enemigo que empujo el mago este turno: pierde su turno (nullptr si no hay)

		// Marcadores de casillas (movimiento y alcance de la habilidad) y las bolas que vuelan
		sf::Texture texturaMovimiento, texturaAlcance, texturaObjetivo, texturaBola, texturaBolaEspectro;
		std::vector<std::unique_ptr<Proyectil>> proyectiles; // vacio cuando no hay ninguna bola volando
		std::vector<Explosion> explosiones;
		std::vector<Muro> muros; // las casillas de fuego que hay ahora en el tablero
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

		// La casilla que esta debajo del mouse (x = columna, y = fila). Puede caer fuera del tablero.
		sf::Vector2i casillaBajoElMouse() const;

		// --- Partida ---
		// Partida nueva: arranca del nivel 1.
		void reiniciarPartida();
		// Arma un nivel de cero: tablero, codice, mago (con vida y mana llenos) y la primera oleada.
		void empezarNivel(int numero);
		// Pone en el tablero a los enemigos de la oleada actual del nivel.
		void empezarOleada();
		// Si se perdio, se gano o hay que pasar a la proxima oleada.
		void revisarFinDePartida();

		// --- Turno del jugador ---
		// Usa la habilidad preparada sobre esa casilla (si se puede: enemigo al alcance y mana suficiente).
		void usarHabilidad(int fila, int columna);
		// Si se puede empujar a ese enemigo: la casilla de atras existe, esta libre y la puede pisar (o es abismo).
		bool sePuedeEmpujar(const Enemigo& enemigo) const;
		// Casilla a la que iria el enemigo si lo empujan: un paso mas, alejandose del mago (x = columna, y = fila).
		sf::Vector2i destinoEmpuje(const Enemigo& enemigo) const;
		// Empuja al enemigo una casilla, alejandolo del mago. Si cae al abismo y no vuela, muere.
		// Hay que llamar antes a sePuedeEmpujar().
		void empujar(Enemigo& enemigo, int danio);
		// Despues de que el mago actua: saca a los muertos y le pasa el turno a los enemigos.
		void terminarTurnoJugador();

		// --- Muro de fuego ---
		// Si en esa casilla puede ir fuego: dentro del tablero, suelo comun y sin nadie encima.
		bool casillaLibreParaMuro(int fila, int columna) const;
		// Si se puede poner el muro con el centro en esa casilla (libre y al alcance del mago).
		bool sePuedePonerMuro(int fila, int columna) const;
		// Las casillas donde quedaria el muro: el centro y sus dos vecinas, perpendicular a la linea
		// mago -> centro. Solo devuelve las que estan libres (cerca de un borde puede quedar mas corto).
		std::vector<sf::Vector2i> casillasDelMuro(int fila, int columna) const;
		// Prende el fuego en esas casillas.
		void ponerMuro(int fila, int columna);
		// Al empezar el turno del jugador: a cada casilla de fuego le queda un turno menos. Las que llegan a 0 se apagan.
		void apagarMuros();
		// Si el enemigo esta parado en el fuego, se quema (solo el espectro puede estar ahi).
		void quemarSiEstaEnFuego(Enemigo& enemigo);

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
