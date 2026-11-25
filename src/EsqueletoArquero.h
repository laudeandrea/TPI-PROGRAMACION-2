#pragma once
#include "Enemigo.h"

class EsqueletoArquero : public Enemigo {
private:
	int alcanceAtaque;
public:
	EsqueletoArquero(int _fila, int _columna, int _vida, int _danio, int _alcanceAtaque);
	TipoEnemigo getTipo() const override;
	bool mover(const Mapa& mapa, const std::vector<std::vector<bool>>& ocupadas,
		int filaObjetivo, int columnaObjetivo) override;
	Ataque atacar() override;
	int getAlcanceAtaque() const override;
	bool atacaADistancia() const override;
};