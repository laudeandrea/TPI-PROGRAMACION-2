#include "Catalogo.h"
#include <filesystem> // std::filesystem::create_directories: crea la carpeta Datos si no existe
#include "Caballero.h"
#include "Espectro.h"
#include "Golem.h"
#include "EsqueletoArquero.h"

// Donde estan los archivos (relativo a la carpeta desde donde se ejecuta el juego, igual que Imagenes/)
static const std::string CARPETA_DATOS = "Datos";

// Va antes del constructor, que la usa
template <typename T>
std::vector<T> Catalogo::leerOCrear(const std::string& ruta, const std::vector<T>& porDefecto) const {
	Archivo<T> archivo(ruta);
	std::vector<T> registros = archivo.leerTodos();
	if (!registros.empty()) {
		return registros;
	}

	// No existe (o esta vacio): se crea con los valores por defecto. Si no se puede escribir
	// (por ejemplo, la carpeta es de solo lectura), igual se juega con estos valores
	std::filesystem::create_directories(CARPETA_DATOS);
	archivo.guardarTodos(porDefecto);
	return porDefecto;
}

Catalogo::Catalogo() {
	habilidades = leerOCrear(CARPETA_DATOS + "/Habilidades.dat", habilidadesPorDefecto());
	enemigos = leerOCrear(CARPETA_DATOS + "/TiposEnemigo.dat", enemigosPorDefecto());
	niveles = leerOCrear(CARPETA_DATOS + "/Niveles.dat", nivelesPorDefecto());
	apariciones = leerOCrear(CARPETA_DATOS + "/NivelEnemigos.dat", aparicionesPorDefecto());
	dificultades = leerOCrear(CARPETA_DATOS + "/Dificultades.dat", dificultadesPorDefecto());
}

// ---------------------------------------------------------------- Valores por defecto

std::vector<DatosHabilidad> Catalogo::habilidadesPorDefecto() const {
	// tipo, nombre, danio, costo de mana, alcance, duracion (turnos), nivel desde el que se tiene
	return {
		DatosHabilidad(TipoHabilidad::BOLADEENERGIA, "Bola de energia", 10, 3, 3, 0, 1),
		// Empuje: el enemigo tiene que estar pegado (alcance 1). Si cae al abismo, no vuelve
		DatosHabilidad(TipoHabilidad::EMPUJE, "Empuje", 5, 3, 1, 0, 1),
		// Muro: el danio es lo que pierde el espectro cada vez que se quema
		DatosHabilidad(TipoHabilidad::MURO, "Muro de fuego", 10, 4, 3, 3, 2)
	};
}

std::vector<DatosEnemigo> Catalogo::enemigosPorDefecto() const {
	// tipo, nombre, vida, danio, alcance
	return {
		DatosEnemigo(TipoEnemigo::CABALLERO, "Caballero", 20, 12, 1), // 2 bolas de energia
		// Espectro: 6 de danio (era 8). En la simulacion era el que mas le pegaba al codice en el nivel 2
		DatosEnemigo(TipoEnemigo::ESPECTRO, "Espectro", 20, 6, 3),
		// Golem: 40 de vida (era 60), 4 bolas. Con 60 casi nunca se lo mataba y su explosion al morir no se veia
		DatosEnemigo(TipoEnemigo::GOLEM, "Golem", 40, 30, 1),          // pega fuerte, pero una sola vez
		DatosEnemigo(TipoEnemigo::ESQUELETO, "Esqueleto arquero", 20, 9, 4)
	};
}

std::vector<DatosNivel> Catalogo::nivelesPorDefecto() const {
	// numero, tablero (S = suelo, C = columna rota, X = abismo; cada string es una fila, de la 0 a la 7),
	// fila y columna del codice, fila y columna del mago, cantidad de oleadas, vida del codice, historia (sin tildes)
	return {
		DatosNivel(1, {
				"SSSSSSSS",
				"SSSSSSSS",
				"SSCSXXSS",
				"SSSSXXSS",
				"SSSSSCSS",
				"SSXXSSSS",
				"SSXXSSCS",
				"SSSSSSSS"
			}, 5, 0, 4, 1, 2, 80, {
				"El gran archimago desaparecio y las",
				"criaturas del caos invadieron su torre.",
				"Tomas, su aprendiz, tiene que cuidar el",
				"codice: el libro donde estan sus hechizos.",
				"Si lo destruyen, la magia se pierde."
			}),
		// Nivel 2: dos pozos parten el tablero en dos. Para llegar al codice (arriba) los enemigos tienen
		// que pasar por el medio (columnas 3 y 4) o por los bordes. El muro de fuego sirve para cerrar uno de esos pasos.
		// El mago arranca al lado del codice, no adelante (en (2,4) la imagen del mago lo tapaba).
		// El codice tiene 100 de vida (en el nivel 1, 80): en la simulacion el salto de dificultad era muy grande
		DatosNivel(2, {
				"SSSSSSSS",
				"SSSSSSCS",
				"SCSSSSSS",
				"SXXSSXXS",
				"SXXSSXXS",
				"SSSSSSSS",
				"SSSCSSSS",
				"SSSSSSSS"
			}, 1, 3, 2, 2, 2, 100, {
				"En el segundo piso, Tomas encuentra una",
				"pagina perdida del grimorio: el muro de",
				"fuego. Dos pozos parten la sala en dos y",
				"las criaturas suben desde abajo.",
				"Hay que cerrarles el paso."
			}),
			// Nivel 3: la prueba final. Dos abismos a los costados dejan un pasillo central y las columnas de
			// (3,3) y (4,2) obligan a zigzaguear. Los arqueros se quedan a 4 casillas del codice y tiran desde ahi.
			// El codice tiene 120 de vida (80 -> 100 -> 120)
			DatosNivel(3,{
					"SSSSSSSS",
					"XXSSSSXX",
					"XXSSSSXX",
					"SSSCSSSS",
					"SSCSSSSS",
					"SSSSSSSS",
					"SSSSSSSS",
					"SSSSSSSS"
				}, 0, 3, 1, 2, 2, 120,{
					"En el ultimo piso espera la cripta:",
					"los esqueletos arqueros cuidan la torre.",
					"Sus flechas llegan antes de que puedas",
					"verlos: hay que cortarles el paso.",
					"El codice resiste, pero no para siempre."
				})
	};
}

std::vector<DatosAparicion> Catalogo::aparicionesPorDefecto() const {
	// nivel, oleada, tipo, fila, columna. Todas las casillas de salida tienen que ser de suelo
	return {
		// Nivel 1: salen del lado derecho, lejos del codice (que esta en la columna 0).
		// Primera oleada: uno cuerpo a cuerpo y uno de lejos. Ultima: tres, con el golem de remate
		DatosAparicion(1, 1, TipoEnemigo::CABALLERO, 4, 6),
		DatosAparicion(1, 1, TipoEnemigo::ESPECTRO, 1, 3),
		DatosAparicion(1, 2, TipoEnemigo::CABALLERO, 0, 7),
		DatosAparicion(1, 2, TipoEnemigo::ESPECTRO, 3, 7),
		DatosAparicion(1, 2, TipoEnemigo::GOLEM, 7, 6),

		// Nivel 2: salen de abajo (filas 6 y 7), del otro lado de los pozos. El codice esta arriba, en (1, 3)
		DatosAparicion(2, 1, TipoEnemigo::CABALLERO, 6, 1),
		DatosAparicion(2, 1, TipoEnemigo::CABALLERO, 6, 6),
		DatosAparicion(2, 1, TipoEnemigo::ESPECTRO, 7, 4),
		DatosAparicion(2, 2, TipoEnemigo::GOLEM, 7, 1),
		DatosAparicion(2, 2, TipoEnemigo::CABALLERO, 6, 4),
		DatosAparicion(2, 2, TipoEnemigo::GOLEM, 7, 6),
		DatosAparicion(2, 2, TipoEnemigo::ESPECTRO, 6, 7),

		// Nivel 3: todos salen de la ultima fila, que es la unica puerta de la cripta.
		// Primera oleada: dos caballeros abren paso y un arquero dispara desde atras.
		// Ultima: dos arqueros cruzados, un golem de escudo y un espectro que vuela por arriba de todo
		DatosAparicion(3, 1, TipoEnemigo::CABALLERO, 7, 1),
		DatosAparicion(3, 1, TipoEnemigo::CABALLERO, 7, 6),
		DatosAparicion(3, 1, TipoEnemigo::ESQUELETO, 7, 3),
		DatosAparicion(3, 2, TipoEnemigo::ESQUELETO, 7, 1),
		DatosAparicion(3, 2, TipoEnemigo::GOLEM, 7, 3),
		DatosAparicion(3, 2, TipoEnemigo::ESQUELETO, 7, 6),
		DatosAparicion(3, 2, TipoEnemigo::ESPECTRO, 7, 4)
	};
}

std::vector<DatosDificultad> Catalogo::dificultadesPorDefecto() const {
	// numero, nombre, descripcion (sin tildes, hasta 47 letras), % de la vida del codice, % de la vida de los enemigos,
	// % del danio de los enemigos, mana que recupera el mago por turno.
	// Los valores salen de simular mil partidas por nivel con jugadores automaticos de tres tipos (ver README)
	return {
		// Facil: para el que recien empieza. Un jugador "novato" gana mas o menos 6 de cada 10 veces cada piso
		DatosDificultad(1, "Facil", "Codice mas fuerte y enemigos que pegan menos", 175, 100, 50, 3),
		// Normal: como fue pensado el juego. Un jugador "medio" gana casi 9 de cada 10 veces cada piso
		DatosDificultad(2, "Normal", "Como fue pensado el juego", 100, 100, 100, 2),
		// Dificil: para el que ya lo domina. Jugando sin errores se gana; el jugador "medio" gana entre 2 y 5 de cada 10
		DatosDificultad(3, "Dificil", "Enemigos mas fuertes y un codice mas fragil", 65, 150, 150, 2)
	};
}

// ---------------------------------------------------------------- Habilidades

const DatosHabilidad& Catalogo::getDatosHabilidad(TipoHabilidad tipo) const {
	for (const DatosHabilidad& datos : habilidades) {
		if (datos.getTipo() == tipo) {
			return datos;
		}
	}
	return habilidades[0]; // no deberia pasar: el archivo tiene las tres
}

Habilidad Catalogo::crearHabilidad(TipoHabilidad tipo) const {
	const DatosHabilidad& datos = getDatosHabilidad(tipo);
	return Habilidad(datos.getTipo(), datos.getNombre(), datos.getDanio(), datos.getCostoMana(),
		datos.getAlcance(), datos.getDuracion());
}

std::vector<Habilidad> Catalogo::getHabilidadesHastaNivel(int nivel) const {
	std::vector<Habilidad> resultado;
	for (const DatosHabilidad& datos : habilidades) {
		if (datos.getNivelDesbloqueo() <= nivel) {
			resultado.push_back(crearHabilidad(datos.getTipo()));
		}
	}
	return resultado;
}

std::vector<DatosHabilidad> Catalogo::getHabilidadesNuevas(int nivel) const {
	std::vector<DatosHabilidad> resultado;
	for (const DatosHabilidad& datos : habilidades) {
		if (datos.getNivelDesbloqueo() == nivel) {
			resultado.push_back(datos);
		}
	}
	return resultado;
}

// ---------------------------------------------------------------- Enemigos

const DatosEnemigo& Catalogo::getDatosEnemigo(TipoEnemigo tipo) const {
	for (const DatosEnemigo& datos : enemigos) {
		if (datos.getTipo() == tipo) {
			return datos;
		}
	}
	return enemigos[0];
}

std::unique_ptr<Enemigo> Catalogo::crearEnemigo(TipoEnemigo tipo, int fila, int columna, const DatosDificultad& dificultad) const {
	const DatosEnemigo& datos = getDatosEnemigo(tipo);
	// La vida y el danio de TiposEnemigo.dat, con el porcentaje de la dificultad (en Normal quedan iguales)
	int vida = dificultad.ajustarVidaEnemigo(datos.getVida());
	int danio = dificultad.ajustarDanioEnemigo(datos.getDanio());
	switch (tipo) {
	case TipoEnemigo::ESPECTRO:
		return std::make_unique<Espectro>(fila, columna, vida, danio, datos.getAlcance());
	case TipoEnemigo::GOLEM:
		return std::make_unique<Golem>(fila, columna, vida, danio);
	case TipoEnemigo::ESQUELETO:
		return std::make_unique<EsqueletoArquero>(fila, columna, vida, danio, datos.getAlcance());
	default:
		return std::make_unique<Caballero>(fila, columna, vida, danio);
	}
}

// ---------------------------------------------------------------- Niveles

int Catalogo::getCantNiveles() const {
	return (int)niveles.size();
}

const DatosNivel& Catalogo::getNivel(int numero) const {
	for (const DatosNivel& nivel : niveles) {
		if (nivel.getNumero() == numero) {
			return nivel;
		}
	}
	return niveles[0];
}

std::vector<DatosAparicion> Catalogo::getApariciones(int nivel, int oleada) const {
	std::vector<DatosAparicion> resultado;
	for (const DatosAparicion& aparicion : apariciones) {
		if (aparicion.getNivel() == nivel && aparicion.getOleada() == oleada) {
			resultado.push_back(aparicion);
		}
	}
	return resultado;
}

// ---------------------------------------------------------------- Dificultades

const std::vector<DatosDificultad>& Catalogo::getDificultades() const {
	return dificultades;
}

const DatosDificultad& Catalogo::getDificultad(int numero) const {
	for (const DatosDificultad& dificultad : dificultades) {
		if (dificultad.getNumero() == numero) {
			return dificultad;
		}
	}
	// No existe (por ejemplo, una partida guardada con un numero raro): la Normal, o la primera que haya
	for (const DatosDificultad& dificultad : dificultades) {
		if (dificultad.getNumero() == DIFICULTAD_NORMAL) {
			return dificultad;
		}
	}
	return dificultades[0];
}
