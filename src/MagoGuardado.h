#pragma once

/* El registro de Mago.dat: como estaba el mago cuando se dejo el nivel por la mitad.
*  Las habilidades no se guardan: dependen del nivel (cada una dice desde que nivel se tiene). */
class MagoGuardado {
	private:
		int vida;
		int mana;
		int fila, columna;
		bool seMovio; // si ya habia caminado en ese turno
	public:
		MagoGuardado();
		MagoGuardado(int _vida, int _mana, int _fila, int _columna, bool _seMovio);
		int getVida() const;
		int getMana() const;
		int getFila() const;
		int getColumna() const;
		bool getSeMovio() const;
};
