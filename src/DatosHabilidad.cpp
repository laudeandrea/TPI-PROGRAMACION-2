#include "DatosHabilidad.h"
#include <cstring>

DatosHabilidad::DatosHabilidad()
	: tipo(0), danio(0), costoMana(0), alcance(0), duracion(0), nivelDesbloqueo(1) {
	nombre[0] = '\0';
}

DatosHabilidad::DatosHabilidad(TipoHabilidad _tipo, const std::string& _nombre, int _danio, int _costoMana,
	int _alcance, int _duracion, int _nivelDesbloqueo)
	: tipo(static_cast<int>(_tipo)), danio(_danio), costoMana(_costoMana), alcance(_alcance),
	  duracion(_duracion), nivelDesbloqueo(_nivelDesbloqueo) {
	// strncpy_s copia el texto al arreglo sin pasarse de su tamanio (_TRUNCATE: si no entra, lo corta)
	strncpy_s(nombre, sizeof(nombre), _nombre.c_str(), _TRUNCATE);
}

TipoHabilidad DatosHabilidad::getTipo() const {
	return static_cast<TipoHabilidad>(tipo);
}

std::string DatosHabilidad::getNombre() const {
	return std::string(nombre);
}

int DatosHabilidad::getDanio() const {
	return danio;
}

int DatosHabilidad::getCostoMana() const {
	return costoMana;
}

int DatosHabilidad::getAlcance() const {
	return alcance;
}

int DatosHabilidad::getDuracion() const {
	return duracion;
}

int DatosHabilidad::getNivelDesbloqueo() const {
	return nivelDesbloqueo;
}
