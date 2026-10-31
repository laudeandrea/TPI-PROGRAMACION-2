#pragma once

enum class TipoTerreno {
	SUELO,
	COLUMNAROTA,
	ABISMO
};

class Casilla {
	private:
		TipoTerreno tipo;
	public:
		Casilla(TipoTerreno t = TipoTerreno::SUELO);
		TipoTerreno getTipoTerreno();
		void setTipoTerreno(TipoTerreno _tipoTerreno);
		bool esTransitable();
};