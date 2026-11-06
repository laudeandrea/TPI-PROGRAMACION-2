#include "Juego.h"

#include <algorithm>
#include <cstdlib> // std::abs

// La ventana y la camara se crean en la lista de inicializacion (despues de los ":")
// porque no tienen constructor sin parametros
Juego::Juego()
	: window(sf::VideoMode(ANCHO_VENTANA, ALTO_VENTANA), "El Codice Del Mago"),
	  mago(4,1),
	  codice(5,0,TipoHabilidad::BOLADEENERGIA),
	  camara(mapa.getTamanioMundo(), MARGEN_CAMARA), 
	  estado(EstadoJuego::MENU),
	  fase(FaseTurno::JUGADOR),
	  habilidadPreparada(HabilidadSeleccionada::NINGUNA),
	  magoSeleccionado(false),
	  enemigoEmpujado(nullptr)
{
	enemigos.push_back(std::make_unique<Caballero>(4, 6));
	enemigos.push_back(std::make_unique<Espectro>(1, 3));
	enemigos.push_back(std::make_unique<Golem>(1, 5));

	// La casilla donde se encuentra el codice (pergamino) queda ocupada, no puede ser pisada.
	mapa.setTipoTerreno(codice.getFila(),codice.getColumna(),TipoTerreno::SUELO_OCUPADO);

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

	// Habilidad: marcadores de las casillas al alcance y la bola de energia
	texturaMovimiento.loadFromFile("Imagenes/marcador_movimiento.png");
	texturaAlcance.loadFromFile("Imagenes/marcador_alcance.png");
	texturaObjetivo.loadFromFile("Imagenes/marcador_objetivo.png");
	texturaBola.loadFromFile("Imagenes/bola_energia.png");

	// Camara: encuadra el tablero y lo estira para llenar la ventana
	camara.ajustarAVentana(window.getSize());
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
			int opcion = menu.procesarEvento(event, window);
			if (opcion == 0) {              // Nueva partida
				estado = EstadoJuego::JUGANDO;
				fase = FaseTurno::JUGADOR;
				proyectil.reset(); // por si se salio al menu con una bola volando
				enemigoEmpujado = nullptr; // por si se salio al menu con un empuje a medias
				habilidadPreparada = HabilidadSeleccionada::NINGUNA;
			}
			if (opcion == 2) {              // Salir
				window.close();
			}
			// opcion 1 ("Como jugar"): todavia no hay pantalla, no hace nada
		}
		else {
			// Escape vuelve al menu (idea de Santiago)
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
				habilidadPreparada = HabilidadSeleccionada::NINGUNA; // al salir al menu se cancela la habilidad
				magoSeleccionado = false;
				estado = EstadoJuego::MENU;
				enemigoEmpujado = nullptr; // por si se salio al menu con un empuje a medias
			}

			// Click izquierdo: si la habilidad esta preparada dispara, si no mueve al mago
			if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left){
				sf::Vector2i pixel(event.mouseButton.x, event.mouseButton.y);
				// Con la vista de la camara: la que queda puesta en la ventana es la del HUD (la ultima que se dibujo)
				sf::Vector2f posicionMundo = window.mapPixelToCoords(pixel, camara.getVista());
				sf::Vector2i casilla = mapa.mundoAcasilla(posicionMundo); // x = columna, y = fila

				if (habilidadPreparada != HabilidadSeleccionada::NINGUNA) {
					const Habilidad& habilidad = mago.getHabilidad(habilidadPreparada);
					Enemigo* objetivo = buscarEnemigoEn(casilla.y, casilla.x);
					int distancia = distanciaAlMago(casilla.y, casilla.x);

					// gastarMana va ultimo: por el cortocircuito del &&, solo gasta si todo lo anterior se cumple
					if (habilidadPreparada == HabilidadSeleccionada::BOLA_DE_ENERGIA && fase == FaseTurno::JUGADOR && objetivo != nullptr
						&& distancia <= habilidad.getAlcance() && mago.gastarMana(habilidad.getCostoMana())) {
						// La bola sale del pecho del mago y va al pecho del enemigo.
						// El danio se aplica cuando llega (ver impactarProyectil)
						sf::Vector2f altura(0.f, ALTURA_PECHO);
						sf::Vector2f origen = mapa.casillaAmundo(mago.getFila(), mago.getColumna()) - altura;
						sf::Vector2f destino = mapa.casillaAmundo(casilla.y, casilla.x) - altura;

						proyectil = std::make_unique<Proyectil>(origen, destino, texturaBola,
							habilidad.getDanio(), casilla.y, casilla.x);

						habilidadPreparada = HabilidadSeleccionada::NINGUNA;
						fase = FaseTurno::DISPARANDO;
					}
					else if (habilidadPreparada == HabilidadSeleccionada::EMPUJE
						&& fase == FaseTurno::JUGADOR
						&& objetivo != nullptr
						&& distancia <= habilidad.getAlcance()) {

						// Direccion del empuje: del mago hacia el enemigo
						int pasoFila = 0, pasoColumna = 0;
						if (objetivo->getFila() > mago.getFila()) pasoFila = 1;
						else if (objetivo->getFila() < mago.getFila()) pasoFila = -1;

						if (objetivo->getColumna() > mago.getColumna()) pasoColumna = 1;
						else if (objetivo->getColumna() < mago.getColumna()) pasoColumna = -1;

						int nuevaFila = objetivo->getFila() + pasoFila;
						int nuevaColumna = objetivo->getColumna() + pasoColumna;

						// Empuje forzado: no usa puedeMover (el enemigo no elige),
						// pero el destino tiene que existir, estar libre y poder recibirlo.
						bool puedeEmpujar = mapa.estaDentro(nuevaFila, nuevaColumna)
							&& !casillaOcupada(nuevaFila, nuevaColumna);

						if (puedeEmpujar) {
							TipoTerreno terreno = mapa.getTipoTerreno(nuevaFila, nuevaColumna);
							// El abismo se permite siempre: si no puede pisarlo, se cae
							puedeEmpujar = objetivo->puedePisar(terreno) || terreno == TipoTerreno::ABISMO;
						}

						// gastarMana ultimo: solo gasta si el empuje se va a hacer
						if (puedeEmpujar && mago.gastarMana(habilidad.getCostoMana())) {
							objetivo->mover(nuevaFila, nuevaColumna);

							// Si cayo al abismo y no puede estar ahi, muere
							if (mapa.getTipoTerreno(nuevaFila, nuevaColumna) == TipoTerreno::ABISMO
								&& !objetivo->puedePisar(TipoTerreno::ABISMO)) {
								objetivo->recibirDanio(objetivo->getVida());
							}

							// Sacar a los que murieron (mismo criterio que impactarProyectil)
							bool sobrevivio = objetivo->estaVivo();
							enemigos.erase(std::remove_if(enemigos.begin(), enemigos.end(),
								[](const std::unique_ptr<Enemigo>& e) { return !e->estaVivo(); }),
								enemigos.end());

							enemigoEmpujado = sobrevivio ? objetivo : nullptr;
							habilidadPreparada = HabilidadSeleccionada::NINGUNA;
							fase = FaseTurno::ENEMIGOS;
						}
					}
				}
				else {
					// --- MOVER ---
					// 1er click en el mago: lo selecciona y muestra a donde puede ir.
					// 2do click: en una casilla marcada lo mueve; en cualquier otro lado cancela.
					if (fase == FaseTurno::JUGADOR) {
						if (mago.estaEn(casilla.y, casilla.x)) {
							magoSeleccionado = !magoSeleccionado;
						}
						else if (magoSeleccionado) {
							if (magoPuedeIrA(casilla.y, casilla.x)) {
								mago.mover(casilla.y, casilla.x);
								fase = FaseTurno::ENEMIGOS;
							}
							magoSeleccionado = false;
						}
					}
				}
			}
			//reemplace la anterior eleccion de Q para la habilidad por 1 y 2
			if (event.type == sf::Event::KeyPressed && fase == FaseTurno::JUGADOR) {
				HabilidadSeleccionada elegida = HabilidadSeleccionada::NINGUNA;
				if (event.key.code == sf::Keyboard::Num1 || event.key.code == sf::Keyboard::Numpad1) {
					elegida = HabilidadSeleccionada::BOLA_DE_ENERGIA;
				}
				if (event.key.code == sf::Keyboard::Num2 || event.key.code == sf::Keyboard::Numpad2) {
					elegida = HabilidadSeleccionada::EMPUJE;
				}

				if (elegida != HabilidadSeleccionada::NINGUNA) {
					// apretar la misma tecla dos veces cancela
					habilidadPreparada = (habilidadPreparada == elegida)
						? HabilidadSeleccionada::NINGUNA : elegida;
					magoSeleccionado = false;
				}
			}
		}
	}
	
};

bool Juego::casillaOcupada(int fila, int columna) const {
	if (mago.estaEn(fila, columna)) return true;
	for (const auto& enemigo : enemigos) {
		if (enemigo->estaEn(fila, columna)) return true;
	}
	return false;
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
		for (const auto& enemigo : enemigos) {
			enemigo->mostrarBarraVida(window, mapa);
		}

		// La bola vuela por encima de todo
		if (proyectil != nullptr) {
			proyectil->mostrar(window);
		}

		// HUD: fijo en la pantalla (cambia a la vista por defecto)
		hud.dibujar(window, camara.getVista(), mago, habilidadPreparada, magoSeleccionado);
	}

	window.display();
};

void Juego::dibujarAlcance() {
	int alcance = mago.getHabilidad(habilidadPreparada).getAlcance();

	// Recorre el cuadrado alrededor del mago y se queda con las casillas a distancia 1..alcance
	for (int fila = mago.getFila() - alcance; fila <= mago.getFila() + alcance; fila++) {
		for (int columna = mago.getColumna() - alcance; columna <= mago.getColumna() + alcance; columna++) {
			int distancia = distanciaAlMago(fila, columna);
			if (distancia == 0 || distancia > alcance || !mapa.estaDentro(fila, columna)) {
				continue; // continue = saltea esta casilla y sigue con la proxima
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

void Juego::impactarProyectil() {
	Enemigo* objetivo = buscarEnemigoEn(proyectil->getFilaObjetivo(), proyectil->getColumnaObjetivo());
	if (objetivo != nullptr) {
		objetivo->recibirDanio(proyectil->getDanio());
		// El numero aparece arriba de la cabeza: los sprites miden 64 de alto
		sf::Vector2f cabeza = mapa.casillaAmundo(objetivo->getFila(), objetivo->getColumna()) - sf::Vector2f(0.f, 72.f);
		hud.agregarDanio(cabeza, proyectil->getDanio());
	}

	// Saca del vector a los enemigos que quedaron sin vida
	enemigos.erase(std::remove_if(enemigos.begin(), enemigos.end(),
		[](const std::unique_ptr<Enemigo>& e) { return !e->estaVivo(); }), enemigos.end());

	proyectil.reset(); // borra la bola (el unique_ptr queda en nullptr)
	fase = FaseTurno::ENEMIGOS;
}

void Juego::actualizar(float dt) {
	hud.actualizar(dt); // los numeros de danio se animan siempre

	if (estado != EstadoJuego::JUGANDO) return;

	if (fase == FaseTurno::DISPARANDO) {
		proyectil->actualizar(dt);
		if (proyectil->yaLlego()) {
			impactarProyectil();
		}
		return; // mientras vuela, los enemigos esperan
	}

	if (fase != FaseTurno::ENEMIGOS) return;

	for (auto& enemigo : enemigos) {
		if (!enemigo->estaVivo()) continue;
		if (enemigo.get() == enemigoEmpujado) continue; // el empujado ya se movio, no hace nada mas

		sf::Vector2i destino = enemigo->calcularPaso(mapa, mago);

		bool quiereMoverse = !enemigo->estaEn(destino.y, destino.x);
		if (quiereMoverse && puedeMover(*enemigo, destino.y, destino.x)) {
			enemigo->mover(destino.y, destino.x);
		}
	}
	enemigoEmpujado = nullptr; // ya se movio, no hace nada mas

	fase = FaseTurno::JUGADOR;
	mago.recuperarMana(MANA_POR_TURNO); // empieza el turno del jugador: recupera un poco de mana
}

int Juego::distanciaAlMago(int fila, int columna) const {
	return std::abs(mago.getFila() - fila) + std::abs(mago.getColumna() - columna);
}

Enemigo* Juego::buscarEnemigoEn(int fila, int columna) const{
	for (const auto& enemigo : enemigos) {
			if (enemigo->estaEn(fila,columna)) {
					return enemigo.get(); // devuelve el puntero a ESE enemigo
			}
	}
	return nullptr; // si no habia nadie
}
