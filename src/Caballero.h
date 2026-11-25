#pragma once
#include "Enemigo.h"

// El "Guerrero" del diagrama de clases: pelea cuerpo a cuerpo (tiene que estar pegado para pegar).
// La vida y el danio salen de TiposEnemigo.dat
class Caballero : public Enemigo {
	public:
		Caballero(int _fila, int _columna, int _vida, int _danio);
		TipoEnemigo getTipo() const override;
		// Camina hacia el objetivo, rodeando lo que no puede pisar (pozos, columnas y fuego).
		bool mover(const Mapa& mapa, const std::vector<std::vector<bool>>& ocupadas,
			int filaObjetivo, int columnaObjetivo) override;
		// Golpe cuerpo a cuerpo a la casilla que habia marcado.
		Ataque atacar() override;
};
