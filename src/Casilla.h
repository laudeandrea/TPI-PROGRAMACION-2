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
};