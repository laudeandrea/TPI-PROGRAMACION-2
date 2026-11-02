#pragma once
#include "Enemigo.h"

class Espectro : public Enemigo {
    private:
        static const int VIDA_INICIAL = 40;
		static const int DANIO = 15;
	public:
		Espectro(int _fila, int _columna);
		/** @brief Es fantasma: atraviesa columnas rotas y vuela sobre el abismo. */
		bool puedePisar(TipoTerreno terreno) const override;
};