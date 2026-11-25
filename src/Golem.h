#pragma once
#include "Enemigo.h"

// Explota: cuando llega a su objetivo y tambien cuando lo matan. La vida y el danio salen de TiposEnemigo.dat
class Golem : public Enemigo {
	private:
		// Las 8 casillas que lo rodean (las diagonales tambien), con su danio.
		Ataque explosion() const;
	public:
		Golem(int _fila, int _columna, int _vida, int _danio);
		TipoEnemigo getTipo() const override;
		// Camina hacia el objetivo, como el caballero: es pesado, no vuela.
		bool mover(const Mapa& mapa, const std::vector<std::vector<bool>>& ocupadas,
			int filaObjetivo, int columnaObjetivo) override;
		// No apunta solo al objetivo: la explosion le pega a todo lo que este cerca (tambien a otros enemigos).
		Ataque planearAtaque(int filaObjetivo, int columnaObjetivo) const override;
		// Su ataque es explotar.
		Ataque atacar() override;
		// Explota: le pega a las 8 casillas de alrededor y se destruye.
		Ataque explotar();
		// Si lo matan, explota igual (salvo que se caiga al abismo: ver Juego::empujar).
		Ataque ataqueAlMorir() const override;
};
