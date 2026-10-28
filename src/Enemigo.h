#pragma once
#include "Personaje.h"

/* Clase base de los enemigos: lo que comparten todos (vida, danio, dibujarse) */
class Enemigo : public Personaje {
	protected:
		int danio;
	public:
		Enemigo(int _fila, int _columna, int _vida, int _danio);
		int getDanio() const;
};
