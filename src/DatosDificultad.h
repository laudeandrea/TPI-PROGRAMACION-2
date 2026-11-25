#pragma once
#include <string>

/* Un registro de Datos/Dificultades.dat: como cambia el juego en cada dificultad.
*  Son porcentajes sobre los valores de los otros archivos (100 = igual que en Normal), mas el mana por turno.
*  Por ejemplo, con porcentajeVidaCodice = 175, un codice de 80 de vida pasa a tener 140. */
class DatosDificultad {
	private:
		int numero;                  // 1 = Facil, 2 = Normal, 3 = Dificil (el orden en la pantalla de elegir)
		char nombre[12];             // sin tildes, como el resto de los textos
		char descripcion[48];        // lo que se lee al marcarla (con el '\0' entran 47 letras)
		int porcentajeVidaCodice;
		int porcentajeVidaEnemigos;
		int porcentajeDanioEnemigos;
		int manaPorTurno;            // lo que recupera el mago al empezar cada turno suyo

		// valor * porcentaje / 100, redondeado al entero mas cercano.
		int aplicarPorcentaje(int valor, int porcentaje) const;
	public:
		DatosDificultad();
		DatosDificultad(int _numero, const std::string& _nombre, const std::string& _descripcion,
			int _porcentajeVidaCodice, int _porcentajeVidaEnemigos, int _porcentajeDanioEnemigos, int _manaPorTurno);
		int getNumero() const;
		std::string getNombre() const;
		std::string getDescripcion() const;
		int getPorcentajeVidaCodice() const;
		int getPorcentajeVidaEnemigos() const;
		int getPorcentajeDanioEnemigos() const;
		int getManaPorTurno() const;
		// Los valores de Datos/ ajustados a esta dificultad.
		int ajustarVidaCodice(int vidaBase) const;
		int ajustarVidaEnemigo(int vidaBase) const;
		int ajustarDanioEnemigo(int danioBase) const;
};
