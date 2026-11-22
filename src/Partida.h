#pragma once

class Partida {
private:
	int nivel; // proximo nivel a jugar (1 = desde el principio)
public:
	Partida(int _nivel = 1);
	int getNivel() const;
	void setNivel(int _nivel);
};