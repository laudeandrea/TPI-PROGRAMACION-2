#include "Enemigo.h"
#include <cmath>

Enemigo::Enemigo(int _fila, int _columna, int _vida, int _danio)
	: Personaje(_fila, _columna, _vida), danio(_danio), tieneAnuncio(false) {
}

int Enemigo::getDanio() const {
	return danio;
}

bool Enemigo::puedeIrA(const Mapa& mapa, int fila, int columna) const {
	if (!mapa.esValida(fila, columna)) {
		return false;   // fuera del tablero: nunca, porque getTipoTerreno no valida
	}
	return puedePisar(mapa.getTipoTerreno(fila, columna));
};

sf::Vector2i Enemigo::calcularPaso(const Mapa& mapa, int filaObjetivo, int columnaObjetivo) const {
	int dFila = filaObjetivo - getFila();
	int dColumna = columnaObjetivo - getColumna();

	sf::Vector2i actual(columna, fila);   // (x = columna, y = fila)

	if (puedeAtacar(filaObjetivo, columnaObjetivo)) return actual; // ya lo alcanza, no se mueve

	int pasoFila = 0, pasoColumna = 0;
	if (dFila > 0) {
		pasoFila = 1;
	}
	else if (dFila < 0) {
		pasoFila = -1;
	}
	else {
		pasoFila = 0;
	}

	if (dColumna > 0) {
		pasoColumna = 1;
	}
	else if (dColumna < 0) {
		pasoColumna = -1;
	}
	else {
		pasoColumna = 0;
	}

	sf::Vector2i primero, segundo;

	if (std::abs(dFila) >= std::abs(dColumna)) {
		primero = sf::Vector2i(columna, fila + pasoFila);
		segundo = sf::Vector2i(columna + pasoColumna, fila);
	}
	else {
		primero = sf::Vector2i(columna + pasoColumna, fila);
		segundo = sf::Vector2i(columna, fila + pasoFila);
	}

	if (puedeIrA(mapa, primero.y, primero.x)) return primero;
	if (puedeIrA(mapa, segundo.y, segundo.x)) return segundo;

	return actual;
}

int Enemigo::getAlcanceAtaque() const {
	return 1;
}

bool Enemigo::puedeAtacar(int filaObjetivo, int columnaObjetivo) const{
	int distancia = std::abs(filaObjetivo - fila) + std::abs(columnaObjetivo - columna);
	return distancia >= 1 && distancia <= getAlcanceAtaque(); // virtual: cada enemigo dice su alcance
}

Ataque Enemigo::planearAtaque(int filaObjetivo, int columnaObjetivo) const {
	Ataque ataque;
	ataque.casillas.push_back(sf::Vector2i(columnaObjetivo, filaObjetivo));
	ataque.danio = danio;
	return ataque;
}

Ataque Enemigo::ataqueAlMorir() const {
	return Ataque(); // sin casillas: no hace nada
}

bool Enemigo::atacaADistancia() const {
	return false;
}

void Enemigo::anunciarAtaque(const Ataque& ataque) {
	ataqueAnunciado = ataque;
	tieneAnuncio = true;
}

void Enemigo::cancelarAtaque() {
	ataqueAnunciado = Ataque();
	tieneAnuncio = false;
}

bool Enemigo::tieneAtaqueAnunciado() const {
	return tieneAnuncio;
}

const Ataque& Enemigo::getAtaqueAnunciado() const {
	return ataqueAnunciado;
}

Ataque Enemigo::tomarAtaqueAnunciado() {
	Ataque ataque = ataqueAnunciado; // se copia antes de borrarlo
	cancelarAtaque();
	return ataque;
}

// ---------------------------------------------------------------- Caminar

// Busqueda "en anchura" (BFS): primero todas las casillas a 1 paso, despues las de 2, y asi, anotando desde que
// casilla se llego a cada una. La primera casilla desde la que le llega el ataque es la mas cercana; desde ahi se
// vuelve para atras por el camino hasta el primer paso. Como solo avanza por casillas que puede pisar y que estan
// libres, rodea los pozos, las columnas, el fuego y a los otros personajes.
sf::Vector2i Enemigo::pasoHacia(const Mapa& mapa, const std::vector<std::vector<bool>>& ocupadas,
	int filaObjetivo, int columnaObjetivo) const {
	int n = mapa.getCantCasillas();
	sf::Vector2i inicio(columna, fila); // x = columna, y = fila

	// desde[fila][columna] = la casilla de la que se vino; (-1, -1) = todavia no se llego
	std::vector<std::vector<sf::Vector2i>> desde(n, std::vector<sf::Vector2i>(n, sf::Vector2i(-1, -1)));
	desde[inicio.y][inicio.x] = inicio;

	std::vector<sf::Vector2i> encontradas;
	encontradas.push_back(inicio);

	// Las 4 direcciones en las que se puede dar un paso (x = columna, y = fila)
	const sf::Vector2i direcciones[4] = { {1, 0}, {-1, 0}, {0, 1}, {0, -1} };

	for (size_t i = 0; i < encontradas.size(); i++) {
		sf::Vector2i actual = encontradas[i];

		// Desde aca le llega el ataque: es la casilla buena mas cercana (la BFS las encuentra en orden de distancia)
		int distanciaAlObjetivo = std::abs(actual.y - filaObjetivo) + std::abs(actual.x - columnaObjetivo);
		if (distanciaAlObjetivo >= 1 && distanciaAlObjetivo <= getAlcanceAtaque()) {
			// Vuelve para atras por el camino hasta la casilla que esta pegada al inicio: ese es el paso
			while (desde[actual.y][actual.x] != inicio && actual != inicio) {
				actual = desde[actual.y][actual.x];
			}
			return actual;
		}

		for (const sf::Vector2i& direccion : direcciones) {
			sf::Vector2i vecina = actual + direccion;
			// puedeIrA mira primero que este dentro del tablero (asi no se lee "ocupadas" fuera de rango)
			if (!puedeIrA(mapa, vecina.y, vecina.x) || ocupadas[vecina.y][vecina.x]) continue;
			if (desde[vecina.y][vecina.x].x != -1) continue; // ya la habiamos encontrado

			desde[vecina.y][vecina.x] = actual;
			encontradas.push_back(vecina);
		}
	}

	// No hay ningun camino libre (lo tapan otros personajes): se acerca en linea recta
	return calcularPaso(mapa, filaObjetivo, columnaObjetivo);
}

bool Enemigo::darPaso(const Mapa& mapa, const std::vector<std::vector<bool>>& ocupadas, int filaObjetivo, int columnaObjetivo) {
	sf::Vector2i paso = pasoHacia(mapa, ocupadas, filaObjetivo, columnaObjetivo); // x = columna, y = fila
	if (estaEn(paso.y, paso.x) || ocupadas[paso.y][paso.x]) {
		return false; // ya esta donde quiere, o la casilla esta tapada (pasa si no habia camino libre)
	}
	// Mapa::mover lo mueve solo si la casilla existe y la puede pisar
	return mapa.mover(*this, paso.y, paso.x);
}
