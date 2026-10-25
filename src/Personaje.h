#pragma once
#include "Mapa.h"

class Personaje {
	protected:
		int fila, columna;
		int vida;
		sf::Texture textura;
	public:
		Personaje(int _fila, int _columna, int _vida);
		void mostrar(sf::RenderWindow& ventana, const Mapa& mapa) const; // Se dibuja
		void mover (int _fila, int _columna); //Cambia de casilla
        void recibirDanio();
};