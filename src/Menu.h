#pragma once
#include <SFML/Graphics.hpp>

class Menu {
    private:
        sf::Texture texturaFondo, texturaTitulo, texturaSelector; // Imagenes cargadas
        sf::Sprite fondo, titulo, selector; // Lo que se dibuja, cada uno con su posicion
        sf::Font fuente; 
        sf::Text opciones[4]; // 0 = Continuar, 1 = Nueva partida, 2 = Como jugar, 3 = Salir
        bool hayPartida; // si existe, se puede elegir "Continuar"
        sf::View vista; // Vista fija de 1280x720
        int seleccionada; // Indice de la opción
        static constexpr float Y_PRIMERA_OPCION = 520.f; // centro de "Continuar"
        static constexpr float SEPARACION_OPCIONES = 40.f; // distancia entre opciones
    public:
        Menu();
        int procesarEvento(const sf::Event& evento, const sf::RenderWindow& ventana); // flechas o mouse eligen la opcion. Enter o click devuelven la opción elegida, sino -1.
        void dibujar(sf::RenderWindow& ventana); // dibuja fondo, titulo, opciones y selector
		void setHayPartida(bool _hayPartida); // si hay partida guardada, se puede elegir "Continuar"
};  