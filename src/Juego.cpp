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

		// Click izquierdo mueve al mago a la casilla clickeada si es terreno tipo suelo
		if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left){
			sf::Vector2i pixel(event.mouseButton.x, event.mouseButton.y);
			sf::Vector2f posicionMundo = window.mapPixelToCoords(pixel);
			sf::Vector2i casilla = mapa.mundoAcasilla(posicionMundo);

			if (mapa.esValida(casilla.y, casilla.x)){
				mago.mover(casilla.y, casilla.x);
			}
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
