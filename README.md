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

Al compilar se copian solas al lado del `.exe` las DLL de SFML y las carpetas `Imagenes`, `Fuentes`, `Sonidos` y
`Datos`, así que el juego
también se abre con doble clic. La versión **Debug** solo funciona en PCs que tengan Visual Studio instalado;
para pasarle el juego a alguien, usá la carpeta de **Release** completa. El Release se abre sin la ventana negra
de consola (en `juego_pr2.vcxproj`: `SubSystem` = `Windows` y `EntryPointSymbol` = `mainCRTStartup`, para que el
programa siga empezando en `main()`); el Debug la mantiene, porque sirve para ver mensajes mientras se programa.

La carpeta `build\` se puede borrar cuando quieras: se vuelve a generar al compilar y no se sube al repositorio.

La partida se guarda en tres archivos: `partida.dat`, `Mago.dat` y `Enemigos.dat` (ver "El guardado"). Quedan en
la carpeta desde donde se ejecuta el juego: si lo corrés desde Visual Studio, es la raíz del proyecto; si abrís el
`.exe` con doble clic, quedan al lado del ejecutable. Son archivos **binarios**, así que abrirlos con el Notepad
muestra caracteres raros: eso es normal. No se suben al repositorio.

## Cómo se juega

Hay que defender el **códice** (el libro sobre el atril) en los **3 pisos** de la torre, de **2 oleadas** cada uno.
Se pierde si el mago se queda sin vida o si destruyen el códice; se gana al terminar con la última oleada del piso 3.
El códice tiene 80 de vida en el piso 1, 100 en el piso 2 y 120 en el piso 3 (en Normal).

**Dificultad.** Al elegir "Jugar" se elige **Fácil**, **Normal** o **Difícil** ([vista previa](Imagenes/vista_previa_dificultad.png)).
Queda guardada con la partida y se ve abajo a la derecha del panel. Cada una cambia estos valores (están en
`Datos/Dificultades.dat`):

| Dificultad | Vida del códice | Vida de los enemigos | Daño de los enemigos | Maná por turno | Pensada para |
|---|---|---|---|---|---|
| Fácil | 175 % | 100 % | 50 % | 3 | El que recién empieza |
| Normal | 100 % | 100 % | 100 % | 2 | Como fue pensado el juego |
| Difícil | 65 % | 150 % | 150 % | 2 | El que ya lo domina |

Los números de las tablas de abajo (y los de "Como jugar") son los de **Normal**.
Antes de cada piso aparece un cartel con la historia (Enter para empezar).

| Nivel | Tablero | Oleadas |
|---|---|---|
| 1 | El códice a la izquierda, los enemigos salen de la derecha. | 1) caballero y espectro. 2) caballero, espectro y golem. |
| 2 | Dos pozos parten el tablero. El códice está arriba y los enemigos salen de abajo: para llegar pasan por el medio o por los bordes ([vista previa](Imagenes/vista_previa_nivel2.png)). Se aprende el **muro de fuego**. | 1) dos caballeros y un espectro. 2) dos golems, un caballero y un espectro. |
| 3 | La cripta: dos abismos a los costados y dos columnas en el medio obligan a zigzaguear por el pasillo central. Aparece el **esqueleto arquero**. | 1) dos caballeros, un arquero y un espectro. 2) dos arqueros, y un golem y un espectro que salen de las esquinas. |

Todo esto (tableros, oleadas, vida y daño de cada enemigo, valores de cada habilidad y la historia) está en los
archivos de `Datos/`, no en el código: ver "Los archivos de datos".

**Ataques anunciados.** Los enemigos no pegan en el momento: al final de su turno marcan con rayas rojas la
casilla a la que van a pegar, con el número de vida que sacan, y tienen un "!" arriba de la cabeza. El golpe
llega en su turno siguiente, a esa casilla, esté quien esté: si el mago se corre, no le pasa nada (y si hay otro
enemigo, le pega a él). El códice no se puede mover, así que hay que matar o empujar al enemigo antes.

Al ganar un nivel aparece el cartel "Nivel superado", con la historia del piso siguiente y la habilidad nueva si
hay una. El nivel siguiente ya queda guardado: **Enter** sigue jugando y **Escape** vuelve al menú. El mago
empieza cada nivel con la vida y el maná llenos.

El menú principal tiene **Continuar**, **Jugar**, **Como jugar**, **Creditos** y **Salir**. "Continuar" retoma la
partida guardada (si quedó por la mitad, tal como estaba); "Jugar" arranca una partida nueva y borra el guardado.
"Creditos" muestra quiénes hicieron el juego y de dónde salen la música, los sonidos y la fuente (Enter, Escape o
un click vuelven al menú).
Mientras no haya ninguna partida guardada, "Continuar" se ve en gris y no se puede elegir. "Como jugar" explica
las reglas en 6 páginas (el objetivo, el turno, los ataques anunciados, las habilidades, el muro de fuego y los
enemigos) que se pasan con las flechas ← →; Escape vuelve al menú ([vista previa](Imagenes/vista_previa_ayuda.png)).

| Tecla | Qué hace |
|---|---|
| Click en el mago | Muestra a dónde puede caminar. Click en una casilla marcada lo mueve. |
| **1** | Bola de energía: 10 de daño a un enemigo a 3 casillas o menos. Cuesta 3 de maná. |
| **2** | Empuje: 5 de daño a un enemigo pegado, lo corre una casilla y le **cancela el ataque** que tenía marcado. Si cae al abismo, muere sin explotar (el espectro flota). Si atrás hay una columna, el borde u otro personaje, no se puede empujar y no gasta maná. Cuesta 3 de maná. |
| **3** | Muro de fuego (desde el piso 2): prende fuego en 3 casillas vacías, a 3 casillas del mago o menos, de costado a la línea entre el mago y la casilla elegida (al pasar el mouse se ve en rojo dónde va a quedar). Dura 3 turnos de los enemigos. Nadie lo puede pisar, salvo el espectro, que lo cruza pero pierde 10 de vida cada vez que entra o que empieza su turno ahí. Cuesta 4 de maná. |
| **Espacio** | Terminar el turno. |
| **Escape** | Guardar y volver al menú. |

Las habilidades y "Terminar turno" también se eligen con un click en el panel de la derecha. Cada casilla de
fuego muestra cuántos turnos de los enemigos le quedan. **-** y **=** bajan y suben la música (se ve un momento
abajo a la izquierda).

En cada turno el mago puede moverse una vez y después tirar una habilidad. Tirar una habilidad o apretar Espacio
termina el turno (moverse no). Después juegan los enemigos y el mago recupera maná (2 en Normal).

| Enemigo | Vida | Cómo ataca |
|---|---|---|
| Caballero | 20 | Cuerpo a cuerpo: 12 de daño. |
| Espectro | 20 | De lejos (hasta 3 casillas) con una bola de ectoplasma: 6 de daño. Atraviesa columnas, abismos y el muro de fuego (quemándose). |
| Golem | 40 | Explota: 30 de daño a las 8 casillas de alrededor (también a otros enemigos). Explota cuando llega a su objetivo **y también cuando lo matan**; si cae al abismo, no explota. Si su explosión mata a otro golem, ese también explota. |
| Esqueleto arquero | 20 | De lejos (hasta 4 casillas) con una flecha: 9 de daño. La flecha pasa por arriba de columnas y pozos (no hace falta línea de visión). |

Cada enemigo va por lo que tenga más cerca: el mago o el códice.

## Estructura

```
README.md              Este archivo: cómo compilar y cómo funciona el código
CONTRIBUTING.md        Pautas de código y de arte
SFML_usado.txt         Qué hace cada función de SFML que usa el juego, y dónde se usa
juego_pr2.slnx         Solución (abrir esta)
juego_pr2.vcxproj      Proyecto: rutas, SFML y copia automática de DLL, imágenes, sonidos y datos
juego_pr2.rc           Ícono del .exe (usa Imagenes/icono.ico)
src/                   Código del juego (.cpp y .h)
Imagenes/              Texturas e íconos, y sus créditos (CREDITOS.txt)
Fuentes/               Fuente del juego (Pixelify Sans) y su licencia (OFL.txt)
Sonidos/               Efectos y música (WAV/OGG) y sus créditos
Datos/                 El contenido del juego: niveles, oleadas, enemigos y habilidades (.dat binarios)
libs/SFML-2.6.2/       SFML: headers, .lib y DLL (Debug con -d, Release sin -d)
build/                 Generado al compilar (ignorado por git)
```

## Cómo funciona el código

### Las clases

| Clase | Archivos | Qué hace |
|---|---|---|
| `Juego` | `Juego.h` / `Juego.cpp` | Tiene la ventana, los menús, el catálogo, el mago, el nivel que se está jugando (`nivelActual`) y la cámara. Sabe en qué pantalla está (`estado`). `iniciar()` prepara todo y corre el game loop, `jugarTurno()` hace avanzar el turno y `cambiarNivel()` pasa al nivel siguiente. |
| `Catalogo` | `Catalogo.h` / `Catalogo.cpp` | Lee una sola vez los cinco archivos de `Datos/` y los guarda en vectores. Responde preguntas como "¿cómo es el nivel 2?" (`getNivel()`), "¿qué enemigos salen en la oleada 1?" (`getApariciones()`) o "¿qué habilidades sabe el mago en el nivel 2?" (`getHabilidadesHastaNivel()`), y crea los enemigos con su vida y su daño ajustados a la dificultad (`crearEnemigo()`). `getDificultad()` devuelve una dificultad por su número (si no existe, la Normal). Si falta un archivo, lo crea con los valores por defecto que están en `Catalogo.cpp`. |
| `Archivo<T>` | `Archivo.h` | Una **plantilla** (template) para leer y escribir un archivo binario de registros de cualquier tipo: `leerTodos()`, `leerPrimero()`, `guardarTodos()`, `agregar()`, `contarRegistros()`, `existe()` y `borrar()`. Se escribe una vez y sirve para todos: `Archivo<DatosNivel>`, `Archivo<Partida>`... Por ser plantilla, todo el código va en el `.h`. `leerTodos()` no lee un archivo cuyo tamaño no es múltiplo del registro (es de otra versión de la clase): devuelve un vector vacío y el `Catalogo` lo vuelve a crear. |
| `DatosHabilidad` | `DatosHabilidad.h` / `.cpp` | Un registro de `Habilidades.dat`: tipo, nombre, daño, costo de maná, alcance, duración y desde qué nivel se tiene. |
| `DatosEnemigo` | `DatosEnemigo.h` / `.cpp` | Un registro de `TiposEnemigo.dat`: tipo, nombre, vida, daño y alcance. |
| `DatosNivel` | `DatosNivel.h` / `.cpp` | Un registro de `Niveles.dat`: número, tablero, dónde arrancan el códice y el mago, cantidad de oleadas, vida del códice y la historia. |
| `DatosDificultad` | `DatosDificultad.h` / `.cpp` | Un registro de `Dificultades.dat`: número, nombre, descripción, porcentaje de la vida del códice, de la vida y del daño de los enemigos, y el maná por turno. `ajustarVidaCodice()`, `ajustarVidaEnemigo()` y `ajustarDanioEnemigo()` aplican esos porcentajes (redondeando al entero más cercano). |
| `MenuDificultad` | `MenuDificultad.h` / `.cpp` | La pantalla de elegir la dificultad. Funciona como el `Menu` (flechas o mouse, Enter o click) y usa su mismo arte; abajo explica la dificultad marcada. `procesarEvento()` devuelve el número de la elegida, `VOLVER` (Escape) o `NADA`. |
| `DatosAparicion` | `DatosAparicion.h` / `.cpp` | Un registro de `NivelEnemigos.dat`: un enemigo que aparece en una oleada de un nivel, y en qué casilla. |
| `Mapa` | `Mapa.h` / `Mapa.cpp` | El tablero de 8×8 (`casillas[8][8]`): qué hay en cada casilla, dónde se dibuja cada una y cómo. El constructor carga las imágenes; `cargar()` arma el tablero con el dibujo del `Nivel`. `esValida()` dice si una casilla existe, `getCasilla()` la devuelve y `mover()` pone a un personaje en una casilla si existe y la puede pisar. |
| `Casilla` | `Casilla.h` / `Casilla.cpp` | Una casilla del tablero: su tipo de terreno. `esTransitable()` dice si se puede caminar por ella (solo el suelo común). |
| `Menu` | `Menu.h` / `Menu.cpp` | Pantalla de inicio: dibuja fondo, título y opciones, y se elige con flechas + Enter o con el mouse. El arte y las medidas están en [CONTRIBUTING.md](CONTRIBUTING.md#menú-principal). |
| `Ayuda` | `Ayuda.h` / `Ayuda.cpp` | Pantalla de "Como jugar": 6 páginas, cada una con título, dibujo (`ayuda_N.png`) y texto. Los textos están en el constructor y los números (vida, daño, maná...) los saca del `Catalogo`, así que si cambia un `.dat`, la ayuda se actualiza sola. Medidas en [CONTRIBUTING.md](CONTRIBUTING.md#pantalla-como-jugar). |
| `Camara` | `Camara.h` / `Camara.cpp` | Encuadra el tablero en el 75 % izquierdo de la ventana (el resto es el panel del HUD) y lo adapta al tamaño sin deformarlo. |
| `Personaje` | `Personaje.h` / `Personaje.cpp` | Clase base de todo lo que se para en el tablero: en qué casilla está, su vida, su imagen y cómo dibujarse. |
| `Mago` | `Mago.h` / `Mago.cpp` | Hereda de `Personaje`. Agrega el maná (`gastarMana` no lo deja quedar negativo) y sus habilidades en un `std::vector<Habilidad>`. Al empezar cada nivel el `Juego` le enseña las que le corresponden (`aprenderHabilidad()`); `tieneHabilidad()` y `getHabilidad()` las buscan por su tipo. `atacar()` usa una habilidad sobre una casilla: gasta el maná y devuelve el `Ataque` (si no alcanza el maná, no gasta nada y devuelve uno vacío). |
| `Habilidad` | `Habilidad.h` / `Habilidad.cpp` | Una habilidad del mago: tipo (`TipoHabilidad`), nombre, daño, costo de maná, alcance (en casillas) y duración (solo el muro). `usar()` arma el `Ataque` sobre una casilla. |
| `Enemigo` | `Enemigo.h` / `Enemigo.cpp` | Hereda de `Personaje` y es **abstracta**: `getTipo()`, `mover()` y `atacar()` son `= 0`, así que cada hija los tiene que definir (polimorfismo). `mover()` da su paso hacia el objetivo y `atacar()` ejecuta el ataque que había anunciado. Para caminar, las hijas usan `darPaso()`, que busca el camino más corto con `pasoHacia()` rodeando lo que no pueden pisar y a los otros personajes. Tiene el daño (el *ataque* del diagrama), el alcance (`getAlcanceAtaque()`) y el **ataque anunciado** (`anunciarAtaque()`, `cancelarAtaque()`, `tieneAtaqueAnunciado()`). `planearAtaque()` dice qué casillas golpearía, `ataqueAlMorir()` qué hace al morir y `atacaADistancia()` si su ataque viaja como un proyectil. |
| `Caballero` | `Caballero.h` / `Caballero.cpp` | Hereda de `Enemigo` (es el *Guerrero* del diagrama). `mover()`: camina rodeando pozos, columnas y fuego. `atacar()`: un golpe cuerpo a cuerpo a la casilla que marcó. |
| `Espectro` | `Espectro.h` / `Espectro.cpp` | Hereda de `Enemigo`. Redefine `puedePisar` (vuela sobre abismos, columnas y el muro de fuego), `getAlcanceAtaque` (3), `atacaADistancia`, `mover()` (vuela) y `atacar()`, que llama a `lanzarProyectil()`: su ataque es una bola. |
| `Golem` | `Golem.h` / `Golem.cpp` | Hereda de `Enemigo`. Redefine `planearAtaque` (las 8 casillas de alrededor), `mover()` (camina), `atacar()`, que llama a `explotar()` (le pega a las 8 casillas y se queda sin vida), y `ataqueAlMorir` (la misma explosión). |
| `EsqueletoArquero` | `EsqueletoArquero.h` / `EsqueletoArquero.cpp` | Hereda de `Enemigo`. Camina como el caballero (`darPaso()`) pero ataca de lejos: `getAlcanceAtaque()` (4) y `atacaADistancia()` hacen que su ataque viaje como una flecha. No tiene habilidad propia: se diferencia por el alcance. |
| `Ataque` | `Ataque.h` | `struct` con lo que hace un ataque: a qué casillas les pega y cuánto. Lo usan el mago (`atacar()`), las habilidades (`usar()`), los enemigos (`atacar()`) y los proyectiles (`colisionar()`). El que ataca no sabe quién está ahí: el `Juego` lastima al que esté (`daniarCasilla()`). |
| `Pergamino` | `Pergamino.h` / `Pergamino.cpp` | El códice que hay que defender (el *Cristal* del diagrama): su casilla, su vida (la máxima la dice cada nivel: 80, 100 y 120) y su dibujo (cambia a dañado con menos de la mitad). No es un `Personaje`. |
| `Nivel` | `Nivel.h` / `Nivel.cpp` | El nivel que se está jugando. Como en el diagrama de clases, **tiene** su `Mapa`, sus enemigos (`std::vector<std::unique_ptr<Enemigo>>`) y el códice (`Pergamino`); el `Juego` los usa con `getMapa()`, `getEnemigos()` y `getCodice()`. `iniciar()` arma el tablero y el códice (con la vida de la dificultad); `getOleada()`, `hayOtraOleada()`, `siguienteOleada()` y `crearOleada()` manejan las oleadas, y `completar()` lo marca como ganado. Los datos se los pide al `Catalogo`, que guarda como puntero. |
| `Partida` | `Partida.h` / `Partida.cpp` | El registro de `partida.dat`: el nivel, la dificultad, si quedó por la mitad (`enCurso`), la oleada, la vida del códice y los muros de fuego que había. |
| `MagoGuardado` | `MagoGuardado.h` / `.cpp` | El registro de `Mago.dat`: vida, maná, casilla y si ya había caminado en ese turno. |
| `EnemigoGuardado` | `EnemigoGuardado.h` / `.cpp` | Un registro de `Enemigos.dat`: tipo, casilla, vida y el ataque que tenía anunciado. Se arma directo desde un `Enemigo`. |
| `PartidaArchivo` | `PartidaArchivo.h` / `PartidaArchivo.cpp` | Maneja los tres archivos de la partida guardada con un `Archivo<T>` para cada uno. `guardar()` guarda un nivel sin empezar, `guardarEnCurso()` uno por la mitad, `leer()` / `leerMago()` / `leerEnemigos()` los leen y `borrar()` los elimina. |
| `Proyectil` | `Proyectil.h` / `Proyectil.cpp` | Lo que vuela hasta una casilla y guarda el daño que va a hacer. `mover()` lo hace avanzar en su `direccion` y, al llegar, `colisionar()` devuelve el `Ataque` (su casilla y su daño). Tiene un tipo (`TipoProyectil`: `BOLA_ENERGIA`, `BOLA_ESPECTRO` o `FLECHA`) que decide cómo se dibuja: las bolas son 2 cuadros de 16×16 que se alternan, y la flecha es un solo dibujo de 36×14 que se gira hacia donde vuela. El `Juego` también mira el tipo para elegir el sonido al llegar. |
| `Hud` | `Hud.h` / `Hud.cpp` | Lo que se dibuja fijo en la pantalla: el panel de la derecha (nivel, oleada, vida del mago y del códice, maná, habilidades, el botón "Terminar turno" y qué hacer ahora), los números de daño que suben, los números de los ataques anunciados y los carteles (historia, victoria, derrota). También dice qué habilidad o botón hay debajo del mouse (`habilidadEn()`, `estaSobreTerminarTurno()`). |
| `Muro` | `Juego.h` | `struct` con una casilla del muro de fuego y cuántos turnos le quedan. Mientras dura, esa casilla del mapa es `TipoTerreno::MURO`. |
| `TipoTerreno` | `Casilla.h` | `enum` con los terrenos posibles: `SUELO`, `SUELO_OCUPADO` (donde está el códice: nadie lo pisa), `COLUMNAROTA` (la *ROCA* del diagrama), `ABISMO` y `MURO` (fuego: solo lo cruza el espectro). |
| `TipoEnemigo` | `Enemigo.h` | `enum` con los tipos de enemigo: `CABALLERO` (0), `ESPECTRO` (1), `GOLEM` (2) y `ESQUELETO` (3, el arquero). El número es el que se guarda en los `.dat`. |
| `HabilidadSeleccionada` | `Habilidad.h` | `enum` con la habilidad que tiene preparada el jugador: `NINGUNA`, `BOLA_DE_ENERGIA` (= 1), `EMPUJE` (= 2) y `MURO` (= 3). El número coincide con la tecla, y siempre es el `TipoHabilidad` + 1. |
| `TipoHabilidad` | `Habilidad.h` | `enum` con lo que hace cada habilidad: `BOLADEENERGIA` (0), `EMPUJE` (1) y `MURO` (2). |
| `EstadoJuego` | `Juego.h` | `enum` con la pantalla en la que está el juego: `MENU`, `ELEGIR_DIFICULTAD`, `COMO_JUGAR`, `CREDITOS`, `HISTORIA`, `JUGANDO`, `NIVEL_SUPERADO`, `GANASTE`, `PERDISTE`. |
| `FaseTurno` | `Juego.h` | `enum` con de quién es el turno: `JUGADOR`, `DISPARANDO` (la bola del mago está volando), `ENEMIGOS_ATACAN` (ejecutan lo que anunciaron) y `ENEMIGOS_ESPERAN` (vuelan sus bolas y flechas; después caminan y anuncian). |

`main.cpp` solo crea un `Juego` y llama a `iniciar()`.

### El diagrama de clases del informe

Todo lo que está en el diagrama del Anexo I existe en el código. Algunas cosas tienen otro nombre, y hay
clases de más (las pantallas, el HUD, los sonidos, los archivos): eso no es un problema, el diagrama era una
maqueta. Dónde está cada cosa:

| En el diagrama | En el código |
|---|---|
| `Juego`: `nivelActual`, `estado`, `iniciar()`, `jugarTurno()`, `cambiarNivel()`; tiene 1 `Mago` y 1..* `Nivel` | Igual. `nivelActual` es el `Nivel` que se juega (los datos de todos están en el `Catalogo`) y `estado` es un `enum`. |
| `Nivel`: `numero`, `oleada`, `iniciar()`, `siguienteOleada()`, `completar()`; tiene 1 `Mapa`, 1..* `Enemigo` y 1..* `Cristal` | Igual: el mapa, los enemigos y el códice son atributos del `Nivel`. |
| `Mapa`: `casillas[8][8]`, `mostrar()`, `esValida()`, `mover()`; tiene 64 `Casilla` | Igual. |
| `Casilla`: `tipo`, `esTransitable()` | Igual. |
| `TipoTerreno`: SUELO, ROCA, AGUA, ABISMO | SUELO, `COLUMNAROTA` (es la roca) y ABISMO (en el diagrama, AGUA se cambia por ABISMO). Además SUELO_OCUPADO y MURO. |
| `Personaje`: `vida`, `x`, `y`, `mover()`, `recibirDanio()` | Igual. `x` e `y` se llaman `fila` y `columna` (regla de CONTRIBUTING: son casillas, no píxeles). |
| `Mago`: `mana`, `atacar()`; usa 0..4 `Habilidad` | Igual (tiene hasta 3). |
| `Enemigo`: `ataque`; `mover()` y `atacar()` virtuales | Igual: `mover()` y `atacar()` son virtuales puras. El `ataque` se llama `danio`. |
| `Guerrero`: `mover()`, `atacar()` | `Caballero` (mismo rol, otro nombre). |
| `Espectro`: `mover()`, `atacar()`, `lanzarProyectil()`; lanza `Proyectil` | Igual. |
| `Golem`: `mover()`, `atacar()`, `explotar()` | Igual. |
| `Cristal`: `vida`, `x`, `y`, `recibirDanio()` | `Pergamino` (el códice). Hay uno por nivel. |
| `Habilidad`: `nombre`, `danio`, `costoMana`, `alcance`, `usar()`; genera `Proyectil` | Igual. El ataque de la bola que arma `usar()` sale como un `Proyectil`. |
| `Proyectil`: `x`, `y`, `danio`, `velocidad`, `direccion`, `mover()`, `colisionar()` | Igual. `x` e `y` son la `posicion` (un `sf::Vector2f`) y la velocidad es la constante `VELOCIDAD`. |

### El guardado

La partida se guarda **sola**: al empezar cada turno del jugador, al salir con Escape y al cerrar la ventana
(si es el turno del jugador). Así "Continuar" sigue exactamente desde donde quedó. También se guarda al ganar un
nivel (el que sigue, sin empezar) y al perder (el mismo nivel, sin empezar: "Continuar" lo vuelve a intentar).
Al ganar el último nivel o al elegir "Jugar", se borra.

Son tres archivos, porque cada uno tiene registros de un tipo distinto:

| Archivo | Registro | Qué guarda |
|---|---|---|
| `partida.dat` | `Partida` (uno) | Nivel, dificultad, si quedó por la mitad, oleada, vida del códice y los muros de fuego (hasta 12). |
| `Mago.dat` | `MagoGuardado` (uno) | Vida, maná, casilla y si ya había caminado. |
| `Enemigos.dat` | `EnemigoGuardado` (uno por enemigo) | Tipo, casilla, vida y el ataque anunciado. |

- Los registros solo tienen `int`, `bool` y arreglos (`char[]`, `int[][]`): **nada de `std::string` ni
  `std::vector`**, porque `fwrite`/`fread` copian los bytes del objeto tal como están en memoria
  (`sizeof(...)`) y esos tipos guardan un puntero a otra parte.
- `PartidaArchivo` es el único que toca los archivos, y lo hace con un `Archivo<T>` para cada uno
  (`fopen_s`, `fread`, `fwrite`, `fseek`, `ftell`, `fclose` y `remove`, todo adentro de `Archivo.h`).
- Si el nivel no había empezado (por ejemplo, se guardó entre un nivel y otro), solo existe `partida.dat`: al
  continuar se arma el nivel de cero y se muestra su historia.
- Las habilidades del mago no se guardan: se deducen del nivel.

### Los archivos de datos (`Datos/`)

El contenido del juego no está escrito en el código: lo lee el `Catalogo` de cinco archivos binarios al abrir
el juego.

| Archivo | Registro | Qué tiene |
|---|---|---|
| `Habilidades.dat` | `DatosHabilidad` | Las 3 habilidades: nombre, daño, costo, alcance, duración y nivel en el que se aprenden. |
| `TiposEnemigo.dat` | `DatosEnemigo` | Los 4 enemigos: nombre, vida, daño y alcance. |
| `Niveles.dat` | `DatosNivel` | Los 3 niveles: tablero, posición del códice y del mago, cantidad de oleadas, vida del códice e historia. |
| `NivelEnemigos.dat` | `DatosAparicion` | Cada enemigo de cada oleada: nivel, oleada, tipo y casilla de salida. |
| `Dificultades.dat` | `DatosDificultad` | Fácil, Normal y Difícil: cuánto cambian la vida del códice, la vida y el daño de los enemigos, y el maná por turno. |

**Para cambiar un valor:** se cambia en la función `...PorDefecto()` que corresponde de `Catalogo.cpp`, se borra
ese `.dat` de `Datos/` y se abre el juego desde Visual Studio: como falta, lo vuelve a crear con los valores
nuevos. Después hay que subir el `.dat` nuevo al repositorio (son parte del juego, como las imágenes).
Agregar un nivel es lo mismo: un `DatosNivel` más y sus `DatosAparicion`, sin tocar `Juego` ni `Nivel`.

Lo único que sigue en el código es la vida, el maná y el movimiento del mago (`Mago.h`).

**De dónde salen los valores.** Se simularon mil partidas por nivel, en cada dificultad, con tres jugadores
automáticos: el *experto* (mira dos turnos adelante y no se equivoca), el *medio* (juega bien pero se equivoca
seguido) y el *novato* (casi al azar). Cuántas veces gana cada uno el piso 1 / el piso 2 / el piso 3:

| Dificultad | Novato | Medio | Experto |
|---|---|---|---|
| Fácil | **68 % / 60 % / 63 %** | 98 % / 100 % / 100 % | 100 % / 100 % / 100 % |
| Normal | 44 % / 14 % / 16 % | **90 % / 86 % / 93 %** | 100 % / 100 % / 100 % |
| Difícil | 10 % / 1 % / 1 % | 58 % / 29 % / 39 % | **99 % / 100 % / 100 %** |

En negrita, el jugador para el que está pensada cada dificultad: en Fácil el novato gana la mayoría de las veces,
en Normal el medio casi siempre, y en Difícil solo se gana siempre jugando sin errores.

**El piso 3** se simuló después de que se agregó. Como estaba al principio era el más fácil de los tres (en Normal el
medio ganaba 98 %): el golem salía en el medio, pegado al espectro, y como todos suben por el mismo pasillo su
explosión mataba a sus compañeros. Bajar la vida del códice o subir el daño del arquero casi no cambiaba nada.
Lo que sí funcionó fue cambiar las oleadas: un espectro más en la primera, y en la segunda el golem y el espectro
salen de las esquinas. La vida del códice (120) y los valores del arquero quedaron como estaban.

### Sonidos

Los archivos van en `Sonidos/` y se cargan una sola vez en la clase `Sonidos`: `sf::SoundBuffer` para los
efectos cortos y `sf::Music` para la música de fondo, que se lee del disco y se reproduce en loop. SFML
carga WAV, OGG y FLAC; **no** carga MP3.

| Archivo | Cuándo suena |
|---|---|
| `musica_medieval.wav` | De fondo, en loop, desde que abre el juego (volumen 7; se cambia con `-` y `=`). |
| `inicio.ogg` | Al elegir "Jugar" (después de la dificultad) o "Continuar". |
| `bola_energia.ogg` | Al lanzar la bola de energía. |
| `impacto.ogg` | Cuando llega una bola (la del mago o la del espectro). |
| `ataque_enemigo.wav` | Cuando el caballero ejecuta su ataque cuerpo a cuerpo. |
| `explosion_golem.wav` | Cuando un golem explota, al llegar o al morir. |
| `quemadura.wav` | Cuando un enemigo se quema en el muro de fuego. |
| `ataque_espectro.ogg` | Cuando el espectro lanza su bola de ectoplasma. |
| `arco_disparo.wav` + `flecha_vuelo.wav` | Cuando el esqueleto arquero dispara. Suenan juntos: el silbido de la flecha arranca suave, cuando el disparo ya se está apagando. |
| `flecha_impacto.wav` | Cuando llega la flecha. |
| `victoria.wav` | Al terminar la última oleada del último nivel. |
| `derrota.ogg` | Cuando muere el mago o destruyen el códice. |

Victoria y derrota paran la música para que se escuche el jingle; al volver al menú, la música arranca de
nuevo. El origen y las licencias de cada archivo están en `Sonidos/CREDITOS.txt`.

Los tres sonidos del arquero (`arco_disparo.wav`, `flecha_vuelo.wav` y `flecha_impacto.wav`) ya están en
`Sonidos/` y acreditados, pero **todavía no están conectados** en `Sonidos.cpp`: el arquero dispara en silencio.

### Qué pasa al ejecutar

1. **`Juego()`** (constructor): crea la ventana de 1280×720. C++ crea los atributos en el orden del `.h`:
   primero el `Catalogo` (que lee `Datos/`), después el `Mago`, el `Nivel` (que carga las imágenes de su mapa),
   la `Camara`, los menús y la `Ayuda`. Arranca con `estado = EstadoJuego::MENU`.
2. **`iniciar()`** (lo llama `main`): le pone el ícono a la ventana, carga las imágenes de los marcadores, las
   bolas y la flecha, arma el nivel 1 con `empezarNivel(1)`, arranca la música y corre el game loop.
   - **`empezarNivel(numero)`** arma un nivel de cero: crea el `Nivel` y llama a `nivelActual.iniciar()`, que
     carga su tablero y pone el códice (con la vida del nivel ajustada por la dificultad). Después crea el mago
     donde dice el nivel (con vida y maná llenos), le enseña las habilidades de ese nivel y de los anteriores
     (`catalogo.getHabilidadesHastaNivel()`), borra los proyectiles y los muros que hubieran quedado y pone la primera
     oleada (`empezarOleada()`, que agrega los enemigos al nivel).
3. **El game loop** repite en cada frame (60 por segundo):
   - **`procesarEventos()`**: cierra la ventana si la cierran (guardando antes, si se está jugando) y reajusta
     la cámara si le cambian el tamaño. El resto depende del estado:
     - **En el menú**: le pasa cada evento a `menu.procesarEvento()`, que devuelve la opción elegida
       (o `-1`). Antes, `menu.setHayPartida(partidaArchivo.existe())` habilita o deshabilita "Continuar".
       "Continuar" llama a `continuarPartida()`, que toma la dificultad guardada: si el nivel no había empezado,
       lo arma y muestra su historia; si quedó por la mitad, lo arma y le pisa encima lo guardado (oleada, códice, muros, mago y
       enemigos con sus ataques anunciados). "Jugar" pasa a `ELEGIR_DIFICULTAD`, con la Normal marcada. "Como jugar" llama a `ayuda.reiniciar()` y pasa a
       `COMO_JUGAR`; "Creditos" pasa a `CREDITOS`; "Salir" cierra la ventana.
     - **En los créditos** (`CREDITOS`): Enter, Escape o un click vuelven a `MENU`. Se dibuja el fondo del menú y
       encima el cartel de `hud.dibujarHistoria()` con los nombres del grupo y los recursos usados.
     - **En "Como jugar"**: le pasa cada evento a `ayuda.procesarEvento()`. Si devuelve `true` (Escape,
       click en "[Esc] Volver" o Enter en la última página), vuelve a `MENU`.
     - **Elegir la dificultad** (`ELEGIR_DIFICULTAD`): le pasa cada evento a `menuDificultad.procesarEvento()`.
       Si devuelve un número, `reiniciarPartida(numero)` guarda esa dificultad en `dificultad`, borra los
       archivos, arma el nivel 1 y muestra su historia; si devuelve `VOLVER`, vuelve al menú.
     - **Historia** (`HISTORIA`): Enter pasa a `JUGANDO`; Escape vuelve al menú.
     - **Jugando**: lo maneja `procesarEventoJugando()`. Escape llama a `volverAlMenu()`, que guarda y vuelve
       al menú (solo en el turno del jugador: en el de los enemigos, que dura menos de un segundo, se ignora).
       Un click **fuera** del tablero se pasa a la vista por defecto y se le pregunta al `Hud` si cayó sobre una
       habilidad (`hud.habilidadEn()`) o sobre "Terminar turno" (`hud.estaSobreTerminarTurno()`).
       Para moverse, click en el mago: se marcan las casillas a las que puede caminar este turno
       (`casillasAlcanzables()`: una búsqueda por pasos que rodea pozos, columnas y personajes, hasta
       `MOVIMIENTO` pasos, constante en `Mago.h`). Click en una marcada lo mueve (con `Mapa::mover()`); click en cualquier
       otro lado cancela. Moverse no termina el turno: `magoSeMovio` queda en `true` y todavía puede tirar una habilidad.
       **1** a **3** (o el click en el panel) llaman a `elegirHabilidad()`: prepara la habilidad
       (`habilidadPreparada`; la misma otra vez la cancela; si el mago todavía no la aprendió, no pasa nada) y se
       marcan las casillas al alcance. El click en una casilla llama a `usarHabilidad()`, que revisa que se pueda
       y llama a `mago.atacar()` (gasta el maná y devuelve el `Ataque`). Después, según el `TipoHabilidad`:
       - **Bola:** crea un `Proyectil` (fase `DISPARANDO`).
       - **Empuje:** primero pregunta `sePuedeEmpujar()` (si no hay lugar atrás, no pasa nada y no se gasta
         maná). `empujar()` le cancela el ataque anunciado, le hace el daño y lo corre. Si cae al abismo se le saca
         toda la vida **sin** pasar por `daniarCasilla()`, así el golem que cae no explota.
       - **Muro de fuego:** se marcan las casillas vacías al alcance y, con el mouse arriba de una
         (`casillaBajoElMouse()`), en rojo las 3 donde quedaría el fuego (`casillasDelMuro()`). `ponerMuro()`
         cambia esas casillas a `TipoTerreno::MURO` y las guarda en `muros` con la duración de `Habilidades.dat`.

       **Espacio** (o el botón) llama a `pasarTurno()`.
     - **Nivel superado** (`NIVEL_SUPERADO`): el nivel siguiente ya quedó guardado al ganar. **Enter** llama a
       `cambiarNivel()`, que arma el nivel siguiente y vuelve a `JUGANDO`; **Escape** (o **S**) vuelve al menú.
     - **Fin de partida** (`GANASTE` o `PERDISTE`): Enter o Escape vuelven al menú.
   - **`actualizar(dt)`**: `dt` son los segundos desde el cuadro anterior (lo mide un `sf::Clock`). Anima los
     números, el fuego y los golpes y, si se está jugando, llama a **`jugarTurno(dt)`**, que hace avanzar el turno:
     - Mientras vuela la bola del mago (`DISPARANDO`), `moverProyectiles()` la mueve (`Proyectil::mover()`); al
       llegar, `colisionar()` devuelve su golpe y `daniarCasilla()` lastima a quien esté ahí. Se termina el turno.
     - `terminarTurnoJugador()` saca a los enemigos sin vida, pasa a `ENEMIGOS_ATACAN` y llama a
       `revisarFinDePartida()`: si no quedan enemigos, aparece la próxima oleada y el jugador vuelve a jugar
       (respiro, para que la vea aparecer); si era la última, se pasa a `NIVEL_SUPERADO` o a `GANASTE`.
     - **Turno de los enemigos, primera parte** (`ejecutarAtaquesEnemigos()`): cada uno, en orden, se quema si
       está parado en el fuego y, si tenía un ataque anunciado, llama a `enemigo.atacar()` (`virtual`: el
       caballero golpea, el espectro y el arquero tiran un proyectil (bola de ectoplasma o flecha) y el golem
       `explotar()`, que lo destruye). El ataque cae en las casillas marcadas, esté quien esté: los que atacan de
       lejos tiran un proyectil por casilla; los demás pegan en el momento (y las casillas se marcan en rojo un
       instante, `explosiones`). Al terminar, pasa a `ENEMIGOS_ESPERAN`.
     - **Segunda parte** (`moverEnemigos()`): cuando llegaron los proyectiles y pasó `PAUSA_ENEMIGOS` (para que se
       vean los golpes), cada enemigo elige lo que tenga más cerca (`objetivoDe()`: el mago o el códice). Si no
       le llega el ataque, llama a `enemigo.mover()` (`virtual`) con las casillas ocupadas
       (`casillasOcupadas()`): da un paso por el camino más corto hasta una casilla desde la que pueda atacar
       (`Enemigo::pasoHacia()`; si no hay camino libre, prueba en línea recta con `calcularPaso()`). Si desde
       donde quedó le llega, **anuncia** el próximo ataque: `anunciarAtaque(planearAtaque(...))`.
     - **Explosión al morir:** cada vez que `daniarCasilla()` deja a un enemigo sin vida, llama a
       `alMorirEnemigo()`, que hace su `ataqueAlMorir()` (`virtual`: solo el golem tiene uno). Si esa explosión
       mata a otro golem, `daniarCasilla()` vuelve a llamar a `alMorirEnemigo()` (reacción en cadena); no se
       repite para siempre porque a los muertos `buscarEnemigoEn()` no los encuentra.
     - **Fuego:** `quemarSiEstaEnFuego()` le hace al enemigo parado en un muro el daño del muro en
       `Habilidades.dat` (10). Se llama al empezar el turno de cada enemigo, después de que da un paso y en
       `empujar()`. Solo el espectro puede estar ahí.

       Al terminar vuelve el turno del jugador (`empezarTurnoJugador()`): el mago recupera el maná de la
       dificultad (2 en Normal), a cada casilla de fuego le queda un turno menos (`apagarMuros()`), se revisa si se
       perdió y se guarda la partida.
   - El fuego se anima solo: `Mapa::actualizar(dt)` suma el tiempo y `Mapa::mostrar()` elige uno de los
     3 cuadros de `muro_fuego_animacion.png` con `setTextureRect`, igual que la bola.
   - **`dibujar()`**: limpia la pantalla y, según el estado, dibuja el menú, "Como jugar" (`ayuda.dibujar()`)
     o el tablero (`dibujarTablero()`), en este orden (lo último que se dibuja queda arriba):
     1. el fondo, con la vista por defecto;
     2. con la cámara: el mapa, los marcadores (movimiento y alcance de la habilidad), los ataques anunciados
        (`marcador_peligro.png`, arriba de los otros marcadores para que siempre se vean), las casillas rojas de
        los golpes, los personajes y el códice ordenados por profundidad (`fila + columna`, de atrás hacia
        adelante), las barras de vida y los proyectiles;
     3. con la vista por defecto: los números de los ataques anunciados y los "!" (`hud.dibujarAmenazas()`), los
        turnos que le quedan a cada casilla de fuego (`hud.dibujarTurnosFuego()`) y el HUD (`hud.dibujar()`): los
        números de daño y el panel de la derecha;
     4. los carteles: la historia (`hud.dibujarHistoria()`, antes de cada piso y en "Nivel superado") o
        victoria / derrota (`hud.dibujarFinDePartida()`).

     En cualquier pantalla, si se tocó el volumen hace menos de 1,5 s (`tiempoVolumen`), al final se dibuja
     "Musica: 7" abajo a la izquierda (`hud.dibujarVolumen()`), que se desvanece en el último medio segundo.

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
