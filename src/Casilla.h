#pragma once

enum class TipoTerreno {
	SUELO,
	SUELO_OCUPADO,
	COLUMNAROTA,
	ABISMO,
	MURO // muro de fuego del mago: dura unos turnos y despues vuelve a ser SUELO
};

class Casilla {
	private:
		TipoTerreno tipo;
	public:
		Casilla(TipoTerreno t = TipoTerreno::SUELO);
		TipoTerreno getTipoTerreno() const;
		void setTipoTerreno(TipoTerreno _tipoTerreno);
		// Si se puede caminar por ella: solo el suelo comun (no las columnas, el abismo, el fuego ni la casilla
		// del codice). El espectro ademas vuela por arriba de algunas: ver Espectro::puedePisar.
		bool esTransitable() const;
};