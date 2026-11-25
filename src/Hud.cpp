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
static const sf::Color FUEGO(240, 140, 56); // solo para lo del muro de fuego


// Medidas del panel de la derecha. Empieza donde termina el tablero (el 75 % de la ventana, ver Camara)
static const float X_PANEL = 960.f, ANCHO_PANEL = 320.f;
static const float X = X_PANEL + 24.f;                     // margen izquierdo del contenido
static const float X_DERECHA = X_PANEL + ANCHO_PANEL - 24.f; // donde termina el contenido (margen derecho)
static const float ANCHO_PUNTO = 24.f, ALTO_PUNTO = 12.f, SEPARACION = 3.f;
static const float ANCHO_BARRA = 10 * (ANCHO_PUNTO + SEPARACION) - SEPARACION; // lo mismo que 10 puntos de mana
static const float ALTO_LINEA = 30.f;                      // entre una linea de texto y la siguiente

// De arriba hacia abajo (cada parte separada por una linea)
static const float Y_NIVEL = 24.f;
static const float Y_VIDA = 84.f, Y_CODICE = 144.f, Y_MANA = 204.f; // el titulo; la barra va 32 px mas abajo
static const float Y_HABILIDADES = 284.f;
static const float Y_PRIMERA_HABILIDAD = Y_HABILIDADES + ALTO_LINEA + 4.f;
static const float Y_AYUDA = 480.f; // deja lugar para las 3 habilidades y el boton de terminar turno
static const float Y_MENU = 676.f;

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

void Hud::dibujarTextoDerecha(sf::RenderWindow& ventana, sf::Text texto, float xDerecha, float y) const {
	// getLocalBounds: el tamanio del texto. Se corre a la izquierda todo su ancho, asi termina en xDerecha
	sf::FloatRect caja = texto.getLocalBounds();
	texto.setPosition(std::floor(xDerecha - caja.left - caja.width), y);
	ventana.draw(texto);
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
	const Pergamino& codice, const Nivel& nivel, HabilidadSeleccionada habilidadPreparada, bool magoSeleccionado,
	bool turnoJugador, sf::Vector2f mouse, const std::string& nombreDificultad) const {
	ventana.setView(ventana.getDefaultView());
	dibujarNumeros(ventana, vistaCamara);
	dibujarPanel(ventana);

	// Arriba: en que nivel y en que oleada estamos
	sf::Text textoNivel = crearTexto("Nivel " + std::to_string(nivel.getNumero()) + "/" + std::to_string(nivel.getCantNiveles()), 22, GEMA);
	textoNivel.setPosition(X, Y_NIVEL);
	ventana.draw(textoNivel);
	sf::Text oleada = crearTexto("Oleada " + std::to_string(nivel.getOleada()) + "/" + std::to_string(nivel.getCantOleadas()), 22, PIEDRA);
	dibujarTextoDerecha(ventana, oleada, X_DERECHA, Y_NIVEL);
	dibujarSeparador(ventana, Y_NIVEL + 40.f);

	// Vida del mago, vida del codice y mana
	dibujarBarra(ventana, Y_VIDA, "Vida", mago.getVida(), mago.getVidaMaxima(), PELIGRO);
	dibujarBarra(ventana, Y_CODICE, "Codice", codice.getVida(), codice.getVidaMaxima(), PELIGRO);
	dibujarMana(ventana, mago, habilidadPreparada);
	dibujarSeparador(ventana, Y_HABILIDADES - 16.f);

	dibujarHabilidades(ventana, mago, habilidadPreparada, turnoJugador, mouse);
	dibujarSeparador(ventana, Y_AYUDA - 16.f);

	dibujarAyuda(ventana, mago, habilidadPreparada, magoSeleccionado);

	// Abajo de todo, como volver al menu (Tenue: es secundario)
	sf::Text menu = crearTexto("[Esc] Menu", 22, TENUE);
	menu.setPosition(X, Y_MENU);
	ventana.draw(menu);
	dibujarTextoDerecha(ventana, crearTexto(nombreDificultad, 22, TENUE), X_DERECHA, Y_MENU);
}

void Hud::dibujarPanel(sf::RenderWindow& ventana) const {
	// Fondo casi opaco: el fondo del juego se adivina apenas atras
	sf::RectangleShape panel(sf::Vector2f(ANCHO_PANEL, 720.f));
	panel.setPosition(X_PANEL, 0.f);
	panel.setFillColor(sf::Color(FONDO.r, FONDO.g, FONDO.b, 235));
	ventana.draw(panel);

	// Borde de la izquierda, para separarlo del tablero
	sf::RectangleShape borde(sf::Vector2f(2.f, 720.f));
	borde.setPosition(X_PANEL, 0.f);
	borde.setFillColor(PIZARRA);
	ventana.draw(borde);
}

void Hud::dibujarSeparador(sf::RenderWindow& ventana, float y) const {
	sf::RectangleShape linea(sf::Vector2f(X_DERECHA - X, 2.f));
	linea.setPosition(X, y);
	linea.setFillColor(PIZARRA);
	ventana.draw(linea);
}

void Hud::dibujarBarra(sf::RenderWindow& ventana, float y, const std::string& titulo,
	int actual, int maximo, sf::Color color) const {
	// Arriba: el titulo a la izquierda y "actual/maximo" a la derecha
	sf::Text textoTitulo = crearTexto(titulo, 22, PIEDRA);
	textoTitulo.setPosition(X, y);
	ventana.draw(textoTitulo);
	dibujarTextoDerecha(ventana, crearTexto(std::to_string(actual) + "/" + std::to_string(maximo), 22, PELIGRO_CLARO), X_DERECHA, y);

	// Abajo: fondo vacio y, arriba, el relleno proporcional a lo que queda (igual que las barras de vida de los enemigos)
	float yBarra = y + 32.f;
	sf::RectangleShape fondo(sf::Vector2f(ANCHO_BARRA, ALTO_PUNTO));
	fondo.setPosition(X, yBarra);
	fondo.setFillColor(PIZARRA);
	fondo.setOutlineThickness(2.f);
	fondo.setOutlineColor(FONDO);
	ventana.draw(fondo);

	sf::RectangleShape relleno(sf::Vector2f(ANCHO_BARRA * actual / maximo, ALTO_PUNTO));
	relleno.setPosition(X, yBarra);
	relleno.setFillColor(color);
	ventana.draw(relleno);
}

void Hud::dibujarMana(sf::RenderWindow& ventana, const Mago& mago, HabilidadSeleccionada habilidadPreparada) const {
	// Lo que va a costar la habilidad elegida (0 si no hay ninguna, o si no alcanza el mana: no se va a gastar nada)
	int costo = 0;
	if (habilidadPreparada != HabilidadSeleccionada::NINGUNA && mago.getMana() >= mago.getHabilidad(habilidadPreparada).getCostoMana()) {
		costo = mago.getHabilidad(habilidadPreparada).getCostoMana();
	}

	// Titulo y numero: "Mana ... 8/10"
	sf::Text titulo = crearTexto("Mana", 22, PIEDRA);
	titulo.setPosition(X, Y_MANA);
	ventana.draw(titulo);
	dibujarTextoDerecha(ventana, crearTexto(std::to_string(mago.getMana()) + "/" + std::to_string(mago.getManaMaximo()), 22, GEMA), X_DERECHA, Y_MANA);

	// Barra de puntos: un rectangulito por cada punto de mana
	for (int i = 0; i < mago.getManaMaximo(); i++) {
		sf::RectangleShape punto(sf::Vector2f(ANCHO_PUNTO, ALTO_PUNTO));
		punto.setPosition(X + i * (ANCHO_PUNTO + SEPARACION), Y_MANA + 32.f);
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
}

std::vector<HabilidadSeleccionada> Hud::habilidadesDelMago(const Mago& mago) const {
	std::vector<HabilidadSeleccionada> lista;
	for (int i = 1; i <= static_cast<int>(HabilidadSeleccionada::MURO); i++) { // 1 = bola, 2 = empuje, 3 = muro
		if (mago.tieneHabilidad(HabilidadSeleccionada(i))) {
			lista.push_back(HabilidadSeleccionada(i));
		}
	}
	return lista;
}

sf::FloatRect Hud::cajaHabilidad(int posicion) const {
	// Un poco mas ancha que el texto, para que sea facil apuntarle
	return sf::FloatRect(X - 6.f, Y_PRIMERA_HABILIDAD + posicion * ALTO_LINEA, X_DERECHA - X + 12.f, ALTO_LINEA);
}

sf::FloatRect Hud::cajaTerminarTurno(const Mago& mago) const {
	float y = Y_PRIMERA_HABILIDAD + habilidadesDelMago(mago).size() * ALTO_LINEA + 8.f;
	return sf::FloatRect(X - 6.f, y, X_DERECHA - X + 12.f, ALTO_LINEA + 2.f);
}

HabilidadSeleccionada Hud::habilidadEn(sf::Vector2f punto, const Mago& mago) const {
	std::vector<HabilidadSeleccionada> lista = habilidadesDelMago(mago);
	for (size_t i = 0; i < lista.size(); i++) {
		if (cajaHabilidad((int)i).contains(punto)) {
			return lista[i];
		}
	}
	return HabilidadSeleccionada::NINGUNA;
}

bool Hud::estaSobreTerminarTurno(sf::Vector2f punto, const Mago& mago) const {
	return cajaTerminarTurno(mago).contains(punto);
}

void Hud::dibujarHabilidades(sf::RenderWindow& ventana, const Mago& mago, HabilidadSeleccionada habilidadPreparada,
	bool turnoJugador, sf::Vector2f mouse) const {
	// Encabezado: "Habilidades ... Mana" (la columna de la derecha es lo que cuesta cada una)
	sf::Text titulo = crearTexto("Habilidades", 22, TENUE);
	titulo.setPosition(X, Y_HABILIDADES);
	ventana.draw(titulo);
	dibujarTextoDerecha(ventana, crearTexto("Mana", 22, TENUE), X_DERECHA, Y_HABILIDADES);

	// Una por linea: "[1] Bola de energia ... 3". La preparada va en Gema; las que no alcanza el mana, en Tenue.
	// Se pueden elegir con la tecla o con un click: la que esta debajo del mouse tiene un fondo Pizarra
	std::vector<HabilidadSeleccionada> lista = habilidadesDelMago(mago);
	for (size_t i = 0; i < lista.size(); i++) {
		HabilidadSeleccionada cual = lista[i];
		const Habilidad& habilidad = mago.getHabilidad(cual);
		sf::FloatRect caja = cajaHabilidad((int)i);

		if (turnoJugador && caja.contains(mouse)) {
			sf::RectangleShape resaltado(sf::Vector2f(caja.width, caja.height));
			resaltado.setPosition(caja.left, caja.top);
			resaltado.setFillColor(PIZARRA);
			ventana.draw(resaltado);
		}

		sf::Color color = PIEDRA;
		if (cual == habilidadPreparada) {
			color = GEMA;
		}
		else if (mago.getMana() < habilidad.getCostoMana()) {
			color = TENUE;
		}

		std::string tecla = std::to_string(static_cast<int>(cual));
		sf::Text nombre = crearTexto("[" + tecla + "] " + habilidad.getNombre(), 22, color);
		nombre.setPosition(X, caja.top);
		ventana.draw(nombre);
		dibujarTextoDerecha(ventana, crearTexto(std::to_string(habilidad.getCostoMana()), 22, color), X_DERECHA, caja.top);
	}

	// Abajo, el boton de pasar el turno: un rectangulo con borde (con el mouse encima, se rellena)
	sf::FloatRect caja = cajaTerminarTurno(mago);
	bool encima = turnoJugador && caja.contains(mouse);
	sf::RectangleShape boton(sf::Vector2f(caja.width - 4.f, caja.height - 4.f));
	boton.setPosition(caja.left + 2.f, caja.top + 2.f); // el borde de 2 px va por fuera
	boton.setFillColor(encima ? PIZARRA : FONDO);
	boton.setOutlineThickness(2.f);
	boton.setOutlineColor(PIZARRA);
	ventana.draw(boton);

	sf::Text pasar = crearTexto("[Espacio] Terminar turno", 22, encima ? PIEDRA : TENUE);
	sf::FloatRect cajaTexto = pasar.getLocalBounds();
	// Centrado en el boton
	pasar.setOrigin(std::floor(cajaTexto.left + cajaTexto.width / 2), std::floor(cajaTexto.top + cajaTexto.height / 2));
	pasar.setPosition(std::floor(caja.left + caja.width / 2), std::floor(caja.top + caja.height / 2));
	ventana.draw(pasar);
}

void Hud::dibujarAyuda(sf::RenderWindow& ventana, const Mago& mago, HabilidadSeleccionada habilidadPreparada, bool magoSeleccionado) const {
	// El panel es angosto: los textos largos se cortan en renglones con '\n'
	std::string contenido = "Click en el mago\npara moverlo";
	sf::Color color = TENUE;

	if (magoSeleccionado) {
		contenido = "Click en una casilla\nmarcada para moverte";
		color = PIEDRA;
	}
	else if (habilidadPreparada != HabilidadSeleccionada::NINGUNA) {
		// La tecla es el numero del enum: 1 = bola, 2 = empuje, 3 = muro
		std::string tecla = std::to_string(static_cast<int>(habilidadPreparada));
		if (mago.getHabilidad(habilidadPreparada).getTipo() == TipoHabilidad::MURO) {
			contenido = "Pasa el mouse por una\ncasilla marcada y\nclick para prender\nel fuego\n[" + tecla + "] cancela";
		}
		else {
			contenido = "Click en un enemigo\nmarcado\n[" + tecla + "] cancela";
		}
		color = GEMA;
	}

	sf::Text texto = crearTexto(contenido, 22, color);
	texto.setLineSpacing(1.2f);
	texto.setPosition(X, Y_AYUDA);
	ventana.draw(texto);
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
	if (gano) {
		dibujarCartel(ventana, "VICTORIA", GEMA, motivo, "Enter para volver al menu");
	}
	else {
		dibujarCartel(ventana, "DERROTA", PELIGRO_CLARO, motivo, "Enter para volver al menu");
	}
}

void Hud::dibujarAmenazas(sf::RenderWindow& ventana, const sf::View& vistaCamara,
	const std::vector<Amenaza>& amenazas, const std::vector<sf::Vector2f>& atacantes) const {
	ventana.setView(ventana.getDefaultView());

	// El numero va en el centro de la casilla marcada. Igual que los numeros de danio: mundo -> pixel -> HUD
	for (const Amenaza& amenaza : amenazas) {
		sf::Vector2i pixel = ventana.mapCoordsToPixel(amenaza.posicionMundo, vistaCamara);
		sf::Vector2f pantalla = ventana.mapPixelToCoords(pixel, ventana.getDefaultView());

		sf::Text texto = crearTexto(std::to_string(amenaza.danio), 22, PELIGRO_CLARO);
		sf::FloatRect caja = texto.getLocalBounds();
		texto.setOrigin(std::floor(caja.left + caja.width / 2), std::floor(caja.top + caja.height / 2));
		texto.setPosition(std::floor(pantalla.x), std::floor(pantalla.y));
		ventana.draw(texto);
	}

	// Un "!" arriba de la cabeza de cada enemigo que va a atacar
	for (const sf::Vector2f& posicion : atacantes) {
		sf::Vector2i pixel = ventana.mapCoordsToPixel(posicion, vistaCamara);
		sf::Vector2f pantalla = ventana.mapPixelToCoords(pixel, ventana.getDefaultView());

		sf::Text texto = crearTexto("!", 22, PELIGRO_CLARO);
		sf::FloatRect caja = texto.getLocalBounds();
		texto.setOrigin(std::floor(caja.left + caja.width / 2), std::floor(caja.top + caja.height));
		texto.setPosition(std::floor(pantalla.x), std::floor(pantalla.y));
		ventana.draw(texto);
	}
}

void Hud::dibujarTurnosFuego(sf::RenderWindow& ventana, const sf::View& vistaCamara,
	const std::vector<TurnosFuego>& fuegos) const {
	ventana.setView(ventana.getDefaultView());
	for (const TurnosFuego& fuego : fuegos) {
		// Mundo -> pixel de la ventana -> coordenada del HUD (igual que los numeros de danio)
		sf::Vector2i pixel = ventana.mapCoordsToPixel(fuego.posicionMundo, vistaCamara);
		sf::Vector2f pantalla = ventana.mapPixelToCoords(pixel, ventana.getDefaultView());

		sf::Text texto = crearTexto(std::to_string(fuego.turnos), 22, FUEGO);
		sf::FloatRect caja = texto.getLocalBounds();
		texto.setOrigin(std::floor(caja.left + caja.width / 2), std::floor(caja.top + caja.height / 2));
		texto.setPosition(std::floor(pantalla.x), std::floor(pantalla.y));
		ventana.draw(texto);
	}
}

void Hud::dibujarVolumen(sf::RenderWindow& ventana, int volumen, float opacidad) const {
	ventana.setView(ventana.getDefaultView());
	sf::Uint8 alfa = (sf::Uint8)(255 * opacidad);
	sf::Text texto = crearTexto("Musica: " + std::to_string(volumen), 22, sf::Color(PIEDRA.r, PIEDRA.g, PIEDRA.b, alfa));
	texto.setOutlineColor(sf::Color(FONDO.r, FONDO.g, FONDO.b, alfa));
	texto.setPosition(16.f, 720.f - 16.f - 30.f); // abajo a la izquierda, a 16 px de los bordes
	ventana.draw(texto);
}

void Hud::dibujarVelo(sf::RenderWindow& ventana) const {
	sf::RectangleShape velo(sf::Vector2f(1280.f, 720.f));
	velo.setFillColor(sf::Color(FONDO.r, FONDO.g, FONDO.b, 190));
	ventana.draw(velo);
}

void Hud::dibujarHistoria(sf::RenderWindow& ventana, const std::string& titulo, const std::string& aviso,
	const std::vector<std::string>& lineas, const std::string& instruccion) const {
	ventana.setView(ventana.getDefaultView());
	dibujarVelo(ventana);

	// Panel del medio: 760x380, centrado (de (260, 170) a (1020, 550))
	sf::RectangleShape panel(sf::Vector2f(760.f, 380.f));
	panel.setPosition(260.f, 170.f);
	panel.setFillColor(FONDO);
	panel.setOutlineThickness(2.f);
	panel.setOutlineColor(PIZARRA);
	ventana.draw(panel);

	// Titulo x2 y aviso, centrados en x = 640
	sf::Text textoTitulo = crearTexto(titulo, 22, GEMA);
	textoTitulo.setScale(2.f, 2.f);
	sf::FloatRect caja = textoTitulo.getLocalBounds();
	textoTitulo.setOrigin(std::floor(caja.left + caja.width / 2), std::floor(caja.top + caja.height / 2));
	textoTitulo.setPosition(640.f, 215.f);
	ventana.draw(textoTitulo);

	if (!aviso.empty()) {
		sf::Text textoAviso = crearTexto(aviso, 22, GEMA);
		caja = textoAviso.getLocalBounds();
		textoAviso.setOrigin(std::floor(caja.left + caja.width / 2), std::floor(caja.top + caja.height / 2));
		textoAviso.setPosition(640.f, 262.f);
		ventana.draw(textoAviso);
	}

	// La historia, alineada a la izquierda, una linea cada 30 px
	for (size_t i = 0; i < lineas.size(); i++) {
		sf::Text linea = crearTexto(lineas[i], 22, PIEDRA);
		linea.setPosition(300.f, 290.f + i * ALTO_LINEA);
		ventana.draw(linea);
	}

	sf::Text textoInstruccion = crearTexto(instruccion, 22, TENUE);
	caja = textoInstruccion.getLocalBounds();
	textoInstruccion.setOrigin(std::floor(caja.left + caja.width / 2), std::floor(caja.top + caja.height / 2));
	textoInstruccion.setPosition(640.f, 515.f);
	ventana.draw(textoInstruccion);
}

void Hud::dibujarCartel(sf::RenderWindow& ventana, const std::string& titulo, sf::Color colorTitulo,
	const std::string& detalle, const std::string& instruccion) const {
	ventana.setView(ventana.getDefaultView());

	dibujarVelo(ventana);

	// Panel del medio: 480x200, centrado
	sf::RectangleShape panel(sf::Vector2f(480.f, 200.f));
	panel.setPosition(400.f, 260.f);
	panel.setFillColor(FONDO);
	panel.setOutlineThickness(2.f);
	panel.setOutlineColor(PIZARRA);
	ventana.draw(panel);

	// Tres lineas centradas en x = 640: titulo, detalle y que hacer
	sf::Text textos[3] = {
		crearTexto(titulo, 22, colorTitulo),
		crearTexto(detalle, 22, PIEDRA),
		crearTexto(instruccion, 22, TENUE)
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
