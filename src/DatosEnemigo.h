#pragma once
#include <string>
#include "Enemigo.h"

/* Un registro de Datos/TiposEnemigo.dat: la vida y el ataque base de un tipo de enemigo. */
class DatosEnemigo {
	private:
		int tipo;        // TipoEnemigo guardado como numero (0 = caballero, 1 = espectro, 2 = golem, 3 = esqueleto)
		char nombre[20];
		int vida;
		int danio;
		int alcance;     // desde cuantas casillas ataca (1 = tiene que estar pegado)
	public:
		DatosEnemigo();
		DatosEnemigo(TipoEnemigo _tipo, const std::string& _nombre, int _vida, int _danio, int _alcance);
		TipoEnemigo getTipo() const;
		std::string getNombre() const;
		int getVida() const;
		int getDanio() const;
		int getAlcance() const;
};
