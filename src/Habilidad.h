#pragma once

#include <string>

// Que hace la habilidad. El Juego mira esto para saber si tira una bola, empuja, etc.
enum class TipoHabilidad { BOLADEENERGIA, EMPUJE, MURO, PASO_ETEREO};

class Habilidad {
    private:
        TipoHabilidad tipo;
        std::string nombre;
        int danio, costoMana, alcance;

    public:
        Habilidad(TipoHabilidad _tipo, std::string _nombre, int _danio, int _costoMana, int _alcance);
        TipoHabilidad getTipo() const;
        std::string getNombre() const;
        void setNombre(std::string _nombre);
        int getDanio() const;
        void setDanio(int _danio);
        int getCostoMana() const;
        void setCostoMana(int _costoMana);
        int getAlcance() const;
        void setAlcance(int _alcance);

        void usar();
};