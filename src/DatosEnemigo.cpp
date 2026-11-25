#include "DatosEnemigo.h"
#include <cstring>

DatosEnemigo::DatosEnemigo() : tipo(0), vida(1), danio(0), alcance(1) {
	nombre[0] = '\0';
}

DatosEnemigo::DatosEnemigo(TipoEnemigo _tipo, const std::string& _nombre, int _vida, int _danio, int _alcance)
	: tipo(static_cast<int>(_tipo)), vida(_vida), danio(_danio), alcance(_alcance) {
	strncpy_s(nombre, sizeof(nombre), _nombre.c_str(), _TRUNCATE);
}

TipoEnemigo DatosEnemigo::getTipo() const {
	return static_cast<TipoEnemigo>(tipo);
}

std::string DatosEnemigo::getNombre() const {
	return std::string(nombre);
}

int DatosEnemigo::getVida() const {
	return vida;
}

int DatosEnemigo::getDanio() const {
	return danio;
}

int DatosEnemigo::getAlcance() const {
	return alcance;
}
