#pragma once
#include <SFML/Graphics.hpp>
#include "Ataque.h"

/* Un proyectil que viaja en linea recta hasta la casilla objetivo: la bola del mago, la bola del
*  espectro o la flecha del esqueleto arquero. Al llegar, el Juego aplica el danio (el proyectil solo
*  se mueve y se dibuja).
*  La imagen puede ser una tira de cuadros (animacion) o un solo dibujo. Si rota, el dibujo se gira
*  para apuntar hacia donde va (la flecha). */
class Proyectil {
private:
	static constexpr float VELOCIDAD = 320.f;        // pixeles del mundo por segundo
	static constexpr float TIEMPO_POR_CUADRO = 0.1f; // cada cuanto cambia el dibujo (animacion)

	sf::Vector2f posicion, destino;
	sf::Vector2f direccion;     // hacia donde va, con largo 1 (se calcula al crearla)
	const sf::Texture& textura; // referencia: la textura la guarda el Juego, aca solo se usa
	float tiempoVivo;           // segundos desde que salio, para la animacion
	bool llego;

	// Como esta dibujada la imagen (por defecto, la bola: cuadros de 16x16 y 2 cuadros)
	int anchoCuadro, altoCuadro, cantCuadros;
	bool rota;                  // true: se gira para apuntar hacia el destino (la flecha)

	int danio;
	int filaObjetivo, columnaObjetivo;

public:
	Proyectil(sf::Vector2f _origen, sf::Vector2f _destino, const sf::Texture& _textura,
		int _danio, int _filaObjetivo, int _columnaObjetivo,
		int _anchoCuadro = 16, int _altoCuadro = 16, int _cantCuadros = 2, bool _rota = false);

	// Avanza hacia el destino. dt = segundos que pasaron desde el cuadro anterior.
	void mover(float dt);
	void mostrar(sf::RenderWindow& ventana) const;

	bool yaLlego() const;
	// Lo que hace al chocar: le pega a la casilla a la que iba, con su danio (el Juego lastima al que este ahi).
	Ataque colisionar() const;
	sf::Vector2f getPosicion() const;
};