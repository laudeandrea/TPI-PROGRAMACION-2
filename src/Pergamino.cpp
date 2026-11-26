#include "Pergamino.h"

Pergamino::Pergamino(int _fila, int _columna, TipoHabilidad _habilidad, int _vidaMaxima)
	: fila(_fila), columna(_columna), vidaActual(_vidaMaxima), vidaMaxima(_vidaMaxima), habilidad(_habilidad) {
	texturaIntacto.loadFromFile("Imagenes/pergamino_codice_iso.png");
	texturaDaniado.loadFromFile("Imagenes/pergamino_codice_danio_iso.png");
}

int Pergamino::getFila() const {
	return fila;
}

int Pergamino::getColumna() const {
	return columna;
}

int Pergamino::getVida() const {
	return vidaActual;
}

int Pergamino::getVidaMaxima() const {
	return vidaMaxima;
}

void Pergamino::recibirDanio(int cantidadDanio) {
	vidaActual -= cantidadDanio;
	if (vidaActual < 0) {
		vidaActual = 0;
	}
}

void Pergamino::setVida(int _vida) {
	vidaActual = _vida;
	if (vidaActual < 0) vidaActual = 0;
	if (vidaActual > vidaMaxima) vidaActual = vidaMaxima;
}

bool Pergamino::estaDestruido() const {
	return vidaActual == 0;
}

TipoHabilidad Pergamino::getHabilidad() const {
	return habilidad;
}

void Pergamino::mostrar(sf::RenderWindow& ventana, const Mapa& mapa) const {
	sf::Sprite sprite;

	// Con la mitad de la vida o menos, el libro se ve roto
	if (vidaActual > vidaMaxima / 2) {
		sprite.setTexture(texturaIntacto);
	}
	else {
		sprite.setTexture(texturaDaniado);
	}

	// Igual que las baldosas: el centro de la cara de arriba del atril va en el centro de la casilla
	sf::FloatRect bordes = sprite.getLocalBounds();
	sprite.setOrigin(bordes.width / 2, bordes.height - CENTRO_ROMBO_DESDE_ABAJO);
	sprite.setPosition(mapa.casillaAmundo(fila, columna));

	ventana.draw(sprite);
}
