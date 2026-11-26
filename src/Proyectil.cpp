#include "Proyectil.h"
#include <cmath>

// La textura es una referencia, y las referencias SOLO se pueden inicializar en la lista de inicializacion.
// Los atributos van en el mismo orden que en el .h: C++ los inicializa en ese orden, no en el de esta lista
Proyectil::Proyectil(TipoProyectil _tipo, sf::Vector2f _origen, sf::Vector2f _destino, const sf::Texture& _textura,
	int _danio, int _filaObjetivo, int _columnaObjetivo)
	: tipo(_tipo), posicion(_origen), destino(_destino), textura(_textura), tiempoVivo(0.f), llego(false),
	  anchoCuadro(16), altoCuadro(16), cantCuadros(2), rota(false),
	  danio(_danio), filaObjetivo(_filaObjetivo), columnaObjetivo(_columnaObjetivo) {
	// Las bolas (la del mago y la del espectro) son 2 cuadros de 16x16 uno al lado del otro: los valores de arriba.
	// La flecha es un solo dibujo de 36x14 que apunta a la derecha: se gira hacia donde vuela (ver mostrar)
	if (tipo == TipoProyectil::FLECHA) {
		anchoCuadro = 36;
		altoCuadro = 14;
		cantCuadros = 1;
		rota = true;
	}

	// La direccion es el camino completo dividido por su largo: queda un "paso" de largo 1 hacia el destino
	sf::Vector2f camino = destino - posicion;
	float largo = std::sqrt(camino.x * camino.x + camino.y * camino.y);
	if (largo > 0.f) {
		direccion = camino / largo;
	}
	else {
		llego = true; // ya estaba en el destino
	}
}

void Proyectil::mover(float dt) {
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
		// direccion tiene largo 1: por paso = avanza "paso" pixeles hacia alla
		posicion += direccion * paso;
	}
}

void Proyectil::mostrar(sf::RenderWindow& ventana) const {
	sf::Sprite sprite(textura);

	// Si la imagen tiene varios cuadros, se muestra uno u otro segun el tiempo
	int cuadro = 0;
	if (cantCuadros > 1) {
		cuadro = (int)(tiempoVivo / TIEMPO_POR_CUADRO) % cantCuadros;
	}
	sprite.setTextureRect(sf::IntRect(cuadro * anchoCuadro, 0, anchoCuadro, altoCuadro));

	sprite.setOrigin(anchoCuadro / 2.f, altoCuadro / 2.f); // el centro del dibujo
	sprite.setPosition(posicion);

	// La flecha se gira para apuntar hacia donde va (la imagen apunta a la derecha)
	if (rota) {
		float grados = std::atan2(direccion.y, direccion.x) * 180.f / 3.14159265f;
		sprite.setRotation(grados);
	}

	ventana.draw(sprite);
}

TipoProyectil Proyectil::getTipo() const {
	return tipo;
}

bool Proyectil::yaLlego() const {
	return llego;
}

Ataque Proyectil::colisionar() const {
	Ataque golpe;
	golpe.casillas.push_back(sf::Vector2i(columnaObjetivo, filaObjetivo)); // x = columna, y = fila
	golpe.danio = danio;
	return golpe;
}

sf::Vector2f Proyectil::getPosicion() const {
	return posicion;
}
