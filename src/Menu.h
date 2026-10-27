#pragma once
#include <SFML/graphics.hpp>

class Menu {
    private:
        sf::Texture texturaFondo, texturaTitulo, texturaSelector; // Imagenes cargadas
        sf::Sprite fondo, titulo, selector; // Lo que se dibuja, cada uno con su posicion
        sf::Font fuente; 
        sf::Text opciones[3]; // Opciones del menu (como todavía no hay guardado, no se agregó la opción "Continuar")
        sf::View vista; // Vista fija de 800x600 (moldeable)
        int seleccionada; // Indice de la opción
    public:
        Menu();
        int procesarEvento(const sf::Event& evento, const sf::RenderWindow& ventana); // flechas o mouse eligen la opcion. Enter o click devuelven la opción elegida, sino -1.
        void dibujar(sf::RenderWindow& ventana); // dibuja fondo, titulo, opciones y selector
};  