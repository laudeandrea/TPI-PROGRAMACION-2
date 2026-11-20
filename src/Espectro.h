#pragma once
#include "Enemigo.h"

class Espectro : public Enemigo {
    private:
        static const int VIDA_INICIAL = 40;
		static const int DANIO = 8;
		static const int ALCANCE_ATAQUE = 3;
	public:
		Espectro(int _fila, int _columna);
		// Es fantasma: atraviesa columnas rotas y el muro de fuego (quemandose) y vuela sobre el abismo.
		bool puedePisar(TipoTerreno terreno) const override;
		// Ataca de lejos: no necesita estar pegado.
		int getAlcanceAtaque() const override;
		// Tira una bola de ectoplasma (el daño se aplica cuando llega).
		bool lanzaProyectil() const override;
};