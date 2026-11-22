#pragma once
#include "Partida.h"

class PartidaArchivo {
private:
	static constexpr const char* NOMBRE_ARCHIVO = "partida.dat";
public:
	bool existe() const;
	bool guardar(const Partida& partida) const;
	Partida leer() const;
	int getCantidadRegistros() const;
	bool borrar() const;
};