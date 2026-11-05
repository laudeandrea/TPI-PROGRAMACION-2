#pragma once

#include <vector>
#include "Personaje.h"
#include "Habilidad.h"

class Mago : public Personaje {
    private:
        int mana;
        std::vector<Habilidad> habilidades;
        static const int VIDA_INICIAL = 100;
        static const int MANA_INICIAL = 10;
        static const int MANA_MAXIMO = 10;
        static const int MOVIMIENTO = 3; // cuantos pasos puede caminar por turno (cambiar aca para probar otros)
    public:
        Mago(int _fila, int _columna);
        int getMana() const;
        int getManaMaximo() const;
        int getMovimiento() const;
        bool gastarMana(int _cantidad);
        /** @brief Suma mana sin pasarse del maximo. */
        void recuperarMana(int _cantidad);
        const Habilidad& getHabilidad(int _indice) const;

};