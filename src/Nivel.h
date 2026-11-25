#pragma once
#include <memory>
#include <string>
#include <vector>
#include "Enemigo.h"
#include "Catalogo.h"
#include "Mapa.h"
#include "Pergamino.h"

/* Un piso de la torre mientras se juega. Como en el diagrama de clases, el Nivel TIENE su mapa (el tablero),
*  sus enemigos y lo que hay que defender (el "Cristal" del diagrama: el codice, clase Pergamino).
*  Los datos para armarlo (el tablero, donde arrancan el codice y el mago, las oleadas y la historia) los saca
*  del Catalogo, que los leyo de Datos/. Cuando se mata a todos los de una oleada, aparece la siguiente
*  (siguienteOleada). Despues de la ultima, el nivel queda completado (completar) y se pasa al proximo, si hay. */
class Nivel {
	private:
		const Catalogo* catalogo; // puntero: el catalogo es del Juego, aca solo se consulta
		int numero;      // empieza en 1
		int dificultad;  // el numero de la dificultad (los enemigos y el codice se crean segun ella)
		int oleada;      // la oleada en la que se esta peleando, empieza en 0
		bool completado; // si ya se gano (no quedan oleadas)
		Mapa mapa;
		std::vector<std::unique_ptr<Enemigo>> enemigos; // unique_ptr: cada enemigo es de un tipo distinto (polimorfismo)
		Pergamino codice;
		// El registro de este nivel en Niveles.dat (lo guarda el catalogo).
		const DatosNivel& getDatos() const;
	public:
		Nivel(int _numero, const Catalogo& _catalogo, int _dificultad = Catalogo::DIFICULTAD_NORMAL);
		// Arma el nivel de cero: el tablero, el codice (con la vida que le da la dificultad) y vuelve a la primera
		// oleada, todavia sin enemigos (los pone el Juego con crearOleada, porque tiene que mirar donde esta el mago).
		void iniciar();
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
		// La vida del codice en Niveles.dat (antes de ajustarla por la dificultad).
		int getVidaCodice() const;
		// El texto que se muestra antes de jugarlo.
		std::vector<std::string> getHistoria() const;

		// --- Oleadas ---
		// Para mostrar en el HUD: "Oleada 1/2" (empieza en 1, no en 0)
		int getOleada() const;
		// Para cargar una partida guardada (empieza en 1, como getOleada).
		void setOleada(int _oleada);
		int getCantOleadas() const;
		bool hayOtraOleada() const;
		void siguienteOleada();
		// Crea los enemigos de la oleada actual, cada uno en su casilla de salida.
		std::vector<std::unique_ptr<Enemigo>> crearOleada() const;
		// Marca el nivel como ganado (ya no quedan oleadas).
		void completar();
		bool estaCompletado() const;

		// --- Lo que tiene el nivel (el Juego lo usa a traves de estas referencias) ---
		Mapa& getMapa();
		const Mapa& getMapa() const;
		std::vector<std::unique_ptr<Enemigo>>& getEnemigos();
		const std::vector<std::unique_ptr<Enemigo>>& getEnemigos() const;
		Pergamino& getCodice();
		const Pergamino& getCodice() const;
};
