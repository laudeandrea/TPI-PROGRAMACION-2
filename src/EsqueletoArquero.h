#pragma once
#include "Enemigo.h"

// El enemigo del piso 3: camina como el caballero, pero ataca de lejos con una flecha.
// La vida, el danio y el alcance salen de TiposEnemigo.dat
class EsqueletoArquero : public Enemigo {
	private:
		int alcanceAtaque;
	public:
		EsqueletoArquero(int _fila, int _columna, int _vida, int _danio, int _alcanceAtaque);
		TipoEnemigo getTipo() const override;
		// Camina hacia el objetivo, rodeando lo que no puede pisar (no flota: ver puedePisar en Personaje).
		// Para apenas le llega su ataque, asi que se queda lejos.
		bool mover(const Mapa& mapa, const std::vector<std::vector<bool>>& ocupadas,
			int filaObjetivo, int columnaObjetivo) override;
		// Tira una flecha a la casilla que marco. El Juego la hace volar y aplica el danio cuando llega.
		Ataque atacar() override;
		// Ataca de lejos: no necesita estar pegado.
		int getAlcanceAtaque() const override;
		bool atacaADistancia() const override;
};
