#include "Habilidad.h"

Habilidad::Habilidad(TipoHabilidad _tipo, std::string _nombre, int _danio, int _costoMana, int _alcance, int _duracion)
	: tipo(_tipo), nombre(_nombre), danio(_danio), costoMana(_costoMana), alcance(_alcance), duracion(_duracion) {
}

TipoHabilidad Habilidad::getTipo() const {
	return tipo;
}

std::string Habilidad::getNombre() const {
	return nombre;
}

void Habilidad::setNombre(std::string _nombre) {
	nombre = _nombre;
}

int Habilidad::getDanio() const {
	return danio;
}

void Habilidad::setDanio(int _danio) {
	danio = _danio;
}

int Habilidad::getCostoMana() const {
	return costoMana;
}

void Habilidad::setCostoMana(int _costoMana) {
	costoMana = _costoMana;
}

int Habilidad::getAlcance() const {
	return alcance;
}

void Habilidad::setAlcance(int _alcance) {
	alcance = _alcance;
}

int Habilidad::getDuracion() const {
	return duracion;
}

Ataque Habilidad::usar(int filaObjetivo, int columnaObjetivo) const {
	Ataque ataque;
	ataque.casillas.push_back(sf::Vector2i(columnaObjetivo, filaObjetivo)); // x = columna, y = fila
	ataque.danio = danio;
	return ataque;
}
