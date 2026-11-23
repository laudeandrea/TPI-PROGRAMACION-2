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

		sf::Music musica;

		// Carga el archivo en el buffer y lo engancha al sonido. Si falla, avisa por consola.
		void cargar(sf::SoundBuffer& buffer, sf::Sound& sonido, const std::string& ruta, float volumen);

	public:
		Sonidos();
		void iniciarMusica(); // arranca la musica si no esta sonando
		// Sube o baja el volumen de la musica (0 a 100) y lo muestra por consola.
		// Sirve para calibrar sin recompilar: - baja y = sube.
		void cambiarVolumenMusica(float cambio);
		float getVolumenMusica() const;
		void reproducirBola();
		void reproducirImpacto();
		void reproducirAtaque();
		void reproducirAtaqueEspectro();
		void reproducirInicio();
		void reproducirVictoria();
		void reproducirDerrota();
};
