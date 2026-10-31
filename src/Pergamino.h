#pragma once

#include <SFML/graphics.hpp>
#include "Mapa.h"

enum class TipoHabilidad { BOLADEFUEGO, EMPUJE, MURO, PASO_ETEREO};

class Pergamino {
    private:
        static const int VIDA_MAXIMA = 10;
        static constexpr float CENTRO_ROMBO_DESDE_ABAJO = 34.f; // del fondo de la imagen al centro de la cara de arriba
        int fila, columna;
        int vidaActual;
        TipoHabilidad habilidad;
        sf::Texture texturaIntacto, texturaDaniado;
    public:
        Pergamino(int _fila, int _columna, TipoHabilidad _habilidad);
        void recibirDanio(int cantidadDanio);
        bool estaDestruido() const;
        TipoHabilidad getHabilidad() const;
        void mostrar(sf::RenderWindow& ventana, const Mapa& mapa) const;
};