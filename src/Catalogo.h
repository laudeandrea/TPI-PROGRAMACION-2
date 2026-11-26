#pragma once
#include <memory>
#include <string>
#include <vector>
#include "Archivo.h"
#include "DatosHabilidad.h"
#include "DatosEnemigo.h"
#include "DatosNivel.h"
#include "DatosAparicion.h"
#include "DatosDificultad.h"
#include "Habilidad.h"
#include "Enemigo.h"

/* El contenido del juego, leido de los archivos de la carpeta Datos/:
*   Habilidades.dat    -> nombre, danio, costo de mana, alcance y nivel de cada hechizo
*   TiposEnemigo.dat   -> vida, danio y alcance de cada tipo de enemigo
*   Niveles.dat        -> el tablero, las posiciones, la cantidad de oleadas y la historia de cada nivel
*   NivelEnemigos.dat  -> que enemigo aparece en cada oleada de cada nivel, y en que casilla
*   Dificultades.dat   -> como cambian la vida del codice, la vida y el danio de los enemigos y el mana en
*                         Facil, Normal y Dificil
*
*  Se leen una sola vez, al abrir el juego. Si falta alguno (o esta vacio), se crea con los valores
*  por defecto que estan en Catalogo.cpp, asi el juego siempre arranca.
*  Para cambiar un valor: cambiarlo en Catalogo.cpp, borrar el .dat de Datos/ y abrir el juego. */
class Catalogo {
	private:
		std::vector<DatosHabilidad> habilidades;
		std::vector<DatosEnemigo> enemigos;
		std::vector<DatosNivel> niveles;
		std::vector<DatosAparicion> apariciones;
		std::vector<DatosDificultad> dificultades;

		// Los valores por defecto de cada archivo (lo que se escribe si el archivo no existe).
		std::vector<DatosHabilidad> habilidadesPorDefecto() const;
		std::vector<DatosEnemigo> enemigosPorDefecto() const;
		std::vector<DatosNivel> nivelesPorDefecto() const;
		std::vector<DatosAparicion> aparicionesPorDefecto() const;
		std::vector<DatosDificultad> dificultadesPorDefecto() const;

		// Lee un archivo; si no existe o esta vacio, lo crea con esos registros y devuelve esos mismos.
		// Es una plantilla (como Archivo<T>): sirve para los cinco tipos de registro.
		template <typename T>
		std::vector<T> leerOCrear(const std::string& ruta, const std::vector<T>& porDefecto) const;

	public:
		static const int DIFICULTAD_NORMAL = 2; // la que se usa si no se eligio otra

		Catalogo();

		// --- Habilidades ---
		const DatosHabilidad& getDatosHabilidad(TipoHabilidad tipo) const;
		// La habilidad lista para que la aprenda el mago.
		Habilidad crearHabilidad(TipoHabilidad tipo) const;
		// Las que el mago ya sabe usar en ese nivel (las de su nivel y las de antes).
		std::vector<Habilidad> getHabilidadesHastaNivel(int nivel) const;
		// Las que se aprenden justo en ese nivel (para avisarle al jugador).
		std::vector<DatosHabilidad> getHabilidadesNuevas(int nivel) const;

		// --- Enemigos ---
		const DatosEnemigo& getDatosEnemigo(TipoEnemigo tipo) const;
		// Crea un enemigo de ese tipo, con la vida y el danio de TiposEnemigo.dat ajustados a la dificultad.
		std::unique_ptr<Enemigo> crearEnemigo(TipoEnemigo tipo, int fila, int columna, const DatosDificultad& dificultad) const;

		// --- Niveles ---
		int getCantNiveles() const;
		// numero empieza en 1. Si no existe, devuelve el primero.
		const DatosNivel& getNivel(int numero) const;
		// Los enemigos de esa oleada de ese nivel (las dos empiezan en 1).
		std::vector<DatosAparicion> getApariciones(int nivel, int oleada) const;

		// --- Dificultades ---
		// Todas, en el orden del archivo (es el orden en que se muestran para elegir).
		const std::vector<DatosDificultad>& getDificultades() const;
		// numero: 1 = Facil, 2 = Normal, 3 = Dificil. Si no existe, devuelve la Normal.
		const DatosDificultad& getDificultad(int numero) const;
};
