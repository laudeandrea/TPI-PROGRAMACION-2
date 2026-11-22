#include <fstream>
#include <cstdio>

#include "Juego.h"
#include "PartidaArchivo.h"

#include <algorithm>
#include <cstdlib> // std::abs

// La ventana y la camara se crean en la lista de inicializacion (despues de los ":")
// porque no tienen constructor sin parametros
Juego::Juego()
	: window(sf::VideoMode(ANCHO_VENTANA, ALTO_VENTANA), "El Codice Del Mago"),
	  // El mago y el codice se crean en cualquier casilla: empezarNivel(1), al final, los pone donde dice el nivel
	  mago(0, 0),
	  codice(0, 0, TipoHabilidad::BOLADEENERGIA),
	  nivel(1),
	  camara(mapa.getTamanioMundo(), MARGEN_CAMARA),
	  estado(EstadoJuego::MENU),
	  fase(FaseTurno::JUGADOR),
	  habilidadPreparada(HabilidadSeleccionada::NINGUNA),
	  magoSeleccionado(false),
	  magoSeMovio(false),
	  enemigoEmpujado(nullptr)
{
	//Inicialización de la ventana
	window.setFramerateLimit(60);

	// Icono de la ventana
	sf::Image icono;
	if (icono.loadFromFile("Imagenes/icono_ventana.png")) {
		window.setIcon(icono.getSize().x, icono.getSize().y, icono.getPixelsPtr());
	};

	// Fondo de la pantalla de juego (se dibuja fijo, detras del tablero)
	texturaFondo.loadFromFile("Imagenes/fondo_juego.png");
	fondo.setTexture(texturaFondo);

	// Habilidad: marcadores de las casillas al alcance y las bolas (la del mago y la de los espectros)
	texturaMovimiento.loadFromFile("Imagenes/marcador_movimiento.png");
	texturaAlcance.loadFromFile("Imagenes/marcador_alcance.png");
	texturaObjetivo.loadFromFile("Imagenes/marcador_objetivo.png");
	texturaBola.loadFromFile("Imagenes/bola_energia.png");
	texturaBolaEspectro.loadFromFile("Imagenes/bola_espectro.png");

	// Camara: encuadra el tablero y lo estira para llenar la ventana
	camara.ajustarAVentana(window.getSize());

	empezarNivel(1);
};

void Juego::ejecutar() {
	//GameLoop (update del juego)
	while (window.isOpen())
	{
		// restart() devuelve el tiempo desde la vuelta anterior y vuelve a empezar a contar
		float dt = reloj.restart().asSeconds();

		procesarEventos();
		actualizar(dt);
		dibujar();
	}
};

// Leer la cola de mensajes
void Juego::procesarEventos() {
	sf::Event event;
	while (window.pollEvent(event)) {
		// Esto vale siempre, en el menu y jugando
		if (event.type == sf::Event::Closed) {
			window.close();
		}

		if (event.type == sf::Event::Resized) {
			camara.ajustarAVentana(window.getSize());
		}

		// Lo demas depende de en que pantalla estamos
		if (estado == EstadoJuego::MENU) {
			// El menu se entera en cada vuelta si hay una partida para continuar
			menu.setHayPartida(partidaArchivo.existe());

			int opcion = menu.procesarEvento(event, window);
			if (opcion == 0) {              // Continuar: retoma el nivel guardado
				empezarNivel(partidaArchivo.leer().getNivel());
				estado = EstadoJuego::JUGANDO;
			}
			if (opcion == 1) {              // Nueva partida: siempre arranca de cero
				reiniciarPartida();
				estado = EstadoJuego::JUGANDO;
			}
			if (opcion == 3) {              // Salir
				window.close();
			}
			// opcion 2 ("Como jugar"): todavia no hay pantalla, no hace nada
		}
		else if (estado == EstadoJuego::NIVEL_SUPERADO) {
			// Enter: seguir al proximo nivel. S: guardar y volver al menu.
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
				empezarNivel(nivel.getNumero() + 1);
				estado = EstadoJuego::JUGANDO;
			}
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::S) {
				if (partidaArchivo.guardar(Partida(nivel.getNumero() + 1))) {
					estado = EstadoJuego::MENU; // si no pudo guardar, se queda en el cartel
				}
			}
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
				estado = EstadoJuego::MENU; // salir sin guardar
			}
		}
		else if (estado == EstadoJuego::GANASTE || estado == EstadoJuego::PERDISTE) {
			// Cartel de fin de partida: Enter o Escape vuelven al menu
			if (event.type == sf::Event::KeyPressed
				&& (event.key.code == sf::Keyboard::Enter || event.key.code == sf::Keyboard::Escape)) {
				estado = EstadoJuego::MENU;
			}
		}
		else {
			// Escape vuelve al menu
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
				habilidadPreparada = HabilidadSeleccionada::NINGUNA; // al salir al menu se cancela la habilidad
				magoSeleccionado = false;
				estado = EstadoJuego::MENU;
			}

			// Click izquierdo: si hay una habilidad preparada la usa, si no mueve al mago
			if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left
				// Solo los clicks sobre el tablero: los del panel del HUD (a la derecha) no cuentan
				&& window.getViewport(camara.getVista()).contains(event.mouseButton.x, event.mouseButton.y)){
				sf::Vector2i pixel(event.mouseButton.x, event.mouseButton.y);
				// Con la vista de la camara: la que queda puesta en la ventana es la del HUD (la ultima que se dibujo)
				sf::Vector2f posicionMundo = window.mapPixelToCoords(pixel, camara.getVista());
				sf::Vector2i casilla = mapa.mundoAcasilla(posicionMundo); // x = columna, y = fila

				if (habilidadPreparada != HabilidadSeleccionada::NINGUNA) {
					usarHabilidad(casilla.y, casilla.x);
				}
				else {
					// --- MOVER ---
					// 1er click en el mago: lo selecciona y muestra a donde puede ir.
					// 2do click: en una casilla marcada lo mueve; en cualquier otro lado cancela.
					// Moverse NO termina el turno: despues puede tirar una habilidad o pasar con Espacio.
					// Se mueve una sola vez por turno, y antes de atacar (la habilidad termina el turno)
					if (fase == FaseTurno::JUGADOR && !magoSeMovio) {
						if (mago.estaEn(casilla.y, casilla.x)) {
							magoSeleccionado = !magoSeleccionado;
						}
						else if (magoSeleccionado) {
							if (magoPuedeIrA(casilla.y, casilla.x)) {
								mago.mover(casilla.y, casilla.x);
								magoSeMovio = true;
							}
							magoSeleccionado = false;
						}
					}
				}
			}

			// Habilidades del mago: 1 la bola, 2 el empuje (idea de Santiago: reemplazan a la Q), 3 el muro de fuego.
			// Si se apreta la misma otra vez, se cancela. Solo en el turno del jugador
			if (event.type == sf::Event::KeyPressed && fase == FaseTurno::JUGADOR) {
				// Que numero se apreto (0 = ninguno)
				int numero = 0;
				if (event.key.code == sf::Keyboard::Num1 || event.key.code == sf::Keyboard::Numpad1) numero = 1;
				if (event.key.code == sf::Keyboard::Num2 || event.key.code == sf::Keyboard::Numpad2) numero = 2;
				if (event.key.code == sf::Keyboard::Num3 || event.key.code == sf::Keyboard::Numpad3) numero = 3;

				// El numero es el valor del enum: 1 = bola, 2 = empuje, 3 = muro. Si todavia no la aprendio, no hace nada
				HabilidadSeleccionada elegida = HabilidadSeleccionada(numero);
				if (elegida != HabilidadSeleccionada::NINGUNA && mago.tieneHabilidad(elegida)) {
					// apretar la misma tecla dos veces cancela
					habilidadPreparada = (habilidadPreparada == elegida)
						? HabilidadSeleccionada::NINGUNA : elegida;
					magoSeleccionado = false; // con una habilidad en la mano no se elige casilla para caminar
				}

				// Espacio: pasa el turno sin hacer nada. Si el mago queda rodeado y sin mana, es la unica salida
				if (event.key.code == sf::Keyboard::Space) {
					habilidadPreparada = HabilidadSeleccionada::NINGUNA;
					magoSeleccionado = false;
					terminarTurnoJugador();
				}
			}
		}
	}

};

bool Juego::casillaOcupada(int fila, int columna) const {
	if (mago.estaEn(fila, columna)) return true;
	return buscarEnemigoEn(fila, columna) != nullptr;
};

bool Juego::puedeMover(const Personaje& quien, int fila, int columna) const {
	if (!mapa.estaDentro(fila, columna)) {
		return false;
	}
	if (!quien.puedePisar(mapa.getTipoTerreno(fila, columna))) {
		return false;
	}
	return !casillaOcupada(fila, columna);
};

void Juego::dibujar() {
	window.clear();

	if(estado == EstadoJuego::MENU){
		menu.setHayPartida(partidaArchivo.existe());
		menu.dibujar(window);
	}else{
		window.setView(window.getDefaultView()); // vista de la ventana: 1280x720
		window.draw(fondo);
		camara.aplicar(window);
		mapa.mostrar(window);

		// Marcadores: arriba del piso, abajo de los personajes
		if (magoSeleccionado && fase == FaseTurno::JUGADOR) {
			dibujarMovimiento();
		}
		if (habilidadPreparada != HabilidadSeleccionada::NINGUNA && fase == FaseTurno::JUGADOR) {
			dibujarAlcance();
		}
		dibujarExplosiones();

		/* personajes ordenados por profundidad, lo de adelante se dibuja ultimo */
		std::vector<Personaje*> personajes;
		personajes.push_back(&mago);
		for (const auto& enemigo : enemigos) personajes.push_back(enemigo.get());

		std::sort(personajes.begin(), personajes.end(),
			[](const Personaje* a, const Personaje* b) {
				return (a->getFila() + a->getColumna()) < (b->getFila() + b->getColumna());
			});

		// El codice no es un Personaje, asi que se mete en la fila en su lugar segun su profundidad:
		// se dibuja justo antes del primer personaje que este mas adelante que el
		int profundidadCodice = codice.getFila() + codice.getColumna();
		bool codiceDibujado = false;

		for (const Personaje* p : personajes) {
			if (!codiceDibujado && p->getFila() + p->getColumna() > profundidadCodice) {
				codice.mostrar(window, mapa);
				codiceDibujado = true;
			}
			p->mostrar(window, mapa);
		}
		if (!codiceDibujado) {
			codice.mostrar(window, mapa); // todos estaban detras del codice
		}

		// Barras de vida: despues de todos los personajes, asi ningun otro las tapa
		mago.mostrarBarraVida(window, mapa);
		for (const auto& enemigo : enemigos) {
			enemigo->mostrarBarraVida(window, mapa);
		}

		// Las bolas vuelan por encima de todo
		for (const auto& proyectil : proyectiles) {
			proyectil->mostrar(window);
		}

		// HUD: fijo en la pantalla (cambia a la vista por defecto)
		hud.dibujar(window, camara.getVista(), mago, codice, nivel, habilidadPreparada, magoSeleccionado);

		// Carteles: arriba de todo, con el tablero oscurecido atras
		if (estado == EstadoJuego::NIVEL_SUPERADO) {
			// Si en el proximo nivel aprende el muro, se avisa (sin tildes: SFML no las muestra bien)
			std::string aviso = "Preparate para el nivel " + std::to_string(nivel.getNumero() + 1);
			if (nivel.getNumero() + 1 == NIVEL_MURO) {
				aviso = "Aprendiste el muro de fuego [3]";
			}
			hud.dibujarNivelSuperado(window, aviso);
		}
		if (estado == EstadoJuego::GANASTE) {
			hud.dibujarFinDePartida(window, true, "Sobreviviste a todas las oleadas");
		}
		if (estado == EstadoJuego::PERDISTE) {
			// Sin tildes: SFML no las muestra bien sin convertir el texto
			std::string motivo = "Destruyeron el codice";
			if (!mago.estaVivo()) {
				motivo = "El mago cayo en combate";
			}
			hud.dibujarFinDePartida(window, false, motivo);
		}
	}

	window.display();
};

void Juego::dibujarAlcance() {
	const Habilidad& habilidad = mago.getHabilidad(habilidadPreparada);
	int alcance = habilidad.getAlcance();
	bool esMuro = habilidad.getTipo() == TipoHabilidad::MURO;

	// Recorre el cuadrado alrededor del mago y se queda con las casillas a distancia 1..alcance
	for (int fila = mago.getFila() - alcance; fila <= mago.getFila() + alcance; fila++) {
		for (int columna = mago.getColumna() - alcance; columna <= mago.getColumna() + alcance; columna++) {
			int distancia = distanciaAlMago(fila, columna);
			if (distancia == 0 || distancia > alcance || !mapa.estaDentro(fila, columna)) {
				continue; // continue = saltea esta casilla y sigue con la proxima
			}

			// El muro va en casillas vacias: se marcan solo esas
			if (esMuro) {
				if (casillaLibreParaMuro(fila, columna)) {
					dibujarMarcador(texturaAlcance, fila, columna);
				}
				continue;
			}

			if (buscarEnemigoEn(fila, columna) != nullptr) {
				dibujarMarcador(texturaObjetivo, fila, columna);  // rojo: ahi se puede tirar
			}
			else if (mapa.getTipoTerreno(fila, columna) != TipoTerreno::ABISMO) {
				dibujarMarcador(texturaAlcance, fila, columna);   // celeste: llega, pero no hay nadie
			}
			// en el abismo vacio no se marca nada: no hay piso
		}
	}

	// Muro: con el mouse arriba de una casilla valida, se ve en rojo donde quedarian las 3 casillas de fuego
	if (esMuro) {
		sf::Vector2i casilla = casillaBajoElMouse(); // x = columna, y = fila
		if (sePuedePonerMuro(casilla.y, casilla.x)) {
			for (const sf::Vector2i& fuego : casillasDelMuro(casilla.y, casilla.x)) {
				dibujarMarcador(texturaObjetivo, fuego.y, fuego.x);
			}
		}
	}
}

sf::Vector2i Juego::casillaBajoElMouse() const {
	// Igual que con el click: pixel de la ventana -> punto del mundo (con la vista de la camara) -> casilla
	sf::Vector2i pixel = sf::Mouse::getPosition(window);
	sf::Vector2f posicionMundo = window.mapPixelToCoords(pixel, camara.getVista());
	return mapa.mundoAcasilla(posicionMundo);
}

void Juego::dibujarMovimiento() {
	for (const sf::Vector2i& casilla : casillasAlcanzables(mago, mago.getMovimiento())) {
		dibujarMarcador(texturaMovimiento, casilla.y, casilla.x);
	}
}

void Juego::dibujarMarcador(const sf::Texture& textura, int fila, int columna) {
	sf::Sprite marcador(textura);
	// La imagen es el rombo de 64x40: su centro (32, 20) va en el centro de la casilla
	marcador.setOrigin(32.f, 20.f);
	marcador.setPosition(mapa.casillaAmundo(fila, columna));
	window.draw(marcador);
}

void Juego::dibujarExplosiones() {
	for (const Explosion& explosion : explosiones) {
		// El mismo rombo rojo de los objetivos, que se va desvaneciendo (alpha: 255 = opaco, 0 = invisible)
		float opacidad = 1.f - explosion.tiempo / DURACION_EXPLOSION;
		sf::Sprite marcador(texturaObjetivo);
		marcador.setColor(sf::Color(255, 255, 255, (sf::Uint8)(255 * opacidad)));
		marcador.setOrigin(32.f, 20.f);
		marcador.setPosition(mapa.casillaAmundo(explosion.fila, explosion.columna));
		window.draw(marcador);
	}
}

// Busqueda "en anchura" (BFS): primero todas las casillas a 1 paso, despues las de 2, y asi.
// Como solo avanza por casillas que se pueden pisar, rodea los pozos y las columnas.
std::vector<sf::Vector2i> Juego::casillasAlcanzables(const Personaje& quien, int pasosMaximos) const {
	int n = mapa.getCantCasillas();

	// pasos[fila][columna] = cuantos pasos cuesta llegar; -1 = todavia no se llego
	std::vector<std::vector<int>> pasos(n, std::vector<int>(n, -1));

	// La lista de casillas encontradas. Se recorre mientras crece: cada una agrega a sus vecinas
	std::vector<sf::Vector2i> encontradas;
	encontradas.push_back(sf::Vector2i(quien.getColumna(), quien.getFila()));
	pasos[quien.getFila()][quien.getColumna()] = 0;

	// Las 4 direcciones en las que se puede dar un paso (x = columna, y = fila)
	const sf::Vector2i direcciones[4] = { {1, 0}, {-1, 0}, {0, 1}, {0, -1} };

	for (size_t i = 0; i < encontradas.size(); i++) {
		sf::Vector2i actual = encontradas[i];
		int pasosActual = pasos[actual.y][actual.x];
		if (pasosActual == pasosMaximos) {
			continue; // desde aca ya no le quedan pasos
		}

		for (const sf::Vector2i& direccion : direcciones) {
			sf::Vector2i vecina = actual + direccion;
			// puedeMover ya mira que este dentro, que el terreno se pueda pisar y que este libre
			if (!puedeMover(quien, vecina.y, vecina.x)) continue;
			if (pasos[vecina.y][vecina.x] != -1) continue; // ya la habiamos encontrado

			pasos[vecina.y][vecina.x] = pasosActual + 1;
			encontradas.push_back(vecina);
		}
	}

	// La primera es la casilla donde ya esta parado: no cuenta como destino
	encontradas.erase(encontradas.begin());
	return encontradas;
}

bool Juego::magoPuedeIrA(int fila, int columna) const {
	for (const sf::Vector2i& casilla : casillasAlcanzables(mago, mago.getMovimiento())) {
		if (casilla.y == fila && casilla.x == columna) {
			return true;
		}
	}
	return false;
}

// La misma busqueda en anchura que casillasAlcanzables, pero sin limite de pasos y anotando
// desde que casilla se llego a cada una. Asi, al encontrar el destino, se puede volver para atras hasta el primer paso.
sf::Vector2i Juego::pasoHacia(const Enemigo& enemigo, int filaObjetivo, int columnaObjetivo) const {
	int n = mapa.getCantCasillas();
	sf::Vector2i inicio(enemigo.getColumna(), enemigo.getFila()); // x = columna, y = fila

	// desde[fila][columna] = la casilla de la que se vino; (-1, -1) = todavia no se llego
	std::vector<std::vector<sf::Vector2i>> desde(n, std::vector<sf::Vector2i>(n, sf::Vector2i(-1, -1)));
	desde[inicio.y][inicio.x] = inicio;

	std::vector<sf::Vector2i> encontradas;
	encontradas.push_back(inicio);

	const sf::Vector2i direcciones[4] = { {1, 0}, {-1, 0}, {0, 1}, {0, -1} };

	for (size_t i = 0; i < encontradas.size(); i++) {
		sf::Vector2i actual = encontradas[i];

		// Desde aca le llega el ataque: es la casilla buena mas cercana (la BFS las encuentra en orden de distancia)
		int distanciaAlObjetivo = distancia(actual.y, actual.x, filaObjetivo, columnaObjetivo);
		if (distanciaAlObjetivo >= 1 && distanciaAlObjetivo <= enemigo.getAlcanceAtaque()) {
			// Vuelve para atras por el camino hasta la casilla que esta pegada al inicio: ese es el paso
			while (desde[actual.y][actual.x] != inicio && actual != inicio) {
				actual = desde[actual.y][actual.x];
			}
			return actual;
		}

		for (const sf::Vector2i& direccion : direcciones) {
			sf::Vector2i vecina = actual + direccion;
			if (!puedeMover(enemigo, vecina.y, vecina.x)) continue;
			if (desde[vecina.y][vecina.x].x != -1) continue; // ya la habiamos encontrado

			desde[vecina.y][vecina.x] = actual;
			encontradas.push_back(vecina);
		}
	}

	// No hay ningun camino libre (lo tapan otros personajes): se acerca en linea recta, como antes
	return enemigo.calcularPaso(mapa, filaObjetivo, columnaObjetivo);
}

void Juego::actualizar(float dt) {
	hud.actualizar(dt);  // los numeros de danio se animan siempre
	mapa.actualizar(dt); // y el fuego de los muros tambien

	// Las casillas rojas de los golpes tambien se animan siempre, y se borran cuando terminan
	for (Explosion& explosion : explosiones) {
		explosion.tiempo += dt;
	}
	explosiones.erase(std::remove_if(explosiones.begin(), explosiones.end(),
		[](const Explosion& e) { return e.tiempo >= DURACION_EXPLOSION; }), explosiones.end());

	if (estado != EstadoJuego::JUGANDO) return;

	// Mientras vuelan bolas, nadie hace nada mas. Cuando llegan todas, sigue el turno que corresponde
	if (fase == FaseTurno::DISPARANDO || fase == FaseTurno::ENEMIGOS_DISPARANDO) {
		moverProyectiles(dt);
		if (proyectiles.empty()) {
			if (fase == FaseTurno::DISPARANDO) {
				terminarTurnoJugador();   // era la bola del mago
			}
			else {
				empezarTurnoJugador();    // eran las de los espectros
			}
		}
		return;
	}

	if (fase == FaseTurno::ENEMIGOS) {
		turnoEnemigos();
	}
}

// ---------------------------------------------------------------- Partida

void Juego::reiniciarPartida() {
	partidaArchivo.borrar();
	empezarNivel(1);
}

void Juego::empezarNivel(int numero) {
	nivel = Nivel(numero);

	// El tablero nuevo pisa al anterior (y con el, los muros de fuego que hubieran quedado)
	mapa.cargar(nivel.getDisenio());
	muros.clear();

	// Se crean de nuevo y se copian encima de los viejos (asi vuelven con la vida y el mana llenos)
	codice = Pergamino(nivel.getFilaCodice(), nivel.getColumnaCodice(), TipoHabilidad::BOLADEENERGIA);
	// La casilla donde se encuentra el codice queda ocupada, no puede ser pisada
	mapa.setTipoTerreno(codice.getFila(), codice.getColumna(), TipoTerreno::SUELO_OCUPADO);

	mago = Mago(nivel.getFilaMago(), nivel.getColumnaMago());
	if (numero >= NIVEL_MURO) {
		// Va tercera porque es la tecla 3 (ver HabilidadSeleccionada). No hace danio: frena a los enemigos
		mago.aprenderHabilidad(Habilidad(TipoHabilidad::MURO, "Muro de fuego", 0, COSTO_MURO, ALCANCE_MURO));
	}

	enemigos.clear();
	proyectiles.clear();
	explosiones.clear();
	hud.limpiar();

	fase = FaseTurno::JUGADOR;
	habilidadPreparada = HabilidadSeleccionada::NINGUNA;
	magoSeleccionado = false;
	magoSeMovio = false;
	enemigoEmpujado = nullptr;

	empezarOleada();
}

void Juego::empezarOleada() {
	for (auto& enemigo : nivel.crearOleada()) {
		// Si no puede quedarse en su casilla de salida (por ejemplo, el mago esta parado ahi o hay fuego),
		// lo pone en la casilla libre mas cercana. casillasAlcanzables las devuelve de la mas cercana a la mas lejana
		if (!puedeMover(*enemigo, enemigo->getFila(), enemigo->getColumna())) {
			std::vector<sf::Vector2i> libres = casillasAlcanzables(*enemigo, 3);
			if (libres.empty()) continue; // no hay lugar: esta oleada viene con uno menos
			enemigo->mover(libres[0].y, libres[0].x);
		}
		// std::move: un unique_ptr no se puede copiar, se "pasa" al vector (y el de aca queda vacio)
		enemigos.push_back(std::move(enemigo));
	}
}

void Juego::revisarFinDePartida() {
	if (!mago.estaVivo() || codice.estaDestruido()) {
		estado = EstadoJuego::PERDISTE;
		habilidadPreparada = HabilidadSeleccionada::NINGUNA;
		magoSeleccionado = false;
		return;
	}

	if (enemigos.empty()) {
		if (nivel.hayOtraOleada()) {
			nivel.pasarDeOleada();
			empezarOleada();

			// Respiro: la oleada nueva ya esta en el tablero, pero no actua en el mismo instante.
			// Le devolvemos el turno al jugador para que vea de donde salen y se prepare.
			// No se le da mana extra: eso lo hace empezarTurnoJugador() al empezar un turno normal.
			fase = FaseTurno::JUGADOR;
			habilidadPreparada = HabilidadSeleccionada::NINGUNA;
			magoSeleccionado = false;
			magoSeMovio = false;
		}
		else {
			if (nivel.hayOtroNivel()) {
				// Ya no se guarda solo: el jugador decide en el cartel (ver procesarEventos)
				estado = EstadoJuego::NIVEL_SUPERADO;
			}
			else {
				partidaArchivo.borrar(); // se termino el juego
				estado = EstadoJuego::GANASTE;
			}
			habilidadPreparada = HabilidadSeleccionada::NINGUNA;
			magoSeleccionado = false;
		}
	}
}

// ---------------------------------------------------------------- Turno del jugador

void Juego::usarHabilidad(int fila, int columna) {
	if (fase != FaseTurno::JUGADOR) {
		return;
	}
	const Habilidad& habilidad = mago.getHabilidad(habilidadPreparada);

	// El muro es distinto a las otras: va en una casilla VACIA, no sobre un enemigo
	if (habilidad.getTipo() == TipoHabilidad::MURO) {
		if (!sePuedePonerMuro(fila, columna)) {
			return;
		}
		if (!mago.gastarMana(habilidad.getCostoMana())) {
			return;
		}
		ponerMuro(fila, columna);
		habilidadPreparada = HabilidadSeleccionada::NINGUNA;
		terminarTurnoJugador(); // es instantaneo, como el empuje
		return;
	}

	// Bola y empuje: necesitan un enemigo al alcance
	Enemigo* objetivo = buscarEnemigoEn(fila, columna);
	int distancia = distanciaAlMago(fila, columna);
	if (objetivo == nullptr || distancia > habilidad.getAlcance()) {
		return;
	}

	// El empuje solo se hace si hay lugar atras del enemigo: si no, no pasa nada y no se gasta mana (idea de Santiago)
	if (habilidad.getTipo() == TipoHabilidad::EMPUJE && !sePuedeEmpujar(*objetivo)) {
		return;
	}

	// gastarMana va ultimo: si no alcanza el mana, no se hace nada
	if (!mago.gastarMana(habilidad.getCostoMana())) {
		return;
	}

	if (habilidad.getTipo() == TipoHabilidad::BOLADEENERGIA) {
		// La bola sale del pecho del mago y va al pecho del enemigo.
		// El danio se aplica cuando llega (ver moverProyectiles)
		sf::Vector2f altura(0.f, ALTURA_PECHO);
		sf::Vector2f origen = mapa.casillaAmundo(mago.getFila(), mago.getColumna()) - altura;
		sf::Vector2f destino = mapa.casillaAmundo(fila, columna) - altura;

		proyectiles.push_back(std::make_unique<Proyectil>(origen, destino, texturaBola,
			habilidad.getDanio(), fila, columna));
		fase = FaseTurno::DISPARANDO;
	}
	else if (habilidad.getTipo() == TipoHabilidad::EMPUJE) {
		empujar(*objetivo, habilidad.getDanio());
		terminarTurnoJugador(); // el empuje es instantaneo: no hay nada que esperar
	}

	habilidadPreparada = HabilidadSeleccionada::NINGUNA;
}

sf::Vector2i Juego::destinoEmpuje(const Enemigo& enemigo) const {
	// Para que lado: desde el mago hacia el enemigo. Como estan pegados, es un paso en fila o en columna
	int fila = enemigo.getFila() + (enemigo.getFila() - mago.getFila());
	int columna = enemigo.getColumna() + (enemigo.getColumna() - mago.getColumna());
	return sf::Vector2i(columna, fila);
}

bool Juego::sePuedeEmpujar(const Enemigo& enemigo) const {
	sf::Vector2i destino = destinoEmpuje(enemigo);

	// Contra el borde o contra otro personaje no se puede
	if (!mapa.estaDentro(destino.y, destino.x) || casillaOcupada(destino.y, destino.x)) {
		return false;
	}

	// Empuje forzado: el enemigo no elige, pero tiene que poder pararse ahi.
	// El abismo se permite siempre: si no puede pisarlo, se cae (contra una columna o el codice, no)
	TipoTerreno terreno = mapa.getTipoTerreno(destino.y, destino.x);
	return enemigo.puedePisar(terreno) || terreno == TipoTerreno::ABISMO;
}

void Juego::empujar(Enemigo& enemigo, int danio) {
	sf::Vector2i destino = destinoEmpuje(enemigo); // se calcula antes de moverlo

	daniarCasilla(enemigo.getFila(), enemigo.getColumna(), danio);
	if (!enemigo.estaVivo()) return; // el golpe ya lo mato

	enemigo.mover(destino.y, destino.x); // virtual puedePisar: el espectro flota sobre el abismo y no se cae

	if (!enemigo.puedePisar(mapa.getTipoTerreno(destino.y, destino.x))) {
		// No puede volar: cae al pozo y no vuelve. Se le saca toda la vida que le quedaba
		daniarCasilla(destino.y, destino.x, enemigo.getVida());
		return;
	}

	// Si lo empujaron al fuego (solo puede pasarle al espectro), se quema
	quemarSiEstaEnFuego(enemigo);
	if (!enemigo.estaVivo()) return;

	// Quedo aturdido por el golpe: este turno no hace nada (idea de Santiago)
	enemigoEmpujado = &enemigo;
}

void Juego::terminarTurnoJugador() {
	sacarMuertos();
	fase = FaseTurno::ENEMIGOS;
	revisarFinDePartida(); // si se mato al ultimo, aparece la proxima oleada (o se gana)
}

// ---------------------------------------------------------------- Muro de fuego

bool Juego::casillaLibreParaMuro(int fila, int columna) const {
	// Solo suelo comun: no en el abismo, ni en una columna, ni en el codice (SUELO_OCUPADO), ni encima de otro fuego
	return mapa.estaDentro(fila, columna)
		&& mapa.getTipoTerreno(fila, columna) == TipoTerreno::SUELO
		&& !casillaOcupada(fila, columna);
}

bool Juego::sePuedePonerMuro(int fila, int columna) const {
	int alcance = mago.getHabilidad(HabilidadSeleccionada::MURO).getAlcance();
	int distancia = distanciaAlMago(fila, columna);
	return distancia >= 1 && distancia <= alcance && casillaLibreParaMuro(fila, columna);
}

std::vector<sf::Vector2i> Juego::casillasDelMuro(int fila, int columna) const {
	// Hacia que lado mira el mago: si la casilla esta mas lejos en filas que en columnas, el mago "mira" a lo
	// largo de las filas, y el muro se arma de costado (a lo largo de las columnas). Y al reves.
	// Ejemplo: mago en (4,1), click en (4,3) -> el muro queda en (3,3), (4,3) y (5,3)
	int distanciaFilas = std::abs(fila - mago.getFila());
	int distanciaColumnas = std::abs(columna - mago.getColumna());

	sf::Vector2i costado; // x = columna, y = fila
	if (distanciaFilas >= distanciaColumnas) {
		costado = sf::Vector2i(1, 0); // las vecinas son la de la izquierda y la de la derecha (otra columna)
	}
	else {
		costado = sf::Vector2i(0, 1); // las vecinas son la de arriba y la de abajo (otra fila)
	}

	sf::Vector2i centro(columna, fila);
	sf::Vector2i candidatas[3] = { centro - costado, centro, centro + costado };

	std::vector<sf::Vector2i> casillas;
	for (const sf::Vector2i& casilla : candidatas) {
		if (casillaLibreParaMuro(casilla.y, casilla.x)) {
			casillas.push_back(casilla);
		}
	}
	return casillas;
}

void Juego::ponerMuro(int fila, int columna) {
	for (const sf::Vector2i& casilla : casillasDelMuro(fila, columna)) {
		mapa.setTipoTerreno(casilla.y, casilla.x, TipoTerreno::MURO);
		muros.push_back({ casilla.y, casilla.x, TURNOS_MURO });
	}
}

void Juego::apagarMuros() {
	for (Muro& muro : muros) {
		muro.turnosRestantes--;
		if (muro.turnosRestantes <= 0) {
			mapa.setTipoTerreno(muro.fila, muro.columna, TipoTerreno::SUELO); // el fuego solo se pone sobre suelo
		}
	}
	// Saca del vector los que se apagaron (mismo truco que con los enemigos muertos)
	muros.erase(std::remove_if(muros.begin(), muros.end(),
		[](const Muro& m) { return m.turnosRestantes <= 0; }), muros.end());
}

void Juego::quemarSiEstaEnFuego(Enemigo& enemigo) {
	if (mapa.getTipoTerreno(enemigo.getFila(), enemigo.getColumna()) != TipoTerreno::MURO) {
		return;
	}
	daniarCasilla(enemigo.getFila(), enemigo.getColumna(), DANIO_FUEGO);
	explosiones.push_back({ enemigo.getFila(), enemigo.getColumna(), 0.f }); // la casilla se marca en rojo un momento
}

// ---------------------------------------------------------------- Turno de los enemigos

void Juego::turnoEnemigos() {
	// Con indice y no con "for (auto& ...)": asi queda claro que no se borra nada mientras se recorre.
	// Los que mueren en este turno (por la explosion de un golem) se sacan todos juntos al final
	for (size_t i = 0; i < enemigos.size(); i++) {
		Enemigo& enemigo = *enemigos[i];
		if (!enemigo.estaVivo()) continue;
		if (&enemigo == enemigoEmpujado) continue; // lo empujaron: pierde este turno

		// Si empieza su turno parado en el fuego, se vuelve a quemar (asi no le conviene quedarse ahi)
		quemarSiEstaEnFuego(enemigo);
		if (!enemigo.estaVivo()) continue;

		// Va por lo que tenga mas cerca: el mago o el codice. Si estan igual de lejos, prefiere el codice
		int distanciaMago = distancia(enemigo.getFila(), enemigo.getColumna(), mago.getFila(), mago.getColumna());
		int distanciaCodice = distancia(enemigo.getFila(), enemigo.getColumna(), codice.getFila(), codice.getColumna());
		int filaObjetivo = codice.getFila(), columnaObjetivo = codice.getColumna();
		if (distanciaMago < distanciaCodice) {
			filaObjetivo = mago.getFila();
			columnaObjetivo = mago.getColumna();
		}

		// Cada enemigo hace UNA cosa por turno: si le llega ataca, si no camina.
		// (Si camina y queda pegado, recien ataca el turno siguiente: el jugador tiene tiempo de reaccionar)
		if (enemigo.puedeAtacar(filaObjetivo, columnaObjetivo)) {
			// virtual: se ejecuta el atacar del enemigo que sea de verdad (el del golem explota)
			Ataque ataque = enemigo.atacar(filaObjetivo, columnaObjetivo);

			for (const sf::Vector2i& casilla : ataque.casillas) {
				if (enemigo.lanzaProyectil()) {
					// Sale de su pecho y va al pecho del objetivo; el danio se aplica cuando llega
					sf::Vector2f altura(0.f, ALTURA_PECHO);
					sf::Vector2f origen = mapa.casillaAmundo(enemigo.getFila(), enemigo.getColumna()) - altura;
					sf::Vector2f destino = mapa.casillaAmundo(casilla.y, casilla.x) - altura;
					proyectiles.push_back(std::make_unique<Proyectil>(origen, destino, texturaBolaEspectro,
						ataque.danio, casilla.y, casilla.x));
				}
				else if (mapa.estaDentro(casilla.y, casilla.x)) {
					daniarCasilla(casilla.y, casilla.x, ataque.danio);
					explosiones.push_back({ casilla.y, casilla.x, 0.f }); // la casilla se marca en rojo un momento
				}
			}
		}
		else {
			sf::Vector2i destino = pasoHacia(enemigo, filaObjetivo, columnaObjetivo);

			bool quiereMoverse = !enemigo.estaEn(destino.y, destino.x);
			if (quiereMoverse && puedeMover(enemigo, destino.y, destino.x)) {
				enemigo.mover(destino.y, destino.x);
				// El espectro puede cruzar el muro de fuego, pero se quema al entrar
				quemarSiEstaEnFuego(enemigo);
			}
		}
	}

	enemigoEmpujado = nullptr; // el proximo turno ya juega normal
	sacarMuertos();

	if (proyectiles.empty()) {
		empezarTurnoJugador();
	}
	else {
		fase = FaseTurno::ENEMIGOS_DISPARANDO; // hay que esperar a que lleguen las bolas de los espectros
	}
}

void Juego::empezarTurnoJugador() {
	fase = FaseTurno::JUGADOR;
	magoSeMovio = false;                // turno nuevo: puede volver a caminar
	mago.recuperarMana(MANA_POR_TURNO); // empieza el turno del jugador: recupera un poco de mana
	apagarMuros();                      // y al fuego le queda un turno menos
	revisarFinDePartida();
}

// ---------------------------------------------------------------- Danio

void Juego::daniarCasilla(int fila, int columna, int danio) {
	if (!mapa.estaDentro(fila, columna)) return;

	// El numero aparece arriba de la cabeza: los sprites miden 64 de alto
	sf::Vector2f posicion = mapa.casillaAmundo(fila, columna);

	if (mago.estaEn(fila, columna)) {
		mago.recibirDanio(danio);
		hud.agregarDanio(posicion - sf::Vector2f(0.f, ALTURA_CABEZA), danio);
	}

	if (codice.getFila() == fila && codice.getColumna() == columna) {
		codice.recibirDanio(danio);
		hud.agregarDanio(posicion - sf::Vector2f(0.f, ALTURA_CODICE), danio);
	}

	Enemigo* enemigo = buscarEnemigoEn(fila, columna);
	if (enemigo != nullptr) {
		enemigo->recibirDanio(danio);
		hud.agregarDanio(posicion - sf::Vector2f(0.f, ALTURA_CABEZA), danio);
	}
}

void Juego::moverProyectiles(float dt) {
	for (auto& proyectil : proyectiles) {
		proyectil->actualizar(dt);
		if (proyectil->yaLlego()) {
			daniarCasilla(proyectil->getFilaObjetivo(), proyectil->getColumnaObjetivo(), proyectil->getDanio());
		}
	}

	// Borra las que ya llegaron (mismo truco que con los enemigos muertos)
	proyectiles.erase(std::remove_if(proyectiles.begin(), proyectiles.end(),
		[](const std::unique_ptr<Proyectil>& p) { return p->yaLlego(); }), proyectiles.end());
}

void Juego::sacarMuertos() {
	enemigos.erase(std::remove_if(enemigos.begin(), enemigos.end(),
		[](const std::unique_ptr<Enemigo>& e) { return !e->estaVivo(); }), enemigos.end());
}

int Juego::distancia(int fila1, int columna1, int fila2, int columna2) const {
	return std::abs(fila1 - fila2) + std::abs(columna1 - columna2);
}

int Juego::distanciaAlMago(int fila, int columna) const {
	return distancia(fila, columna, mago.getFila(), mago.getColumna());
}

Enemigo* Juego::buscarEnemigoEn(int fila, int columna) const{
	for (const auto& enemigo : enemigos) {
			// Los muertos todavia pueden estar en el vector hasta que se saquen: esos no cuentan
			if (enemigo->estaVivo() && enemigo->estaEn(fila,columna)) {
					return enemigo.get(); // devuelve el puntero a ESE enemigo
			}
	}
	return nullptr; // si no habia nadie
}
