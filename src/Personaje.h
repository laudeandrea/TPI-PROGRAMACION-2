#pragma once
#include "Mapa.h"

class Personaje {
	protected:
		int fila, columna;
		int vida;
		sf::Texture textura;
	public:
		Personaje(int _fila, int _columna, int _vida);
		virtual ~Personaje() = default; // virtual: si se borra por Personaje* se destruye la hija tambien

		void mostrar(sf::RenderWindow& ventana, const Mapa& mapa) const; // Se dibuja
		void mover (int _fila, int _columna); //Cambia de casilla

		int getFila() const;
		int getColumna() const;
		int getVida() const;
		bool estaVivo() const;
		bool estaEn(int _fila, int _columna) const;

		/** @brief Indica si este personaje puede pisar ese tipo de terreno.
		 *  Por defecto solo el suelo: las hijas lo redefinen (ej: el espectro atraviesa el abismo). */
		virtual bool puedePisar(TipoTerreno terreno) const;

		virtual void recibirDanio(int cantidad);
};