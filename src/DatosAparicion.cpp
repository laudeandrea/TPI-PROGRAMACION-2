#include "DatosAparicion.h"

DatosAparicion::DatosAparicion() : nivel(1), oleada(1), tipo(0), fila(0), columna(0) {
}

DatosAparicion::DatosAparicion(int _nivel, int _oleada, TipoEnemigo _tipo, int _fila, int _columna)
	: nivel(_nivel), oleada(_oleada), tipo(static_cast<int>(_tipo)), fila(_fila), columna(_columna) {
}

int DatosAparicion::getNivel() const {
	return nivel;
}

int DatosAparicion::getOleada() const {
	return oleada;
}

TipoEnemigo DatosAparicion::getTipo() const {
	return static_cast<TipoEnemigo>(tipo);
}

int DatosAparicion::getFila() const {
	return fila;
}

int DatosAparicion::getColumna() const {
	return columna;
}
