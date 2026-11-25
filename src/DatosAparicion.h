#pragma once
#include "Enemigo.h"

/* Un registro de Datos/NivelEnemigos.dat: un enemigo que aparece en una oleada de un nivel, y en que casilla.
*  Cada oleada son varios registros con el mismo nivel y la misma oleada. */
class DatosAparicion {
	private:
		int nivel;   // empieza en 1
		int oleada;  // empieza en 1
		int tipo;    // TipoEnemigo guardado como numero
		int fila, columna;
	public:
		DatosAparicion();
		DatosAparicion(int _nivel, int _oleada, TipoEnemigo _tipo, int _fila, int _columna);
		int getNivel() const;
		int getOleada() const;
		TipoEnemigo getTipo() const;
		int getFila() const;
		int getColumna() const;
};
