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
static const sf::Color PELIGRO(176, 74, 74);
static const sf::Color PELIGRO_CLARO(232, 120, 110);

// La tecla de cada habilidad, en el mismo orden que el vector del mago (tienen que coincidir con Juego::procesarEventos)
static const std::string TECLAS[] = { "Q", "W" };

// Medidas del HUD de abajo a la izquierda (ahi no esta el tablero)
static const float X = 32.f;
static const float X_BARRA = X + 88.f;       // todas las barras empiezan en la misma x, despues del titulo mas largo ("Codice")
static const float Y_VIDA = 548.f, Y_CODICE = 580.f, Y_MANA = 612.f, Y_AYUDA = 638.f;
static const float ANCHO_PUNTO = 16.f, ALTO_PUNTO = 12.f, SEPARACION = 3.f;

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

void Hud::limpiar() {
	numeros.clear();
}

void Hud::dibujar(sf::RenderWindow& ventana, const sf::View& vistaCamara, const Mago& mago,
	const Pergamino& codice, const Nivel& nivel, int habilidadPreparada, bool magoSeleccionado) const {
	ventana.setView(ventana.getDefaultView());
	dibujarNumeros(ventana, vistaCamara);

	// Arriba a la izquierda: en que oleada estamos
	sf::Text oleada = crearTexto("Oleada " + std::to_string(nivel.getOleada()) + "/" + std::to_string(nivel.getCantOleadas()), 22, PIEDRA);
	oleada.setPosition(X, 24.f);
	ventana.draw(oleada);

	// Abajo a la izquierda: vida del mago, vida del codice, mana y la ayuda
	dibujarBarra(ventana, Y_VIDA, "Vida", mago.getVida(), mago.getVidaMaxima(), PELIGRO);
	dibujarBarra(ventana, Y_CODICE, "Codice", codice.getVida(), codice.getVidaMaxima(), PELIGRO);
	dibujarMana(ventana, mago, habilidadPreparada);
	dibujarAyuda(ventana, mago, habilidadPreparada, magoSeleccionado);
}

void Hud::dibujarBarra(sf::RenderWindow& ventana, float y, const std::string& titulo,
	int actual, int maximo, sf::Color color) const {
	// Mismo ancho que la barra de mana (10 puntos), asi quedan alineadas
	const float ANCHO = 10 * (ANCHO_PUNTO + SEPARACION) - SEPARACION;

	sf::Text textoTitulo = crearTexto(titulo, 22, PIEDRA);
	textoTitulo.setPosition(X, y - 6.f);
	ventana.draw(textoTitulo);

	// Fondo vacio y, arriba, el relleno proporcional a lo que queda (igual que las barras de vida de los enemigos)
	sf::RectangleShape fondo(sf::Vector2f(ANCHO, ALTO_PUNTO));
	fondo.setPosition(X_BARRA, y);
	fondo.setFillColor(PIZARRA);
	fondo.setOutlineThickness(2.f);
	fondo.setOutlineColor(FONDO);
	ventana.draw(fondo);

	sf::RectangleShape relleno(sf::Vector2f(ANCHO * actual / maximo, ALTO_PUNTO));
	relleno.setPosition(X_BARRA, y);
	relleno.setFillColor(color);
	ventana.draw(relleno);

	sf::Text numero = crearTexto(std::to_string(actual) + "/" + std::to_string(maximo), 22, PELIGRO_CLARO);
	numero.setPosition(X_BARRA + ANCHO + 8.f, y - 6.f);
	ventana.draw(numero);
}

void Hud::dibujarMana(sf::RenderWindow& ventana, const Mago& mago, int habilidadPreparada) const {
	// Lo que va a costar la habilidad elegida (0 si no hay ninguna, o si no alcanza el mana: no se va a gastar nada)
	int costo = 0;
	if (habilidadPreparada != -1 && mago.getMana() >= mago.getHabilidad(habilidadPreparada).getCostoMana()) {
		costo = mago.getHabilidad(habilidadPreparada).getCostoMana();
	}

	// Titulo
	sf::Text titulo = crearTexto("Mana", 22, PIEDRA);
	titulo.setPosition(X, Y_MANA - 6.f);
	ventana.draw(titulo);

	// Barra de puntos: un rectangulito por cada punto de mana
	for (int i = 0; i < mago.getManaMaximo(); i++) {
		sf::RectangleShape punto(sf::Vector2f(ANCHO_PUNTO, ALTO_PUNTO));
		punto.setPosition(X_BARRA + i * (ANCHO_PUNTO + SEPARACION), Y_MANA);
		punto.setOutlineThickness(2.f);
		punto.setOutlineColor(FONDO);

		bool lleno = i < mago.getMana();
		// Con una habilidad preparada, los ultimos puntos llenos son los que se van a gastar
		bool seVaAGastar = lleno && i >= mago.getMana() - costo;

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
	float xNumero = X_BARRA + mago.getManaMaximo() * (ANCHO_PUNTO + SEPARACION) + 8.f;
	sf::Text numero = crearTexto(std::to_string(mago.getMana()) + "/" + std::to_string(mago.getManaMaximo()), 22, GEMA);
	numero.setPosition(xNumero, Y_MANA - 6.f);
	ventana.draw(numero);
}

void Hud::dibujarAyuda(sf::RenderWindow& ventana, const Mago& mago, int habilidadPreparada, bool magoSeleccionado) const {
	// Con el mago seleccionado, la ayuda explica como moverse
	if (magoSeleccionado) {
		sf::Text texto = crearTexto("Click en una casilla marcada para moverte", 22, PIEDRA);
		texto.setPosition(X, Y_AYUDA);
		ventana.draw(texto);
		return;
	}

	// Con una habilidad preparada, explica como tirarla y como cancelarla
	if (habilidadPreparada != -1) {
		sf::Text texto = crearTexto("Click en un enemigo marcado  -  " + TECLAS[habilidadPreparada] + " cancela", 22, GEMA);
		texto.setPosition(X, Y_AYUDA);
		ventana.draw(texto);
		return;
	}

	// Si no, la lista de habilidades una al lado de la otra: "[Q] Bola de energia (2)".
	// Las que no alcanza el mana van en Tenue
	float x = X;
	for (int i = 0; i < mago.getCantHabilidades(); i++) {
		const Habilidad& habilidad = mago.getHabilidad(i);
		bool alcanza = mago.getMana() >= habilidad.getCostoMana();

		std::string contenido = "[" + TECLAS[i] + "] " + habilidad.getNombre() + " (" + std::to_string(habilidad.getCostoMana()) + ")";
		sf::Text texto = crearTexto(contenido, 22, alcanza ? PIEDRA : TENUE);
		texto.setPosition(x, Y_AYUDA);
		ventana.draw(texto);

		x += texto.getLocalBounds().width + 24.f; // la proxima va a la derecha, con un espacio
	}

	// Al final, como pasar el turno (Tenue: es secundario)
	sf::Text pasar = crearTexto("[Espacio] Pasar turno", 22, TENUE);
	pasar.setPosition(x, Y_AYUDA);
	ventana.draw(pasar);
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

void Hud::dibujarFinDePartida(sf::RenderWindow& ventana, bool gano, const std::string& motivo) const {
	ventana.setView(ventana.getDefaultView());

	// Oscurece todo lo de atras (el tablero se sigue viendo, apagado)
	sf::RectangleShape velo(sf::Vector2f(1280.f, 720.f));
	velo.setFillColor(sf::Color(FONDO.r, FONDO.g, FONDO.b, 190));
	ventana.draw(velo);

	// Panel del medio: 480x200, centrado
	sf::RectangleShape panel(sf::Vector2f(480.f, 200.f));
	panel.setPosition(400.f, 260.f);
	panel.setFillColor(FONDO);
	panel.setOutlineThickness(2.f);
	panel.setOutlineColor(PIZARRA);
	ventana.draw(panel);

	// Tres lineas centradas en x = 640: titulo, motivo y que hacer
	std::string titulo = gano ? "VICTORIA" : "DERROTA";
	sf::Color colorTitulo = gano ? GEMA : PELIGRO_CLARO;
	sf::Text textos[3] = {
		crearTexto(titulo, 22, colorTitulo),
		crearTexto(motivo, 22, PIEDRA),
		crearTexto("Enter para volver al menu", 22, TENUE)
	};
	// El titulo se agranda x2 con setScale (y no con tamanio 44): asi los pixeles de la letra se duplican y sigue nitida
	textos[0].setScale(2.f, 2.f);

	const float Y_LINEAS[3] = { 310.f, 370.f, 420.f };
	for (int i = 0; i < 3; i++) {
		sf::FloatRect caja = textos[i].getLocalBounds();
		textos[i].setOrigin(std::floor(caja.left + caja.width / 2), std::floor(caja.top + caja.height / 2));
		textos[i].setPosition(640.f, Y_LINEAS[i]);
		ventana.draw(textos[i]);
	}
}
