#include "MagoGuardado.h"

MagoGuardado::MagoGuardado() : vida(0), mana(0), fila(0), columna(0), seMovio(false) {
}

MagoGuardado::MagoGuardado(int _vida, int _mana, int _fila, int _columna, bool _seMovio)
	: vida(_vida), mana(_mana), fila(_fila), columna(_columna), seMovio(_seMovio) {
}

int MagoGuardado::getVida() const {
	return vida;
}

int MagoGuardado::getMana() const {
	return mana;
}

int MagoGuardado::getFila() const {
	return fila;
}

int MagoGuardado::getColumna() const {
	return columna;
}

bool MagoGuardado::getSeMovio() const {
	return seMovio;
}
