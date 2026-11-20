#pragma once

#include <vector>
#include "Personaje.h"
#include "Habilidad.h"

class Mago : public Personaje {
    private:
        int mana;
        std::vector<Habilidad> habilidades;
        // 130: aguanta 5 golpes del caballero (25) y 4 del golem (30)
        static const int VIDA_INICIAL = 130;
        static const int MANA_INICIAL = 10;
        static const int MANA_MAXIMO = 10;
        static const int MOVIMIENTO = 3; // cuantos pasos puede caminar por turno (cambiar aca para probar otros)
    public:
        Mago(int _fila, int _columna);
        int getMana() const;
        int getManaMaximo() const;
        int getMovimiento() const;
        bool gastarMana(int _cantidad);
        // Suma mana sin pasarse del maximo.
        void recuperarMana(int _cantidad);
        // Hay que llamar antes a tieneHabilidad(): si no la aprendio, se sale del vector.
        const Habilidad& getHabilidad(HabilidadSeleccionada _habilidad) const;
        int getCantidadHabilidades() const;
        // Si ya aprendio esa habilidad (el muro de fuego recien se aprende en el nivel 2).
        bool tieneHabilidad(HabilidadSeleccionada _habilidad) const;
        // Agrega una habilidad al final. Tiene que seguir el orden de HabilidadSeleccionada.
        void aprenderHabilidad(const Habilidad& _habilidad);

};