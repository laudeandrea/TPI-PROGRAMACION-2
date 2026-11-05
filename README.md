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
herramientas/          Scripts en Python que generan el arte del tablero y de las habilidades (ver CONTRIBUTING)
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
| `Mago` | `Mago.h` / `Mago.cpp` | Hereda de `Personaje`. Agrega el maná (`gastarMana` no lo deja quedar negativo) y sus habilidades en un `std::vector<Habilidad>`: nace con la bola de energía. |
| `Habilidad` | `Habilidad.h` / `Habilidad.cpp` | Una habilidad del mago: nombre, daño, costo de maná y alcance (en casillas). |
| `Proyectil` | `Proyectil.h` / `Proyectil.cpp` | La bola que tira una habilidad: vuela del mago al objetivo (animada) y guarda el daño que va a hacer. |
| `Hud` | `Hud.h` / `Hud.cpp` | Lo que se dibuja fijo en la pantalla: los puntos de maná, la ayuda de la Q y los números de daño que suben. |
| `TipoTerreno` | `Casilla.h` | `enum` con los terrenos posibles: `SUELO`, `SUELO_OCUPADO` (donde está el códice: nadie lo pisa), `COLUMNAROTA`, `ABISMO`. |
| `EstadoJuego` | `Juego.h` | `enum` con la pantalla en la que está el juego: `MENU`, `JUGANDO`. |
| `FaseTurno` | `Juego.h` | `enum` con de quién es el turno: `JUGADOR`, `DISPARANDO` (la bola está volando y nadie se mueve) y `ENEMIGOS`. |

`main.cpp` solo crea un `Juego` y llama a `ejecutar()`.

### Qué pasa al ejecutar

1. **`Juego()`** (constructor): crea la ventana de 1280×720, le pone el ícono, crea el `Mapa`
   (que arma el tablero y carga las texturas), el `Mago` en su casilla inicial, la `Camara` encuadrando
   el tablero y el `Menu`. Arranca con `estado = EstadoJuego::MENU`.
2. **`ejecutar()`**: repite en cada frame (60 por segundo):
   - **`procesarEventos()`**: cierra la ventana si la cierran y reajusta la cámara si le cambian el tamaño.
     El resto depende del estado:
     - **En el menú**: le pasa cada evento a `menu.procesarEvento()`, que devuelve la opción elegida
       (o `-1`). "Nueva partida" cambia el estado a `JUGANDO` y "Salir" cierra la ventana.
     - **Jugando**: Escape vuelve al menú. Para moverse, click en el mago: se marcan las casillas a las que
       puede caminar este turno (`casillasAlcanzables()`: una búsqueda por pasos que rodea pozos, columnas y
       personajes, hasta `MOVIMIENTO` pasos, constante en `Mago.h`). Click en una marcada lo mueve; click en
       cualquier otro lado cancela.
       **Q** prepara la bola de energía (otra Q la cancela) y marca las casillas al alcance (celeste) y
       los enemigos a los que se puede tirar (rojo). Con la habilidad preparada, el click sobre un
       enemigo al alcance (distancia en filas + columnas) gasta maná y crea un `Proyectil`: la fase
       pasa a `DISPARANDO`.
   - **`actualizar(dt)`**: `dt` son los segundos desde el cuadro anterior (lo mide un `sf::Clock`).
     - Mientras la bola vuela, la mueve. Cuando llega, `impactarProyectil()` le hace el daño al enemigo,
       muestra el número de daño, saca a los enemigos sin vida y le pasa el turno a los enemigos.
     - En el turno de los enemigos, cada uno da un paso hacia el mago. Al terminar, vuelve el turno del
       jugador y el mago recupera 1 de maná (`MANA_POR_TURNO`).
   - **`dibujar()`**: limpia la pantalla y, según el estado, dibuja el menú o el tablero, en este orden
     (lo último que se dibuja queda arriba):
     1. el fondo, con la vista por defecto;
     2. con la cámara: el mapa, los marcadores de alcance (si la Q está apretada), los personajes y el
        códice ordenados por profundidad (`fila + columna`, de atrás hacia adelante), las barras de vida
        de los enemigos y la bola;
     3. el HUD (`Hud`), otra vez con la vista por defecto.

     El códice no es un `Personaje`: se dibuja justo antes del primer personaje que esté más adelante que él.

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
- El HUD (`Hud`) se dibuja con la vista por defecto de la ventana (`window.getDefaultView()`), no con
  la cámara del tablero, para que quede fijo en la pantalla. Los números de daño están en el mundo
  (arriba del enemigo), así que se pasan a la pantalla con `mapCoordsToPixel` + `mapPixelToCoords`.

### Estado actual y lo que viene

- **Hoy:** vista isométrica estilo *Into the Breach*. Casillas en rombo de 64×40 (`ANCHO_CASILLAS` y
  `ALTO_CASILLAS` en `Mapa.h`), dibujadas como cubos con grosor (texturas `*_iso.png`). Es un poco más
  alto que el isométrico clásico (64×32), para que el tablero se vea más desde arriba.
- **Cómo funciona la isométrica:** en `casillaAmundo()`, avanzar una columna mueve media casilla a la
  derecha y media hacia abajo; avanzar una fila, media a la izquierda y media hacia abajo. Por eso la X
  depende de `columna - fila` y la Y de `columna + fila`. A la X se le suma medio tablero para que la
  esquina izquierda no quede en negativo (la cámara asume que el mundo empieza en 0).
- **Paredes:** antes de cada baldosa, `mostrar()` mira la casilla de adelante de cada lado (`fila + 1`
  para el izquierdo, `columna + 1` para el derecho). Si queda fuera del tablero dibuja roca colgando
  (`pared_*_iso.png`); si es abismo, la pared interna del pozo (`pozo_*_iso.png`). Lo dibuja
  `dibujarPared()`, con el origen en (32, 50) de la imagen (`CENTRO_ROMBO_DESDE_ARRIBA`).
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
