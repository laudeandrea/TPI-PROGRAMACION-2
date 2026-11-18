#pragma once
#include <memory>
#include <vector>
#include "Enemigo.h"

/* Las oleadas de enemigos de un nivel. Cuando se mata a todos los de una oleada, aparece la siguiente.
*  Despues de la ultima, se gano el nivel. */
class Nivel {
	private:
		static const int CANT_OLEADAS = 2;
		int numero;
		int oleada; // la oleada en la que se esta peleando, empieza en 0
	public:
		Nivel(int _numero);
		int getNumero() const;
		// Para mostrar en el HUD: "Oleada 1/3" (empieza en 1, no en 0)
		int getOleada() const;
		int getCantOleadas() const;
		bool hayOtraOleada() const;
		void pasarDeOleada();
		// Crea los enemigos de la oleada actual, cada uno en su casilla de salida.
		std::vector<std::unique_ptr<Enemigo>> crearOleada() const;
};
