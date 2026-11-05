#pragma once

enum class TipoTerreno {
	SUELO,
	SUELO_OCUPADO,
	COLUMNAROTA,
	ABISMO
};

class Casilla {
	private:
		TipoTerreno tipo;
	public:
		Casilla(TipoTerreno t = TipoTerreno::SUELO);
		TipoTerreno getTipoTerreno() const;
		void setTipoTerreno(TipoTerreno _tipoTerreno);
};