#pragma once
#include "Enemigo.h"

class Golem : public Enemigo {
	private:
		static const int VIDA_INICIAL = 60;
		static const int DANIO = 10;
	public:
		Golem(int _fila, int _columna);
};
