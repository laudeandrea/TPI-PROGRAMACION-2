#pragma once
#include "Enemigo.h"

class Golem : public Enemigo {
	private:
		static const int VIDA_INICIAL = 60;
		static const int DANIO = 30; // pega fuerte, pero una sola vez: se destruye al explotar
	public:
		Golem(int _fila, int _columna);
		// Explota: lastima las 8 casillas de alrededor (tambien a otros enemigos) y el golem se destruye.
		Ataque atacar(int filaObjetivo, int columnaObjetivo) override;
};
