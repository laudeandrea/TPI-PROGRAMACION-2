#pragma once
#include <SFML/graphics.hpp>
#include "Casilla.h"

class Mapa {
	private:
		static const int CANT_CASILLAS = 8; // static const = La constante es una sola para toda la clase, en vez de una copia por cada objeto Mapa
		static constexpr float ANCHO_CASILLAS = 64.f; // (constexpr: porque C++ solo permite inicializar static const SOLO si es entero)
		static constexpr float ALTO_CASILLAS = 40.f;
		static constexpr float CENTRO_ROMBO_DESDE_ABAJO = 34.f; // La distancia desde el fondo de la imagen hasta el centro de la cara de arriba
		static constexpr float CENTRO_ROMBO_DESDE_ARRIBA = 50.f; // Las paredes se apoyan en (32, 50)
		static const int CANT_VARIANTES_SUELO = 4; // lisa, con grieta, con musgo, con runa
		Casilla casillas[CANT_CASILLAS][CANT_CASILLAS];
		int variantesSuelo[CANT_CASILLAS][CANT_CASILLAS]; // Que dibujo de suelo le toca a cada casilla

		sf::Texture texturasSuelo[CANT_VARIANTES_SUELO]; // Solo cambia el dibujo: todas son TipoTerreno::SUELO
		sf::Texture texturaColumna;
		sf::Texture texturaAbismo;
		sf::Texture texturaPozoIzquierda; // Pared interna del pozo (cuando adelante hay abismo)
		sf::Texture texturaPozoDerecha;
		sf::Texture texturaParedIzquierda; // Roca que cuelga bajo el borde de abajo-izquierda de la casilla
		sf::Texture texturaParedDerecha;   // Roca que cuelga bajo el borde de abajo-derecha de la casilla

		void dibujarPared(sf::RenderWindow& ventana, const sf::Texture& textura, int fila, int columna);
	public:
		Mapa();
		void mostrar(sf::RenderWindow& ventana);
		sf::Vector2f casillaAmundo(int fila, int columna) const;
		sf::Vector2i mundoAcasilla(sf::Vector2f posicion) const;
		sf::Vector2f getTamanioMundo() const;
		/** @brief Cuantas casillas tiene cada lado del tablero (8). */
		int getCantCasillas() const;
		
		/*Si la fila y la columna caen dentro del tablero. No mira el terreno. */
		bool estaDentro(int _fila, int _columna) const;	
		/*Terreno de una casilla. Hay que llamar antes a estaDentro(). */
		TipoTerreno getTipoTerreno(int _fila, int _columna) const;
		/*Cambia el terreno de una casilla. Hay que llamar antes a estaDentro().*/
		void setTipoTerreno(int _fila, int _columna, TipoTerreno _tipo);
};