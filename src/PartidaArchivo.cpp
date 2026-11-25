#include "PartidaArchivo.h"

PartidaArchivo::PartidaArchivo()
	: archivoPartida("partida.dat"), archivoMago("Mago.dat"), archivoEnemigos("Enemigos.dat") {
}

bool PartidaArchivo::existe() const {
	return archivoPartida.existe();
}

bool PartidaArchivo::guardar(const Partida& partida) const {
	// El nivel arranca de cero: si quedaron el mago y los enemigos de antes, se borran
	archivoMago.borrar();
	archivoEnemigos.borrar();
	return archivoPartida.guardarTodos({ partida }); // { partida } = un vector con un solo registro
}

bool PartidaArchivo::guardarEnCurso(const Partida& partida, const MagoGuardado& mago,
	const std::vector<EnemigoGuardado>& enemigos) const {
	// Los tres tienen que salir bien: con "&&", si falla uno no se sigue
	return archivoPartida.guardarTodos({ partida })
		&& archivoMago.guardarTodos({ mago })
		&& archivoEnemigos.guardarTodos(enemigos);
}

Partida PartidaArchivo::leer() const {
	Partida partida; // nivel 1, sin empezar
	// Si el archivo esta roto o es de una version vieja del juego (era de 4 bytes), no se puede leer
	// un registro entero: se arranca del nivel 1
	if (!archivoPartida.leerPrimero(partida)) {
		return Partida();
	}
	// Si quedo por la mitad pero falta Mago.dat, se empieza ese nivel desde el principio
	if (partida.getEnCurso() && !archivoMago.existe()) {
		partida.setEnCurso(false);
	}
	return partida;
}

MagoGuardado PartidaArchivo::leerMago() const {
	MagoGuardado mago;
	archivoMago.leerPrimero(mago);
	return mago;
}

std::vector<EnemigoGuardado> PartidaArchivo::leerEnemigos() const {
	return archivoEnemigos.leerTodos();
}

bool PartidaArchivo::borrar() const {
	archivoMago.borrar();
	archivoEnemigos.borrar();
	return archivoPartida.borrar();
}
