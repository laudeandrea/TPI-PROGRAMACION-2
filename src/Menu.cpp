#include "Menu.h"
#include <string>
#include <cmath>

Menu::Menu(){
    // Carga las imagenes
    texturaFondo.loadFromFile("Imagenes/menu_fondo.png");
    texturaSelector.loadFromFile("Imagenes/menu_selector.png");
    texturaTitulo.loadFromFile("Imagenes/menu_titulo.png");
    fuente.loadFromFile("Fuentes/PixelifySans-Regular.ttf");    

    // Asigna cada textura a su sprite y lo ubica
    fondo.setTexture(texturaFondo);
    selector.setTexture(texturaSelector);
    titulo.setTexture(texturaTitulo);

    titulo.setPosition(138, 48);
    
    // Arma la vista fija
    vista = sf::View(sf::FloatRect(0, 0, 800, 600));

    // Se inicializa así no arranca con un valor basura
    seleccionada = 0;

    // Textos de las opciones (sin tildes, asi SFML los muestra bien sin convertirlos)
    std::string nombres[3] = { "Nueva partida", "Como jugar", "Salir" };
    
    for (int i=0; i<3;i++){
        opciones[i].setFont(fuente);
        opciones[i].setString(nombres[i]);
        opciones[i].setCharacterSize(22);
        opciones[i].setFillColor(sf::Color(184, 178, 164)); // Piedra

        // El origen va al medio del texto, asi setPosition ubica su centro
        sf::FloatRect caja = opciones[i].getLocalBounds();
        // std:: floor = redondea para abajo, saca los decimales
        opciones[i].setOrigin(std::floor(caja.left + caja.width / 2),
                            std::floor(caja.top + caja.height / 2));
        opciones[i].setPosition(400.f, 400.f + i * 40.f);
    }
};

int Menu::procesarEvento(const sf::Event& evento, const sf::RenderWindow& ventana){
    if (evento.type == sf::Event::KeyPressed) {
        if (evento.key.code == sf::Keyboard::Down) {
            seleccionada = (seleccionada + 1) % 3;      // baja; desde "Salir" vuelve a la primera
        }
        if (evento.key.code == sf::Keyboard::Up) {
            seleccionada = (seleccionada + 2) % 3;      // sube; desde la primera va a "Salir"
        }
        if (evento.key.code == sf::Keyboard::Enter) {
            return seleccionada;                        // avisa qué opción se eligió
        }
    }

    // Mouse encima de una opcion: queda elegida
    if (evento.type == sf::Event::MouseMoved) {
        // El pixel de la ventana se pasa a coordenadas del menu (sirve aunque la ventana cambie de tamaño)
        sf::Vector2i pixel(evento.mouseMove.x, evento.mouseMove.y);
        sf::Vector2f punto = ventana.mapPixelToCoords(pixel, vista);

        for (int i = 0; i < 3; i++) {
            if (opciones[i].getGlobalBounds().contains(punto)) {
                seleccionada = i;
            }
        }
    }

    // Click izquierdo sobre una opcion: se elige esa
    if (evento.type == sf::Event::MouseButtonPressed && evento.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2i pixel(evento.mouseButton.x, evento.mouseButton.y);
        sf::Vector2f punto = ventana.mapPixelToCoords(pixel, vista);

        for (int i = 0; i < 3; i++) {
            if (opciones[i].getGlobalBounds().contains(punto)) {
                return i;
            }
        }
    }

    return -1;                                         // no se eligió nada
};

void Menu::dibujar(sf::RenderWindow& ventana){
    ventana.setView(vista);
    ventana.draw(fondo);
    ventana.draw(titulo);

    // Dibuja las opciones: la elegida en Gema, las demas en Piedra
    for (int i=0;i<3;i++){
        if (i == seleccionada) {
            opciones[i].setFillColor(sf::Color(95, 224, 232));   // Gema
        } else {
            opciones[i].setFillColor(sf::Color(184, 178, 164));  // Piedra
        }
        ventana.draw(opciones[i]);
    }

    // Selector: 14 px a la izquierda del texto elegido, centrado en su altura
    sf::FloatRect caja = opciones[seleccionada].getGlobalBounds();
    selector.setPosition(caja.left - 14.f - 18.f, 400.f + seleccionada * 40.f - 9.f);
    ventana.draw(selector);
};