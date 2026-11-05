#include "Proyectil.h"
#include <cmath>

// La textura es una referencia, y las referencias SOLO se pueden inicializar en la lista de inicializacion
Proyectil::Proyectil(sf::Vector2f _origen, sf::Vector2f _destino, const sf::Texture& _textura,
	int _danio, int _filaObjetivo, int _columnaObjetivo)
	: posicion(_origen), destino(_destino), textura(_textura), tiempoVivo(0.f), llego(false),
	  danio(_danio), filaObjetivo(_filaObjetivo), columnaObjetivo(_columnaObjetivo)
{
}

void Proyectil::actualizar(float dt) {
	tiempoVivo += dt;

	// Cuanto falta y para que lado
	sf::Vector2f falta = destino - posicion;
	float distancia = std::sqrt(falta.x * falta.x + falta.y * falta.y);
	float paso = VELOCIDAD * dt; // cuanto avanza en este cuadro

	if (paso >= distancia) {
		// Si en este cuadro llegaria (o se pasaria), lo deja justo en el destino
		posicion = destino;
		llego = true;
	}
	else {
		// falta / distancia = la direccion con largo 1; por paso = avanza "paso" pixeles hacia alla
		posicion += falta / distancia * paso;
	}
}

void Proyectil::mostrar(sf::RenderWindow& ventana) const {
	sf::Sprite sprite(textura);

	// La imagen tiene 2 cuadros uno al lado del otro: se muestra uno u otro segun el tiempo
	int cuadro = (int)(tiempoVivo / TIEMPO_POR_CUADRO) % 2;
	sprite.setTextureRect(sf::IntRect(cuadro * TAMANIO_CUADRO, 0, TAMANIO_CUADRO, TAMANIO_CUADRO));

	sprite.setOrigin(TAMANIO_CUADRO / 2.f, TAMANIO_CUADRO / 2.f); // el centro de la bola
	sprite.setPosition(posicion);
	ventana.draw(sprite);
}

bool Proyectil::yaLlego() const {
	return llego;
}

int Proyectil::getDanio() const {
	return danio;
}

int Proyectil::getFilaObjetivo() const {
	return filaObjetivo;
}

int Proyectil::getColumnaObjetivo() const {
	return columnaObjetivo;
}

sf::Vector2f Proyectil::getPosicion() const {
	return posicion;
}
