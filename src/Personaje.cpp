#include "Personaje.h"

Personaje::Personaje(int _fila, int _columna, int _vida){
	fila = _fila;
	columna = _columna;
	vida = _vida;
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