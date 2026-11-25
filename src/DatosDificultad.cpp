#include "DatosDificultad.h"
#include <cstring>

DatosDificultad::DatosDificultad()
	: numero(2), porcentajeVidaCodice(100), porcentajeVidaEnemigos(100), porcentajeDanioEnemigos(100), manaPorTurno(2) {
	strncpy_s(nombre, sizeof(nombre), "Normal", _TRUNCATE);
	descripcion[0] = '\0';
}

DatosDificultad::DatosDificultad(int _numero, const std::string& _nombre, const std::string& _descripcion,
	int _porcentajeVidaCodice, int _porcentajeVidaEnemigos, int _porcentajeDanioEnemigos, int _manaPorTurno)
	: numero(_numero), porcentajeVidaCodice(_porcentajeVidaCodice), porcentajeVidaEnemigos(_porcentajeVidaEnemigos),
	  porcentajeDanioEnemigos(_porcentajeDanioEnemigos), manaPorTurno(_manaPorTurno) {
	strncpy_s(nombre, sizeof(nombre), _nombre.c_str(), _TRUNCATE);
	strncpy_s(descripcion, sizeof(descripcion), _descripcion.c_str(), _TRUNCATE);
}

int DatosDificultad::aplicarPorcentaje(int valor, int porcentaje) const {
	// Con enteros, la division corta los decimales. Sumando 50 antes de dividir por 100 se redondea
	// al mas cercano: 150 % de 25 = 37,5 -> (25 * 150 + 50) / 100 = 38
	return (valor * porcentaje + 50) / 100;
}

int DatosDificultad::getNumero() const {
	return numero;
}

std::string DatosDificultad::getNombre() const {
	return std::string(nombre);
}

std::string DatosDificultad::getDescripcion() const {
	return std::string(descripcion);
}

int DatosDificultad::getPorcentajeVidaCodice() const {
	return porcentajeVidaCodice;
}

int DatosDificultad::getPorcentajeVidaEnemigos() const {
	return porcentajeVidaEnemigos;
}

int DatosDificultad::getPorcentajeDanioEnemigos() const {
	return porcentajeDanioEnemigos;
}

int DatosDificultad::getManaPorTurno() const {
	return manaPorTurno;
}

int DatosDificultad::ajustarVidaCodice(int vidaBase) const {
	return aplicarPorcentaje(vidaBase, porcentajeVidaCodice);
}

int DatosDificultad::ajustarVidaEnemigo(int vidaBase) const {
	return aplicarPorcentaje(vidaBase, porcentajeVidaEnemigos);
}

int DatosDificultad::ajustarDanioEnemigo(int danioBase) const {
	return aplicarPorcentaje(danioBase, porcentajeDanioEnemigos);
}
