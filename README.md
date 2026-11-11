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

## Cómo se juega

Hay que defender el **códice** (el libro sobre el atril) de **3 oleadas** de enemigos. Se pierde si el mago
se queda sin vida o si destruyen el códice; se gana al terminar con la última oleada.

| Tecla | Qué hace |
|---|---|
| Click en el mago | Muestra a dónde puede caminar. Click en una casilla marcada lo mueve. |
| **Q** | Bola de energía: 10 de daño a un enemigo a 3 casillas o menos. Cuesta 2 de maná. |
| **W** | Empuje: 5 de daño a un enemigo pegado y lo corre una casilla. Si cae al abismo, muere (el espectro flota). Cuesta 3 de maná. |
| **Espacio** | Pasar el turno. |
| **Escape** | Volver al menú. |

Moverse, tirar una habilidad o pasar gasta el turno. Después juegan los enemigos y el mago recupera 1 de maná.

| Enemigo | Vida | Cómo ataca |
|---|---|---|
| Caballero | 30 | Cuerpo a cuerpo: 25 de daño. |
| Espectro | 40 | De lejos (hasta 3 casillas) con una bola de ectoplasma: 15 de daño. Atraviesa columnas y abismos. |
| Golem | 60 | Explota al llegar: 30 de daño a las 8 casillas de alrededor (también a otros enemigos) y se destruye. |

Cada enemigo va por lo que tenga más cerca: el mago o el códice.

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
herramientas/          Scripts en Python que generan el arte del tablero, de las habilidades y de los ataques (ver CONTRIBUTING)
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
| `Mago` | `Mago.h` / `Mago.cpp` | Hereda de `Personaje`. Agrega el maná (`gastarMana` no lo deja quedar negativo) y sus habilidades en un `std::vector<Habilidad>`: la bola de energía (Q) y el empuje (W). |
| `Habilidad` | `Habilidad.h` / `Habilidad.cpp` | Una habilidad del mago: tipo (`TipoHabilidad`), nombre, daño, costo de maná y alcance (en casillas). |
| `Enemigo` | `Enemigo.h` / `Enemigo.cpp` | Hereda de `Personaje`. Lo común a todos los enemigos: el daño, el alcance de su ataque (`getAlcanceAtaque()`, `virtual`) y `atacar()`, también `virtual`, que devuelve un `Ataque`. |
| `Caballero` | `Caballero.h` / `Caballero.cpp` | Hereda de `Enemigo` y usa el ataque de la base: un golpe a la casilla de al lado. |
| `Espectro` | `Espectro.h` / `Espectro.cpp` | Hereda de `Enemigo`. Redefine `puedePisar` (vuela sobre abismos y columnas), `getAlcanceAtaque` (3) y `lanzaProyectil` (su ataque viaja como una bola). |
| `Golem` | `Golem.h` / `Golem.cpp` | Hereda de `Enemigo`. Redefine `atacar`: explota, le pega a las 8 casillas de alrededor y se queda sin vida. |
| `Ataque` | `Enemigo.h` | `struct` con lo que hace un ataque: a qué casillas les pega y cuánto. El enemigo no sabe quién está ahí: el `Juego` lastima al que esté (`daniarCasilla()`). |
| `Pergamino` | `Pergamino.h` / `Pergamino.cpp` | El códice que hay que defender: su casilla, su vida (50) y su dibujo (cambia a dañado con menos de la mitad). No es un `Personaje`. |
| `Nivel` | `Nivel.h` / `Nivel.cpp` | Las oleadas de enemigos: en cuál se está (`getOleada()`), si hay otra (`hayOtraOleada()`) y `crearOleada()`, que arma los enemigos de la oleada actual. |
| `Proyectil` | `Proyectil.h` / `Proyectil.cpp` | Una bola que vuela (animada) hasta una casilla y guarda el daño que va a hacer. La usan la Q del mago y el espectro. |
| `Hud` | `Hud.h` / `Hud.cpp` | Lo que se dibuja fijo en la pantalla: la oleada, la vida del mago y del códice, el maná, la ayuda de las teclas, los números de daño que suben y el cartel de victoria o derrota. |
| `TipoTerreno` | `Casilla.h` | `enum` con los terrenos posibles: `SUELO`, `SUELO_OCUPADO` (donde está el códice: nadie lo pisa), `COLUMNAROTA`, `ABISMO`. |
| `TipoHabilidad` | `Habilidad.h` | `enum` con lo que hace cada habilidad: `BOLADEENERGIA`, `EMPUJE` (y `MURO`, `PASO_ETEREO`, que todavía no existen). |
| `EstadoJuego` | `Juego.h` | `enum` con la pantalla en la que está el juego: `MENU`, `JUGANDO`, `GANASTE`, `PERDISTE`. |
| `FaseTurno` | `Juego.h` | `enum` con de quién es el turno: `JUGADOR`, `DISPARANDO` (la bola del mago está volando), `ENEMIGOS` y `ENEMIGOS_DISPARANDO` (están volando las bolas de los espectros). Mientras vuela una bola, nadie se mueve. |

`main.cpp` solo crea un `Juego` y llama a `ejecutar()`.

### Qué pasa al ejecutar

1. **`Juego()`** (constructor): crea la ventana de 1280×720, le pone el ícono, crea el `Mapa`
   (que arma el tablero y carga las texturas), el `Mago` y el códice en sus casillas iniciales, el `Nivel`,
   la `Camara` encuadrando el tablero y el `Menu`, y pone la primera oleada (`empezarOleada()`).
   Arranca con `estado = EstadoJuego::MENU`.
2. **`ejecutar()`**: repite en cada frame (60 por segundo):
   - **`procesarEventos()`**: cierra la ventana si la cierran y reajusta la cámara si le cambian el tamaño.
     El resto depende del estado:
     - **En el menú**: le pasa cada evento a `menu.procesarEvento()`, que devuelve la opción elegida
       (o `-1`). "Nueva partida" llama a `reiniciarPartida()` (mago, códice y nivel nuevos, primera oleada)
       y cambia el estado a `JUGANDO`; "Salir" cierra la ventana.
     - **Jugando**: Escape vuelve al menú. Para moverse, click en el mago: se marcan las casillas a las que
       puede caminar este turno (`casillasAlcanzables()`: una búsqueda por pasos que rodea pozos, columnas y
       personajes, hasta `MOVIMIENTO` pasos, constante en `Mago.h`). Click en una marcada lo mueve; click en
       cualquier otro lado cancela.
       **Q** o **W** preparan una habilidad (`habilidadPreparada` guarda cuál: 0, 1 o `SIN_HABILIDAD`; la
       misma tecla otra vez la cancela) y se marcan las casillas al alcance (celeste) y los enemigos a los
       que se puede tirar (rojo). El click sobre un enemigo marcado llama a `usarHabilidad()`, que gasta el
       maná y, según el `TipoHabilidad`, crea un `Proyectil` (fase `DISPARANDO`) o empuja (`empujar()`).
       **Espacio** pasa el turno.
     - **Fin de partida** (`GANASTE` o `PERDISTE`): Enter o Escape vuelven al menú.
   - **`actualizar(dt)`**: `dt` son los segundos desde el cuadro anterior (lo mide un `sf::Clock`).
     - Mientras vuelan bolas, `moverProyectiles()` las mueve. Cada una que llega le hace el daño a quien
       esté en su casilla (`daniarCasilla()`). Cuando llegaron todas, sigue el turno que corresponde.
     - Cuando el mago termina su acción, `terminarTurnoJugador()` saca a los enemigos sin vida, le pasa el
       turno a los enemigos y llama a `revisarFinDePartida()`: si no quedan enemigos, aparece la próxima
       oleada o se gana.
     - En el turno de los enemigos (`turnoEnemigos()`), cada uno elige lo que tenga más cerca (el mago o el
       códice) y hace una sola cosa: si le llega (`puedeAtacar()`) ataca; si no, da un paso.
       - **Atacar:** `atacar()` es `virtual`, así que se ejecuta el del enemigo que sea de verdad. Devuelve
         un `Ataque` con las casillas y el daño. Si el enemigo `lanzaProyectil()` (el espectro), sale una bola
         por casilla; si no, el daño se aplica en el momento y las casillas se marcan en rojo un instante
         (`explosiones`). El golem, al explotar, se queda sin vida.
       - **Caminar:** `pasoHacia()` busca el camino más corto (otra búsqueda por pasos, que anota de qué casilla
         se llegó a cada una) hasta una casilla desde la que pueda atacar, y da el primer paso. Si no hay
         camino libre, prueba en línea recta con `Enemigo::calcularPaso()`.

       Al terminar vuelve el turno del jugador (`empezarTurnoJugador()`): el mago recupera 1 de maná
       (`MANA_POR_TURNO`) y se revisa si se perdió.
   - **`dibujar()`**: limpia la pantalla y, según el estado, dibuja el menú o el tablero, en este orden
     (lo último que se dibuja queda arriba):
     1. el fondo, con la vista por defecto;
     2. con la cámara: el mapa, los marcadores (movimiento, alcance de la habilidad y las casillas rojas de
        los golpes), los personajes y el códice ordenados por profundidad (`fila + columna`, de atrás hacia
        adelante), las barras de vida y las bolas;
     3. el HUD (`Hud`), otra vez con la vista por defecto;
     4. si la partida terminó, el cartel de victoria o derrota (`hud.dibujarFinDePartida()`).

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
  `textura.loadFromFile("Imagenes/mago_64.png");`.
- **Fuentes:** van en `Fuentes/` y se cargan igual: `fuente.loadFromFile("Fuentes/PixelifySans-Regular.ttf");`.
- Si agregás sonido (`sf::Sound`, `sf::Music`), no hay que configurar nada: `sfml-audio` y `openal32.dll`
  ya se enlazan y se copian.
