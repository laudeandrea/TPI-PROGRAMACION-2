#include "Mago.h"

Mago::Mago(int _fila, int _columna) 
	: Personaje(_fila, _columna, VIDA_INICIAL){
	mana = MANA_INICIAL;
	// Las habilidades se las ensena el Juego al empezar cada nivel, segun Habilidades.dat
	textura.loadFromFile("Imagenes/mago_64.png");
}

int Mago::getMana() const{
	return mana;
}

int Mago::getManaMaximo() const{
	return MANA_MAXIMO;
}

int Mago::getMovimiento() const{
	return MOVIMIENTO;
}

void Mago::recuperarMana(int _cantidad){
	mana += _cantidad;
	if (mana > MANA_MAXIMO){
		mana = MANA_MAXIMO;
	}
}

bool Mago::gastarMana(int _cantidad){

	if (mana < _cantidad){
		return false;
	}
	
	mana -= _cantidad;
	return true;
}

void Mago::setMana(int _mana){
	mana = _mana;
	if (mana < 0) mana = 0;
	if (mana > MANA_MAXIMO) mana = MANA_MAXIMO;
}

// HabilidadSeleccionada es el TipoHabilidad + 1 (1 = bola, que es el tipo 0). Se busca la que tenga ese tipo
const Habilidad& Mago::getHabilidad(HabilidadSeleccionada _habilidad) const{
	for (const Habilidad& habilidad : habilidades) {
		if (static_cast<int>(habilidad.getTipo()) + 1 == static_cast<int>(_habilidad)) {
			return habilidad;
		}
	}
	return habilidades[0]; // no deberia pasar: antes se pregunta tieneHabilidad()
}

int Mago::getCantidadHabilidades() const{
	return (int)habilidades.size();
}

bool Mago::tieneHabilidad(HabilidadSeleccionada _habilidad) const{
	for (const Habilidad& habilidad : habilidades) {
		if (static_cast<int>(habilidad.getTipo()) + 1 == static_cast<int>(_habilidad)) {
			return true;
		}
	}
	return false;
}

void Mago::aprenderHabilidad(const Habilidad& _habilidad){
	habilidades.push_back(_habilidad);
}

void Mago::olvidarHabilidades(){
	habilidades.clear();
}

Ataque Mago::atacar(HabilidadSeleccionada _habilidad, int filaObjetivo, int columnaObjetivo){
	const Habilidad& habilidad = getHabilidad(_habilidad);
	if (!gastarMana(habilidad.getCostoMana())) {
		return Ataque(); // sin casillas: no alcanzo el mana
	}
	return habilidad.usar(filaObjetivo, columnaObjetivo);
}
