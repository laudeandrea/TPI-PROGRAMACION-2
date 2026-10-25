#include "Mapa.h"

Mapa::Mapa() {
	// Dibujo de la matriz del mapa con cada tipo de suelo

	// 9 a pesar de el tablero ser de 8x8, es porque le dejamos espacio al caracter nulo '\0', sino da error el compilador
	const char disenio[8][9] = {

	// S = suelo normal, C = columna rota, X = abismo
	"SSSSSSXX",
	"SSSSSSSX",
	"SCSSSSXX",
	"SSSSCSSX",
	"SSSSSSSX",
	"SSSSCSXX",
	"SSCSSSSX",
	"SSSSSXXX"
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
		}
	}

texturaSuelo.loadFromFile("Imagenes/suelo_iso.png"); 
texturaColumna.loadFromFile("Imagenes/columna_rota_iso.png");
texturaAbismo.loadFromFile("Imagenes/abismo_iso.png");

}

void Mapa::mostrar(sf::RenderWindow& ventana) {
	
	for (int fila = 0; fila < CANT_CASILLAS; fila++) {
		for (int columna = 0; columna < CANT_CASILLAS; columna++) {
		
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
					sprite.setTexture(texturaSuelo);
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

// Tamaño total del tablero en el mundo (vista isométrica). 
sf::Vector2f Mapa::getTamanioMundo() const{
	return sf::Vector2f(CANT_CASILLAS * ANCHO_CASILLAS, CANT_CASILLAS * ALTO_CASILLAS);
};

void Mapa::mover() {

};

bool Mapa::esValida() {
	return false;
};
