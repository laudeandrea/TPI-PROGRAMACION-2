#include "Partida.h"

Partida::Partida(int _nivel, int _dificultad)
	: nivel(_nivel), dificultad(_dificultad), enCurso(false), oleada(1), vidaCodice(0), cantidadMuros(0) {
	// Se llena con ceros para no guardar basura en el archivo
	for (int i = 0; i < MAX_MUROS; i++) {
		muros[i][0] = 0;
		muros[i][1] = 0;
		muros[i][2] = 0;
	}
}

int Partida::getNivel() const {
	return nivel;
}

void Partida::setNivel(int _nivel) {
	nivel = _nivel;
}

int Partida::getDificultad() const {
	return dificultad;
}

bool Partida::getEnCurso() const {
	return enCurso;
}

void Partida::setEnCurso(bool _enCurso) {
	enCurso = _enCurso;
}

int Partida::getOleada() const {
	return oleada;
}

void Partida::setOleada(int _oleada) {
	oleada = _oleada;
}

int Partida::getVidaCodice() const {
	return vidaCodice;
}

void Partida::setVidaCodice(int _vidaCodice) {
	vidaCodice = _vidaCodice;
}

int Partida::getCantidadMuros() const {
	return cantidadMuros;
}

void Partida::agregarMuro(int fila, int columna, int turnosRestantes) {
	if (cantidadMuros >= MAX_MUROS) {
		return;
	}
	muros[cantidadMuros][0] = fila;
	muros[cantidadMuros][1] = columna;
	muros[cantidadMuros][2] = turnosRestantes;
	cantidadMuros++;
}

int Partida::getFilaMuro(int indice) const {
	return muros[indice][0];
}

int Partida::getColumnaMuro(int indice) const {
	return muros[indice][1];
}

int Partida::getTurnosMuro(int indice) const {
	return muros[indice][2];
}
