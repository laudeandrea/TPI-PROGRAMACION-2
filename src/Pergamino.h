#pragma once

#include <SFML/graphics.hpp>
#include "Mapa.h"
#include "Habilidad.h" // TipoHabilidad: la habilidad que guarda el codice

class Pergamino {
    private:
        static const int VIDA_MAXIMA = 50;
        static constexpr float CENTRO_ROMBO_DESDE_ABAJO = 34.f; // del fondo de la imagen al centro de la cara de arriba
        int fila, columna;
        int vidaActual;
        TipoHabilidad habilidad;
        sf::Texture texturaIntacto, texturaDaniado;
    public:
        Pergamino(int _fila, int _columna, TipoHabilidad _habilidad);
        int getFila() const;
        int getColumna() const;
        int getVida() const;
        int getVidaMaxima() const;
        void recibirDanio(int cantidadDanio);
        bool estaDestruido() const;
        TipoHabilidad getHabilidad() const;
        void mostrar(sf::RenderWindow& ventana, const Mapa& mapa) const;
};