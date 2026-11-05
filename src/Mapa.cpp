#include "Mapa.h"
#include <cmath>
#include <cstdlib>
#include <string> // std::to_string, para armar el nombre de cada archivo de suelo

Mapa::Mapa() {
	// Dibujo de la matriz del mapa con cada tipo de suelo

	// 9 a pesar de el tablero ser de 8x8, es porque le dejamos espacio al caracter nulo '\0', sino da error el compilador
	const char disenio[8][9] = {

	// S = suelo normal, C = columna rota, X = abismo
	"SSSSSSSS",
	"SSSSSSSS",
	"SSCSXXSS",
	"SSSSXXSS",
	"SSSSSCSS",
	"SSXXSSSS",
	"SSXXSSCS",
	"SSSSSSSS"
};

	for (int fila = 0;fila < 8;fila++) {
		for (int columna = 0; columna < 8;columna++) {
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

			// Sorteo del dibujo de suelo de esta casilla (una sola vez, por eso va en el constructor)
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

	// Variantes de suelo: suelo_1_iso.png ... suelo_4_iso.png (el numero del archivo es i + 1)
	for (int i = 0; i < CANT_VARIANTES_SUELO; i++) {
		texturasSuelo[i].loadFromFile("Imagenes/suelo_" + std::to_string(i + 1) + "_iso.png");
	}

	texturaColumna.loadFromFile("Imagenes/columna_rota_iso.png");
	texturaAbismo.loadFromFile("Imagenes/abismo_iso.png");
	texturaParedIzquierda.loadFromFile("Imagenes/pared_izquierda_iso.png");
	texturaParedDerecha.loadFromFile("Imagenes/pared_derecha_iso.png");
	texturaPozoIzquierda.loadFromFile("Imagenes/pozo_izquierda_iso.png");
	texturaPozoDerecha.loadFromFile("Imagenes/pozo_derecha_iso.png");

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
				if (!estaDentro(fila + 1, columna)) {
					dibujarPared(ventana, texturaParedIzquierda, fila, columna); // borde del tablero: roca colgando
				} else if (getTipoTerreno(fila + 1, columna) == TipoTerreno::ABISMO) {
					dibujarPared(ventana, texturaPozoIzquierda, fila, columna);  // adelante hay pozo
				}
				// Lado derecho: mira la casilla de adelante-derecha (columna + 1)
				if (!estaDentro(fila, columna + 1)) {
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

void Mapa::mover() {

};

// Una casilla es valida si esta dentro del tablero y se puede pisar
bool Mapa::esValida(int _fila, int _columna) const {
	if (!estaDentro(_fila, _columna)){
		return false;
	}
	return casillas[_fila][_columna].esTransitable();
};

bool Mapa::estaDentro(int _fila, int _columna) const {
	return _fila >= 0 && _fila < CANT_CASILLAS && _columna >= 0 && _columna < CANT_CASILLAS;
};

TipoTerreno Mapa::getTipoTerreno(int _fila, int _columna) const {
	return casillas[_fila][_columna].getTipoTerreno();
};

void Mapa::setTipoTerreno(int _fila, int _columna, TipoTerreno _tipo){
	casillas[_fila][_columna].setTipoTerreno(_tipo);
};