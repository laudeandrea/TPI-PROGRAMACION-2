#include "Juego.h"
#include <algorithm>

// La ventana y la camara se crean en la lista de inicializacion (despues de los ":")
// porque no tienen constructor sin parametros
Juego::Juego()
	: window(sf::VideoMode(ANCHO_VENTANA, ALTO_VENTANA), "El Codice Del Mago"),
	  mago(4,1),
	  codice(5,0,TipoHabilidad::BOLADEFUEGO),
	  camara(mapa.getTamanioMundo(), MARGEN_CAMARA), 
	  estado(EstadoJuego::MENU),
	  fase(FaseTurno::JUGADOR)
{
	enemigos.push_back(std::make_unique<Caballero>(4, 6));
	enemigos.push_back(std::make_unique<Espectro>(1, 3));
	enemigos.push_back(std::make_unique<Golem>(1, 5));
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
		actualizar();
		dibujar();
	}
};

// Leer la cola de mensajes
void Juego::procesarEventos() {
	sf::Event event;
	while (window.pollEvent(event)) {
		// Esto vale siempre, en el menu y jugando
		if (event.type == sf::Event::Closed) {
			window.close();
		}

		if (event.type == sf::Event::Resized) {
			camara.ajustarAVentana(window.getSize());
		}

		// Lo demas depende de en que pantalla estamos
		if (estado == EstadoJuego::MENU) {
			int opcion = menu.procesarEvento(event, window);
			if (opcion == 0) {              // Nueva partida
				estado = EstadoJuego::JUGANDO;
				fase = FaseTurno::JUGADOR;
			}
			if (opcion == 2) {              // Salir
				window.close();
			}
			// opcion 1 ("Como jugar"): todavia no hay pantalla, no hace nada
		}
		else {
			// Escape vuelve al menu (idea de Santiago)
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
				estado = EstadoJuego::MENU;
			}

			// Click izquierdo mueve al mago si el terreno se lo permite y la casilla esta libre
			if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left){
				sf::Vector2i pixel(event.mouseButton.x, event.mouseButton.y);
				sf::Vector2f posicionMundo = window.mapPixelToCoords(pixel);
				sf::Vector2i casilla = mapa.mundoAcasilla(posicionMundo);

				if (fase == FaseTurno::JUGADOR && puedeMover(mago, casilla.y, casilla.x)) {
					mago.mover(casilla.y, casilla.x);
					fase = FaseTurno::ENEMIGOS;
				}
			}
		}
	}
	
};

bool Juego::casillaOcupada(int fila, int columna) const {
	if (mago.estaEn(fila, columna)) return true;
	for (const auto& enemigo : enemigos) {
		if (enemigo->estaEn(fila, columna)) return true;
	}
	return false;
};

bool Juego::puedeMover(const Personaje& quien, int fila, int columna) const {
	if (!mapa.estaDentro(fila, columna)) {
		return false;
	}
	if (!quien.puedePisar(mapa.getTipoTerreno(fila, columna))) {
		return false;
	}
	return !casillaOcupada(fila, columna);
};

void Juego::dibujar() {
	window.clear();

	if(estado == EstadoJuego::MENU){
		menu.dibujar(window);
	}else{
		camara.aplicar(window);
		mapa.mostrar(window);
		/* personajes ordenados por profundidad, lo de adelante se dibuja ultimo */
		std::vector<Personaje*> personajes;
		personajes.push_back(&mago);
		for (const auto& enemigo : enemigos) personajes.push_back(enemigo.get());

		std::sort(personajes.begin(), personajes.end(),
			[](const Personaje* a, const Personaje* b) {
				return (a->getFila() + a->getColumna()) < (b->getFila() + b->getColumna());
			});

		for (const Personaje* p : personajes) p->mostrar(window, mapa);
		codice.mostrar(window,mapa);
	}

	window.display();
};

void Juego::actualizar() {
	if (estado != EstadoJuego::JUGANDO || fase != FaseTurno::ENEMIGOS) return;
	for (auto& enemigo : enemigos) {
		if (!enemigo->estaVivo()) continue;

		sf::Vector2i destino = enemigo->calcularPaso(mapa, mago);

		bool quiereMoverse = !enemigo->estaEn(destino.y, destino.x);
		if (quiereMoverse && puedeMover(*enemigo, destino.y, destino.x)) {
			enemigo->mover(destino.y, destino.x);
		}
	}
	fase = FaseTurno::JUGADOR;
}
