#pragma once
#include <SFML/Graphics.hpp>
#include "Mapa.h"
#include "Habilidad.h" // TipoHabilidad: la habilidad que guarda el codice

/* El codice: el libro sobre el atril que hay que defender (el "Cristal" del diagrama de clases).
*  No es un Personaje: no se mueve ni ataca, solo recibe danio. Su vida la da cada nivel (Niveles.dat), ajustada
*  por la dificultad. Si llega a 0, se pierde la partida. */
class Pergamino {
	private:
		static constexpr float CENTRO_ROMBO_DESDE_ABAJO = 34.f; // del fondo de la imagen al centro de la cara de arriba
		int fila, columna;
		int vidaActual;
		int vidaMaxima;
		TipoHabilidad habilidad; // el hechizo que guarda (esta en el diagrama de clases)
		sf::Texture texturaIntacto, texturaDaniado;
	public:
		Pergamino(int _fila, int _columna, TipoHabilidad _habilidad, int _vidaMaxima = 80);
		int getFila() const;
		int getColumna() const;
		int getVida() const;
		int getVidaMaxima() const;
		void recibirDanio(int cantidadDanio);
		// Para cargar una partida guardada (entre 0 y la vida maxima).
		void setVida(int _vida);
		bool estaDestruido() const;
		TipoHabilidad getHabilidad() const;
		// Dibuja el atril con el libro en el centro de su casilla. Con la mitad de la vida o menos se ve roto.
		void mostrar(sf::RenderWindow& ventana, const Mapa& mapa) const;
};
