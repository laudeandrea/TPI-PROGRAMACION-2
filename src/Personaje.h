#pragma once
#include "Mapa.h"

/* Clase base de todo lo que camina por el tablero: el mago y los enemigos (ver Mago y Enemigo).
*  Tiene lo que comparten: en que casilla esta, la vida, la imagen y como se dibuja.
*  Los atributos son protected (no private) para que las hijas los puedan usar. */
class Personaje {
	protected:
		int fila, columna;
		int vida;
		int vidaMaxima; // la vida con la que nace: sirve para saber cuanto llenar la barra
		sf::Texture textura;
	public:
		Personaje(int _fila, int _columna, int _vida);
		virtual ~Personaje() = default; // virtual: si se borra por Personaje* se destruye la hija tambien

		// Dibuja la imagen con los pies en el centro de su casilla.
		void mostrar(sf::RenderWindow& ventana, const Mapa& mapa) const;
		// Dibuja la barra de vida arriba de la cabeza. Hay que llamarla con la vista de la camara puesta.
		void mostrarBarraVida(sf::RenderWindow& ventana, const Mapa& mapa) const;
		// Lo pone en esa casilla sin mirar el terreno (eso lo hace Mapa::mover). El empuje lo usa asi.
		void mover(int _fila, int _columna);

		int getFila() const;
		int getColumna() const;
		int getVida() const;
		int getVidaMaxima() const;
		bool estaVivo() const;
		bool estaEn(int _fila, int _columna) const;

		// Indica si este personaje puede pisar ese tipo de terreno.
		// Por defecto solo el suelo: las hijas lo redefinen (ej: el espectro atraviesa el abismo).
		virtual bool puedePisar(TipoTerreno terreno) const;

		// Le resta vida, sin bajar de 0.
		virtual void recibirDanio(int cantidad);
		// Para cargar una partida guardada: pone la vida (entre 0 y la vida maxima).
		void setVida(int _vida);
};
