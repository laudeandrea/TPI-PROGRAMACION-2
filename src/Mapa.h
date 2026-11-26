#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "Casilla.h"

class Personaje; // "declaracion adelantada": Mapa::mover recibe un Personaje, y Personaje.h ya incluye Mapa.h

/* El tablero de 8x8: que terreno tiene cada casilla y como se dibuja en vista isometrica.
*  Tambien hace las cuentas entre casillas (fila, columna) y pixeles del mundo (casillaAmundo y mundoAcasilla).
*  Dibuja en coordenadas del mundo: la Camara lo acomoda en la ventana. */
class Mapa {
	private:
		static const int CANT_CASILLAS = 8; // static: la constante es una sola para toda la clase, no una por cada Mapa
		static constexpr float ANCHO_CASILLAS = 64.f; // constexpr: C++ solo deja inicializar asi un static const entero
		static constexpr float ALTO_CASILLAS = 40.f;
		static constexpr float CENTRO_ROMBO_DESDE_ABAJO = 34.f;  // del fondo de la imagen al centro de la cara de arriba
		static constexpr float CENTRO_ROMBO_DESDE_ARRIBA = 50.f; // las paredes se apoyan en (32, 50)
		static const int CANT_VARIANTES_SUELO = 4;               // lisa, con grieta, con musgo, con runa
		static const int CUADROS_FUEGO = 3;                      // la imagen del muro tiene 3 cuadros uno al lado del otro
		static constexpr float TIEMPO_POR_CUADRO = 0.15f;        // segundos que se ve cada cuadro del fuego
		Casilla casillas[CANT_CASILLAS][CANT_CASILLAS];
		int variantesSuelo[CANT_CASILLAS][CANT_CASILLAS]; // que dibujo de suelo le toca a cada casilla
		float tiempoFuego; // segundos que pasaron, para saber que cuadro del fuego mostrar

		sf::Texture texturasSuelo[CANT_VARIANTES_SUELO]; // solo cambia el dibujo: todas son TipoTerreno::SUELO
		sf::Texture texturaColumna;
		sf::Texture texturaAbismo;
		sf::Texture texturaMuro;           // muro de fuego animado
		sf::Texture texturaPozoIzquierda;  // pared interna del pozo (cuando adelante hay abismo)
		sf::Texture texturaPozoDerecha;
		sf::Texture texturaParedIzquierda; // roca que cuelga bajo el borde de abajo-izquierda de la casilla
		sf::Texture texturaParedDerecha;   // roca que cuelga bajo el borde de abajo-derecha de la casilla

		// Dibuja una pared (roca o pozo) colgando bajo esa casilla.
		void dibujarPared(sf::RenderWindow& ventana, const sf::Texture& textura, int fila, int columna);
	public:
		Mapa();
		// Arma el tablero a partir de un dibujo: 8 strings de 8 letras (S = suelo, C = columna rota, X = abismo).
		// Cada nivel tiene el suyo (ver Nivel::getDisenio).
		void cargar(const std::vector<std::string>& disenio);
		// Hace avanzar la animacion del fuego. dt = segundos desde el cuadro anterior.
		void actualizar(float dt);
		// Dibuja todas las casillas, de atras (fila 0) hacia adelante, con sus paredes.
		void mostrar(sf::RenderWindow& ventana);
		// El centro de la casilla, en pixeles del mundo. Es LA cuenta para saber donde dibujar algo.
		sf::Vector2f casillaAmundo(int fila, int columna) const;
		// Al reves: en que casilla cae un punto del mundo, como (x = columna, y = fila). Puede caer fuera del tablero.
		sf::Vector2i mundoAcasilla(sf::Vector2f posicion) const;
		// Lo que ocupa el tablero en el mundo (la Camara lo usa para encuadrarlo).
		sf::Vector2f getTamanioMundo() const;
		// Cuantas casillas tiene cada lado del tablero (8).
		int getCantCasillas() const;

		// Si la fila y la columna caen dentro del tablero (si la casilla existe). No mira el terreno.
		bool esValida(int _fila, int _columna) const;
		// Mueve al personaje a esa casilla si existe y la puede pisar (el espectro vuela, los demas no).
		// No mira si hay otro personaje ahi: eso lo sabe el que llama. Devuelve si lo movio.
		bool mover(Personaje& personaje, int _fila, int _columna) const;
		// La casilla en esa fila y columna. Hay que llamar antes a esValida().
		const Casilla& getCasilla(int _fila, int _columna) const;
		// Terreno de una casilla. Hay que llamar antes a esValida().
		TipoTerreno getTipoTerreno(int _fila, int _columna) const;
		// Cambia el terreno de una casilla. Hay que llamar antes a esValida().
		void setTipoTerreno(int _fila, int _columna, TipoTerreno _tipo);
};
