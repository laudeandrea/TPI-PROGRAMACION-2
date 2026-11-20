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

int Nivel::getCantNiveles() const {
	return CANT_NIVELES;
}

bool Nivel::hayOtroNivel() const {
	return numero < CANT_NIVELES;
}

// ---------------------------------------------------------------- Tablero y posiciones

std::vector<std::string> Nivel::getDisenio() const {
	// S = suelo, C = columna rota, X = abismo. Cada string es una fila (de la 0 a la 7)
	if (numero == 1) {
		return {
			"SSSSSSSS",
			"SSSSSSSS",
			"SSCSXXSS",
			"SSSSXXSS",
			"SSSSSCSS",
			"SSXXSSSS",
			"SSXXSSCS",
			"SSSSSSSS"
		};
	}

	// Nivel 2: dos pozos parten el tablero en dos. Para llegar al codice (arriba) los enemigos tienen
	// que pasar por el medio (columnas 3 y 4) o por los bordes. El muro de fuego sirve para cerrar uno de esos pasos
	return {
		"SSSSSSSS",
		"SSSSSSCS",
		"SCSSSSSS",
		"SXXSSXXS",
		"SXXSSXXS",
		"SSSSSSSS",
		"SSSCSSSS",
		"SSSSSSSS"
	};
}

int Nivel::getFilaCodice() const {
	return numero == 1 ? 5 : 1;
}

int Nivel::getColumnaCodice() const {
	return numero == 1 ? 0 : 3;
}

int Nivel::getFilaMago() const {
	return numero == 1 ? 4 : 2;
}

int Nivel::getColumnaMago() const {
	// Nivel 2: al lado del codice, no adelante (en (2,4) la imagen del mago lo tapaba)
	return numero == 1 ? 1 : 2;
}

// ---------------------------------------------------------------- Oleadas

int Nivel::getOleada() const {
	return oleada + 1;
}

int Nivel::getCantOleadas() const {
	return CANT_OLEADAS;
}

bool Nivel::hayOtraOleada() const {
	return oleada < getCantOleadas() - 1;
}

void Nivel::pasarDeOleada() {
	oleada++;
}

std::vector<std::unique_ptr<Enemigo>> Nivel::crearOleada() const {
	std::vector<std::unique_ptr<Enemigo>> enemigos;

	if (numero == 1) {
		crearOleadaNivel1(enemigos);
	}
	else {
		crearOleadaNivel2(enemigos);
	}

	return enemigos; // el vector se "mueve" al que lo recibe, no se copia (los unique_ptr no se pueden copiar)
}

void Nivel::crearOleadaNivel1(std::vector<std::unique_ptr<Enemigo>>& enemigos) const {
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
}

void Nivel::crearOleadaNivel2(std::vector<std::unique_ptr<Enemigo>>& enemigos) const {
	// Salen de abajo (filas 6 y 7), del otro lado de los pozos. El codice esta arriba, en (1, 3)
	switch (oleada) {
	case 0: // primera: dos caballeros por los costados y un espectro por el medio
		enemigos.push_back(std::make_unique<Caballero>(6, 1));
		enemigos.push_back(std::make_unique<Caballero>(6, 6));
		enemigos.push_back(std::make_unique<Espectro>(7, 4));
		break;
	default: // la ultima: dos golems, un caballero y un espectro
		enemigos.push_back(std::make_unique<Golem>(7, 1));
		enemigos.push_back(std::make_unique<Caballero>(6, 4));
		enemigos.push_back(std::make_unique<Golem>(7, 6));
		enemigos.push_back(std::make_unique<Espectro>(6, 7));
		break;
	}
}
