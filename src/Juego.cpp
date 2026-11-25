#include "Juego.h"

#include <algorithm>
#include <cstdlib> // std::abs

// La ventana, la camara y el nivel se crean en la lista de inicializacion (despues de los ":")
// porque no tienen constructor sin parametros. Lo demas (imagenes, nivel 1, musica) lo prepara iniciar()
Juego::Juego()
	: window(sf::VideoMode(ANCHO_VENTANA, ALTO_VENTANA), "El Codice Del Mago"),
	  // El mago se crea en cualquier casilla: empezarNivel(1) lo pone donde dice el nivel
	  mago(0, 0),
	  nivelActual(1, catalogo),
	  camara(nivelActual.getMapa().getTamanioMundo(), MARGEN_CAMARA),
	  menuDificultad(catalogo),
	  ayuda(catalogo),
	  estado(EstadoJuego::MENU),
	  dificultad(Catalogo::DIFICULTAD_NORMAL),
	  fase(FaseTurno::JUGADOR),
	  tiempoFase(0.f),
	  tiempoVolumen(DURACION_AVISO_VOLUMEN), // arranca "vencido": no se muestra hasta que se toque el volumen
	  habilidadPreparada(HabilidadSeleccionada::NINGUNA),
	  magoSeleccionado(false),
	  magoSeMovio(false)
{
}

void Juego::iniciar() {
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

	// Marcadores de las casillas (a donde camina, a donde llega la habilidad, a donde van a pegar los enemigos)
	// y las bolas (la del mago y la de los espectros)
	texturaMovimiento.loadFromFile("Imagenes/marcador_movimiento.png");
	texturaAlcance.loadFromFile("Imagenes/marcador_alcance.png");
	texturaObjetivo.loadFromFile("Imagenes/marcador_objetivo.png");
	texturaPeligro.loadFromFile("Imagenes/marcador_peligro.png");
	texturaBola.loadFromFile("Imagenes/bola_energia.png");
	texturaBolaEspectro.loadFromFile("Imagenes/bola_espectro.png");

	// Camara: encuadra el tablero y lo estira para llenar la ventana
	camara.ajustarAVentana(window.getSize());

	empezarNivel(1);
	sonidos.iniciarMusica(); // la musica de fondo arranca ya en el menu

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
			// Si se cierra en medio de un nivel, se guarda como esta: "Continuar" sigue desde aca
			if (estado == EstadoJuego::JUGANDO && fase == FaseTurno::JUGADOR) {
				guardarPartidaEnCurso();
			}
			window.close();
		}

		if (event.type == sf::Event::Resized) {
			camara.ajustarAVentana(window.getSize());
		}

		// Volumen de la musica con - y = (funciona en el menu y jugando, para calibrarlo a gusto).
		// Se muestra un momento abajo a la izquierda (en el .exe de Release no hay consola donde verlo)
		if (event.type == sf::Event::KeyPressed) {
			if (event.key.code == sf::Keyboard::Hyphen || event.key.code == sf::Keyboard::Equal) {
				sonidos.cambiarVolumenMusica(event.key.code == sf::Keyboard::Hyphen ? -1.f : 1.f);
				tiempoVolumen = 0.f;
			}
		}

		bool enter = event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter;
		bool escape = event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape;

		// Lo demas depende de en que pantalla estamos
		if (estado == EstadoJuego::MENU) {
			// El menu se entera en cada vuelta si hay una partida para continuar
			menu.setHayPartida(partidaArchivo.existe());

			int opcion = menu.procesarEvento(event, window);
			if (opcion == 0) {              // Continuar: retoma la partida guardada
				continuarPartida();
				sonidos.reproducirInicio();
			}
			if (opcion == 1) {              // Nueva partida: primero se elige la dificultad (arranca marcada la Normal)
				menuDificultad.setSeleccionada(Catalogo::DIFICULTAD_NORMAL);
				estado = EstadoJuego::ELEGIR_DIFICULTAD;
			}
			if (opcion == 2) {              // Como jugar: siempre arranca en la primera pagina
				ayuda.reiniciar();
				estado = EstadoJuego::COMO_JUGAR;
			}
			if (opcion == 3) {              // Salir
				window.close();
			}
		}
		else if (estado == EstadoJuego::ELEGIR_DIFICULTAD) {
			// Enter o click: arranca la partida nueva con esa dificultad (siempre de cero). Escape: vuelve al menu
			int elegida = menuDificultad.procesarEvento(event, window);
			if (elegida == MenuDificultad::VOLVER) {
				estado = EstadoJuego::MENU;
			}
			else if (elegida != MenuDificultad::NADA) {
				reiniciarPartida(elegida);
				sonidos.reproducirInicio();
			}
		}
		else if (estado == EstadoJuego::COMO_JUGAR) {
			// Las flechas pasan de pagina; si devuelve true (Escape, "Volver"), se vuelve al menu
			if (ayuda.procesarEvento(event, window)) {
				estado = EstadoJuego::MENU;
			}
		}
		else if (estado == EstadoJuego::HISTORIA) {
			// Enter empieza a jugar el piso. Escape vuelve al menu (el nivel ya quedo guardado)
			if (enter) {
				estado = EstadoJuego::JUGANDO;
				guardarPartidaEnCurso();
			}
			if (escape) {
				estado = EstadoJuego::MENU;
			}
		}
		else if (estado == EstadoJuego::NIVEL_SUPERADO) {
			// El proximo nivel ya quedo guardado al ganar este (ver revisarFinDePartida).
			// Enter: seguir. Escape (o S, como antes): volver al menu; "Continuar" arranca en el nivel que sigue
			if (enter) {
				cambiarNivel();
			}
			if (escape || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::S)) {
				estado = EstadoJuego::MENU;
			}
		}
		else if (estado == EstadoJuego::GANASTE || estado == EstadoJuego::PERDISTE) {
			// Cartel de fin de partida: Enter o Escape vuelven al menu
			if (enter || escape) {
				estado = EstadoJuego::MENU;
				sonidos.iniciarMusica(); // vuelve la musica de fondo
			}
		}
		else {
			procesarEventoJugando(event);
		}
	}

};

void Juego::procesarEventoJugando(const sf::Event& event) {
	// Escape guarda y vuelve al menu
	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
		volverAlMenu();
		return;
	}

	if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
		sf::Vector2i pixel(event.mouseButton.x, event.mouseButton.y);

		// Click en el panel de la derecha (fuera del tablero): las habilidades y "Terminar turno" se eligen con el mouse
		if (!window.getViewport(camara.getVista()).contains(pixel)) {
			// El panel usa la vista por defecto (1280x720), asi que el pixel se pasa con esa vista
			sf::Vector2f punto = window.mapPixelToCoords(pixel, window.getDefaultView());
			HabilidadSeleccionada cual = hud.habilidadEn(punto, mago);
			if (cual != HabilidadSeleccionada::NINGUNA) {
				elegirHabilidad(cual);
			}
			else if (hud.estaSobreTerminarTurno(punto, mago)) {
				pasarTurno();
			}
			return;
		}

		// Click sobre el tablero. Con la vista de la camara: la que queda puesta en la ventana es la del HUD (la ultima que se dibujo)
		Mapa& mapa = nivelActual.getMapa();
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
						mapa.mover(mago, casilla.y, casilla.x);
						magoSeMovio = true;
					}
					magoSeleccionado = false;
				}
			}
		}
	}

	// Habilidades del mago: 1 la bola, 2 el empuje (idea de Santiago: reemplazan a la Q), 3 el muro de fuego.
	// Si se apreta la misma otra vez, se cancela
	if (event.type == sf::Event::KeyPressed) {
		// Que numero se apreto (0 = ninguno)
		int numero = 0;
		if (event.key.code == sf::Keyboard::Num1 || event.key.code == sf::Keyboard::Numpad1) numero = 1;
		if (event.key.code == sf::Keyboard::Num2 || event.key.code == sf::Keyboard::Numpad2) numero = 2;
		if (event.key.code == sf::Keyboard::Num3 || event.key.code == sf::Keyboard::Numpad3) numero = 3;

		// El numero es el valor del enum: 1 = bola, 2 = empuje, 3 = muro
		if (numero != 0) {
			elegirHabilidad(HabilidadSeleccionada(numero));
		}

		// Espacio: pasa el turno sin hacer nada. Si el mago queda rodeado y sin mana, es la unica salida
		if (event.key.code == sf::Keyboard::Space) {
			pasarTurno();
		}
	}
}

bool Juego::casillaOcupada(int fila, int columna) const {
	if (mago.estaEn(fila, columna)) return true;
	return buscarEnemigoEn(fila, columna) != nullptr;
};

bool Juego::puedeMover(const Personaje& quien, int fila, int columna) const {
	const Mapa& mapa = nivelActual.getMapa();
	if (!mapa.esValida(fila, columna)) {
		return false;
	}
	if (!quien.puedePisar(mapa.getTipoTerreno(fila, columna))) {
		return false;
	}
	return !casillaOcupada(fila, columna);
};

// ---------------------------------------------------------------- Dibujo

void Juego::dibujar() {
	window.clear();

	if(estado == EstadoJuego::MENU){
		menu.setHayPartida(partidaArchivo.existe());
		menu.dibujar(window);
	}else if (estado == EstadoJuego::ELEGIR_DIFICULTAD) {
		menuDificultad.dibujar(window);
	}else if (estado == EstadoJuego::COMO_JUGAR) {
		ayuda.dibujar(window);
	}else{
		dibujarTablero();

		// Carteles: arriba de todo, con el tablero oscurecido atras (sin tildes: SFML no las muestra bien)
		if (estado == EstadoJuego::HISTORIA) {
			hud.dibujarHistoria(window, "PISO " + std::to_string(nivelActual.getNumero()), "",
				nivelActual.getHistoria(), "Enter para empezar");
		}
		if (estado == EstadoJuego::NIVEL_SUPERADO) {
			// La historia del piso que sigue, y si en ese piso aprende una habilidad, se avisa
			int proximo = nivelActual.getNumero() + 1;
			std::string aviso = "Preparate para el piso " + std::to_string(proximo);
			std::vector<DatosHabilidad> nuevas = catalogo.getHabilidadesNuevas(proximo);
			if (!nuevas.empty()) {
				int tecla = static_cast<int>(nuevas[0].getTipo()) + 1; // HabilidadSeleccionada = TipoHabilidad + 1
				aviso = "Aprendiste: " + nuevas[0].getNombre() + " [" + std::to_string(tecla) + "]";
			}
			// La historia se pide al catalogo (crear un Nivel cargaria las imagenes de su mapa en cada cuadro)
			hud.dibujarHistoria(window, "NIVEL SUPERADO", aviso, catalogo.getNivel(proximo).getHistoria(),
				"Enter: seguir  |  Esc: menu");
		}
		if (estado == EstadoJuego::GANASTE) {
			hud.dibujarFinDePartida(window, true, "Sobreviviste a todas las oleadas");
		}
		if (estado == EstadoJuego::PERDISTE) {
			std::string motivo = "Destruyeron el codice";
			if (!mago.estaVivo()) {
				motivo = "El mago cayo en combate";
			}
			hud.dibujarFinDePartida(window, false, motivo);
		}
	}

	// El aviso del volumen va arriba de todo, en cualquier pantalla. Se desvanece en el ultimo medio segundo
	if (tiempoVolumen < DURACION_AVISO_VOLUMEN) {
		float opacidad = std::min(1.f, (DURACION_AVISO_VOLUMEN - tiempoVolumen) / 0.5f);
		hud.dibujarVolumen(window, (int)(sonidos.getVolumenMusica() + 0.5f), opacidad);
	}

	window.display();
};

void Juego::dibujarTablero() {
	// Lo que tiene el nivel que se esta jugando
	Mapa& mapa = nivelActual.getMapa();
	const auto& enemigos = nivelActual.getEnemigos();
	const Pergamino& codice = nivelActual.getCodice();

	window.setView(window.getDefaultView()); // vista de la ventana: 1280x720
	window.draw(fondo);
	camara.aplicar(window);
	mapa.mostrar(window);

	// Marcadores: arriba del piso, abajo de los personajes
	bool turnoJugador = estado == EstadoJuego::JUGANDO && fase == FaseTurno::JUGADOR;
	if (magoSeleccionado && turnoJugador) {
		dibujarMovimiento();
	}
	if (habilidadPreparada != HabilidadSeleccionada::NINGUNA && turnoJugador) {
		dibujarAlcance();
	}
	// Los ataques anunciados van despues: asi se ven aunque la casilla tambien este marcada para caminar
	dibujarAtaquesAnunciados();
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

	// Cuanto danio llega a cada casilla marcada, y un "!" arriba de cada enemigo que va a atacar.
	// Los numeros los dibuja el HUD (con la vista por defecto, para que la letra se vea nitida)
	int n = mapa.getCantCasillas();
	std::vector<std::vector<int>> danioPorCasilla(n, std::vector<int>(n, 0));
	std::vector<Hud::Amenaza> amenazas;
	std::vector<sf::Vector2f> atacantes;
	for (const auto& enemigo : enemigos) {
		if (!enemigo->estaVivo() || !enemigo->tieneAtaqueAnunciado()) continue;
		atacantes.push_back(mapa.casillaAmundo(enemigo->getFila(), enemigo->getColumna()) - sf::Vector2f(0.f, ALTURA_CABEZA + 6.f));
		const Ataque& ataque = enemigo->getAtaqueAnunciado();
		for (const sf::Vector2i& casilla : ataque.casillas) {
			if (mapa.esValida(casilla.y, casilla.x)) {
				danioPorCasilla[casilla.y][casilla.x] += ataque.danio; // si dos le pegan a la misma, se suman
			}
		}
	}
	for (int fila = 0; fila < n; fila++) {
		for (int columna = 0; columna < n; columna++) {
			if (danioPorCasilla[fila][columna] > 0) {
				amenazas.push_back({ mapa.casillaAmundo(fila, columna), danioPorCasilla[fila][columna] });
			}
		}
	}
	hud.dibujarAmenazas(window, camara.getVista(), amenazas, atacantes);

	// Cuantos turnos le quedan a cada casilla de fuego (un poco mas abajo del centro, para no tapar las llamas)
	std::vector<Hud::TurnosFuego> fuegos;
	for (const Muro& muro : muros) {
		fuegos.push_back({ mapa.casillaAmundo(muro.fila, muro.columna) + sf::Vector2f(0.f, 8.f), muro.turnosRestantes });
	}
	hud.dibujarTurnosFuego(window, camara.getVista(), fuegos);

	// HUD: fijo en la pantalla (cambia a la vista por defecto). Le pasa donde esta el mouse para resaltar los botones
	sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window), window.getDefaultView());
	hud.dibujar(window, camara.getVista(), mago, codice, nivelActual, habilidadPreparada, magoSeleccionado, turnoJugador, mouse,
		catalogo.getDificultad(dificultad).getNombre());
}

void Juego::dibujarAlcance() {
	const Mapa& mapa = nivelActual.getMapa();
	const Habilidad& habilidad = mago.getHabilidad(habilidadPreparada);
	int alcance = habilidad.getAlcance();
	TipoHabilidad tipo = habilidad.getTipo();

	// Recorre el cuadrado alrededor del mago y se queda con las casillas a distancia 1..alcance
	for (int fila = mago.getFila() - alcance; fila <= mago.getFila() + alcance; fila++) {
		for (int columna = mago.getColumna() - alcance; columna <= mago.getColumna() + alcance; columna++) {
			int distancia = distanciaAlMago(fila, columna);
			if (distancia == 0 || distancia > alcance || !mapa.esValida(fila, columna)) {
				continue; // continue = saltea esta casilla y sigue con la proxima
			}

			// El muro va en casillas vacias: se marcan solo esas
			if (tipo == TipoHabilidad::MURO) {
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
	if (tipo == TipoHabilidad::MURO) {
		sf::Vector2i casilla = casillaBajoElMouse(); // x = columna, y = fila
		if (sePuedePonerMuro(casilla.y, casilla.x)) {
			for (const sf::Vector2i& fuego : casillasDelMuro(casilla.y, casilla.x)) {
				dibujarMarcador(texturaObjetivo, fuego.y, fuego.x);
			}
		}
	}
}

void Juego::dibujarAtaquesAnunciados() {
	const Mapa& mapa = nivelActual.getMapa();
	for (const auto& enemigo : nivelActual.getEnemigos()) {
		if (!enemigo->estaVivo() || !enemigo->tieneAtaqueAnunciado()) continue;
		for (const sf::Vector2i& casilla : enemigo->getAtaqueAnunciado().casillas) {
			if (mapa.esValida(casilla.y, casilla.x)) {
				dibujarMarcador(texturaPeligro, casilla.y, casilla.x);
			}
		}
	}
}

sf::Vector2i Juego::casillaBajoElMouse() const {
	// Igual que con el click: pixel de la ventana -> punto del mundo (con la vista de la camara) -> casilla
	sf::Vector2i pixel = sf::Mouse::getPosition(window);
	sf::Vector2f posicionMundo = window.mapPixelToCoords(pixel, camara.getVista());
	return nivelActual.getMapa().mundoAcasilla(posicionMundo);
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
	marcador.setPosition(nivelActual.getMapa().casillaAmundo(fila, columna));
	window.draw(marcador);
}

void Juego::dibujarExplosiones() {
	for (const Explosion& explosion : explosiones) {
		// El mismo rombo rojo de los objetivos, que se va desvaneciendo (alpha: 255 = opaco, 0 = invisible)
		float opacidad = 1.f - explosion.tiempo / DURACION_EXPLOSION;
		sf::Sprite marcador(texturaObjetivo);
		marcador.setColor(sf::Color(255, 255, 255, (sf::Uint8)(255 * opacidad)));
		marcador.setOrigin(32.f, 20.f);
		marcador.setPosition(nivelActual.getMapa().casillaAmundo(explosion.fila, explosion.columna));
		window.draw(marcador);
	}
}

// ---------------------------------------------------------------- Caminos

// Busqueda "en anchura" (BFS): primero todas las casillas a 1 paso, despues las de 2, y asi.
// Como solo avanza por casillas que se pueden pisar, rodea los pozos y las columnas.
std::vector<sf::Vector2i> Juego::casillasAlcanzables(const Personaje& quien, int pasosMaximos) const {
	int n = nivelActual.getMapa().getCantCasillas();

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

std::vector<std::vector<bool>> Juego::casillasOcupadas() const {
	int n = nivelActual.getMapa().getCantCasillas();
	std::vector<std::vector<bool>> ocupadas(n, std::vector<bool>(n, false));
	ocupadas[mago.getFila()][mago.getColumna()] = true;
	for (const auto& enemigo : nivelActual.getEnemigos()) {
		if (enemigo->estaVivo()) {
			ocupadas[enemigo->getFila()][enemigo->getColumna()] = true;
		}
	}
	return ocupadas;
}

// ---------------------------------------------------------------- Actualizar

void Juego::actualizar(float dt) {
	hud.actualizar(dt);  // los numeros de danio se animan siempre
	tiempoVolumen += dt; // y el aviso del volumen se va apagando
	nivelActual.getMapa().actualizar(dt); // y el fuego de los muros tambien

	// Las casillas rojas de los golpes tambien se animan siempre, y se borran cuando terminan
	for (Explosion& explosion : explosiones) {
		explosion.tiempo += dt;
	}
	explosiones.erase(std::remove_if(explosiones.begin(), explosiones.end(),
		[](const Explosion& e) { return e.tiempo >= DURACION_EXPLOSION; }), explosiones.end());

	if (estado == EstadoJuego::JUGANDO) {
		jugarTurno(dt);
	}
}

void Juego::jugarTurno(float dt) {
	// La bola del mago: mientras vuela, nadie hace nada mas. Cuando llega, les toca a los enemigos
	if (fase == FaseTurno::DISPARANDO) {
		moverProyectiles(dt);
		if (proyectiles.empty()) {
			terminarTurnoJugador();
		}
		return;
	}

	// Turno de los enemigos, primera parte: pegan (los que habian anunciado un ataque)
	if (fase == FaseTurno::ENEMIGOS_ATACAN) {
		ejecutarAtaquesEnemigos();
		return;
	}

	// Segunda parte: cuando llegan las bolas de los espectros (y paso un momento para ver los golpes), caminan
	if (fase == FaseTurno::ENEMIGOS_ESPERAN) {
		moverProyectiles(dt);
		tiempoFase += dt;
		if (proyectiles.empty() && tiempoFase >= PAUSA_ENEMIGOS) {
			moverEnemigos();
		}
	}
}

// ---------------------------------------------------------------- Partida

void Juego::reiniciarPartida(int nuevaDificultad) {
	dificultad = nuevaDificultad;
	partidaArchivo.borrar();
	empezarNivel(1);
	// Queda guardado el nivel 1 sin empezar: si se vuelve al menu desde la historia, "Continuar" ya anda
	partidaArchivo.guardar(Partida(1, dificultad));
	estado = EstadoJuego::HISTORIA;
}

void Juego::continuarPartida() {
	Partida partida = partidaArchivo.leer();
	// La dificultad con la que se empezo la partida (si el numero no existe, queda la Normal)
	dificultad = catalogo.getDificultad(partida.getDificultad()).getNumero();
	int numero = partida.getNivel();
	if (numero < 1 || numero > catalogo.getCantNiveles()) {
		numero = 1; // por si cambiaron los niveles de Datos/ y ese ya no existe
	}
	empezarNivel(numero);

	if (!partida.getEnCurso()) {
		estado = EstadoJuego::HISTORIA; // el nivel no habia empezado: se muestra su historia
		return;
	}

	// Quedo por la mitad: se arma como estaba. empezarNivel ya puso el tablero y la primera oleada,
	// y aca se pisa con lo guardado
	Mapa& mapa = nivelActual.getMapa();
	auto& enemigos = nivelActual.getEnemigos();
	nivelActual.setOleada(partida.getOleada());
	nivelActual.getCodice().setVida(partida.getVidaCodice());

	for (int i = 0; i < partida.getCantidadMuros(); i++) {
		int fila = partida.getFilaMuro(i);
		int columna = partida.getColumnaMuro(i);
		if (mapa.esValida(fila, columna) && mapa.getTipoTerreno(fila, columna) == TipoTerreno::SUELO) {
			mapa.setTipoTerreno(fila, columna, TipoTerreno::MURO);
			muros.push_back({ fila, columna, partida.getTurnosMuro(i) });
		}
	}

	MagoGuardado datosMago = partidaArchivo.leerMago();
	mago.mover(datosMago.getFila(), datosMago.getColumna());
	mago.setVida(datosMago.getVida());
	mago.setMana(datosMago.getMana());
	magoSeMovio = datosMago.getSeMovio();

	enemigos.clear();
	for (const EnemigoGuardado& guardado : partidaArchivo.leerEnemigos()) {
		std::unique_ptr<Enemigo> enemigo = catalogo.crearEnemigo(guardado.getTipo(), guardado.getFila(), guardado.getColumna(),
			catalogo.getDificultad(dificultad));
		enemigo->setVida(guardado.getVida());
		if (guardado.getTieneAtaque()) {
			enemigo->anunciarAtaque(guardado.getAtaque());
		}
		enemigos.push_back(std::move(enemigo));
	}

	estado = EstadoJuego::JUGANDO;
	revisarFinDePartida(); // por si no quedaba ningun enemigo: trae la proxima oleada
}

void Juego::empezarNivel(int numero) {
	// Un nivel nuevo, armado de cero: su tablero, su codice (con la vida segun la dificultad) y sin enemigos.
	// Se copia encima del anterior (y con el, se van el mapa, los enemigos y el codice viejos)
	nivelActual = Nivel(numero, catalogo, dificultad);
	nivelActual.iniciar();
	muros.clear(); // el tablero nuevo no tiene fuego

	// El mago se crea de nuevo (asi vuelve con la vida y el mana llenos) y sabe las habilidades de este nivel
	// y las de los anteriores (ver Habilidades.dat)
	mago = Mago(nivelActual.getFilaMago(), nivelActual.getColumnaMago());
	for (const Habilidad& habilidad : catalogo.getHabilidadesHastaNivel(numero)) {
		mago.aprenderHabilidad(habilidad);
	}

	proyectiles.clear();
	explosiones.clear();
	hud.limpiar();

	fase = FaseTurno::JUGADOR;
	tiempoFase = 0.f;
	habilidadPreparada = HabilidadSeleccionada::NINGUNA;
	magoSeleccionado = false;
	magoSeMovio = false;

	empezarOleada();
}

void Juego::cambiarNivel() {
	empezarNivel(nivelActual.getNumero() + 1);
	estado = EstadoJuego::JUGANDO;
	guardarPartidaEnCurso();
}

void Juego::empezarOleada() {
	auto& enemigos = nivelActual.getEnemigos();
	for (auto& enemigo : nivelActual.crearOleada()) {
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
	if (!mago.estaVivo() || nivelActual.getCodice().estaDestruido()) {
		estado = EstadoJuego::PERDISTE;
		sonidos.reproducirDerrota();
		habilidadPreparada = HabilidadSeleccionada::NINGUNA;
		magoSeleccionado = false;
		// "Continuar" vuelve a empezar este nivel desde el principio
		partidaArchivo.guardar(Partida(nivelActual.getNumero(), dificultad));
		return;
	}

	if (!nivelActual.getEnemigos().empty()) return; // todavia quedan enemigos: se sigue jugando

	if (nivelActual.hayOtraOleada()) {
		nivelActual.siguienteOleada();
		empezarOleada();

		// Respiro: la oleada nueva ya esta en el tablero, pero no actua en el mismo instante.
		// Le devolvemos el turno al jugador para que vea de donde salen y se prepare.
		// No se le da mana extra: eso lo hace empezarTurnoJugador() al empezar un turno normal.
		fase = FaseTurno::JUGADOR;
		habilidadPreparada = HabilidadSeleccionada::NINGUNA;
		magoSeleccionado = false;
		magoSeMovio = false;
		guardarPartidaEnCurso();
		return;
	}

	// No quedan oleadas: se gano el nivel
	nivelActual.completar();
	if (nivelActual.hayOtroNivel()) {
		estado = EstadoJuego::NIVEL_SUPERADO;
		// Queda guardado el nivel que sigue, sin empezar: si vuelve al menu, "Continuar" arranca ahi
		partidaArchivo.guardar(Partida(nivelActual.getNumero() + 1, dificultad));
	}
	else {
		partidaArchivo.borrar(); // se termino el juego
		estado = EstadoJuego::GANASTE;
		sonidos.reproducirVictoria();
	}
	habilidadPreparada = HabilidadSeleccionada::NINGUNA;
	magoSeleccionado = false;
}

void Juego::guardarPartidaEnCurso() {
	// En el turno de los enemigos hay cosas a medio pasar (bolas volando): no se guarda.
	// Un nivel ya ganado tampoco: lo que queda guardado es el que sigue (ver revisarFinDePartida)
	if (fase != FaseTurno::JUGADOR || nivelActual.estaCompletado()) return;

	Partida partida(nivelActual.getNumero(), dificultad);
	partida.setEnCurso(true);
	partida.setOleada(nivelActual.getOleada());
	partida.setVidaCodice(nivelActual.getCodice().getVida());
	for (const Muro& muro : muros) {
		partida.agregarMuro(muro.fila, muro.columna, muro.turnosRestantes);
	}

	MagoGuardado datosMago(mago.getVida(), mago.getMana(), mago.getFila(), mago.getColumna(), magoSeMovio);

	std::vector<EnemigoGuardado> datosEnemigos;
	for (const auto& enemigo : nivelActual.getEnemigos()) {
		if (enemigo->estaVivo()) {
			datosEnemigos.push_back(EnemigoGuardado(*enemigo));
		}
	}

	partidaArchivo.guardarEnCurso(partida, datosMago, datosEnemigos);
}

void Juego::volverAlMenu() {
	// Solo en el turno del jugador: en el de los enemigos (dura menos de un segundo) se ignora
	if (fase != FaseTurno::JUGADOR) return;
	habilidadPreparada = HabilidadSeleccionada::NINGUNA; // al salir al menu se cancela la habilidad
	magoSeleccionado = false;
	guardarPartidaEnCurso();
	estado = EstadoJuego::MENU;
}

// ---------------------------------------------------------------- Turno del jugador

void Juego::elegirHabilidad(HabilidadSeleccionada elegida) {
	// Solo en el turno del jugador, y si ya la aprendio
	if (fase != FaseTurno::JUGADOR) return;
	if (elegida == HabilidadSeleccionada::NINGUNA || !mago.tieneHabilidad(elegida)) return;

	// Elegir la misma dos veces cancela
	habilidadPreparada = (habilidadPreparada == elegida) ? HabilidadSeleccionada::NINGUNA : elegida;
	magoSeleccionado = false; // con una habilidad en la mano no se elige casilla para caminar
}

void Juego::pasarTurno() {
	if (fase != FaseTurno::JUGADOR) return;
	habilidadPreparada = HabilidadSeleccionada::NINGUNA;
	magoSeleccionado = false;
	terminarTurnoJugador();
}

void Juego::usarHabilidad(int fila, int columna) {
	if (fase != FaseTurno::JUGADOR) {
		return;
	}
	const Mapa& mapa = nivelActual.getMapa();
	const Habilidad& habilidad = mago.getHabilidad(habilidadPreparada);

	// El muro es distinto a las otras: va en una casilla VACIA, no sobre un enemigo
	if (habilidad.getTipo() == TipoHabilidad::MURO) {
		if (!sePuedePonerMuro(fila, columna)) {
			return;
		}
		// Mago::atacar gasta el mana; si no alcanza, devuelve un ataque sin casillas y no se hace nada
		if (mago.atacar(habilidadPreparada, fila, columna).casillas.empty()) {
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

	// El ataque va ultimo: gasta el mana, y si no alcanza no se hace nada
	Ataque ataque = mago.atacar(habilidadPreparada, fila, columna);
	if (ataque.casillas.empty()) {
		return;
	}

	if (habilidad.getTipo() == TipoHabilidad::BOLADEENERGIA) {
		// La bola sale del pecho del mago y va al pecho del enemigo.
		// El danio se aplica cuando llega (ver moverProyectiles)
		sf::Vector2f altura(0.f, ALTURA_PECHO);
		sf::Vector2f origen = mapa.casillaAmundo(mago.getFila(), mago.getColumna()) - altura;
		sf::Vector2f destino = mapa.casillaAmundo(fila, columna) - altura;

		proyectiles.push_back(std::make_unique<Proyectil>(origen, destino, texturaBola, ataque.danio, fila, columna));
		sonidos.reproducirBola();
		fase = FaseTurno::DISPARANDO;
	}
	else if (habilidad.getTipo() == TipoHabilidad::EMPUJE) {
		empujar(*objetivo, ataque.danio);
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
	const Mapa& mapa = nivelActual.getMapa();
	sf::Vector2i destino = destinoEmpuje(enemigo);

	// Contra el borde o contra otro personaje no se puede
	if (!mapa.esValida(destino.y, destino.x) || casillaOcupada(destino.y, destino.x)) {
		return false;
	}

	// Empuje forzado: el enemigo no elige, pero tiene que poder pararse ahi.
	// El abismo se permite siempre: si no puede pisarlo, se cae (contra una columna o el codice, no)
	TipoTerreno terreno = mapa.getTipoTerreno(destino.y, destino.x);
	return enemigo.puedePisar(terreno) || terreno == TipoTerreno::ABISMO;
}

void Juego::empujar(Enemigo& enemigo, int danio) {
	const Mapa& mapa = nivelActual.getMapa();
	sf::Vector2i destino = destinoEmpuje(enemigo); // se calcula antes de moverlo

	// El golpe le hace perder el ataque que tenia anunciado (como "Rafaga de viento" en el informe)
	enemigo.cancelarAtaque();

	daniarCasilla(enemigo.getFila(), enemigo.getColumna(), danio);
	if (!enemigo.estaVivo()) return; // el golpe ya lo mato

	// Empujado: se mueve aunque no pueda pisar ahi (por eso Personaje::mover y no Mapa::mover)
	enemigo.mover(destino.y, destino.x);

	if (!enemigo.puedePisar(mapa.getTipoTerreno(destino.y, destino.x))) {
		// No puede volar: cae al pozo y no vuelve. Se le saca toda la vida que le quedaba.
		// No pasa por daniarCasilla a proposito: el golem que cae al abismo no explota (explota en el fondo)
		hud.agregarDanio(mapa.casillaAmundo(destino.y, destino.x) - sf::Vector2f(0.f, ALTURA_CABEZA), enemigo.getVida());
		enemigo.recibirDanio(enemigo.getVida());
		return;
	}

	// Si lo empujaron al fuego (solo puede pasarle al espectro), se quema
	quemarSiEstaEnFuego(enemigo);
}

void Juego::terminarTurnoJugador() {
	sacarMuertos();
	fase = FaseTurno::ENEMIGOS_ATACAN;
	revisarFinDePartida(); // si se mato al ultimo, aparece la proxima oleada (o se gana)
}

// ---------------------------------------------------------------- Muro de fuego

bool Juego::casillaLibreParaMuro(int fila, int columna) const {
	// Solo suelo comun (transitable): no en el abismo, ni en una columna, ni en el codice, ni encima de otro fuego
	const Mapa& mapa = nivelActual.getMapa();
	return mapa.esValida(fila, columna)
		&& mapa.getCasilla(fila, columna).esTransitable()
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
	Mapa& mapa = nivelActual.getMapa();
	// Cuantos turnos dura sale de Habilidades.dat
	int turnos = mago.getHabilidad(HabilidadSeleccionada::MURO).getDuracion();
	for (const sf::Vector2i& casilla : casillasDelMuro(fila, columna)) {
		mapa.setTipoTerreno(casilla.y, casilla.x, TipoTerreno::MURO);
		muros.push_back({ casilla.y, casilla.x, turnos });
	}
}

void Juego::apagarMuros() {
	Mapa& mapa = nivelActual.getMapa();
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
	if (nivelActual.getMapa().getTipoTerreno(enemigo.getFila(), enemigo.getColumna()) != TipoTerreno::MURO) {
		return;
	}
	// Lo que quema el fuego es el "danio" del muro en Habilidades.dat
	int danioFuego = catalogo.getDatosHabilidad(TipoHabilidad::MURO).getDanio();
	daniarCasilla(enemigo.getFila(), enemigo.getColumna(), danioFuego);
	explosiones.push_back({ enemigo.getFila(), enemigo.getColumna(), 0.f }); // la casilla se marca en rojo un momento
	sonidos.reproducirQuemadura();
}

// ---------------------------------------------------------------- Turno de los enemigos

void Juego::ejecutarAtaquesEnemigos() {
	const Mapa& mapa = nivelActual.getMapa();
	auto& enemigos = nivelActual.getEnemigos();
	bool alguienAtaco = false;

	// Con indice y no con "for (auto& ...)": asi queda claro que no se borra nada mientras se recorre.
	// Los que mueren en este turno (por una explosion) se sacan todos juntos despues
	for (size_t i = 0; i < enemigos.size(); i++) {
		Enemigo& enemigo = *enemigos[i];
		if (!enemigo.estaVivo()) continue;

		// Si empieza su turno parado en el fuego, se vuelve a quemar (asi no le conviene quedarse ahi)
		quemarSiEstaEnFuego(enemigo);
		if (!enemigo.estaVivo() || !enemigo.tieneAtaqueAnunciado()) continue;

		// virtual: cada tipo ataca a su manera (el caballero golpea, el espectro tira una bola, el golem
		// explota y se destruye). Devuelve las casillas que habia marcado y borra el anuncio
		Ataque ataque = enemigo.atacar();
		alguienAtaco = true;

		// El espectro tira una bola y el golem explota (suenan distinto); el caballero pega cuerpo a cuerpo
		if (enemigo.atacaADistancia()) {
			sonidos.reproducirAtaqueEspectro();
		}
		else if (enemigo.getTipo() == TipoEnemigo::GOLEM) {
			sonidos.reproducirExplosion();
		}
		else {
			sonidos.reproducirAtaque();
		}

		// Le pega a la casilla que marco, este quien este ahi (si el mago se fue, no le pasa nada)
		for (const sf::Vector2i& casilla : ataque.casillas) {
			if (!mapa.esValida(casilla.y, casilla.x)) continue;
			if (enemigo.atacaADistancia()) {
				// Sale de su pecho y va al pecho de la casilla; el danio se aplica cuando llega
				sf::Vector2f altura(0.f, ALTURA_PECHO);
				sf::Vector2f origen = mapa.casillaAmundo(enemigo.getFila(), enemigo.getColumna()) - altura;
				sf::Vector2f destino = mapa.casillaAmundo(casilla.y, casilla.x) - altura;
				proyectiles.push_back(std::make_unique<Proyectil>(origen, destino, texturaBolaEspectro,
					ataque.danio, casilla.y, casilla.x));
			}
			else {
				explosiones.push_back({ casilla.y, casilla.x, 0.f }); // la casilla se marca en rojo un momento
				daniarCasilla(casilla.y, casilla.x, ataque.danio);
			}
		}
	}

	// Si nadie ataco, no hace falta la pausa: caminan enseguida
	tiempoFase = alguienAtaco ? 0.f : PAUSA_ENEMIGOS;
	fase = FaseTurno::ENEMIGOS_ESPERAN;
}

void Juego::moverEnemigos() {
	sacarMuertos();

	// Si los ataques mataron al mago o rompieron el codice, se termino
	if (!mago.estaVivo() || nivelActual.getCodice().estaDestruido()) {
		revisarFinDePartida();
		return;
	}

	const Mapa& mapa = nivelActual.getMapa();
	auto& enemigos = nivelActual.getEnemigos();
	for (size_t i = 0; i < enemigos.size(); i++) {
		Enemigo& enemigo = *enemigos[i];
		if (!enemigo.estaVivo()) continue;

		// Si todavia no le llega el ataque a su objetivo, da un paso hacia el.
		// virtual: cada tipo se mueve a su manera (los que caminan rodean pozos y columnas; el espectro vuela)
		sf::Vector2i objetivo = objetivoDe(enemigo); // x = columna, y = fila
		if (!enemigo.puedeAtacar(objetivo.y, objetivo.x)) {
			if (enemigo.mover(mapa, casillasOcupadas(), objetivo.y, objetivo.x)) {
				// El espectro puede cruzar el muro de fuego, pero se quema al entrar
				quemarSiEstaEnFuego(enemigo);
				if (!enemigo.estaVivo()) continue;
			}
			objetivo = objetivoDe(enemigo); // desde la casilla nueva puede tener otra cosa mas cerca
		}

		// Si desde donde quedo le llega, marca a donde va a pegar: el ataque llega en su proximo turno
		if (enemigo.puedeAtacar(objetivo.y, objetivo.x)) {
			enemigo.anunciarAtaque(enemigo.planearAtaque(objetivo.y, objetivo.x));
		}
	}

	sacarMuertos();
	empezarTurnoJugador();
}

sf::Vector2i Juego::objetivoDe(const Enemigo& enemigo) const {
	// Va por lo que tenga mas cerca: el mago o el codice. Si estan igual de lejos, prefiere el codice
	const Pergamino& codice = nivelActual.getCodice();
	int distanciaMago = distancia(enemigo.getFila(), enemigo.getColumna(), mago.getFila(), mago.getColumna());
	int distanciaCodice = distancia(enemigo.getFila(), enemigo.getColumna(), codice.getFila(), codice.getColumna());
	if (distanciaMago < distanciaCodice) {
		return sf::Vector2i(mago.getColumna(), mago.getFila());
	}
	return sf::Vector2i(codice.getColumna(), codice.getFila());
}

void Juego::empezarTurnoJugador() {
	fase = FaseTurno::JUGADOR;
	magoSeMovio = false;                // turno nuevo: puede volver a caminar
	mago.recuperarMana(catalogo.getDificultad(dificultad).getManaPorTurno()); // recupera un poco de mana (segun la dificultad)
	apagarMuros();                      // y al fuego le queda un turno menos
	revisarFinDePartida();

	// Se guarda solo al empezar cada turno: si se cierra el juego, "Continuar" sigue desde aca
	if (estado == EstadoJuego::JUGANDO) {
		guardarPartidaEnCurso();
	}
}

// ---------------------------------------------------------------- Danio

void Juego::daniarCasilla(int fila, int columna, int danio) {
	const Mapa& mapa = nivelActual.getMapa();
	Pergamino& codice = nivelActual.getCodice();
	if (!mapa.esValida(fila, columna)) return;

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
		if (!enemigo->estaVivo()) {
			alMorirEnemigo(*enemigo); // el golem explota al morir
		}
	}
}

void Juego::alMorirEnemigo(const Enemigo& enemigo) {
	// virtual: casi todos devuelven un ataque vacio; el golem, su explosion
	Ataque ataque = enemigo.ataqueAlMorir();
	if (ataque.casillas.empty()) return;

	const Mapa& mapa = nivelActual.getMapa();
	sonidos.reproducirExplosion();
	for (const sf::Vector2i& casilla : ataque.casillas) {
		if (!mapa.esValida(casilla.y, casilla.x)) continue;
		explosiones.push_back({ casilla.y, casilla.x, 0.f });
		// Si la explosion mata a otro golem, ese tambien explota (daniarCasilla vuelve a llamar a esta funcion).
		// No se repite para siempre: a los que ya estan muertos buscarEnemigoEn no los encuentra
		daniarCasilla(casilla.y, casilla.x, ataque.danio);
	}
}

void Juego::moverProyectiles(float dt) {
	for (auto& proyectil : proyectiles) {
		proyectil->mover(dt);
		if (proyectil->yaLlego()) {
			// Al chocar le pega a la casilla a la que iba
			Ataque golpe = proyectil->colisionar();
			for (const sf::Vector2i& casilla : golpe.casillas) {
				daniarCasilla(casilla.y, casilla.x, golpe.danio);
			}
			sonidos.reproducirImpacto();
		}
	}

	// Borra las que ya llegaron (mismo truco que con los enemigos muertos)
	proyectiles.erase(std::remove_if(proyectiles.begin(), proyectiles.end(),
		[](const std::unique_ptr<Proyectil>& p) { return p->yaLlego(); }), proyectiles.end());
}

void Juego::sacarMuertos() {
	auto& enemigos = nivelActual.getEnemigos();
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
	for (const auto& enemigo : nivelActual.getEnemigos()) {
			// Los muertos todavia pueden estar en el vector hasta que se saquen: esos no cuentan
			if (enemigo->estaVivo() && enemigo->estaEn(fila,columna)) {
					return enemigo.get(); // devuelve el puntero a ESE enemigo
			}
	}
	return nullptr; // si no habia nadie
}
