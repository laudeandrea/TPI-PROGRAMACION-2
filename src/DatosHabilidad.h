#pragma once
#include <string>
#include "Habilidad.h"

/* Un registro de Datos/Habilidades.dat: los valores de un hechizo del mago.
*  Es una clase "plana" (solo int y arreglos de char) para poder escribirla tal cual en el archivo. */
class DatosHabilidad {
	private:
		int tipo;            // TipoHabilidad guardado como numero (0 = bola, 1 = empuje, 2 = muro)
		char nombre[30];     // con el '\0' del final entran 29 letras
		int danio;           // en el muro de fuego, lo que pierde el espectro cada vez que se quema
		int costoMana;
		int alcance;         // a cuantas casillas del mago se puede usar
		int duracion;        // turnos que dura (solo el muro de fuego; las demas, 0)
		int nivelDesbloqueo; // desde que nivel la tiene el mago
	public:
		DatosHabilidad();
		DatosHabilidad(TipoHabilidad _tipo, const std::string& _nombre, int _danio, int _costoMana,
			int _alcance, int _duracion, int _nivelDesbloqueo);
		TipoHabilidad getTipo() const;
		std::string getNombre() const;
		int getDanio() const;
		int getCostoMana() const;
		int getAlcance() const;
		int getDuracion() const;
		int getNivelDesbloqueo() const;
};
