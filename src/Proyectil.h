#pragma once
#include <SFML/Graphics.hpp>
#include "Ataque.h"

// Que proyectil es. Decide como se dibuja (ver el constructor) y como suena al llegar (ver Juego::moverProyectiles).
enum class TipoProyectil {
	BOLA_ENERGIA,  // la del mago
	BOLA_ESPECTRO, // la del espectro: el mismo formato que la del mago, con otros colores
	FLECHA         // la del esqueleto arquero
};

/* Un proyectil que viaja en linea recta hasta la casilla objetivo: la bola del mago, la bola del
*  espectro o la flecha del esqueleto arquero. Al llegar, el Juego aplica el danio (el proyectil solo
*  se mueve y se dibuja). */
class Proyectil {
	private:
		static constexpr float VELOCIDAD = 320.f;        // pixeles del mundo por segundo
		static constexpr float TIEMPO_POR_CUADRO = 0.1f; // cada cuanto cambia el dibujo (animacion)

		TipoProyectil tipo;
		sf::Vector2f posicion, destino;
		sf::Vector2f direccion;     // hacia donde va, con largo 1 (se calcula al crearlo)
		const sf::Texture& textura; // referencia: la textura la guarda el Juego, aca solo se usa
		float tiempoVivo;           // segundos desde que salio, para la animacion
		bool llego;

		// Como esta dibujada la imagen: el tamanio de cada cuadro, cuantos cuadros tiene y si se gira
		// hacia donde va. Salen del tipo (ver el constructor)
		int anchoCuadro, altoCuadro, cantCuadros;
		bool rota;

		int danio;
		int filaObjetivo, columnaObjetivo;

	public:
		Proyectil(TipoProyectil _tipo, sf::Vector2f _origen, sf::Vector2f _destino, const sf::Texture& _textura,
			int _danio, int _filaObjetivo, int _columnaObjetivo);

		// Avanza hacia el destino. dt = segundos que pasaron desde el cuadro anterior.
		void mover(float dt);
		void mostrar(sf::RenderWindow& ventana) const;

		TipoProyectil getTipo() const;
		bool yaLlego() const;
		// Lo que hace al chocar: le pega a la casilla a la que iba, con su danio (el Juego lastima al que este ahi).
		Ataque colisionar() const;
		sf::Vector2f getPosicion() const;
};
