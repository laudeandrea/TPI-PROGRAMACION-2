#include "Mapa.h"
#include "Personaje.h"
#include <cmath>
#include <cstdlib>
#include <string> // std::to_string, para armar el nombre de cada archivo de suelo

// El constructor solo carga las imagenes (una vez). El tablero lo arma cargar(), que se llama al empezar cada nivel.
// Mientras no se llame a cargar(), todas las casillas son SUELO (es el valor por defecto de Casilla).
Mapa::Mapa() : tiempoFuego(0.f) {
	// Variantes de suelo: suelo_1_iso.png ... suelo_4_iso.png (el numero del archivo es i + 1)
	for (int i = 0; i < CANT_VARIANTES_SUELO; i++) {
		texturasSuelo[i].loadFromFile("Imagenes/suelo_" + std::to_string(i + 1) + "_iso.png");
	}

	texturaColumna.loadFromFile("Imagenes/columna_rota_iso.png");
	texturaAbismo.loadFromFile("Imagenes/abismo_iso.png");
	texturaMuro.loadFromFile("Imagenes/muro_fuego_animacion.png");
	texturaParedIzquierda.loadFromFile("Imagenes/pared_izquierda_iso.png");
	texturaParedDerecha.loadFromFile("Imagenes/pared_derecha_iso.png");
	texturaPozoIzquierda.loadFromFile("Imagenes/pozo_izquierda_iso.png");
	texturaPozoDerecha.loadFromFile("Imagenes/pozo_derecha_iso.png");

	for (int fila = 0; fila < CANT_CASILLAS; fila++) {
		for (int columna = 0; columna < CANT_CASILLAS; columna++) {
			variantesSuelo[fila][columna] = 0;
		}
	}
}

void Mapa::cargar(const std::vector<std::string>& disenio) {
	// disenio[fila][columna]: con un vector de strings se indexa igual que con la matriz de char de antes
	for (int fila = 0; fila < CANT_CASILLAS; fila++) {
		for (int columna = 0; columna < CANT_CASILLAS; columna++) {
			switch (disenio[fila][columna]) {

			case 'C':
				casillas[fila][columna].setTipoTerreno(TipoTerreno::COLUMNAROTA);
				break;
			case 'X':
				casillas[fila][columna].setTipoTerreno(TipoTerreno::ABISMO);
				break;
			default:
				casillas[fila][columna].setTipoTerreno(TipoTerreno::SUELO);
				break;
			}

			// Sorteo del dibujo de suelo de esta casilla (una sola vez por nivel: si se sorteara al dibujar, el piso titilaria)
			int dado = std::rand() % 100; // numero de 0 a 99
			if (dado < 60) {
				variantesSuelo[fila][columna] = 0; // lisa (60 %)
			} else if (dado < 75) {
				variantesSuelo[fila][columna] = 1; // grieta (15 %)
			} else if (dado < 90) {
				variantesSuelo[fila][columna] = 2; // musgo (15 %)
			} else {
				variantesSuelo[fila][columna] = 3; // runa (10 %)
			}
		}
	}
}

void Mapa::actualizar(float dt) {
	tiempoFuego += dt;
}

// Dibuja una pared (roca o pozo) colgando bajo la casilla. Las imagenes de pared apoyan su punto (32, 50) en el centro del rombo
void Mapa::dibujarPared(sf::RenderWindow& ventana, const sf::Texture& textura, int fila, int columna){
	sf::Sprite pared(textura); // sprite nuevo: así toma el tamaño de ESTA textura
	pared.setOrigin(ANCHO_CASILLAS / 2, CENTRO_ROMBO_DESDE_ARRIBA);
	pared.setPosition(casillaAmundo(fila, columna));
	ventana.draw(pared);
};

void Mapa::mostrar(sf::RenderWindow& ventana) {
	
	for (int fila = 0; fila < CANT_CASILLAS; fila++) {
		for (int columna = 0; columna < CANT_CASILLAS; columna++) {

			// Paredes: van ANTES que la baldosa, asi la baldosa tapa la parte de arriba de la pared
			if (casillas[fila][columna].getTipoTerreno() != TipoTerreno::ABISMO) {
				// Lado izquierdo: mira la casilla de adelante-izquierda (fila + 1)
				if (!esValida(fila + 1, columna)) {
					dibujarPared(ventana, texturaParedIzquierda, fila, columna); // borde del tablero: roca colgando
				} else if (getTipoTerreno(fila + 1, columna) == TipoTerreno::ABISMO) {
					dibujarPared(ventana, texturaPozoIzquierda, fila, columna);  // adelante hay pozo
				}
				// Lado derecho: mira la casilla de adelante-derecha (columna + 1)
				if (!esValida(fila, columna + 1)) {
					dibujarPared(ventana, texturaParedDerecha, fila, columna);
				} else if (getTipoTerreno(fila, columna + 1) == TipoTerreno::ABISMO) {
					dibujarPared(ventana, texturaPozoDerecha, fila, columna);
				}
			}

			sf::Sprite sprite;
			
			// La imagen que muestra, segun el tipo de casilla
			switch (casillas[fila][columna].getTipoTerreno()) {
				case TipoTerreno::COLUMNAROTA:
					sprite.setTexture(texturaColumna);
					break;
				case TipoTerreno::ABISMO:
					sprite.setTexture(texturaAbismo);
					break;
				case TipoTerreno::MURO: {
					// La imagen tiene 3 cuadros de 64x84 uno al lado del otro: se muestra uno segun el tiempo (como la bola)
					int cuadro = (int)(tiempoFuego / TIEMPO_POR_CUADRO) % CUADROS_FUEGO;
					sprite.setTexture(texturaMuro);
					sprite.setTextureRect(sf::IntRect(cuadro * 64, 0, 64, 84));
					break;
				}
				default:
					sprite.setTexture(texturasSuelo[variantesSuelo[fila][columna]]);
					break;
			}

			// Punto de referencia del sprite en el centro de la cara de arriba del cubo
			sf::FloatRect bordes = sprite.getLocalBounds(); // getLocalBounds = tamaño de la imagen original
			sprite.setOrigin(bordes.width / 2, bordes.height - CENTRO_ROMBO_DESDE_ABAJO);

			// Ubicar: Columna = X, Fila = Y
			sf::Vector2f posicion = casillaAmundo(fila, columna);
			sprite.setPosition(posicion);

			// Dibujar en la ventana
			ventana.draw(sprite);
		}
	}
};

// La X depende de columna − fila y la Y de columna + fila; a la X se le suma medio tablero para no quedar en negativo
sf::Vector2f Mapa::casillaAmundo(int fila, int columna) const{
	float x = (columna - fila) * ANCHO_CASILLAS / 2 + CANT_CASILLAS * ANCHO_CASILLAS / 2;
	float y = (columna + fila) * ALTO_CASILLAS / 2 + ALTO_CASILLAS / 2;
	
	return sf::Vector2f(x, y);
};

sf::Vector2i Mapa::mundoAcasilla(sf::Vector2f posicion) const{
	float a = (posicion.x - CANT_CASILLAS * ANCHO_CASILLAS / 2) / (ANCHO_CASILLAS / 2); // columna - fila
	float b = (posicion.y - ALTO_CASILLAS / 2) / (ALTO_CASILLAS / 2); // columna + fila

	// Como el click no suele caer en el centro, se redondea al int más cercano y esa es la casilla a usar
	int columna = (int)std::round((a + b) / 2);
	int fila = (int)std::round((b - a) / 2);

	return sf::Vector2i(columna,fila);
};

// Tamaño total del tablero en el mundo (vista isométrica). 
sf::Vector2f Mapa::getTamanioMundo() const{
	return sf::Vector2f(CANT_CASILLAS * ANCHO_CASILLAS, CANT_CASILLAS * ALTO_CASILLAS);
};

int Mapa::getCantCasillas() const {
	return CANT_CASILLAS;
};

bool Mapa::esValida(int _fila, int _columna) const {
	return _fila >= 0 && _fila < CANT_CASILLAS && _columna >= 0 && _columna < CANT_CASILLAS;
};

TipoTerreno Mapa::getTipoTerreno(int _fila, int _columna) const {
	return casillas[_fila][_columna].getTipoTerreno();
};

void Mapa::setTipoTerreno(int _fila, int _columna, TipoTerreno _tipo){
	casillas[_fila][_columna].setTipoTerreno(_tipo);
};

bool Mapa::mover(Personaje& personaje, int _fila, int _columna) const {
	if (!esValida(_fila, _columna) || !personaje.puedePisar(getTipoTerreno(_fila, _columna))) {
		return false;
	}
	personaje.mover(_fila, _columna);
	return true;
}

const Casilla& Mapa::getCasilla(int _fila, int _columna) const {
	return casillas[_fila][_columna];
}
