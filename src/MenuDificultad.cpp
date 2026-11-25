#include "MenuDificultad.h"
#include <cmath>

// Colores de la paleta (ver CONTRIBUTING.md)
static const sf::Color FONDO(11, 13, 26);
static const sf::Color PIEDRA(184, 178, 164);
static const sf::Color TENUE(92, 101, 124);
static const sf::Color GEMA(95, 224, 232);

// Medidas (ver "Pantalla de dificultad" en CONTRIBUTING.md)
static const float Y_SUBTITULO = 470.f;    // "Elegi la dificultad"
static const float Y_DESCRIPCION = 652.f;  // que es la dificultad marcada
static const float Y_DETALLE = 684.f;      // que cambia, con los numeros

MenuDificultad::MenuDificultad(const Catalogo& catalogo) {
	// El mismo arte que el Menu: fondo, titulo y selector
	texturaFondo.loadFromFile("Imagenes/menu_fondo.png");
	texturaTitulo.loadFromFile("Imagenes/menu_titulo.png");
	texturaSelector.loadFromFile("Imagenes/menu_selector.png");
	fondo.setTexture(texturaFondo);
	titulo.setTexture(texturaTitulo);
	selector.setTexture(texturaSelector);
	titulo.setPosition(378, 168);

	fuente.loadFromFile("Fuentes/PixelifySans-Regular.ttf");
	fuente.setSmooth(false); // pixel art: sin suavizado se ve nitida
	vista = sf::View(sf::FloatRect(0, 0, 1280, 720));

	// Una opcion por cada dificultad de Dificultades.dat, centradas como las del Menu
	dificultades = catalogo.getDificultades();
	for (size_t i = 0; i < dificultades.size(); i++) {
		sf::Text opcion = crearTexto(dificultades[i].getNombre(), PIEDRA);
		opcion.setOutlineThickness(0.f); // las opciones van sin borde, igual que en el Menu
		sf::FloatRect caja = opcion.getLocalBounds();
		opcion.setOrigin(std::floor(caja.left + caja.width / 2), std::floor(caja.top + caja.height / 2));
		opcion.setPosition(640.f, Y_PRIMERA_OPCION + i * SEPARACION_OPCIONES);
		opciones.push_back(opcion);
	}

	seleccionada = 0;
	setSeleccionada(Catalogo::DIFICULTAD_NORMAL);
}

void MenuDificultad::setSeleccionada(int numero) {
	for (size_t i = 0; i < dificultades.size(); i++) {
		if (dificultades[i].getNumero() == numero) {
			seleccionada = (int)i;
		}
	}
}

int MenuDificultad::procesarEvento(const sf::Event& evento, const sf::RenderWindow& ventana) {
	int cantidad = (int)opciones.size();
	if (cantidad == 0) return NADA;

	if (evento.type == sf::Event::KeyPressed) {
		if (evento.key.code == sf::Keyboard::Down) seleccionada = (seleccionada + 1) % cantidad;
		if (evento.key.code == sf::Keyboard::Up) seleccionada = (seleccionada + cantidad - 1) % cantidad;
		if (evento.key.code == sf::Keyboard::Enter) return dificultades[seleccionada].getNumero();
		if (evento.key.code == sf::Keyboard::Escape) return VOLVER;
	}

	// Mouse encima de una opcion: queda marcada (el pixel se pasa a coordenadas de esta vista, como en el Menu)
	if (evento.type == sf::Event::MouseMoved) {
		sf::Vector2f punto = ventana.mapPixelToCoords(sf::Vector2i(evento.mouseMove.x, evento.mouseMove.y), vista);
		for (int i = 0; i < cantidad; i++) {
			if (opciones[i].getGlobalBounds().contains(punto)) seleccionada = i;
		}
	}

	// Click izquierdo sobre una opcion: se elige esa
	if (evento.type == sf::Event::MouseButtonPressed && evento.mouseButton.button == sf::Mouse::Left) {
		sf::Vector2f punto = ventana.mapPixelToCoords(sf::Vector2i(evento.mouseButton.x, evento.mouseButton.y), vista);
		for (int i = 0; i < cantidad; i++) {
			if (opciones[i].getGlobalBounds().contains(punto)) return dificultades[i].getNumero();
		}
	}

	return NADA;
}

void MenuDificultad::dibujar(sf::RenderWindow& ventana) {
	ventana.setView(vista);
	ventana.draw(fondo);
	ventana.draw(titulo);

	dibujarCentrado(ventana, crearTexto("Elegi la dificultad", GEMA), Y_SUBTITULO);

	// Las opciones: la marcada en Gema, las demas en Piedra (como en el Menu)
	for (size_t i = 0; i < opciones.size(); i++) {
		opciones[i].setFillColor((int)i == seleccionada ? GEMA : PIEDRA);
		ventana.draw(opciones[i]);
	}
	if (opciones.empty()) return;

	// Selector: 14 px a la izquierda del texto marcado, centrado en su altura
	sf::FloatRect caja = opciones[seleccionada].getGlobalBounds();
	selector.setPosition(caja.left - 14.f - 18.f, Y_PRIMERA_OPCION + seleccionada * SEPARACION_OPCIONES - 9.f);
	ventana.draw(selector);

	// Abajo: que es la dificultad marcada y que cambia, con los numeros de Dificultades.dat
	const DatosDificultad& marcada = dificultades[seleccionada];
	dibujarCentrado(ventana, crearTexto(marcada.getDescripcion(), PIEDRA), Y_DESCRIPCION);
	std::string detalle = "Codice " + std::to_string(marcada.getPorcentajeVidaCodice()) + "%   Enemigos: vida "
		+ std::to_string(marcada.getPorcentajeVidaEnemigos()) + "%, danio " + std::to_string(marcada.getPorcentajeDanioEnemigos())
		+ "%   Mana +" + std::to_string(marcada.getManaPorTurno()) + " por turno";
	dibujarCentrado(ventana, crearTexto(detalle, TENUE), Y_DETALLE);

	sf::Text volver = crearTexto("[Esc] Volver", TENUE);
	volver.setPosition(24.f, 24.f);
	ventana.draw(volver);
}

sf::Text MenuDificultad::crearTexto(const std::string& contenido, sf::Color color) const {
	sf::Text texto(contenido, fuente, 22);
	texto.setFillColor(color);
	texto.setOutlineThickness(2.f);
	texto.setOutlineColor(FONDO);
	return texto;
}

void MenuDificultad::dibujarCentrado(sf::RenderWindow& ventana, sf::Text texto, float y) const {
	sf::FloatRect caja = texto.getLocalBounds();
	texto.setOrigin(std::floor(caja.left + caja.width / 2), std::floor(caja.top + caja.height / 2));
	texto.setPosition(640.f, y);
	ventana.draw(texto);
}
