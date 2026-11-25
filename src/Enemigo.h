#pragma once
#include <vector>
#include "Personaje.h"
#include "Ataque.h"

// Los tipos de enemigo. El numero de cada uno (0, 1, 2) es el que se guarda en los archivos .dat
enum class TipoEnemigo {
	CABALLERO,
	ESPECTRO,
	GOLEM,
	ESQUELETO
};

/* Clase base de los enemigos: lo que comparten todos (vida, danio, dibujarse, el ataque anunciado).
*  Es abstracta (tiene metodos "= 0"): no se puede crear un Enemigo suelto, solo un Caballero, un Espectro o un Golem.
*  Como en el diagrama de clases, cada tipo redefine mover() y atacar() (polimorfismo).
*
*  ATAQUES ANUNCIADOS: el enemigo no pega en el momento. Al final de su turno marca a que casillas va a
*  pegar (anunciarAtaque) y el ataque recien llega en su turno siguiente (atacar). Mientras tanto, el jugador lo
*  ve en rojo y puede salir de ahi, matarlo o empujarlo (empujarlo le cancela el ataque). */
class Enemigo : public Personaje {
	protected:
		int danio;              // el "ataque" del diagrama de clases
		bool tieneAnuncio;      // si ya marco a donde va a pegar
		Ataque ataqueAnunciado; // las casillas marcadas (solo vale si tieneAnuncio)

		bool puedeIrA(const Mapa& mapa, int fila, int columna) const;
		// Devuelve el ataque que habia anunciado y lo borra (ya se esta ejecutando).
		Ataque tomarAtaqueAnunciado();
		// El primer paso del camino mas corto hasta una casilla desde la que le llegue el ataque al objetivo.
		// Rodea lo que no puede pisar y las casillas ocupadas. Devuelve (x = columna, y = fila); si es su casilla, no se mueve.
		sf::Vector2i pasoHacia(const Mapa& mapa, const std::vector<std::vector<bool>>& ocupadas,
			int filaObjetivo, int columnaObjetivo) const;
		// Da ese paso, si la casilla esta libre y la puede pisar. Devuelve si se movio.
		bool darPaso(const Mapa& mapa, const std::vector<std::vector<bool>>& ocupadas, int filaObjetivo, int columnaObjetivo);
	public:
		Enemigo(int _fila, int _columna, int _vida, int _danio);
		// "= 0": cada hija TIENE que decir que tipo es (sirve para guardarlo en Enemigos.dat).
		virtual TipoEnemigo getTipo() const = 0;
		int getDanio() const;

		// Personaje ya tiene un mover(fila, columna) que lo pone en una casilla. Como aca se declara otro mover
		// (con otros parametros), C++ esconderia el de Personaje; con "using" se pueden usar los dos.
		using Personaje::mover;
		// Su turno de caminar: si todavia no le llega el ataque al objetivo, da un paso hacia el.
		// ocupadas[fila][columna] = true si ahi hay otro personaje. Devuelve si se movio. Cada tipo lo redefine.
		virtual bool mover(const Mapa& mapa, const std::vector<std::vector<bool>>& ocupadas,
			int filaObjetivo, int columnaObjetivo) = 0;
		// Ejecuta el ataque que habia anunciado: devuelve a que casillas le pega y cuanto, y borra el anuncio.
		// Cada tipo lo redefine (el caballero golpea, el espectro tira una bola, el golem explota).
		virtual Ataque atacar() = 0;

		// Casilla a la que quiere moverse para acercarse al objetivo en linea recta, como (x = columna, y = fila).
		// Si devuelve su propia casilla, significa que no se mueve. (Se usa si no hay ningun camino libre.)
		sf::Vector2i calcularPaso(const Mapa& mapa, int filaObjetivo, int columnaObjetivo) const;
		// Desde cuantas casillas puede atacar. Por defecto 1: tiene que estar pegado.
		virtual int getAlcanceAtaque() const;
		bool puedeAtacar(int filaObjetivo, int columnaObjetivo) const;
		// Que casillas golpearia si ataca a ese objetivo. Por defecto, solo la del objetivo.
		// No hace nada todavia: el Juego lo anuncia y lo ejecuta en el turno siguiente (atacar).
		virtual Ataque planearAtaque(int filaObjetivo, int columnaObjetivo) const;
		// El ataque que hace al morir. Por defecto ninguno (el golem explota).
		virtual Ataque ataqueAlMorir() const;
		// Si ataca de lejos: el danio llega con una bola que vuela (el Juego la crea), no en el momento.
		virtual bool atacaADistancia() const;

		// --- Ataque anunciado ---
		void anunciarAtaque(const Ataque& ataque);
		void cancelarAtaque();
		bool tieneAtaqueAnunciado() const;
		// Hay que preguntar antes tieneAtaqueAnunciado().
		const Ataque& getAtaqueAnunciado() const;
};
