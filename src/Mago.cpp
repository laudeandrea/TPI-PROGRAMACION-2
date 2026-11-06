#include "Mago.h"

Mago::Mago(int _fila, int _columna) 
	: Personaje(_fila, _columna, VIDA_INICIAL){
	mana = MANA_INICIAL;
	habilidades.push_back(Habilidad("Bola de energia", 10, 2 , 3));
	habilidades.push_back(Habilidad("Empuje", 0, 3, 2));
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

const Habilidad& Mago::getHabilidad(HabilidadSeleccionada _habilidad) const{
	// BOLA = 1 -> habilidades[0]; EMPUJE = 2 -> habilidades[1]
	return habilidades[static_cast<int>(_habilidad)-1] ;
}

int Mago::getCantidadHabilidades() const {
	return (int)habilidades.size();
}