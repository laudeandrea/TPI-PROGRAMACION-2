#include "Ayuda.h"
#include <cmath>

// Colores de la paleta (ver CONTRIBUTING.md)
static const sf::Color FONDO(11, 13, 26);
static const sf::Color PIZARRA(58, 66, 88);
static const sf::Color PIEDRA(184, 178, 164);
static const sf::Color TENUE(92, 101, 124);
static const sf::Color GEMA(95, 224, 232);

// Medidas de la pantalla (ver "Pantalla Como jugar" en CONTRIBUTING.md e Imagenes/vista_previa_ayuda.png)
static const float X_IZQUIERDA = 160.f, X_DERECHA = 1120.f; // donde empieza y termina el contenido del panel
static const float Y_TITULO = 100.f;                        // centro del titulo
static const float Y_LINEA_TITULO = 140.f, Y_LINEA_PIE = 596.f; // las dos lineas que separan las partes
static const float X_DIBUJO = 360.f, Y_DIBUJO = 375.f;     // centro del dibujo
static const float X_TEXTO = 600.f, Y_TEXTO = 168.f;       // primera linea del texto
static const float ALTO_LINEA = 30.f;                      // entre una linea y la siguiente (igual que el HUD)
static const float Y_PIE = 616.f;                          // textos de abajo
static const float X_FLECHA_ANTERIOR = 580.f, X_FLECHA_SIGUIENTE = 700.f, Y_FLECHAS = 620.f;

Ayuda::Ayuda(const Catalogo& catalogo) {
    // Atras se ve el fondo del menu, oscurecido (ver dibujar)
    texturaFondo.loadFromFile("Imagenes/menu_fondo.png");
    fondo.setTexture(texturaFondo);

    texturaFlecha.loadFromFile("Imagenes/ayuda_flecha.png");
    flechaSiguiente.setTexture(texturaFlecha);
    flechaSiguiente.setPosition(X_FLECHA_SIGUIENTE, Y_FLECHAS);
    // La anterior es la misma imagen dada vuelta: con escala -1 en x se espeja,
    // y queda dibujada hacia la IZQUIERDA de su posicion (de 568 a 580)
    flechaAnterior.setTexture(texturaFlecha);
    flechaAnterior.setScale(-1.f, 1.f);
    flechaAnterior.setPosition(X_FLECHA_ANTERIOR, Y_FLECHAS);

    fuente.loadFromFile("Fuentes/PixelifySans-Regular.ttf");
    vista = sf::View(sf::FloatRect(0, 0, 1280, 720));
    paginaActual = 0;

    // Los textos van sin tildes, como en el resto del juego (SFML no las muestra bien sin convertirlas).
    // Los numeros de las reglas salen del catalogo (Datos/*.dat): si se cambia un valor ahi, la ayuda se entera sola.
    // Cada linea tiene que entrar en 520 px (de x = 600 a x = 1120): unos 42 caracteres. Entran 13 lineas.
    const DatosHabilidad& bola = catalogo.getDatosHabilidad(TipoHabilidad::BOLADEENERGIA);
    const DatosHabilidad& empuje = catalogo.getDatosHabilidad(TipoHabilidad::EMPUJE);
    const DatosHabilidad& muro = catalogo.getDatosHabilidad(TipoHabilidad::MURO);
    const DatosEnemigo& caballero = catalogo.getDatosEnemigo(TipoEnemigo::CABALLERO);
    const DatosEnemigo& espectro = catalogo.getDatosEnemigo(TipoEnemigo::ESPECTRO);
    const DatosEnemigo& golem = catalogo.getDatosEnemigo(TipoEnemigo::GOLEM);

    paginas[0].titulo = "EL OBJETIVO";
    paginas[0].lineas = {
        "#Defender el codice",
        "Los enemigos vienen a destruir el codice,",
        "el libro magico sobre el atril. Se pierde",
        "si lo destruyen o si el mago muere.",
        "",
        "#Pisos y oleadas",
        "La torre tiene " + std::to_string(catalogo.getCantNiveles()) + " pisos, y en cada uno",
        "llegan oleadas: al matar a todos, llega",
        "la siguiente. Se guarda solo cada turno.",
        "",
        "#Dificultad",
        "Se elige al empezar. Los numeros de esta",
        "ayuda son los de Normal."
    };

    // El movimiento (3) y el mana del mago (10) estan en Mago.h; el +2 por turno es el de Normal (Dificultades.dat)
    paginas[1].titulo = "TU TURNO";
    paginas[1].lineas = {
        "#Moverse",
        "Click en el mago: se marcan las casillas",
        "a las que puede ir (hasta 3 pasos).",
        "Click en una de ellas para moverlo.",
        "Se mueve una sola vez por turno.",
        "",
        "#Actuar",
        "Despues de moverse puede usar una",
        "habilidad: eso termina el turno.",
        "[Espacio] termina el turno sin atacar.",
        "",
        "#Mana",
        "Empieza con 10 y recupera 2 por turno."
    };

    paginas[2].titulo = "ATAQUES ANUNCIADOS";
    paginas[2].lineas = {
        "#Las casillas rojas",
        "Cada enemigo marca en rojo la casilla a",
        "la que va a pegar y cuanta vida saca.",
        "Arriba de su cabeza aparece un \"!\".",
        "",
        "#El golpe llega en su proximo turno",
        "Si el mago sale de ahi, no le pasa nada.",
        "El codice no se mueve: hay que matar o",
        "empujar al enemigo antes de que pegue.",
        "",
        "El golpe le pega a cualquiera que este",
        "en la casilla, tambien a otro enemigo."
    };

    paginas[3].titulo = "HABILIDADES";
    paginas[3].lineas = {
        "#[1] Bola de energia",
        "Le saca " + std::to_string(bola.getDanio()) + " de vida a un enemigo que",
        "este a " + std::to_string(bola.getAlcance()) + " casillas o menos.",
        "Gasta " + std::to_string(bola.getCostoMana()) + " de mana.",
        "",
        "#[2] Empuje",
        "Le saca " + std::to_string(empuje.getDanio()) + " a un enemigo pegado, lo corre",
        "una casilla y le cancela el ataque.",
        "Si cae al abismo, no vuelve (el espectro",
        "flota). Gasta " + std::to_string(empuje.getCostoMana()) + " de mana.",
        "",
        "Repetir la tecla (o el click en el panel)",
        "cancela la habilidad."
    };

    paginas[4].titulo = "MURO DE FUEGO";
    paginas[4].lineas = {
        "#[3] Desde el piso " + std::to_string(muro.getNivelDesbloqueo()),
        "Prende 3 casillas por " + std::to_string(muro.getDuracion()) + " turnos. Nadie",
        "lo cruza, salvo el espectro, que pierde",
        std::to_string(muro.getDanio()) + " de vida cada vez que se quema.",
        "Gasta " + std::to_string(muro.getCostoMana()) + " de mana.",
        "",
        "#Donde queda",
        "Se elige una casilla vacia a " + std::to_string(muro.getAlcance()) + " o menos",
        "del mago. El fuego queda de costado, en",
        "esa casilla y en sus dos vecinas: con el",
        "mouse arriba se ve en rojo donde va."
    };

    paginas[5].titulo = "ENEMIGOS";
    paginas[5].lineas = {
        "#Caballero (" + std::to_string(caballero.getVida()) + " de vida)",
        "Pega cuerpo a cuerpo: saca " + std::to_string(caballero.getDanio()) + ".",
        "",
        "#Espectro (" + std::to_string(espectro.getVida()) + " de vida)",
        "Tira bolas desde " + std::to_string(espectro.getAlcance()) + " casillas: saca " + std::to_string(espectro.getDanio()) + ".",
        "Flota: cruza columnas y abismos.",
        "",
        "#Golem (" + std::to_string(golem.getVida()) + " de vida)",
        "Explota al llegar o al morir: saca " + std::to_string(golem.getDanio()),
        "alrededor. Al abismo cae sin explotar.",
        "",
        "Cada enemigo va por lo que tenga mas",
        "cerca: el mago o el codice."
    };

    // El dibujo de cada pagina: Imagenes/ayuda_1.png, ayuda_2.png...
    for (int i = 0; i < CANT_PAGINAS; i++) {
        paginas[i].dibujo.loadFromFile("Imagenes/ayuda_" + std::to_string(i + 1) + ".png");
    }
}

void Ayuda::reiniciar() {
    paginaActual = 0;
}

void Ayuda::pasarPagina(int cuanto) {
    paginaActual += cuanto;
    if (paginaActual < 0) paginaActual = 0;
    if (paginaActual > CANT_PAGINAS - 1) paginaActual = CANT_PAGINAS - 1;
}

bool Ayuda::procesarEvento(const sf::Event& evento, const sf::RenderWindow& ventana) {
    if (evento.type == sf::Event::KeyPressed) {
        if (evento.key.code == sf::Keyboard::Right) pasarPagina(1);
        if (evento.key.code == sf::Keyboard::Left) pasarPagina(-1);
        if (evento.key.code == sf::Keyboard::Escape) return true;
        if (evento.key.code == sf::Keyboard::Enter) {
            // Enter avanza; en la ultima pagina ya no hay mas, asi que vuelve al menu
            if (paginaActual == CANT_PAGINAS - 1) return true;
            pasarPagina(1);
        }
    }

    // Click izquierdo: en las flechas cambia de pagina, en "[Esc] Volver" vuelve al menu
    if (evento.type == sf::Event::MouseButtonPressed && evento.mouseButton.button == sf::Mouse::Left) {
        // Igual que en el Menu: el pixel de la ventana se pasa a coordenadas de esta vista
        sf::Vector2i pixel(evento.mouseButton.x, evento.mouseButton.y);
        sf::Vector2f punto = ventana.mapPixelToCoords(pixel, vista);

        // Las flechas son chicas (12x20): se agranda un poco la zona donde se puede hacer click
        sf::FloatRect zonaAnterior = flechaAnterior.getGlobalBounds();
        sf::FloatRect zonaSiguiente = flechaSiguiente.getGlobalBounds();
        zonaAnterior.left -= 10.f;  zonaAnterior.top -= 10.f;  zonaAnterior.width += 20.f;  zonaAnterior.height += 20.f;
        zonaSiguiente.left -= 10.f; zonaSiguiente.top -= 10.f; zonaSiguiente.width += 20.f; zonaSiguiente.height += 20.f;

        if (zonaAnterior.contains(punto)) pasarPagina(-1);
        if (zonaSiguiente.contains(punto)) pasarPagina(1);
        if (textoVolver().getGlobalBounds().contains(punto)) return true;
    }

    return false;
}

void Ayuda::dibujar(sf::RenderWindow& ventana) {
    ventana.setView(vista);

    // Fondo del menu, oscurecido con un rectangulo casi opaco encima
    ventana.draw(fondo);
    sf::RectangleShape velo(sf::Vector2f(1280.f, 720.f));
    velo.setFillColor(sf::Color(FONDO.r, FONDO.g, FONDO.b, 215));
    ventana.draw(velo);

    // Panel: de (120, 50) a (1160, 670). El borde de SFML se dibuja POR FUERA del rectangulo,
    // por eso el rectangulo arranca 2 px adentro
    sf::RectangleShape panel(sf::Vector2f(1036.f, 616.f));
    panel.setPosition(122.f, 52.f);
    panel.setFillColor(FONDO);
    panel.setOutlineThickness(2.f);
    panel.setOutlineColor(PIZARRA);
    ventana.draw(panel);

    const Pagina& pagina = paginas[paginaActual];

    // Titulo: x2 con setScale (y no con tamanio 44), asi la letra sigue nitida. Centrado en x = 640
    sf::Text titulo = crearTexto(pagina.titulo, GEMA);
    titulo.setScale(2.f, 2.f);
    sf::FloatRect caja = titulo.getLocalBounds();
    titulo.setOrigin(std::floor(caja.left + caja.width / 2), std::floor(caja.top + caja.height / 2));
    titulo.setPosition(640.f, Y_TITULO);
    ventana.draw(titulo);

    // Lineas que separan el titulo y el pie del resto
    sf::RectangleShape linea(sf::Vector2f(X_DERECHA - X_IZQUIERDA, 2.f));
    linea.setFillColor(PIZARRA);
    linea.setPosition(X_IZQUIERDA, Y_LINEA_TITULO);
    ventana.draw(linea);
    linea.setPosition(X_IZQUIERDA, Y_LINEA_PIE);
    ventana.draw(linea);

    // Dibujo: el origen va a su centro, asi setPosition lo centra en la columna de la izquierda
    sf::Sprite dibujo(pagina.dibujo);
    sf::Vector2u tamanio = pagina.dibujo.getSize();
    dibujo.setOrigin(std::floor(tamanio.x / 2.f), std::floor(tamanio.y / 2.f));
    dibujo.setPosition(X_DIBUJO, Y_DIBUJO);
    ventana.draw(dibujo);

    // Texto: una linea abajo de la otra. Las que empiezan con '#' son subtitulos
    for (size_t i = 0; i < pagina.lineas.size(); i++) {
        std::string contenido = pagina.lineas[i];
        sf::Color color = PIEDRA;
        if (!contenido.empty() && contenido[0] == '#') {
            contenido = contenido.substr(1); // substr(1) = todo menos el primer caracter
            color = GEMA;
        }
        sf::Text texto = crearTexto(contenido, color);
        texto.setPosition(X_TEXTO, Y_TEXTO + i * ALTO_LINEA);
        ventana.draw(texto);
    }

    // Pie: a la izquierda como volver, en el medio las flechas con el numero de pagina, a la derecha la ayuda
    ventana.draw(textoVolver());

    sf::Text numero = crearTexto(std::to_string(paginaActual + 1) + " / " + std::to_string(CANT_PAGINAS), PIEDRA);
    caja = numero.getLocalBounds();
    numero.setOrigin(std::floor(caja.left + caja.width / 2), 0.f);
    numero.setPosition(640.f, Y_PIE);
    ventana.draw(numero);

    // Si no hay pagina anterior (o siguiente), su flecha se ve apagada: setColor multiplica los colores
    // de la imagen, y con alfa 50 queda casi transparente
    sf::Color prendida(255, 255, 255, 255), apagada(255, 255, 255, 50);
    flechaAnterior.setColor(paginaActual > 0 ? prendida : apagada);
    flechaSiguiente.setColor(paginaActual < CANT_PAGINAS - 1 ? prendida : apagada);
    ventana.draw(flechaAnterior);
    ventana.draw(flechaSiguiente);

    sf::Text cambiar = crearTexto("Flechas: pagina   [-] [=]: musica", TENUE);
    caja = cambiar.getLocalBounds();
    cambiar.setPosition(std::floor(X_DERECHA - caja.width - caja.left), Y_PIE); // alineado a la derecha
    ventana.draw(cambiar);
}

sf::Text Ayuda::crearTexto(const std::string& contenido, sf::Color color) const {
    sf::Text texto(contenido, fuente, 22);
    texto.setFillColor(color);
    return texto;
}

sf::Text Ayuda::textoVolver() const {
    sf::Text volver = crearTexto("[Esc] Volver", TENUE);
    volver.setPosition(X_IZQUIERDA, Y_PIE);
    return volver;
}
