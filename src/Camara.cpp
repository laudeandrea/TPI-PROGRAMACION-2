#include "Camara.h"

Camara::Camara(sf::Vector2f tamanioMundo, float margen) {
	// El margen se suma de los dos lados, por eso va 2 veces
	areaVisible = sf::Vector2f(tamanioMundo.x + 2 * margen, tamanioMundo.y + 2 * margen);

	// Mira al centro del tablero (el margen es parejo, asi que el centro no cambia)
	vista.setCenter(tamanioMundo.x / 2, tamanioMundo.y / 2);
	vista.setSize(areaVisible);

	// Viewport: en que pedazo de la ventana se dibuja esta vista, en proporcion (0 a 1).
	// (0, 0, 0.75, 1) = desde la izquierda, el 75 % del ancho y todo el alto. A la derecha queda lugar para el HUD
	vista.setViewport(sf::FloatRect(0.f, 0.f, PARTE_TABLERO, 1.f));
};

// Agranda la vista del lado que sobra para que tenga la misma proporcion que su pedazo de ventana (asi no se deforma)
void Camara::ajustarAVentana(sf::Vector2u tamanioVentana) {
	// El tablero no usa toda la ventana: solo el ancho de su viewport
	float proporcionVentana = (tamanioVentana.x * PARTE_TABLERO) / tamanioVentana.y;
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

const sf::View& Camara::getVista() const {
	return vista;
};
