#pragma once

#include "Enemigo.h"

class Caballero : public Enemigo {
	private:
		static const int VIDA_INICIAL = 30;
		static const int DANIO = 25;
	public:
		Caballero(int _fila, int _columna);
};
