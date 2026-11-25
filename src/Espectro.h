#pragma once
#include "Enemigo.h"

// Ataca de lejos y flota. La vida, el danio y el alcance salen de TiposEnemigo.dat
class Espectro : public Enemigo {
    private:
		int alcanceAtaque;
	public:
		Espectro(int _fila, int _columna, int _vida, int _danio, int _alcanceAtaque);
		TipoEnemigo getTipo() const override;
		// Es fantasma: atraviesa columnas rotas y el muro de fuego (quemandose) y vuela sobre el abismo.
		bool puedePisar(TipoTerreno terreno) const override;
		// Vuela hacia el objetivo: el camino mas corto, pasando por arriba de pozos, columnas y fuego.
		bool mover(const Mapa& mapa, const std::vector<std::vector<bool>>& ocupadas,
			int filaObjetivo, int columnaObjetivo) override;
		// Su ataque es tirar una bola (lanzarProyectil).
		Ataque atacar() override;
		// Tira una bola de ectoplasma a la casilla que marco. El Juego la hace volar y aplica el danio cuando llega.
		Ataque lanzarProyectil();
		// Ataca de lejos: no necesita estar pegado.
		int getAlcanceAtaque() const override;
		bool atacaADistancia() const override;
};
