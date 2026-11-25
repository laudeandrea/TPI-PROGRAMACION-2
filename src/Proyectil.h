#pragma once
#include <SFML/Graphics.hpp>
#include "Ataque.h"

/* Una bola que viaja en linea recta hasta la casilla objetivo: la tira el mago (Q) o un espectro.
*  Al llegar, el Juego aplica el danio (el proyectil solo se mueve y se dibuja). */
class Proyectil {
	private:
		static constexpr float VELOCIDAD = 320.f;        // pixeles del mundo por segundo
		static constexpr float TIEMPO_POR_CUADRO = 0.1f; // cada cuanto cambia el dibujo (animacion)
		static const int TAMANIO_CUADRO = 16;            // cada cuadro de la imagen mide 16x16

		sf::Vector2f posicion, destino;
		sf::Vector2f direccion;     // hacia donde va, con largo 1 (se calcula al crearla)
		const sf::Texture& textura; // referencia: la textura la guarda el Juego, aca solo se usa
		float tiempoVivo;           // segundos desde que salio, para la animacion
		bool llego;

		int danio;
		int filaObjetivo, columnaObjetivo;

	public:
		Proyectil(sf::Vector2f _origen, sf::Vector2f _destino, const sf::Texture& _textura,
			int _danio, int _filaObjetivo, int _columnaObjetivo);

		// Avanza hacia el destino. dt = segundos que pasaron desde el cuadro anterior.
		void mover(float dt);
		void mostrar(sf::RenderWindow& ventana) const;

		bool yaLlego() const;
		// Lo que hace al chocar: le pega a la casilla a la que iba, con su danio (el Juego lastima al que este ahi).
		Ataque colisionar() const;
		sf::Vector2f getPosicion() const;
};
