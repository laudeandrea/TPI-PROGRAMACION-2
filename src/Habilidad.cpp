#include "Habilidad.h"

Habilidad::Habilidad(std::string _nombre, int _danio, int _costoMana, int _alcance)
    : nombre(_nombre), danio(_danio), costoMana(_costoMana), alcance(_alcance) {
}


std::string Habilidad::getNombre() const {
    return nombre;
}

void Habilidad::setNombre(std::string _nombre){
    nombre = _nombre;
}

int Habilidad::getDanio() const {
    return danio;
}

void Habilidad::setDanio(int _danio){
    danio = _danio;
}

int Habilidad::getCostoMana() const {
    return costoMana;
}

void Habilidad::setCostoMana(int _costoMana){
    costoMana = _costoMana;
}

int Habilidad::getAlcance() const {
    return alcance;
}

void Habilidad::setAlcance(int _alcance){
    alcance = _alcance;
}

void Habilidad::usar(){

}