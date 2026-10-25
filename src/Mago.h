#pragma once
#include "Personaje.h"

class Mago : public Personaje {
    private:
        int mana;
        static const int VIDA_INICIAL = 100;
        static const int MANA_INICIAL = 10; 
    public:
        Mago(int _fila, int _columna);
};