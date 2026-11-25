#pragma once

#include "PartidaArchivo.h"
#include <SFML/Graphics.hpp>
#include "Sonidos.h"
#include "Camara.h"
#include "Catalogo.h"
#include "Mapa.h"
#include "Mago.h"
#include "Menu.h"
#include "MenuDificultad.h"
#include "Ayuda.h"
#include "Nivel.h"
#include "Pergamino.h"
#include "Proyectil.h"
#include "Hud.h"
#include <memory>
#include <vector>

enum class EstadoJuego{
    MENU,
    ELEGIR_DIFICULTAD, // la pantalla de Facil / Normal / Dificil, al elegir "Nueva partida"
    COMO_JUGAR,     // la pantalla de ayuda que se abre desde el menu
    HISTORIA,       // el cartel con la historia del piso, antes de empezar a jugarlo
    JUGANDO,
    NIVEL_SUPERADO, // se gano un nivel y hay otro: cartel con la historia del proximo y "Enter para seguir"
    GANASTE,        // se gano el ultimo nivel: se muestra el cartel de victoria
    PERDISTE        // murio el mago o destruyeron el codice
};

// El turno de los enemigos tiene dos partes: primero ejecutan los ataques que habian anunciado
// (y se espera a que lleguen las bolas), despues caminan y anuncian el proximo.
enum class FaseTurno {
	JUGADOR,
	DISPARANDO,       // la bola del mago esta volando: nadie se mueve hasta que llegue
	ENEMIGOS_ATACAN,  // los enemigos ejecutan los ataques que anunciaron (dura un cuadro)
	ENEMIGOS_ESPERAN  // vuelan las bolas de los espectros y se ven los golpes; despues caminan y anuncian
};

class Juego {
	private:
		static const int ANCHO_VENTANA = 1280;
		static const int ALTO_VENTANA = 720;
		static constexpr float MARGEN_CAMARA = 64.f; // espacio alrededor del tablero para el HUD
		static constexpr float ALTURA_PECHO = 30.f;  // desde los pies hasta el pecho: de ahi sale y ahi pega la bola
		static constexpr float ALTURA_CABEZA = 72.f; // desde los pies hasta arriba de la cabeza: ahi aparece el numero de danio
		static constexpr float ALTURA_CODICE = 54.f; // lo mismo para el codice (su imagen sobresale 46 px del centro de la casilla)
		static constexpr float DURACION_EXPLOSION = 0.5f; // segundos que se ven las casillas rojas de un golpe
		static constexpr float PAUSA_ENEMIGOS = 0.4f;     // segundos entre que pegan y que caminan (para que se vea el golpe)
		static constexpr float DURACION_AVISO_VOLUMEN = 1.5f; // segundos que se ve "Musica: 7" despues de apretar - o =
		// Los valores de las habilidades (danio, costo, alcance, duracion del muro) estan en Datos/Habilidades.dat,
		// y lo que recupera el mago de mana por turno, en Datos/Dificultades.dat (depende de la dificultad)

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

		// OJO con el orden: C++ crea los atributos en el orden en que estan aca. El catalogo va antes que el
		// nivel, el menu de dificultad y la ayuda (que lo usan al crearse), y el nivel antes que la camara
		// (la camara le pide el tamanio al mapa del nivel)
		Catalogo catalogo;
		Mago mago;
		Nivel nivelActual; // el que se esta jugando: tiene el mapa, los enemigos y el codice (ver Nivel)
		PartidaArchivo partidaArchivo;
		Camara camara;
		Sonidos sonidos;
		Menu menu;
		MenuDificultad menuDificultad; // la pantalla de elegir la dificultad
		Ayuda ayuda; // la pantalla de "Como jugar"
		EstadoJuego estado;
		int dificultad; // la que se eligio al empezar la partida: 1 = Facil, 2 = Normal, 3 = Dificil
		FaseTurno fase;
		float tiempoFase; // segundos desde que empezo ENEMIGOS_ESPERAN
		float tiempoVolumen; // segundos desde que se cambio el volumen (el aviso se ve un rato)
		HabilidadSeleccionada habilidadPreparada;
		bool magoSeleccionado;  // se hizo click en el mago: se muestran las casillas a las que puede ir
		bool magoSeMovio;       // ya camino este turno: puede tirar una habilidad o pasar, pero no volver a moverse

		// Marcadores de casillas (movimiento, alcance de la habilidad, ataques anunciados) y las bolas que vuelan
		sf::Texture texturaMovimiento, texturaAlcance, texturaObjetivo, texturaPeligro, texturaBola, texturaBolaEspectro, texturaFlecha;
		std::vector<std::unique_ptr<Proyectil>> proyectiles; // vacio cuando no hay ninguna bola volando
		std::vector<Explosion> explosiones;
		std::vector<Muro> muros; // las casillas de fuego que hay ahora en el tablero
		Hud hud;
		sf::Clock reloj; // mide el tiempo entre cuadros, para las animaciones

		void procesarEventos();
		// Lo que hacen las teclas y los clicks mientras se juega (separado para que procesarEventos no quede gigante).
		void procesarEventoJugando(const sf::Event& event);
		// Avanza el juego. dt = segundos que pasaron desde el cuadro anterior.
		void actualizar(float dt);
		void dibujar();
		// El tablero con todo lo que hay arriba (lo que se ve jugando y detras de los carteles).
		void dibujarTablero();
		// Marca las casillas a las que llega la habilidad (en rojo si hay un enemigo).
		void dibujarAlcance();
		// Marca las casillas a las que puede caminar el mago este turno.
		void dibujarMovimiento();
		// Marca en rojo las casillas a las que van a pegar los enemigos en su proximo turno.
		void dibujarAtaquesAnunciados();
		// Dibuja un marcador (rombo de 64x40) sobre una casilla.
		void dibujarMarcador(const sf::Texture& textura, int fila, int columna);
		// Las casillas que recibieron un golpe hace poco, en rojo y desvaneciendose.
		void dibujarExplosiones();
		// Hace avanzar el turno: la bola del mago vuela y, cuando les toca, los enemigos atacan, esperan sus bolas
		// y caminan. (Lo que hace el jugador en su turno llega por los eventos: ver procesarEventoJugando.)
		void jugarTurno(float dt);
		// Casillas a las que puede llegar caminando como mucho pasosMaximos pasos (sin contar la suya).
		// Rodea lo que no puede pisar (abismo, columnas, otros personajes). Cada casilla: x = columna, y = fila.
		std::vector<sf::Vector2i> casillasAlcanzables(const Personaje& quien, int pasosMaximos) const;
		// Si el mago puede caminar hasta esa casilla este turno.
		bool magoPuedeIrA(int fila, int columna) const;
		// Que casillas tienen a alguien parado (el mago o un enemigo vivo): ocupadas[fila][columna]. Se la pasa a
		// los enemigos para que caminen sin chocarse (ver Enemigo::mover).
		std::vector<std::vector<bool>> casillasOcupadas() const;
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
		// Partida nueva con esa dificultad: arranca del nivel 1 (con su historia).
		void reiniciarPartida(int nuevaDificultad);
		// Sigue la partida guardada: si quedo un nivel por la mitad, lo arma como estaba.
		void continuarPartida();
		// Arma un nivel de cero: tablero, codice, mago (con vida y mana llenos, y las habilidades del nivel) y la primera oleada.
		void empezarNivel(int numero);
		// Pasa al nivel que sigue (el que quedo guardado al ganar este) y lo empieza a jugar.
		void cambiarNivel();
		// Pone en el tablero a los enemigos de la oleada actual del nivel.
		void empezarOleada();
		// Si se perdio, se gano o hay que pasar a la proxima oleada.
		void revisarFinDePartida();
		// Guarda el nivel como esta ahora (partida.dat, Mago.dat y Enemigos.dat). Solo en el turno del jugador.
		void guardarPartidaEnCurso();
		// Escape mientras se juega: guarda y vuelve al menu.
		void volverAlMenu();

		// --- Turno del jugador ---
		// Prepara una habilidad (o la cancela si ya estaba preparada). Tecla 1 a 3 o click en el panel.
		void elegirHabilidad(HabilidadSeleccionada elegida);
		// Termina el turno sin hacer nada (Espacio o click en "Terminar turno").
		void pasarTurno();
		// Usa la habilidad preparada sobre esa casilla (si se puede: objetivo al alcance y mana suficiente).
		void usarHabilidad(int fila, int columna);
		// Si se puede empujar a ese enemigo: la casilla de atras existe, esta libre y la puede pisar (o es abismo).
		bool sePuedeEmpujar(const Enemigo& enemigo) const;
		// Casilla a la que iria el enemigo si lo empujan: un paso mas, alejandose del mago (x = columna, y = fila).
		sf::Vector2i destinoEmpuje(const Enemigo& enemigo) const;
		// Empuja al enemigo una casilla, alejandolo del mago, y le cancela el ataque que tenia anunciado.
		// Si cae al abismo y no vuela, muere (sin explotar). Hay que llamar antes a sePuedeEmpujar().
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
		// Primera parte: cada enemigo ejecuta el ataque que habia anunciado.
		void ejecutarAtaquesEnemigos();
		// Segunda parte: cada enemigo camina hacia su objetivo y, si le llega, anuncia su proximo ataque.
		void moverEnemigos();
		// A quien va el enemigo: lo que tenga mas cerca, el mago o el codice (x = columna, y = fila).
		sf::Vector2i objetivoDe(const Enemigo& enemigo) const;
		// Vuelve el turno del jugador: recupera mana y se apaga un poco el fuego.
		void empezarTurnoJugador();

		// --- Danio ---
		// Lastima a quien este en esa casilla (mago, codice o enemigo) y muestra el numero.
		// Si un enemigo muere por el golpe y explota al morir (el golem), tambien hace esa explosion.
		void daniarCasilla(int fila, int columna, int danio);
		// Lo que pasa cuando un enemigo muere por un golpe: su ataqueAlMorir (la explosion del golem).
		void alMorirEnemigo(const Enemigo& enemigo);
		// Mueve las bolas que estan volando. Las que llegan hacen su danio y se borran.
		void moverProyectiles(float dt);
		// Saca del vector a los enemigos que quedaron sin vida.
		void sacarMuertos();
	public:
		Juego();
		// Prepara todo (ventana, imagenes, nivel 1, musica) y corre el game loop hasta que se cierra la ventana.
		void iniciar();
};
