#pragma once
#include "Enemigo.h"

/* Un registro de Enemigos.dat: un enemigo que estaba en el tablero cuando se dejo el nivel por la mitad.
*  Tambien guarda el ataque que tenia anunciado, asi al volver el jugador ve las mismas casillas rojas. */
class EnemigoGuardado {
	public:
		static const int MAX_CASILLAS = 8; // el ataque que mas casillas toca es la explosion del golem (8)
	private:
		int tipo; // TipoEnemigo guardado como numero
		int fila, columna;
		int vida;
		bool tieneAtaque;
		int danioAtaque;
		int cantidadCasillas;
		int casillas[MAX_CASILLAS][2]; // fila y columna de cada casilla del ataque anunciado
	public:
		EnemigoGuardado();
		// Copia todo lo que hay que guardar de ese enemigo.
		EnemigoGuardado(const Enemigo& enemigo);
		TipoEnemigo getTipo() const;
		int getFila() const;
		int getColumna() const;
		int getVida() const;
		bool getTieneAtaque() const;
		// El ataque anunciado armado de nuevo (vacio si no tenia).
		Ataque getAtaque() const;
};
