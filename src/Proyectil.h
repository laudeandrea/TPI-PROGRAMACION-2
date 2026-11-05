#pragma once
#include <SFML/graphics.hpp>

/* Lo que tira una habilidad: viaja en linea recta desde el mago hasta la casilla objetivo.
*  Al llegar, el Juego aplica el danio (el proyectil solo se mueve y se dibuja). */
class Proyectil {
	private:
		static constexpr float VELOCIDAD = 320.f;        // pixeles del mundo por segundo
		static constexpr float TIEMPO_POR_CUADRO = 0.1f; // cada cuanto cambia el dibujo (animacion)
		static const int TAMANIO_CUADRO = 16;            // cada cuadro de la imagen mide 16x16

		sf::Vector2f posicion, destino;
		const sf::Texture& textura; // referencia: la textura la guarda el Juego, aca solo se usa
		float tiempoVivo;           // segundos desde que salio, para la animacion
		bool llego;

		int danio;
		int filaObjetivo, columnaObjetivo;

	public:
		Proyectil(sf::Vector2f _origen, sf::Vector2f _destino, const sf::Texture& _textura,
			int _danio, int _filaObjetivo, int _columnaObjetivo);

		/** @brief Avanza hacia el destino. dt = segundos que pasaron desde el cuadro anterior. */
		void actualizar(float dt);
		void mostrar(sf::RenderWindow& ventana) const;

		bool yaLlego() const;
		int getDanio() const;
		int getFilaObjetivo() const;
		int getColumnaObjetivo() const;
		sf::Vector2f getPosicion() const;
};
