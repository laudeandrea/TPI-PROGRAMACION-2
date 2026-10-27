# El Códice del Mago

Juego por turnos en C++20 con [SFML 2.6.2](https://www.sfml-dev.org/), para Programación II.

SFML ya viene incluido en `libs/SFML-2.6.2`, así que no hace falta descargarlo ni configurarlo.

## Requisitos

- **Visual Studio 2026** (Community alcanza) con la carga de trabajo **"Desarrollo para el escritorio con C++"**.
  El proyecto usa el conjunto de herramientas **v145**. Si tenés Visual Studio 2022, al abrirlo te va a pedir
  "Redestinar proyecto" a v143: aceptalo para compilar en tu PC, pero **no subas ese cambio** de `juego_pr2.vcxproj`.
- Git.

## Primeros pasos

1. Cloná el repositorio en una carpeta que **no** esté sincronizada con OneDrive (por ejemplo `C:\dev`).
   OneDrive y git se pisan entre sí y pueden romper el repositorio.
2. Abrí `juego_pr2.slnx` con Visual Studio.
3. Arriba elegí **Debug** y **x64**.
4. **F5** compila y ejecuta.

## Dónde queda el juego

```
build\bin\x64\Debug\juego_pr2.exe     <- para programar y depurar
build\bin\x64\Release\juego_pr2.exe   <- versión para entregar
```

Al compilar se copian solas al lado del `.exe` las DLL de SFML y las carpetas `Imagenes` y `Fuentes`, así que el juego
también se abre con doble clic. La versión **Debug** solo funciona en PCs que tengan Visual Studio instalado;
para pasarle el juego a alguien, usá la carpeta de **Release** completa.

La carpeta `build\` se puede borrar cuando quieras: se vuelve a generar al compilar y no se sube al repositorio.

## Estructura

```
README.md              Este archivo: cómo compilar y cómo funciona el código
CONTRIBUTING.md        Pautas de código y de arte
SFML_usado.txt         Qué hace cada función de SFML que usa el juego, y dónde se usa
juego_pr2.slnx         Solución (abrir esta)
juego_pr2.vcxproj      Proyecto: rutas, SFML y copia automática de DLL/imágenes
juego_pr2.rc           Ícono del .exe (usa Imagenes/icono.ico)
src/                   Código del juego (.cpp y .h)
Imagenes/              Texturas e íconos
Fuentes/               Fuente del juego (Pixelify Sans) y su licencia (OFL.txt)
libs/SFML-2.6.2/       SFML: headers, .lib y DLL (Debug con -d, Release sin -d)
build/                 Generado al compilar (ignorado por git)
```

## Cómo funciona el código

### Las clases

| Clase | Archivos | Qué hace |
|---|---|---|
| `Juego` | `Juego.h` / `Juego.cpp` | Tiene la ventana, el menú, el mapa y la cámara. Sabe en qué pantalla está (`estado`) y corre el game loop. |
| `Mapa` | `Mapa.h` / `Mapa.cpp` | El tablero de 8×8: qué hay en cada casilla, dónde se dibuja cada una y cómo. |
| `Casilla` | `Casilla.h` / `Casilla.cpp` | Una casilla del tablero. Por ahora solo guarda su tipo de terreno. |
| `Menu` | `Menu.h` / `Menu.cpp` | Pantalla de inicio: dibuja fondo, título y opciones, y se elige con flechas + Enter o con el mouse. El arte y las medidas están en [CONTRIBUTING.md](CONTRIBUTING.md#menú-principal). |
| `Camara` | `Camara.h` / `Camara.cpp` | Encuadra el tablero y lo adapta al tamaño de la ventana sin deformarlo. |
| `Personaje` | `Personaje.h` / `Personaje.cpp` | Clase base de todo lo que se para en el tablero: en qué casilla está, su vida, su imagen y cómo dibujarse. |
| `Mago` | `Mago.h` / `Mago.cpp` | Hereda de `Personaje`. Agrega el maná y carga su propia imagen. |
| `TipoTerreno` | `TipoTerreno.h` | `enum` con los terrenos posibles: `SUELO`, `COLUMNAROTA`, `ABISMO`. |
| `EstadoJuego` | `EstadoJuego.h` | `enum` con la pantalla en la que está el juego: `MENU`, `JUGANDO`. |

`main.cpp` solo crea un `Juego` y llama a `ejecutar()`.

### Qué pasa al ejecutar

1. **`Juego()`** (constructor): crea la ventana de 800×600, le pone el ícono, crea el `Mapa`
   (que arma el tablero y carga las texturas), el `Mago` en su casilla inicial, la `Camara` encuadrando
   el tablero y el `Menu`. Arranca con `estado = EstadoJuego::MENU`.
2. **`ejecutar()`**: repite en cada frame (60 por segundo):
   - **`procesarEventos()`**: cierra la ventana si la cierran y reajusta la cámara si le cambian el tamaño.
     El resto depende del estado:
     - **En el menú**: le pasa cada evento a `menu.procesarEvento()`, que devuelve la opción elegida
       (o `-1`). "Nueva partida" cambia el estado a `JUGANDO` y "Salir" cierra la ventana.
     - **Jugando**: el click izquierdo mueve al mago a la casilla clickeada, y Escape vuelve al menú.
   - **`dibujar()`**: limpia la pantalla y, según el estado, dibuja el menú o el tablero (cámara, mapa
     y después el mago: lo último que se dibuja queda arriba).

### Las tres capas: grilla, mundo y pantalla

Es la idea más importante del proyecto, para que el día de mañana se pueda pasar a vista isométrica
tocando lo mínimo.

| Capa | Qué responde | Unidad | Quién se encarga |
|---|---|---|---|
| **Grilla** | ¿En qué casilla está cada cosa? | fila y columna (0 a 7) | La lógica del juego (`Casilla`, `Mapa`) |
| **Mundo** | ¿Dónde se dibuja esa casilla? | píxeles del mundo | `Mapa::casillaAmundo()` |
| **Pantalla** | ¿Qué parte del mundo se ve y a qué tamaño? | píxeles de la ventana | `Camara` |

- **La lógica trabaja siempre en filas y columnas, nunca en píxeles.** Un personaje "está en la casilla
  (3, 5)", no "en x = 352".
- **`casillaAmundo(fila, columna)`** es el único lugar que convierte una casilla en posición. Devuelve el
  **centro** de la casilla (el centro del rombo). Por eso el origen de cada sprite (`setOrigin`) tiene que
  estar en el punto de la imagen que corresponde al centro del rombo.
- **`Mapa` no sabe nada de la ventana:** dibuja el tablero empezando en (0, 0) del mundo. La `Camara` se
  encarga de centrarlo y escalarlo para que llene la ventana.

### La cámara

- Encuadra el tablero más un margen de 64 px de cada lado (`MARGEN_CAMARA` en `Juego.h`), que queda
  libre para el HUD.
- Si la ventana tiene otra proporción que el tablero, la cámara muestra más mundo del lado que sobra.
  Así el tablero nunca se deforma: aparecen bandas a los costados (o arriba y abajo).
- Cuando se agregue el HUD, se va a dibujar con la vista por defecto de la ventana
  (`window.getDefaultView()`), no con la cámara del tablero, para que quede fijo en la pantalla.

### Estado actual y lo que viene

- **Hoy:** vista isométrica estilo *Into the Breach*. Casillas en rombo de 64×32 (`ANCHO_CASILLAS` y
  `ALTO_CASILLAS` en `Mapa.h`), dibujadas como cubos con grosor (texturas `*_iso.png`).
- **Cómo funciona la isométrica:** en `casillaAmundo()`, avanzar una columna mueve media casilla a la
  derecha y media hacia abajo; avanzar una fila, media a la izquierda y media hacia abajo. Por eso la X
  depende de `columna - fila` y la Y de `columna + fila`. A la X se le suma medio tablero para que la
  esquina izquierda no quede en negativo (la cámara asume que el mundo empieza en 0).
- **Por qué solo se ven los costados del borde de adelante:** el mapa se dibuja de atrás (fila 0) hacia
  adelante (fila 7), así que cada cubo tapa los costados de los que tiene atrás.
- **Origen de los sprites:** cada sprite apoya en `casillaAmundo()` un punto distinto de su imagen.
  - Baldosas: el **centro de la cara de arriba** del cubo, que está a `CENTRO_ROMBO_DESDE_ABAJO` (34 px)
    del fondo de la imagen. Ver la convención de arte en [CONTRIBUTING.md](CONTRIBUTING.md).
  - Personajes: los **pies** (la base de la imagen), porque están parados sobre la casilla.

## Al trabajar

Las reglas de código y de arte están en [CONTRIBUTING.md](CONTRIBUTING.md). Leelas antes de programar.

- **Archivos nuevos:** en *Agregar → Nuevo elemento*, cambiá la ubicación a la carpeta `src\`.
  Visual Studio por defecto los crea en la raíz del proyecto.
- **Imágenes nuevas:** van en `Imagenes/` y se cargan con ruta relativa, por ejemplo
  `textura.loadFromFile("Imagenes/suelo.png");`.
- **Fuentes:** van en `Fuentes/` y se cargan igual: `fuente.loadFromFile("Fuentes/PixelifySans-Regular.ttf");`.
- Si agregás sonido (`sf::Sound`, `sf::Music`), no hay que configurar nada: `sfml-audio` y `openal32.dll`
  ya se enlazan y se copian.
