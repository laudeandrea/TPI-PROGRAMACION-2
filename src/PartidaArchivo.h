#pragma once
#include <vector>
#include "Archivo.h"
#include "Partida.h"
#include "MagoGuardado.h"
#include "EnemigoGuardado.h"

/* La partida guardada son tres archivos, en la carpeta desde donde se ejecuta el juego:
*   partida.dat   -> un registro Partida (nivel, si quedo por la mitad, oleada, codice y muros)
*   Mago.dat      -> un registro MagoGuardado
*   Enemigos.dat  -> un registro EnemigoGuardado por cada enemigo que quedaba en el tablero
*  Si el nivel no habia empezado (por ejemplo, se guardo entre un nivel y otro), solo existe partida.dat. */
class PartidaArchivo {
private:
	Archivo<Partida> archivoPartida;
	Archivo<MagoGuardado> archivoMago;
	Archivo<EnemigoGuardado> archivoEnemigos;
public:
	PartidaArchivo();
	bool existe() const;
	// Guarda un nivel que todavia no empezo (borra Mago.dat y Enemigos.dat, que no hacen falta).
	bool guardar(const Partida& partida) const;
	// Guarda un nivel por la mitad: los tres archivos.
	bool guardarEnCurso(const Partida& partida, const MagoGuardado& mago,
		const std::vector<EnemigoGuardado>& enemigos) const;
	// Si no hay partida (o el archivo esta roto), devuelve una que arranca en el nivel 1.
	Partida leer() const;
	MagoGuardado leerMago() const;
	std::vector<EnemigoGuardado> leerEnemigos() const;
	// Borra los tres archivos.
	bool borrar() const;
};
