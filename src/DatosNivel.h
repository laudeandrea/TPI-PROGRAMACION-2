#pragma once
#include <string>
#include <vector>

/* Un registro de Datos/Niveles.dat: un piso de la torre. El tablero, donde arrancan el codice y el mago,
*  cuantas oleadas tiene, cuanta vida tiene el codice y el texto de historia que se muestra antes de jugarlo.
*  Que enemigos salen en cada oleada esta en otro archivo (NivelEnemigos.dat, ver DatosAparicion). */
class DatosNivel {
	public:
		static const int CANT_CASILLAS = 8;        // el tablero es de 8x8
		static const int MAX_LINEAS_HISTORIA = 6;
		static const int LARGO_LINEA_HISTORIA = 48; // con el '\0' entran 47 letras por linea
	private:
		int numero; // empieza en 1
		// Cada fila es un texto de 8 letras + el '\0' del final (por eso 9): S = suelo, C = columna rota, X = abismo
		char disenio[CANT_CASILLAS][CANT_CASILLAS + 1];
		int filaCodice, columnaCodice;
		int filaMago, columnaMago;
		int cantOleadas;
		int vidaCodice;  // cada nivel puede tener un codice mas fuerte o mas debil (es la forma mas directa de ajustar la dificultad)
		char historia[MAX_LINEAS_HISTORIA][LARGO_LINEA_HISTORIA]; // las lineas que no se usan quedan vacias
	public:
		DatosNivel();
		DatosNivel(int _numero, const std::vector<std::string>& _disenio, int _filaCodice, int _columnaCodice,
			int _filaMago, int _columnaMago, int _cantOleadas, int _vidaCodice, const std::vector<std::string>& _historia);
		int getNumero() const;
		// El tablero como 8 strings de 8 letras (lo que pide Mapa::cargar).
		std::vector<std::string> getDisenio() const;
		int getFilaCodice() const;
		int getColumnaCodice() const;
		int getFilaMago() const;
		int getColumnaMago() const;
		int getCantOleadas() const;
		int getVidaCodice() const;
		// Solo las lineas que tienen texto.
		std::vector<std::string> getHistoria() const;
};
