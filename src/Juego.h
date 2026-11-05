#pragma once

#include <SFML/graphics.hpp>
#include "Camara.h"
#include "Mapa.h"
#include "Mago.h"
#include "Menu.h"
#include "Caballero.h"
#include "Golem.h"
#include "Espectro.h"
#include "Pergamino.h"
#include "Proyectil.h"
#include "Hud.h"
#include <memory>
#include <vector>

enum class EstadoJuego{
    MENU,
    JUGANDO
};

enum class FaseTurno {
	JUGADOR,
	DISPARANDO, // la bola esta volando: nadie se mueve hasta que llegue
	ENEMIGOS
};

class Juego {
	private:
		static const int ANCHO_VENTANA = 1280;
		static const int ALTO_VENTANA = 720;
		static constexpr float MARGEN_CAMARA = 64.f; // espacio alrededor del tablero para el HUD
		static const int MANA_POR_TURNO = 1;         // lo que recupera el mago al empezar cada turno suyo
		static constexpr float ALTURA_PECHO = 30.f;  // desde los pies hasta el pecho: de ahi sale y ahi pega la bola

		// El orden importa. C++ crea los atributos en el orden en que estan declarados,
		// y la camara necesita que el mapa ya exista (usa mapa.getTamanioMundo())
		sf::RenderWindow window;
		sf::Texture texturaFondo;
		sf::Sprite fondo;

		Mapa mapa;
		Mago mago;
		std::vector<std::unique_ptr<Enemigo>> enemigos;
		Pergamino codice;
		Camara camara;
		Menu menu;
		EstadoJuego estado;
		FaseTurno fase;
		bool habilidadPreparada;
		bool magoSeleccionado; // se hizo click en el mago: se muestran las casillas a las que puede ir

		// Marcadores de casillas (movimiento y alcance de la habilidad) y la bola que vuela
		sf::Texture texturaMovimiento, texturaAlcance, texturaObjetivo, texturaBola;
		std::unique_ptr<Proyectil> proyectil; // nullptr cuando no hay ninguna bola volando
		Hud hud;
		sf::Clock reloj; // mide el tiempo entre cuadros, para las animaciones

		void procesarEventos();
		/** @brief Avanza el juego. dt = segundos que pasaron desde el cuadro anterior. */
		void actualizar(float dt);
		void dibujar();
		/** @brief Marca las casillas a las que llega la habilidad (en rojo si hay un enemigo). */
		void dibujarAlcance();
		/** @brief Marca las casillas a las que puede caminar el mago este turno. */
		void dibujarMovimiento();
		/** @brief Dibuja un marcador (rombo de 64x40) sobre una casilla. */
		void dibujarMarcador(const sf::Texture& textura, int fila, int columna);
		/** @brief Casillas a las que puede llegar caminando como mucho pasosMaximos pasos (sin contar la suya).
		 *  Rodea lo que no puede pisar (abismo, columnas, otros personajes). Cada casilla: x = columna, y = fila. */
		std::vector<sf::Vector2i> casillasAlcanzables(const Personaje& quien, int pasosMaximos) const;
		/** @brief Si el mago puede caminar hasta esa casilla este turno. */
		bool magoPuedeIrA(int fila, int columna) const;
		/** @brief Lo que pasa cuando la bola llega: danio, numero flotante y sacar a los muertos. */
		void impactarProyectil();
		/** @brief Si hay otro personaje parado en esa casilla. */
		bool casillaOcupada(int fila, int columna) const;
		/** @brief Si ese personaje puede moverse a esa casilla: terreno permitido y libre. */
		bool puedeMover(const Personaje& quien, int fila, int columna) const;
		Enemigo* buscarEnemigoEn(int fila, int columna) const;
		/** @brief Distancia en casillas caminando en fila + en columna (distancia "Manhattan"). */
		int distanciaAlMago(int fila, int columna) const;
	public:
		Juego();
		void ejecutar();
};