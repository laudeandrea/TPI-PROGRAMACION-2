#include "Menu.h"
#include <string>
#include <cmath>

Menu::Menu() {
	// Carga las imagenes
	texturaFondo.loadFromFile("Imagenes/menu_fondo.png");
	texturaSelector.loadFromFile("Imagenes/menu_selector.png");
	texturaTitulo.loadFromFile("Imagenes/menu_titulo.png");
	fuente.loadFromFile("Fuentes/PixelifySans-Regular.ttf");

	// Asigna cada textura a su sprite y lo ubica
	fondo.setTexture(texturaFondo);
	selector.setTexture(texturaSelector);
	titulo.setTexture(texturaTitulo);

	titulo.setPosition(378, 168);

	// Arma la vista fija
	vista = sf::View(sf::FloatRect(0, 0, 1280, 720));

	// Se inicializan asi no arrancan con un valor basura
	seleccionada = 0;
	hayPartida = false;

	// Textos de las opciones (sin tildes, asi SFML los muestra bien sin convertirlos).
	// OJO: el orden tiene que coincidir con los indices que usa Juego:
	// 0 = Continuar, 1 = Nueva partida, 2 = Como jugar, 3 = Salir.
	std::string nombres[4] = { "Continuar", "Nueva partida", "Como jugar", "Salir" };

	for (int i = 0; i < 4; i++) {
		opciones[i].setFont(fuente);
		opciones[i].setString(nombres[i]);
		opciones[i].setCharacterSize(22);
		opciones[i].setFillColor(sf::Color(184, 178, 164)); // Piedra

		// El origen va al medio del texto, asi setPosition ubica su centro
		sf::FloatRect caja = opciones[i].getLocalBounds();
		// std:: floor = redondea para abajo, saca los decimales
		opciones[i].setOrigin(std::floor(caja.left + caja.width / 2),
							std::floor(caja.top + caja.height / 2));
		opciones[i].setPosition(640.f, Y_PRIMERA_OPCION + i * SEPARACION_OPCIONES);
	}
}

int Menu::procesarEvento(const sf::Event& evento, const sf::RenderWindow& ventana) {
	if (evento.type == sf::Event::KeyPressed) {
		if (evento.key.code == sf::Keyboard::Down) {
			do {
				seleccionada = (seleccionada + 1) % 4;
			} while (!hayPartida && seleccionada == 0);  // no se puede elegir "Continuar"
		}
		if (evento.key.code == sf::Keyboard::Up) {
			do {
				seleccionada = (seleccionada + 3) % 4;
			} while (!hayPartida && seleccionada == 0);
		}
		if (evento.key.code == sf::Keyboard::Enter) {
			if (seleccionada == 0 && !hayPartida) {
				return -1; // apretó Enter sobre "Continuar" deshabilitada
			}
			return seleccionada;
		}
	}

	// Mouse encima de una opcion: queda elegida
	if (evento.type == sf::Event::MouseMoved) {
		// El pixel de la ventana se pasa a coordenadas del menu (sirve aunque la ventana cambie de tamaño)
		sf::Vector2i pixel(evento.mouseMove.x, evento.mouseMove.y);
		sf::Vector2f punto = ventana.mapPixelToCoords(pixel, vista);

		for (int i = 0; i < 4; i++) {
			if (i == 0 && !hayPartida) continue;   // "Continuar" no reacciona
			if (opciones[i].getGlobalBounds().contains(punto)) {
				seleccionada = i;
			}
		}
	}

	// Click izquierdo sobre una opcion: se elige esa
	if (evento.type == sf::Event::MouseButtonPressed && evento.mouseButton.button == sf::Mouse::Left) {
		sf::Vector2i pixel(evento.mouseButton.x, evento.mouseButton.y);
		sf::Vector2f punto = ventana.mapPixelToCoords(pixel, vista);

		for (int i = 0; i < 4; i++) {
			if (i == 0 && !hayPartida) continue;   // "Continuar" deshabilitada
			if (opciones[i].getGlobalBounds().contains(punto)) {
				return i;
			}
		}
	}

	return -1;                                         // no se eligió nada
}

void Menu::dibujar(sf::RenderWindow& ventana) {
	ventana.setView(vista);
	ventana.draw(fondo);
	ventana.draw(titulo);

	// Dibuja las opciones: la elegida en Gema, las demas en Piedra
	for (int i = 0; i < 4; i++) {
		if (i == 0 && !hayPartida) {
			opciones[i].setFillColor(sf::Color(90, 86, 80));     // Gris apagado: deshabilitada
		}
		else if (i == seleccionada) {
			opciones[i].setFillColor(sf::Color(95, 224, 232));  // Gema
		}
		else {
			opciones[i].setFillColor(sf::Color(184, 178, 164)); // Piedra
		}
		ventana.draw(opciones[i]);
	}

	// Selector: 14 px a la izquierda del texto elegido, centrado en su altura
	sf::FloatRect caja = opciones[seleccionada].getGlobalBounds();
	selector.setPosition(caja.left - 14.f - 18.f, Y_PRIMERA_OPCION + seleccionada * SEPARACION_OPCIONES - 9.f);
	ventana.draw(selector);
}

void Menu::setHayPartida(bool hay) {
	// Si aparecio una partida guardada, deja elegida "Continuar"
	if (hay && !hayPartida) {
		seleccionada = 0;
	}
	// Si ya no hay partida y el selector estaba en "Continuar", lo manda a "Nueva partida"
	if (!hay && seleccionada == 0) {
		seleccionada = 1;
	}
	hayPartida = hay;
}
