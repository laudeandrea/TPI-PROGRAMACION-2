#pragma once

#include <SFML/Graphics.hpp>
#include "Mapa.h"
#include "Habilidad.h" // TipoHabilidad: la habilidad que guarda el codice

class Pergamino {
    private:
        static constexpr float CENTRO_ROMBO_DESDE_ABAJO = 34.f; // del fondo de la imagen al centro de la cara de arriba
        int fila, columna;
        int vidaActual;
        int vidaMaxima; // la dice cada nivel (Niveles.dat)
        TipoHabilidad habilidad;
        sf::Texture texturaIntacto, texturaDaniado;
    public:
        Pergamino(int _fila, int _columna, TipoHabilidad _habilidad, int _vidaMaxima = 80);
        int getFila() const;
        int getColumna() const;
        int getVida() const;
        int getVidaMaxima() const;
        void recibirDanio(int cantidadDanio);
        // Para cargar una partida guardada.
        void setVida(int _vida);
        bool estaDestruido() const;
        TipoHabilidad getHabilidad() const;
        void mostrar(sf::RenderWindow& ventana, const Mapa& mapa) const;
};