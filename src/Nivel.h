#pragma once
#include <memory>
#include <string>
#include <vector>
#include "Enemigo.h"

/* Todo lo que cambia de un nivel a otro: el tablero, donde arrancan el codice y el mago, y las oleadas
*  de enemigos. Cuando se mata a todos los de una oleada, aparece la siguiente. Despues de la ultima,
*  se gano el nivel (y se pasa al proximo, si hay). */
class Nivel {
	private:
		static const int CANT_NIVELES = 2;
		static const int CANT_OLEADAS = 2; // por ahora todos los niveles tienen 2
		int numero; // empieza en 1
		int oleada; // la oleada en la que se esta peleando, empieza en 0

		// Las oleadas de cada nivel, por separado para que crearOleada() no quede gigante.
		void crearOleadaNivel1(std::vector<std::unique_ptr<Enemigo>>& enemigos) const;
		void crearOleadaNivel2(std::vector<std::unique_ptr<Enemigo>>& enemigos) const;
	public:
		Nivel(int _numero);
		int getNumero() const;
		int getCantNiveles() const;
		// Si despues de este hay otro nivel.
		bool hayOtroNivel() const;

		// El tablero: 8 strings de 8 letras (S = suelo, C = columna rota, X = abismo). Ver Mapa::cargar.
		std::vector<std::string> getDisenio() const;
		// Donde esta el codice y donde arranca el mago. Las dos casillas tienen que ser 'S' en el disenio.
		int getFilaCodice() const;
		int getColumnaCodice() const;
		int getFilaMago() const;
		int getColumnaMago() const;

		// Para mostrar en el HUD: "Oleada 1/2" (empieza en 1, no en 0)
		int getOleada() const;
		int getCantOleadas() const;
		bool hayOtraOleada() const;
		void pasarDeOleada();
		// Crea los enemigos de la oleada actual, cada uno en su casilla de salida.
		std::vector<std::unique_ptr<Enemigo>> crearOleada() const;
};
