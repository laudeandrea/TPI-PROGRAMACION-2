#pragma once
#include <SFML/Audio.hpp>
#include <string>

/* Carga los efectos y la musica una sola vez y los reproduce.
 * Los buffers son miembros: si se destruyen, los sonidos quedan apuntando a memoria invalida.
 * La musica usa sf::Music porque se lee del disco mientras suena (no ocupa RAM como un buffer). */
class Sonidos {
	private:
		sf::SoundBuffer bufferBola;
		sf::Sound bola;

		sf::SoundBuffer bufferImpacto;
		sf::Sound impacto;

		sf::SoundBuffer bufferAtaque;
		sf::Sound ataque;

		sf::SoundBuffer bufferAtaqueEspectro;
		sf::Sound ataqueEspectro;

		sf::SoundBuffer bufferInicio;
		sf::Sound inicio;

		sf::SoundBuffer bufferVictoria;
		sf::Sound victoria;

		sf::SoundBuffer bufferDerrota;
		sf::Sound derrota;

		sf::SoundBuffer bufferExplosion; // el golem, al llegar o al morir
		sf::Sound explosion;

		sf::SoundBuffer bufferQuemadura; // un enemigo que se quema en el muro de fuego
		sf::Sound quemadura;

		// El esqueleto arquero: el disparo y el silbido de la flecha suenan juntos (el silbido arranca suave,
		// cuando el disparo ya se esta apagando), y el impacto cuando la flecha llega
		sf::SoundBuffer bufferDisparoArco;
		sf::Sound disparoArco;
		sf::SoundBuffer bufferVueloFlecha;
		sf::Sound vueloFlecha;
		sf::SoundBuffer bufferImpactoFlecha;
		sf::Sound impactoFlecha;

		sf::Music musica;

		// Carga el archivo en el buffer y lo engancha al sonido. Si falla, avisa por consola.
		void cargar(sf::SoundBuffer& buffer, sf::Sound& sonido, const std::string& ruta, float volumen);

	public:
		Sonidos();
		void iniciarMusica(); // arranca la musica si no esta sonando
		// Sube o baja el volumen de la musica (0 a 100). El Juego lo muestra un momento en pantalla.
		// Sirve para calibrar sin recompilar: - baja y = sube.
		void cambiarVolumenMusica(float cambio);
		float getVolumenMusica() const;
		void reproducirBola();           // el mago tira la bola de energia
		void reproducirImpacto();        // llega una bola (la del mago o la del espectro)
		void reproducirAtaque();         // el caballero pega
		void reproducirAtaqueEspectro(); // el espectro tira su bola
		void reproducirDisparoArco();    // el arquero dispara (el disparo y el silbido de la flecha)
		void reproducirImpactoFlecha();  // llega la flecha
		void reproducirInicio();         // empieza una partida
		void reproducirVictoria();       // se gano el ultimo nivel: para la musica de fondo y suena el jingle
		void reproducirDerrota();        // se perdio: para la musica de fondo y suena el jingle
		void reproducirExplosion();      // el golem explota
		void reproducirQuemadura();      // un enemigo se quema en el muro de fuego
};
