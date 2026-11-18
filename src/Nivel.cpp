#include "Nivel.h"
#include "Caballero.h"
#include "Espectro.h"
#include "Golem.h"

Nivel::Nivel(int _numero) {
	numero = _numero;
	oleada = 0;
}

int Nivel::getNumero() const {
	return numero;
}

int Nivel::getOleada() const {
	return oleada + 1;
}

int Nivel::getCantOleadas() const {
	return CANT_OLEADAS;
}

bool Nivel::hayOtraOleada() const {
	return oleada < CANT_OLEADAS - 1;
}

void Nivel::pasarDeOleada() {
	oleada++;
}

std::vector<std::unique_ptr<Enemigo>> Nivel::crearOleada() const {
	std::vector<std::unique_ptr<Enemigo>> enemigos;

	// Salen del lado derecho del tablero, lejos del codice (que esta en la columna 0).
	// Todas las casillas de salida son de suelo: (fila, columna)
	switch (oleada) {
	case 0: // primera: dos enemigos, uno cuerpo a cuerpo y uno de lejos
		enemigos.push_back(std::make_unique<Caballero>(4, 6));
		enemigos.push_back(std::make_unique<Espectro>(1, 3));
		break;
	default: // la ultima: tres, con el golem de remate
		enemigos.push_back(std::make_unique<Caballero>(0, 7));
		enemigos.push_back(std::make_unique<Espectro>(3, 7));
		enemigos.push_back(std::make_unique<Golem>(7, 6));
		break;
	}

	return enemigos; // el vector se "mueve" al que lo recibe, no se copia (los unique_ptr no se pueden copiar)
}
