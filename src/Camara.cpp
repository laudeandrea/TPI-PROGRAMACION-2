#include "Camara.h"

Camara::Camara(sf::Vector2f tamanioMundo, float margen) {
	// El margen se suma de los dos lados, por eso va 2 veces
	areaVisible = sf::Vector2f(tamanioMundo.x + 2 * margen, tamanioMundo.y + 2 * margen);

	// Mira al centro del tablero (el margen es parejo, asi que el centro no cambia)
	vista.setCenter(tamanioMundo.x / 2, tamanioMundo.y / 2);
	vista.setSize(areaVisible);
};

// Agranda la vista del lado que sobra para que tenga la misma proporcion que la ventana (asi no se deforma)
void Camara::ajustarAVentana(sf::Vector2u tamanioVentana) {
	float proporcionVentana = (float)tamanioVentana.x / tamanioVentana.y;
	float proporcionArea = areaVisible.x / areaVisible.y;

	if (proporcionVentana > proporcionArea) {
		// Ventana más ancha = sobra espacio a los costados
		vista.setSize(areaVisible.y * proporcionVentana, areaVisible.y);
	}
	else {
		// Ventana más alta = sobra espacio arriba y abajo
		vista.setSize(areaVisible.x, areaVisible.x / proporcionVentana);
	}
};

void Camara::aplicar(sf::RenderWindow& ventana) const {
	ventana.setView(vista);
};
