#pragma once

#include "Enemigo.h"

class Caballero : public Enemigo {
	private:
		static const int VIDA_INICIAL = 20; // 2 bolas de energia
		static const int DANIO = 12;
	public:
		Caballero(int _fila, int _columna);
};
