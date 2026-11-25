#include "Pergamino.h"

 Pergamino::Pergamino(int _fila, int _columna, TipoHabilidad _habilidad, int _vidaMaxima){
    vidaMaxima = _vidaMaxima;
    vidaActual = vidaMaxima;
    fila = _fila;
    columna = _columna;
    habilidad = _habilidad;
    texturaIntacto.loadFromFile("Imagenes/pergamino_codice_iso.png");
    texturaDaniado.loadFromFile("Imagenes/pergamino_codice_danio_iso.png");
};

int Pergamino::getFila() const{ return fila; };
    
int Pergamino::getColumna() const{ return columna; };

int Pergamino::getVida() const{ return vidaActual; };

int Pergamino::getVidaMaxima() const{ return vidaMaxima; };

void Pergamino::recibirDanio(int cantidadDanio){
    vidaActual -= cantidadDanio;
    if(vidaActual < 0){
        vidaActual = 0;
    }
 };
    
 void Pergamino::setVida(int _vida){
    vidaActual = _vida;
    if (vidaActual < 0) vidaActual = 0;
    if (vidaActual > vidaMaxima) vidaActual = vidaMaxima;
 };

 bool Pergamino::estaDestruido() const{
    return vidaActual == 0;
};

TipoHabilidad Pergamino::getHabilidad() const{
    return habilidad;
};

void Pergamino::mostrar(sf::RenderWindow& ventana, const Mapa& mapa) const{
    sf::Sprite sprite;

    if (vidaActual > vidaMaxima / 2){
        sprite.setTexture(texturaIntacto);
    }else{
        sprite.setTexture(texturaDaniado);
    }

	sf::FloatRect bordes = sprite.getLocalBounds(); 
    sprite.setOrigin(bordes.width / 2, bordes.height - CENTRO_ROMBO_DESDE_ABAJO);

	// Ubicar: Columna = X, Fila = Y
	sf::Vector2f posicion = mapa.casillaAmundo(fila, columna);
	sprite.setPosition(posicion);

	// Dibujar en la ventana
	ventana.draw(sprite);
};