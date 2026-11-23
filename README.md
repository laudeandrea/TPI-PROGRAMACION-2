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

La partida se guarda en `partida.dat`, en la carpeta desde donde se ejecuta el juego. Si lo corrés desde
Visual Studio, esa carpeta es la raíz del proyecto; si abrís el `.exe` con doble clic, queda al lado del
ejecutable. Es un archivo **binario** (por ahora de 4 bytes: un `int` con el número del próximo nivel), así que
abrirlo con el Notepad muestra caracteres raros: eso es normal. No se sube al repositorio.

## Cómo se juega

Hay que defender el **códice** (el libro sobre el atril) en **2 niveles**, de **2 oleadas** cada uno. Se pierde
si el mago se queda sin vida o si destruyen el códice; se gana al terminar con la última oleada del nivel 2.

| Nivel | Tablero | Oleadas |
|---|---|---|
| 1 | El códice a la izquierda, los enemigos salen de la derecha. | 1) caballero y espectro. 2) caballero, espectro y golem. |
| 2 | Dos pozos parten el tablero. El códice está arriba y los enemigos salen de abajo: para llegar pasan por el medio o por los bordes ([vista previa](Imagenes/vista_previa_nivel2.png)). Se aprende el **muro de fuego**. | 1) dos caballeros y un espectro. 2) dos golems, un caballero y un espectro. |

Al ganar el nivel 1 aparece el cartel "Nivel superado": **Enter** pasa al nivel 2, **S** guarda la partida y
vuelve al menú, y **Escape** vuelve al menú sin guardar. El mago empieza cada nivel con la vida y el maná
llenos.

El menú principal tiene **Continuar** y **Nueva partida**. "Continuar" retoma el nivel de la partida guardada;
"Nueva partida" arranca de cero y borra el guardado. Mientras no haya ninguna partida guardada,
"Continuar" se ve en gris y no se puede elegir.

| Tecla | Qué hace |
|---|---|
| Click en el mago | Muestra a dónde puede caminar. Click en una casilla marcada lo mueve. |
| **1** | Bola de energía: 10 de daño a un enemigo a 3 casillas o menos. Cuesta 3 de maná. |
| **2** | Empuje: 5 de daño a un enemigo pegado y lo corre una casilla; ese turno el enemigo no juega. Si cae al abismo, muere (el espectro flota). Si atrás hay una columna, el borde u otro personaje, no se puede empujar y no gasta maná. Cuesta 3 de maná. |
| **3** | Muro de fuego (solo desde el nivel 2): prende fuego en 3 casillas vacías, a 3 casillas del mago o menos, de costado a la línea entre el mago y la casilla elegida (al pasar el mouse se ve en rojo dónde va a quedar). Dura 3 turnos de los enemigos. Nadie lo puede pisar, salvo el espectro, que lo cruza pero pierde 10 de vida cada vez que entra o que empieza su turno ahí. Cuesta 4 de maná. |
| **Espacio** | Terminar el turno. |
| **Escape** | Volver al menú. |

En cada turno el mago puede moverse una vez y después tirar una habilidad. Tirar una habilidad o apretar Espacio termina el turno (moverse no). Después juegan los enemigos y el mago recupera 2 de maná.

| Enemigo | Vida | Cómo ataca |
|---|---|---|
| Caballero | 20 | Cuerpo a cuerpo: 12 de daño. |
| Espectro | 20 | De lejos (hasta 3 casillas) con una bola de ectoplasma: 8 de daño. Atraviesa columnas, abismos y el muro de fuego (quemándose). |
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
Sonidos/               Efectos y música (WAV/OGG) y sus créditos
libs/SFML-2.6.2/       SFML: headers, .lib y DLL (Debug con -d, Release sin -d)
build/                 Generado al compilar (ignorado por git)
```

## Cómo funciona el código

### Las clases

| Clase | Archivos | Qué hace |
|---|---|---|
| `Juego` | `Juego.h` / `Juego.cpp` | Tiene la ventana, el menú, el mapa y la cámara. Sabe en qué pantalla está (`estado`) y corre el game loop. |
| `Mapa` | `Mapa.h` / `Mapa.cpp` | El tablero de 8×8: qué hay en cada casilla, dónde se dibuja cada una y cómo. El constructor carga las imágenes; `cargar()` arma el tablero con el dibujo que le pasa el `Nivel`. |
| `Casilla` | `Casilla.h` / `Casilla.cpp` | Una casilla del tablero. Por ahora solo guarda su tipo de terreno. |
| `Menu` | `Menu.h` / `Menu.cpp` | Pantalla de inicio: dibuja fondo, título y opciones, y se elige con flechas + Enter o con el mouse. El arte y las medidas están en [CONTRIBUTING.md](CONTRIBUTING.md#menú-principal). |
| `Camara` | `Camara.h` / `Camara.cpp` | Encuadra el tablero en el 75 % izquierdo de la ventana (el resto es el panel del HUD) y lo adapta al tamaño sin deformarlo. |
| `Personaje` | `Personaje.h` / `Personaje.cpp` | Clase base de todo lo que se para en el tablero: en qué casilla está, su vida, su imagen y cómo dibujarse. |
| `Mago` | `Mago.h` / `Mago.cpp` | Hereda de `Personaje`. Agrega el maná (`gastarMana` no lo deja quedar negativo) y sus habilidades en un `std::vector<Habilidad>`: la bola de energía (tecla 1), el empuje (tecla 2) y, desde el nivel 2, el muro de fuego (tecla 3, se agrega con `aprenderHabilidad()`). `tieneHabilidad()` dice si ya la aprendió. |
| `Habilidad` | `Habilidad.h` / `Habilidad.cpp` | Una habilidad del mago: tipo (`TipoHabilidad`), nombre, daño, costo de maná y alcance (en casillas). |
| `Enemigo` | `Enemigo.h` / `Enemigo.cpp` | Hereda de `Personaje`. Lo común a todos los enemigos: el daño, el alcance de su ataque (`getAlcanceAtaque()`, `virtual`) y `atacar()`, también `virtual`, que devuelve un `Ataque`. |
| `Caballero` | `Caballero.h` / `Caballero.cpp` | Hereda de `Enemigo` y usa el ataque de la base: un golpe a la casilla de al lado. |
| `Espectro` | `Espectro.h` / `Espectro.cpp` | Hereda de `Enemigo`. Redefine `puedePisar` (vuela sobre abismos, columnas y el muro de fuego), `getAlcanceAtaque` (3) y `lanzaProyectil` (su ataque viaja como una bola). |
| `Golem` | `Golem.h` / `Golem.cpp` | Hereda de `Enemigo`. Redefine `atacar`: explota, le pega a las 8 casillas de alrededor y se queda sin vida. |
| `Ataque` | `Enemigo.h` | `struct` con lo que hace un ataque: a qué casillas les pega y cuánto. El enemigo no sabe quién está ahí: el `Juego` lastima al que esté (`daniarCasilla()`). |
| `Pergamino` | `Pergamino.h` / `Pergamino.cpp` | El códice que hay que defender: su casilla, su vida (80) y su dibujo (cambia a dañado con menos de la mitad). No es un `Personaje`. |
| `Nivel` | `Nivel.h` / `Nivel.cpp` | Todo lo que cambia de un nivel a otro: el dibujo del tablero (`getDisenio()`), dónde están el códice y el mago, y las oleadas: en cuál se está (`getOleada()`), si hay otra (`hayOtraOleada()`) y `crearOleada()`, que arma los enemigos de la oleada actual. `hayOtroNivel()` dice si después viene otro. |
| `Partida` | `Partida.h` / `Partida.cpp` | El registro que se guarda en disco. Por ahora tiene un solo dato: el número del próximo nivel. Usa solo tipos de tamaño fijo (nada de `std::string`), porque se escribe tal cual está en memoria. |
| `PartidaArchivo` | `PartidaArchivo.h` / `PartidaArchivo.cpp` | Maneja `partida.dat` con las funciones de archivos de C: `fopen`, `fread`, `fwrite`, `fseek`, `ftell` y `fclose`. `existe()` dice si hay partida, `guardar()` pisa el archivo con un registro, `leer()` lo devuelve y `borrar()` lo elimina. |
| `Proyectil` | `Proyectil.h` / `Proyectil.cpp` | Una bola que vuela (animada) hasta una casilla y guarda el daño que va a hacer. La usan la bola de energía del mago y el espectro. |
| `Hud` | `Hud.h` / `Hud.cpp` | Lo que se dibuja fijo en la pantalla: el panel de la derecha (nivel, oleada, vida del mago y del códice, maná, habilidades y qué hacer ahora), los números de daño que suben y los carteles (nivel superado, victoria, derrota). |
| `Muro` | `Juego.h` | `struct` con una casilla del muro de fuego y cuántos turnos le quedan. Mientras dura, esa casilla del mapa es `TipoTerreno::MURO`. |
| `TipoTerreno` | `Casilla.h` | `enum` con los terrenos posibles: `SUELO`, `SUELO_OCUPADO` (donde está el códice: nadie lo pisa), `COLUMNAROTA`, `ABISMO` y `MURO` (fuego: solo lo cruza el espectro). |
| `HabilidadSeleccionada` | `Habilidad.h` | `enum` con la habilidad que tiene preparada el jugador: `NINGUNA`, `BOLA_DE_ENERGIA` (= 1), `EMPUJE` (= 2) y `MURO` (= 3). El número coincide con la tecla y con el orden del vector del mago. |
| `TipoHabilidad` | `Habilidad.h` | `enum` con lo que hace cada habilidad: `BOLADEENERGIA`, `EMPUJE`, `MURO`. |
| `EstadoJuego` | `Juego.h` | `enum` con la pantalla en la que está el juego: `MENU`, `JUGANDO`, `NIVEL_SUPERADO`, `GANASTE`, `PERDISTE`. |
| `FaseTurno` | `Juego.h` | `enum` con de quién es el turno: `JUGADOR`, `DISPARANDO` (la bola del mago está volando), `ENEMIGOS` y `ENEMIGOS_DISPARANDO` (están volando las bolas de los espectros). Mientras vuela una bola, nadie se mueve. |

`main.cpp` solo crea un `Juego` y llama a `ejecutar()`.

### El guardado

La partida se guarda **entre nivel y nivel**, nunca en medio de un nivel. Al terminar un nivel, el cartel
"Nivel superado" ofrece **S** para guardar: se guarda el número del nivel que sigue y se vuelve al menú,
donde "Continuar" arranca ahí. Si se cierra el juego a mitad de un nivel, "Continuar" vuelve a empezar ese
nivel (o el que se haya guardado la última vez).

- `Partida` es el registro: un `int nivel` y nada más. Tiene que ser de tamaño fijo, sin `std::string`,
  porque `fwrite`/`fread` copian los bytes del objeto tal como están en memoria (`sizeof(Partida)`).
- `PartidaArchivo` encapsula el archivo `partida.dat`: `guardar()` lo abre con `fopen(..., "wb")` (crea o
  pisa), `leer()` con `fopen(..., "rb")`, `existe()` lo comprueba abriéndolo, y `borrar()` lo elimina con
  `remove()`. Siempre se cierra con `fclose()`.
- `Juego` es el único que decide cuándo guardar o borrar: guarda al apretar **S** en el cartel, borra al
  elegir "Nueva partida" y al ganar el último nivel.

### Sonidos

Los archivos van en `Sonidos/` y se cargan una sola vez en la clase `Sonidos`: `sf::SoundBuffer` para los
efectos cortos y `sf::Music` para la música de fondo, que se lee del disco y se reproduce en loop. SFML
carga WAV, OGG y FLAC; **no** carga MP3.

| Archivo | Cuándo suena |
|---|---|
| `musica_medieval.wav` | De fondo, en loop, desde que abre el juego (volumen 30). |
| `inicio.ogg` | Al elegir "Nueva partida" o "Continuar". |
| `bola_energia.ogg` | Al lanzar la bola de energía. |
| `impacto.ogg` | Cuando llega cualquier proyectil (la bola del mago o la del espectro). |
| `ataque_enemigo.wav` | Cuando un enemigo ataca cuerpo a cuerpo (caballero y golem). |
| `ataque_espectro.ogg` | Cuando el espectro lanza su bola de ectoplasma. |
| `victoria.wav` | Al terminar la última oleada del nivel 2. |
| `derrota.ogg` | Cuando muere el mago o destruyen el códice. |

Victoria y derrota paran la música para que se escuche el jingle; al volver al menú, la música arranca de
nuevo. El origen y las licencias de cada archivo están en `Sonidos/CREDITOS.txt`.

### Qué pasa al ejecutar

1. **`Juego()`** (constructor): crea la ventana de 1280×720, le pone el ícono, crea el `Mapa` (que carga
   las texturas), la `Camara` encuadrando el tablero y el `Menu`, y arma el nivel 1 con `empezarNivel(1)`.
   Arranca con `estado = EstadoJuego::MENU`.
   - **`empezarNivel(numero)`** arma un nivel de cero: crea el `Nivel`, le pide el dibujo del tablero y se lo
     pasa a `mapa.cargar()`, crea el códice y el mago donde dice el nivel (con vida y maná llenos), le enseña
     el muro de fuego al mago si es el nivel 2 o más (`NIVEL_MURO`), borra enemigos, bolas y muros que
     hubieran quedado y pone la primera oleada (`empezarOleada()`).
2. **`ejecutar()`**: repite en cada frame (60 por segundo):
   - **`procesarEventos()`**: cierra la ventana si la cierran y reajusta la cámara si le cambian el tamaño.
     El resto depende del estado:
     - **En el menú**: le pasa cada evento a `menu.procesarEvento()`, que devuelve la opción elegida
       (o `-1`). Antes, `menu.setHayPartida(partidaArchivo.existe())` habilita o deshabilita "Continuar"
       según haya o no una partida guardada. "Continuar" lee el nivel de `partida.dat` y llama a
       `empezarNivel()` con ese número; "Nueva partida" llama a `reiniciarPartida()` (que borra el archivo
       y hace `empezarNivel(1)`); "Salir" cierra la ventana.
     - **Jugando**: Escape vuelve al menú. Los clicks solo cuentan si caen sobre el tablero (no sobre el panel
       del HUD). Para moverse, click en el mago: se marcan las casillas a las que
       puede caminar este turno (`casillasAlcanzables()`: una búsqueda por pasos que rodea pozos, columnas y
       personajes, hasta `MOVIMIENTO` pasos, constante en `Mago.h`). Click en una marcada lo mueve; click en
       cualquier otro lado cancela. Moverse no termina el turno: `magoSeMovio` queda en `true` (no puede volver
       a caminar hasta el turno siguiente) y todavía puede tirar una habilidad o apretar Espacio.
       **1**, **2** o **3** preparan una habilidad (`habilidadPreparada` guarda cuál, como `HabilidadSeleccionada`;
       la misma tecla otra vez la cancela; si el mago todavía no la aprendió, `tieneHabilidad()` da `false` y no
       pasa nada) y se marcan las casillas al alcance (celeste) y los enemigos a los
       que se puede tirar (rojo). El click sobre un enemigo marcado llama a `usarHabilidad()`, que gasta el
       maná y, según el `TipoHabilidad`, crea un `Proyectil` (fase `DISPARANDO`) o empuja (`empujar()`).
       El empuje primero pregunta `sePuedeEmpujar()`: si no hay lugar atrás, no pasa nada y no se gasta
       maná. El enemigo empujado queda en `enemigoEmpujado` y pierde su próximo turno.
       **Muro de fuego:** se marcan las casillas vacías al alcance y, con el mouse arriba de una
       (`casillaBajoElMouse()`), en rojo las 3 donde quedaría el fuego (`casillasDelMuro()`: el centro y sus
       vecinas de costado a la línea mago → centro; las que no están libres se saltean). El click llama a
       `usarHabilidad()`, que para el muro no busca un enemigo: pregunta `sePuedePonerMuro()`, gasta el maná
       y `ponerMuro()` cambia esas casillas a `TipoTerreno::MURO` y las guarda en `muros`.
       **Espacio** termina el turno.
     - **Nivel superado** (`NIVEL_SUPERADO`): **Enter** llama a `empezarNivel(numero + 1)` y vuelve a
       `JUGANDO`; **S** guarda el nivel siguiente (`partidaArchivo.guardar(Partida(numero + 1))`) y vuelve
       al menú; **Escape** vuelve al menú sin guardar.
     - **Fin de partida** (`GANASTE` o `PERDISTE`): Enter o Escape vuelven al menú.
   - **`actualizar(dt)`**: `dt` son los segundos desde el cuadro anterior (lo mide un `sf::Clock`).
     - Mientras vuelan bolas, `moverProyectiles()` las mueve. Cada una que llega le hace el daño a quien
       esté en su casilla (`daniarCasilla()`). Cuando llegaron todas, sigue el turno que corresponde.
     - Cuando el mago termina su acción, `terminarTurnoJugador()` saca a los enemigos sin vida, le pasa el
       turno a los enemigos y llama a `revisarFinDePartida()`: si no quedan enemigos, aparece la próxima
       oleada; si era la última, se pasa a `NIVEL_SUPERADO` (si `nivel.hayOtroNivel()`) o a `GANASTE`.
       Acá no se guarda solo: la decisión es del jugador en el cartel; al ganar el último nivel se borra
       el guardado (`partidaArchivo.borrar()`).
       La oleada nueva no juega en ese mismo momento: `revisarFinDePartida()` devuelve el
       turno al jugador (respiro) para que la vea aparecer y pueda reaccionar.
     - En el turno de los enemigos (`turnoEnemigos()`), cada uno elige lo que tenga más cerca (el mago o el
       códice) y hace una sola cosa: si le llega (`puedeAtacar()`) ataca; si no, da un paso.
       - **Atacar:** `atacar()` es `virtual`, así que se ejecuta el del enemigo que sea de verdad. Devuelve
         un `Ataque` con las casillas y el daño. Si el enemigo `lanzaProyectil()` (el espectro), sale una bola
         por casilla; si no, el daño se aplica en el momento y las casillas se marcan en rojo un instante
         (`explosiones`). El golem, al explotar, se queda sin vida.
       - **Caminar:** `pasoHacia()` busca el camino más corto (otra búsqueda por pasos, que anota de qué casilla
         se llegó a cada una) hasta una casilla desde la que pueda atacar, y da el primer paso. Si no hay
         camino libre, prueba en línea recta con `Enemigo::calcularPaso()`. Como la búsqueda usa
         `puedePisar()`, los que no pueden pisar el fuego lo rodean solos.
       - **Fuego:** `quemarSiEstaEnFuego()` le hace `DANIO_FUEGO` (10) al enemigo que esté parado en un muro.
         Se llama al empezar el turno de cada enemigo, después de que da un paso y en `empujar()`. Solo el
         espectro puede estar ahí.

       Al terminar vuelve el turno del jugador (`empezarTurnoJugador()`): el mago recupera 2 de maná
       (`MANA_POR_TURNO`), a cada casilla de fuego le queda un turno menos (`apagarMuros()`: las que llegan a 0
       vuelven a `SUELO`; duran `TURNOS_MURO` = 3) y se revisa si se perdió.
   - El fuego se anima solo: `mapa.actualizar(dt)` suma el tiempo y `Mapa::mostrar()` elige uno de los
     3 cuadros de `muro_fuego_animacion.png` con `setTextureRect`, igual que la bola.
   - **`dibujar()`**: limpia la pantalla y, según el estado, dibuja el menú o el tablero, en este orden
     (lo último que se dibuja queda arriba):
     1. el fondo, con la vista por defecto;
     2. con la cámara: el mapa, los marcadores (movimiento, alcance de la habilidad y las casillas rojas de
        los golpes), los personajes y el códice ordenados por profundidad (`fila + columna`, de atrás hacia
        adelante), las barras de vida y las bolas;
     3. el HUD (`Hud`), otra vez con la vista por defecto: los números de daño y el panel de la derecha;
     4. los carteles: nivel superado (`hud.dibujarNivelSuperado()`) o victoria / derrota
        (`hud.dibujarFinDePartida()`). Los dos usan el mismo `dibujarCartel()`.

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

- Encuadra el tablero más un margen de 64 px de cada lado (`MARGEN_CAMARA` en `Juego.h`).
- **Viewport:** la vista de la cámara no ocupa toda la ventana, solo el 75 % de la izquierda
  (`PARTE_TABLERO` en `Camara.h`, que con 1280 de ancho son 960 px). El 25 % de la derecha queda para el panel
  del HUD. Se hace con `vista.setViewport(sf::FloatRect(0, 0, 0.75, 1))`: los valores son proporciones de la
  ventana, no píxeles. `mapPixelToCoords` y `mapCoordsToPixel` ya tienen en cuenta el viewport, así que el
  click en las casillas y los números de daño funcionan igual.
- Si ese pedazo de la ventana tiene otra proporción que el tablero, la cámara muestra más mundo del lado que
  sobra. Así el tablero nunca se deforma: aparecen bandas a los costados (o arriba y abajo).
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
- **Sonidos:** van en `Sonidos/` (WAV u OGG; SFML no carga MP3) y se cargan con ruta relativa, por ejemplo
  `buffer.loadFromFile("Sonidos/impacto.ogg");`. No hay que configurar nada: `sfml-audio` y `openal32.dll`
  ya se enlazan y se copian, y el evento posterior a la compilación copia la carpeta al lado del `.exe`.
