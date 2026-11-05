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
        static constexpr float Y_PRIMERA_OPCION = 520.f; // centro de "Nueva partida"
        static constexpr float SEPARACION_OPCIONES = 40.f; // distancia entre opciones
    public:
        Menu();
        int procesarEvento(const sf::Event& evento, const sf::RenderWindow& ventana); // flechas o mouse eligen la opcion. Enter o click devuelven la opción elegida, sino -1.
        void dibujar(sf::RenderWindow& ventana); // dibuja fondo, titulo, opciones y selector
};  