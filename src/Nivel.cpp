#include "Nivel.h"

// El codice se crea en cualquier casilla: iniciar() lo pone donde dice el nivel, con su vida
Nivel::Nivel(int _numero, const Catalogo& _catalogo, int _dificultad)
	: catalogo(&_catalogo), // & = la direccion del catalogo (se guarda un puntero, no una copia)
	  numero(_numero), dificultad(_dificultad), oleada(0), completado(false),
	  codice(0, 0, TipoHabilidad::BOLADEENERGIA) {
}

void Nivel::iniciar() {
	oleada = 0;
	completado = false;
	enemigos.clear();

	// El tablero nuevo pisa al anterior (y con el, los muros de fuego que hubieran quedado)
	mapa.cargar(getDisenio());

	// El codice, con la vida del nivel ajustada por la dificultad (en Facil, mas; en Dificil, menos)
	int vidaCodice = catalogo->getDificultad(dificultad).ajustarVidaCodice(getVidaCodice());
	codice = Pergamino(getFilaCodice(), getColumnaCodice(), TipoHabilidad::BOLADEENERGIA, vidaCodice);
	// La casilla donde esta el codice queda ocupada: nadie la puede pisar
	mapa.setTipoTerreno(codice.getFila(), codice.getColumna(), TipoTerreno::SUELO_OCUPADO);
}

const DatosNivel& Nivel::getDatos() const {
	return catalogo->getNivel(numero); // -> en vez de . porque catalogo es un puntero
}

int Nivel::getNumero() const {
	return numero;
}

int Nivel::getCantNiveles() const {
	return catalogo->getCantNiveles();
}

bool Nivel::hayOtroNivel() const {
	return numero < getCantNiveles();
}

// ---------------------------------------------------------------- Tablero y posiciones

std::vector<std::string> Nivel::getDisenio() const {
	return getDatos().getDisenio();
}

int Nivel::getFilaCodice() const {
	return getDatos().getFilaCodice();
}

int Nivel::getColumnaCodice() const {
	return getDatos().getColumnaCodice();
}

int Nivel::getFilaMago() const {
	return getDatos().getFilaMago();
}

int Nivel::getColumnaMago() const {
	return getDatos().getColumnaMago();
}

int Nivel::getVidaCodice() const {
	return getDatos().getVidaCodice();
}

std::vector<std::string> Nivel::getHistoria() const {
	return getDatos().getHistoria();
}

// ---------------------------------------------------------------- Oleadas

int Nivel::getOleada() const {
	return oleada + 1;
}

void Nivel::setOleada(int _oleada) {
	oleada = _oleada - 1;
	if (oleada < 0) oleada = 0;
	if (oleada > getCantOleadas() - 1) oleada = getCantOleadas() - 1;
}

int Nivel::getCantOleadas() const {
	return getDatos().getCantOleadas();
}

bool Nivel::hayOtraOleada() const {
	return oleada < getCantOleadas() - 1;
}

void Nivel::siguienteOleada() {
	oleada++;
}

std::vector<std::unique_ptr<Enemigo>> Nivel::crearOleada() const {
	std::vector<std::unique_ptr<Enemigo>> nuevos;
	// En NivelEnemigos.dat las oleadas empiezan en 1 (por eso getOleada() y no "oleada")
	for (const DatosAparicion& aparicion : catalogo->getApariciones(numero, getOleada())) {
		nuevos.push_back(catalogo->crearEnemigo(aparicion.getTipo(), aparicion.getFila(), aparicion.getColumna(),
			catalogo->getDificultad(dificultad)));
	}
	return nuevos; // el vector se "mueve" al que lo recibe, no se copia (los unique_ptr no se pueden copiar)
}

void Nivel::completar() {
	completado = true;
}

bool Nivel::estaCompletado() const {
	return completado;
}

// ---------------------------------------------------------------- Lo que tiene el nivel

Mapa& Nivel::getMapa() {
	return mapa;
}

const Mapa& Nivel::getMapa() const {
	return mapa;
}

std::vector<std::unique_ptr<Enemigo>>& Nivel::getEnemigos() {
	return enemigos;
}

const std::vector<std::unique_ptr<Enemigo>>& Nivel::getEnemigos() const {
	return enemigos;
}

Pergamino& Nivel::getCodice() {
	return codice;
}

const Pergamino& Nivel::getCodice() const {
	return codice;
}
