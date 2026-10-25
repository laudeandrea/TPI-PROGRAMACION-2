#include "Juego.h"

// La ventana y la camara se crean en la lista de inicializacion (despues de los ":")
// porque no tienen constructor sin parametros
Juego::Juego()
	: window(sf::VideoMode(ANCHO_VENTANA, ALTO_VENTANA), "El codice del Mago"),
	  mago(4,1),
	  camara(mapa.getTamanioMundo(), MARGEN_CAMARA)
{
	//Inicialización de la ventana
	window.setFramerateLimit(60);

	// Icono de la ventana
	sf::Image icono;
	if (icono.loadFromFile("Imagenes/icono_ventana.png")) {
		window.setIcon(icono.getSize().x, icono.getSize().y, icono.getPixelsPtr());
	};

	// Camara: encuadra el tablero y lo estira para llenar la ventana
	camara.ajustarAVentana(window.getSize());
};

void Juego::ejecutar() {
	//GameLoop (update del juego)
	while (window.isOpen())
	{
		// Read Input (Actualiza los estados de los periféricos de entrada)
		procesarEventos();

		// CMD - Joystick

		/*
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
			personaje.move(0, -velocity);
		};

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
			personaje.move(0, +velocity);
		};

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
			personaje.move(-velocity, 0);
		};

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
			personaje.move(+velocity, 0);
		};
		*/

		// Update - Actualiza los estados del juego

		dibujar();
	}
};

// Leer la cola de mensajes
void Juego::procesarEventos() {
	sf::Event event;
	while (window.pollEvent(event)) {
		if (event.type == sf::Event::Closed) {
			window.close();
		}

		if (event.type == sf::Event::Resized) {
			camara.ajustarAVentana(window.getSize());
		}
	}
};

void Juego::dibujar() {
	window.clear();

	// Draw
	camara.aplicar(window);
	mapa.mostrar(window);
	mago.mostrar(window,mapa);

	// Display / Flip
	window.display();
};
