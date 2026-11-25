#include "DatosNivel.h"
#include <cstring>

DatosNivel::DatosNivel()
	: numero(0), filaCodice(0), columnaCodice(0), filaMago(0), columnaMago(0), cantOleadas(1), vidaCodice(80) {
	for (int fila = 0; fila < CANT_CASILLAS; fila++) {
		strncpy_s(disenio[fila], sizeof(disenio[fila]), "SSSSSSSS", _TRUNCATE);
	}
	for (int i = 0; i < MAX_LINEAS_HISTORIA; i++) {
		historia[i][0] = '\0';
	}
}

DatosNivel::DatosNivel(int _numero, const std::vector<std::string>& _disenio, int _filaCodice, int _columnaCodice,
	int _filaMago, int _columnaMago, int _cantOleadas, int _vidaCodice, const std::vector<std::string>& _historia)
	: DatosNivel() { // primero hace lo del constructor vacio (tablero de suelo, historia vacia)...
	// ...y despues pisa con lo que llego
	numero = _numero;
	filaCodice = _filaCodice;
	columnaCodice = _columnaCodice;
	filaMago = _filaMago;
	columnaMago = _columnaMago;
	cantOleadas = _cantOleadas;
	vidaCodice = _vidaCodice;

	for (int fila = 0; fila < CANT_CASILLAS && fila < (int)_disenio.size(); fila++) {
		strncpy_s(disenio[fila], sizeof(disenio[fila]), _disenio[fila].c_str(), _TRUNCATE);
	}
	for (int i = 0; i < MAX_LINEAS_HISTORIA && i < (int)_historia.size(); i++) {
		strncpy_s(historia[i], sizeof(historia[i]), _historia[i].c_str(), _TRUNCATE);
	}
}

int DatosNivel::getNumero() const {
	return numero;
}

std::vector<std::string> DatosNivel::getDisenio() const {
	std::vector<std::string> filas;
	for (int fila = 0; fila < CANT_CASILLAS; fila++) {
		filas.push_back(std::string(disenio[fila]));
	}
	return filas;
}

int DatosNivel::getFilaCodice() const {
	return filaCodice;
}

int DatosNivel::getColumnaCodice() const {
	return columnaCodice;
}

int DatosNivel::getFilaMago() const {
	return filaMago;
}

int DatosNivel::getColumnaMago() const {
	return columnaMago;
}

int DatosNivel::getCantOleadas() const {
	return cantOleadas;
}

int DatosNivel::getVidaCodice() const {
	return vidaCodice;
}

std::vector<std::string> DatosNivel::getHistoria() const {
	std::vector<std::string> lineas;
	for (int i = 0; i < MAX_LINEAS_HISTORIA; i++) {
		if (historia[i][0] != '\0') {
			lineas.push_back(std::string(historia[i]));
		}
	}
	return lineas;
}
