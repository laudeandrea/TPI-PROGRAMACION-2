#include "Mago.h"

Mago::Mago(int _fila, int _columna) 
	: Personaje(_fila, _columna, VIDA_INICIAL){
	mana = MANA_INICIAL;
	// El orden importa: tiene que ser el mismo que el de HabilidadSeleccionada (1 = bola, 2 = empuje)
	habilidades.push_back(Habilidad(TipoHabilidad::BOLADEENERGIA, "Bola de energia", 10, 2, 3));
	// Empuje: el enemigo tiene que estar pegado (alcance 1). Si cae al abismo, no vuelve
	habilidades.push_back(Habilidad(TipoHabilidad::EMPUJE, "Empuje", 5, 3, 1));
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
	// BOLA = 1 -> habilidades[0]; EMPUJE = 2 -> habilidades[1]; MURO = 3 -> habilidades[2]
	return habilidades[static_cast<int>(_habilidad)-1] ;
}

int Mago::getCantidadHabilidades() const{
	return (int)habilidades.size();
}

bool Mago::tieneHabilidad(HabilidadSeleccionada _habilidad) const{
	int numero = static_cast<int>(_habilidad);
	return numero >= 1 && numero <= getCantidadHabilidades();
}

void Mago::aprenderHabilidad(const Habilidad& _habilidad){
	habilidades.push_back(_habilidad);
}
