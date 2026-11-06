#include "Hud.h"
#include <algorithm>
#include <cmath>
#include <string>

// Colores de la paleta (ver CONTRIBUTING.md). Tamanio de letra: siempre 22 (Pixelify Sans solo se ve nitida en 22 u 11)
static const sf::Color FONDO(11, 13, 26);
static const sf::Color PIZARRA(58, 66, 88);
static const sf::Color PIEDRA(184, 178, 164);
static const sf::Color TENUE(92, 101, 124);
static const sf::Color GEMA(95, 224, 232);
static const sf::Color GEMA_OSCURA(46, 140, 166);
static const sf::Color PELIGRO_CLARO(232, 120, 110);

Hud::Hud() {
	fuente.loadFromFile("Fuentes/PixelifySans-Regular.ttf");
	// Sin suavizado: la fuente es pixel art, asi se ve nitida como el resto del juego
	fuente.setSmooth(false);
}

sf::Text Hud::crearTexto(const std::string& contenido, unsigned int tamanio, sf::Color color) const {
	sf::Text texto;
	texto.setFont(fuente);
	texto.setString(contenido);
	texto.setCharacterSize(tamanio);
	texto.setFillColor(color);
	// Borde oscuro alrededor de las letras: se leen sobre cualquier fondo
	texto.setOutlineThickness(2.f);
	texto.setOutlineColor(FONDO);
	return texto;
}

void Hud::agregarDanio(sf::Vector2f posicionMundo, int cantidad) {
	numeros.push_back({ posicionMundo, cantidad, 0.f });
}

void Hud::actualizar(float dt) {
	for (NumeroDanio& numero : numeros) {
		numero.tiempo += dt;
	}
	// Saca los que ya terminaron su animacion (mismo truco que con los enemigos muertos)
	numeros.erase(std::remove_if(numeros.begin(), numeros.end(),
		[](const NumeroDanio& n) { return n.tiempo >= DURACION_NUMERO; }), numeros.end());
}

void Hud::dibujar(sf::RenderWindow& ventana, const sf::View& vistaCamara, const Mago& mago,
	HabilidadSeleccionada habilidadPreparada, bool magoSeleccionado) const {
	ventana.setView(ventana.getDefaultView());
	dibujarNumeros(ventana, vistaCamara);
	dibujarMana(ventana, mago, habilidadPreparada, magoSeleccionado);
}

void Hud::dibujarMana(sf::RenderWindow& ventana, const Mago& mago, HabilidadSeleccionada habilidadPreparada, bool magoSeleccionado) const {
	const float X = 32.f, Y = 612.f;                    // abajo a la izquierda: ahi no esta el tablero
	const float ANCHO_PUNTO = 16.f, ALTO_PUNTO = 12.f, SEPARACION = 3.f;

	bool hayHabilidadPreparada = habilidadPreparada != HabilidadSeleccionada::NINGUNA;
	int costo = hayHabilidadPreparada ? mago.getHabilidad(habilidadPreparada).getCostoMana() : 0;
	bool alcanza = mago.getMana() >= costo;

	// Titulo
	sf::Text titulo = crearTexto("Mana", 22, PIEDRA);
	titulo.setPosition(X, Y - 6.f);
	ventana.draw(titulo);

	// Barra de puntos: un rectangulito por cada punto de mana
	float xBarra = X + 64.f;
	for (int i = 0; i < mago.getManaMaximo(); i++) {
		sf::RectangleShape punto(sf::Vector2f(ANCHO_PUNTO, ALTO_PUNTO));
		punto.setPosition(xBarra + i * (ANCHO_PUNTO + SEPARACION), Y);
		punto.setOutlineThickness(2.f);
		punto.setOutlineColor(FONDO);

		bool lleno = i < mago.getMana();
		// Con la habilidad preparada, los ultimos puntos llenos son los que se van a gastar
		bool seVaAGastar = hayHabilidadPreparada && alcanza && lleno && i >= mago.getMana() - costo;

		if (seVaAGastar) {
			punto.setFillColor(GEMA_OSCURA);
		}
		else if (lleno) {
			punto.setFillColor(GEMA);
		}
		else {
			punto.setFillColor(PIZARRA);
		}
		ventana.draw(punto);
	}

	// Numero: "8/10"
	float xNumero = xBarra + mago.getManaMaximo() * (ANCHO_PUNTO + SEPARACION) + 8.f;
	sf::Text numero = crearTexto(std::to_string(mago.getMana()) + "/" + std::to_string(mago.getManaMaximo()), 22, GEMA);
	numero.setPosition(xNumero, Y - 6.f);
	ventana.draw(numero);

	// Ayuda de la habilidad: que hay, cual esta preparada y que tecla cancela
	std::string ayuda;
	sf::Color colorAyuda = PIEDRA;

	if (magoSeleccionado) {
		ayuda = "Click en una casilla marcada para moverte";
	}
	else if (hayHabilidadPreparada) {
		if (!alcanza) {
			ayuda = "No alcanza el mana";
			colorAyuda = TENUE;
		}
		else {
			ayuda = "Click en un enemigo marcado  -  ["
				+ std::to_string(static_cast<int>(habilidadPreparada)) + "] cancela";
			colorAyuda = GEMA;
		}
	}
	else {
		for (int i = 1; i <= mago.getCantidadHabilidades(); i++) {
			const Habilidad& h = mago.getHabilidad(HabilidadSeleccionada(i));
			if (i > 1) ayuda += "   |   ";
			ayuda += "[" + std::to_string(i) + "] " + h.getNombre()
				+ " - " + std::to_string(h.getCostoMana()) + " mana";
		}
	}


	sf::Text textoAyuda = crearTexto(ayuda, 22, colorAyuda);
	textoAyuda.setPosition(X, Y + 26.f);
	ventana.draw(textoAyuda);
}

void Hud::dibujarNumeros(sf::RenderWindow& ventana, const sf::View& vistaCamara) const {
	for (const NumeroDanio& numero : numeros) {
		float avance = numero.tiempo / DURACION_NUMERO; // de 0 (recien aparece) a 1 (termina)

		// Sube en el mundo, y despues se pasa a la pantalla: mundo -> pixel de la ventana -> coordenada del HUD
		sf::Vector2f mundo(numero.posicionMundo.x, numero.posicionMundo.y - SUBIDA_NUMERO * avance);
		sf::Vector2i pixel = ventana.mapCoordsToPixel(mundo, vistaCamara);
		sf::Vector2f pantalla = ventana.mapPixelToCoords(pixel, ventana.getDefaultView());

		// La segunda mitad se va desvaneciendo (alpha: 255 = opaco, 0 = invisible)
		float opacidad = 1.f;
		if (avance > 0.5f) {
			opacidad = 1.f - (avance - 0.5f) * 2.f;
		}
		sf::Uint8 alpha = (sf::Uint8)(255 * opacidad);

		sf::Text texto = crearTexto("-" + std::to_string(numero.cantidad), 22,
			sf::Color(PELIGRO_CLARO.r, PELIGRO_CLARO.g, PELIGRO_CLARO.b, alpha));
		texto.setOutlineColor(sf::Color(FONDO.r, FONDO.g, FONDO.b, alpha));

		// Centrado en x sobre la posicion (std::floor para que caiga en pixel entero y se vea nitido)
		sf::FloatRect caja = texto.getLocalBounds();
		texto.setOrigin(std::floor(caja.left + caja.width / 2), std::floor(caja.top + caja.height));
		texto.setPosition(std::floor(pantalla.x), std::floor(pantalla.y));
		ventana.draw(texto);
	}
}
