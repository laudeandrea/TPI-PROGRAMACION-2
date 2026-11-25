#include "EnemigoGuardado.h"

EnemigoGuardado::EnemigoGuardado()
	: tipo(0), fila(0), columna(0), vida(0), tieneAtaque(false), danioAtaque(0), cantidadCasillas(0) {
	for (int i = 0; i < MAX_CASILLAS; i++) {
		casillas[i][0] = 0;
		casillas[i][1] = 0;
	}
}

EnemigoGuardado::EnemigoGuardado(const Enemigo& enemigo) : EnemigoGuardado() {
	tipo = static_cast<int>(enemigo.getTipo());
	fila = enemigo.getFila();
	columna = enemigo.getColumna();
	vida = enemigo.getVida();
	tieneAtaque = enemigo.tieneAtaqueAnunciado();

	if (tieneAtaque) {
		const Ataque& ataque = enemigo.getAtaqueAnunciado();
		danioAtaque = ataque.danio;
		for (const sf::Vector2i& casilla : ataque.casillas) {
			if (cantidadCasillas >= MAX_CASILLAS) break;
			casillas[cantidadCasillas][0] = casilla.y; // y = fila
			casillas[cantidadCasillas][1] = casilla.x; // x = columna
			cantidadCasillas++;
		}
	}
}

TipoEnemigo EnemigoGuardado::getTipo() const {
	return static_cast<TipoEnemigo>(tipo);
}

int EnemigoGuardado::getFila() const {
	return fila;
}

int EnemigoGuardado::getColumna() const {
	return columna;
}

int EnemigoGuardado::getVida() const {
	return vida;
}

bool EnemigoGuardado::getTieneAtaque() const {
	return tieneAtaque;
}

Ataque EnemigoGuardado::getAtaque() const {
	Ataque ataque;
	ataque.danio = danioAtaque;
	for (int i = 0; i < cantidadCasillas; i++) {
		ataque.casillas.push_back(sf::Vector2i(casillas[i][1], casillas[i][0])); // (x = columna, y = fila)
	}
	return ataque;
}
