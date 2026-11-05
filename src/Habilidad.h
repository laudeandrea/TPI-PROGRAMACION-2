#pragma once

#include <string>

class Habilidad {
    private:
        std::string nombre;
        int danio, costoMana, alcance;
       
    public:
        Habilidad(std::string _nombre, int _danio, int _costoMana, int _alcance);
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