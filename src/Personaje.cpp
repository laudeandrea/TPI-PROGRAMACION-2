#include "Personaje.h"

Personaje::Personaje(int _fila, int _columna, int _vida){
	fila = _fila;
	columna = _columna;
	vida = _vida;
	vidaMaxima = _vida;
};

void Personaje::mostrar(sf::RenderWindow& ventana, const Mapa& mapa) const{

	sf::Sprite sprite;
	sprite.setTexture(textura);

	//en los pies del mago: la base de la imagen se apoya en el centro de la casilla.
	sf::FloatRect bordes = sprite.getLocalBounds(); 
    sprite.setOrigin(bordes.width / 2, bordes.height);

	// Ubicar: Columna = X, Fila = Y
	sf::Vector2f posicion = mapa.casillaAmundo(fila, columna);
	sprite.setPosition(posicion);

	// Dibujar en la ventana
	ventana.draw(sprite);
};

void Personaje::mover (int _fila, int _columna){
	fila = _fila;
	columna = _columna;
};

int Personaje::getFila() const {
	return fila;
};

int Personaje::getColumna() const {
	return columna;
};

int Personaje::getVida() const {
	return vida;
};

int Personaje::getVidaMaxima() const {
	return vidaMaxima;
};

void Personaje::mostrarBarraVida(sf::RenderWindow& ventana, const Mapa& mapa) const {
	const float ANCHO = 32.f, ALTO = 4.f;

	// Arriba de la cabeza: los pies estan en el centro de la casilla y la imagen mide getSize().y de alto
	sf::Vector2f pies = mapa.casillaAmundo(fila, columna);
	float x = pies.x - ANCHO / 2;
	float y = pies.y - textura.getSize().y - 6.f;

	// Fondo: la barra vacia, con un borde oscuro para que se lea sobre cualquier cosa
	sf::RectangleShape fondo(sf::Vector2f(ANCHO, ALTO));
	fondo.setPosition(x, y);
	fondo.setFillColor(sf::Color(58, 66, 88));        // Pizarra
	fondo.setOutlineThickness(1.f);
	fondo.setOutlineColor(sf::Color(11, 13, 26));     // Fondo
	ventana.draw(fondo);

	// Relleno: el ancho es proporcional a la vida que le queda (vida / vidaMaxima)
	sf::RectangleShape relleno(sf::Vector2f(ANCHO * vida / vidaMaxima, ALTO));
	relleno.setPosition(x, y);
	relleno.setFillColor(sf::Color(176, 74, 74));     // Peligro
	ventana.draw(relleno);
};

bool Personaje::estaVivo() const {
	return vida > 0;
};

bool Personaje::estaEn(int _fila, int _columna) const {
	return fila == _fila && columna == _columna;
};

bool Personaje::puedePisar(TipoTerreno terreno) const {
	return terreno == TipoTerreno::SUELO;
};

void Personaje::recibirDanio(int cantidad){
	vida -= cantidad;
	if (vida < 0) {
		vida = 0;
	}
};