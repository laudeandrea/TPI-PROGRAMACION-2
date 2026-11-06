#pragma once

#include <string>

/* Cual de las habilidades del mago esta en la mano.
 * Los valores coinciden con la tecla: 1 = bola, 2 = empuje. */
enum class HabilidadSeleccionada {
    NINGUNA,
    BOLA_DE_ENERGIA,
    EMPUJE,
};

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