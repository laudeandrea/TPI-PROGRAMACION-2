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

};

void Personaje::recibirDanio(){
    
};