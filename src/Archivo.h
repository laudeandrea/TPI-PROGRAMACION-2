#pragma once
#include <cstdio>
#include <string>
#include <vector>

/* Un archivo binario de registros, todos del mismo tipo T (por ejemplo Archivo<DatosHabilidad>).
*  Es una PLANTILLA (template): la clase se escribe una sola vez y sirve para cualquier tipo de registro.
*  Cuando se escribe Archivo<DatosNivel>, el compilador arma una copia de esta clase con T = DatosNivel.
*
*  Por eso esta clase no tiene .cpp: el compilador necesita ver el codigo completo de la plantilla en cada
*  lugar donde se usa, asi que todo va en el .h (es la unica excepcion a la regla de ".h + .cpp").
*
*  OJO: T tiene que ser una clase "plana": solo int, bool, float y arreglos de char. Nada de std::string ni
*  std::vector, porque esos guardan un puntero a otra parte de la memoria, y lo que se escribiria en el
*  archivo es el puntero (que al volver a abrir el juego no apunta a nada) y no el texto.
*
*  Se usa fopen_s en vez de fopen: es la version "segura" que pide Visual Studio (con fopen da error si no
*  se define _CRT_SECURE_NO_WARNINGS). Hace lo mismo, pero devuelve el FILE* por parametro. */
template <typename T>
class Archivo {
	private:
		std::string nombre; // ruta del archivo, por ejemplo "Datos/Habilidades.dat"

		// Abre el archivo en ese modo ("rb", "wb", "ab"). Devuelve NULL si no pudo.
		FILE* abrir(const char* modo) const {
			FILE* archivo = NULL;
			if (fopen_s(&archivo, nombre.c_str(), modo) != 0) {
				return NULL;
			}
			return archivo;
		}

	public:
		Archivo(const std::string& _nombre) : nombre(_nombre) {
		}

		bool existe() const {
			FILE* archivo = abrir("rb");
			if (archivo == NULL) {
				return false;
			}
			fclose(archivo);
			return true;
		}

		// Cuantos registros tiene: el tamanio del archivo dividido el tamanio de un registro.
		int contarRegistros() const {
			FILE* archivo = abrir("rb");
			if (archivo == NULL) {
				return 0;
			}
			fseek(archivo, 0, SEEK_END); // va al final...
			long bytes = ftell(archivo); // ...y pregunta en que byte quedo
			fclose(archivo);
			return (int)(bytes / sizeof(T));
		}

		// Todos los registros, en el orden en que estan en el archivo. Vacio si el archivo no existe
		// o si es de otra version (ver abajo).
		std::vector<T> leerTodos() const {
			std::vector<T> registros;
			FILE* archivo = abrir("rb");
			if (archivo == NULL) {
				return registros;
			}
			// Si el tamanio no es un multiplo exacto del registro, el archivo se escribio con otra version de la
			// clase (por ejemplo, antes de agregarle un atributo): los registros quedarian corridos y se leeria basura
			fseek(archivo, 0, SEEK_END);
			long bytes = ftell(archivo);
			fseek(archivo, 0, SEEK_SET); // vuelve al principio para leer
			if (bytes % sizeof(T) != 0) {
				fclose(archivo);
				return registros;
			}
			T registro;
			while (fread(&registro, sizeof(T), 1, archivo) == 1) {
				registros.push_back(registro);
			}
			fclose(archivo);
			return registros;
		}

		// El primer registro (para los archivos que tienen uno solo, como partida.dat).
		// Devuelve false si el archivo no existe o esta vacio; en ese caso no toca "registro".
		bool leerPrimero(T& registro) const {
			FILE* archivo = abrir("rb");
			if (archivo == NULL) {
				return false;
			}
			bool leyo = fread(&registro, sizeof(T), 1, archivo) == 1;
			fclose(archivo);
			return leyo;
		}

		// Borra lo que hubiera y escribe estos registros. Devuelve false si no pudo escribir alguno.
		bool guardarTodos(const std::vector<T>& registros) const {
			FILE* archivo = abrir("wb"); // "wb" crea el archivo o lo deja vacio si ya existia
			if (archivo == NULL) {
				return false;
			}
			bool ok = true;
			for (const T& registro : registros) {
				if (fwrite(&registro, sizeof(T), 1, archivo) != 1) {
					ok = false;
				}
			}
			fclose(archivo); // siempre cerrar: si no, lo escrito puede no llegar al disco
			return ok;
		}

		// Agrega un registro al final, sin borrar los que ya estaban.
		bool agregar(const T& registro) const {
			FILE* archivo = abrir("ab"); // "ab" = append: escribe al final
			if (archivo == NULL) {
				return false;
			}
			bool ok = fwrite(&registro, sizeof(T), 1, archivo) == 1;
			fclose(archivo);
			return ok;
		}

		bool borrar() const {
			return remove(nombre.c_str()) == 0;
		}
};
