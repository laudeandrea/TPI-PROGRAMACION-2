#pragma once

/* El registro de partida.dat: en que nivel va el jugador y, si dejo un nivel por la mitad, como estaba
*  el tablero (oleada, vida del codice y muros de fuego). Tambien la dificultad que se eligio al empezar. El mago y los enemigos se guardan aparte,
*  en Mago.dat y Enemigos.dat (ver PartidaArchivo). */
class Partida {
public:
	static const int MAX_MUROS = 12; // casillas de fuego que se pueden guardar a la vez
private:
	int nivel;      // nivel a jugar (1 = desde el principio)
	int dificultad; // 1 = Facil, 2 = Normal, 3 = Dificil (ver Dificultades.dat)
	bool enCurso;   // false: el nivel todavia no empezo (se arranca de cero). true: se dejo por la mitad
	int oleada;     // empieza en 1 (solo vale si enCurso)
	int vidaCodice;
	int cantidadMuros;
	int muros[MAX_MUROS][3]; // cada casilla de fuego: fila, columna y turnos que le quedan
public:
	Partida(int _nivel = 1, int _dificultad = 2); // 2 = Normal
	int getNivel() const;
	void setNivel(int _nivel);
	int getDificultad() const;
	bool getEnCurso() const;
	void setEnCurso(bool _enCurso);
	int getOleada() const;
	void setOleada(int _oleada);
	int getVidaCodice() const;
	void setVidaCodice(int _vidaCodice);
	int getCantidadMuros() const;
	// Agrega una casilla de fuego. Si ya hay MAX_MUROS, no la guarda.
	void agregarMuro(int fila, int columna, int turnosRestantes);
	int getFilaMuro(int indice) const;
	int getColumnaMuro(int indice) const;
	int getTurnosMuro(int indice) const;
};
