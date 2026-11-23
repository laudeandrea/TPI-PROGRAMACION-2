#include "Sonidos.h"
#include <iostream>

void Sonidos::cargar(sf::SoundBuffer& buffer, sf::Sound& sonido, const std::string& ruta, float volumen) {
	if (!buffer.loadFromFile(ruta)) {
		std::cout << "No se pudo cargar " << ruta << '\n';
	}
	sonido.setBuffer(buffer);
	sonido.setVolume(volumen); // 0 a 100
}

Sonidos::Sonidos() {
	cargar(bufferBola, bola, "Sonidos/bola_energia.ogg", 70.f);
	cargar(bufferImpacto, impacto, "Sonidos/impacto.ogg", 80.f);
	cargar(bufferAtaque, ataque, "Sonidos/ataque_enemigo.wav", 80.f);
	cargar(bufferAtaqueEspectro, ataqueEspectro, "Sonidos/ataque_espectro.ogg", 70.f);
	cargar(bufferInicio, inicio, "Sonidos/inicio.ogg", 80.f);
	cargar(bufferVictoria, victoria, "Sonidos/victoria.wav", 80.f);
	cargar(bufferDerrota, derrota, "Sonidos/derrota.ogg", 80.f);

	if (!musica.openFromFile("Sonidos/musica_medieval.wav")) {
		std::cout << "No se pudo cargar Sonidos/musica_medieval.wav\n";
	}
	musica.setLoop(true);
	musica.setVolume(7.f); 
}

void Sonidos::iniciarMusica() {
	// Si ya esta sonando, no se reinicia (por ejemplo, al volver al menu desde una partida)
	if (musica.getStatus() != sf::Music::Playing) {
		musica.play();
	}
}

void Sonidos::cambiarVolumenMusica(float cambio) {
	float volumen = musica.getVolume() + cambio;
	if (volumen < 0.f) volumen = 0.f;
	if (volumen > 100.f) volumen = 100.f;
	musica.setVolume(volumen);
	std::cout << "Volumen musica: " << volumen << '\n';
}

float Sonidos::getVolumenMusica() const {
	return musica.getVolume();
}

void Sonidos::reproducirBola() {
	bola.play(); // si ya estaba sonando, vuelve a empezar
}

void Sonidos::reproducirImpacto() {
	impacto.play();
}

void Sonidos::reproducirAtaque() {
	ataque.play();
}

void Sonidos::reproducirAtaqueEspectro() {
	ataqueEspectro.play();
}

void Sonidos::reproducirInicio() {
	inicio.play();
}

void Sonidos::reproducirVictoria() {
	musica.stop(); // se para el fondo para que se escuche el jingle
	victoria.play();
}

void Sonidos::reproducirDerrota() {
	musica.stop();
	derrota.play();
}
