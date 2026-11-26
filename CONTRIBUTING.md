# Pautas del proyecto

Reglas para que el código y el arte encajen entre todos. Si algo no está claro o no estás de acuerdo con
alguna, hablémoslo antes de cambiarla.

Para entender cómo está armado el código, primero leé la sección "Cómo funciona el código" del
[README](README.md).

## Programación orientada a objetos

- **Nada de funciones globales.** Todo va en una clase. Si una función no encaja en ninguna clase
  existente, probablemente haga falta una clase nueva.
- **Cada clase tiene su `.h` y su `.cpp`** en `src/`. El `.h` tiene la declaración y el `.cpp` la
  implementación. Excepción: los `enum` no llevan archivo propio; van en el `.h` de la clase que los
  usa (como `TipoTerreno` en `Casilla.h`), declarados antes de la clase. Otra excepción: las **plantillas**
  (`template`, como `Archivo<T>` en `Archivo.h`) van enteras en el `.h`, porque el compilador necesita ver
  todo su código en cada lugar donde se usan. Y los `struct` que usan varias clases (como `Ataque`) van en
  su propio `.h`, sin `.cpp`, porque son solo datos.
- **Atributos privados.** Si otra clase necesita un dato, se le da un método público (`get...`).
- **Excepción, clases base:** en las clases de las que heredan otras (como `Personaje`), los atributos
  que necesitan las hijas van en `protected`, no en `private`. Con `private`, `Mago` no podría usar la
  `vida` ni la posición aunque herede de `Personaje`. Para las demás clases, `protected` sigue siendo
  como `private`: nadie de afuera los ve.
- **Herencia:** `Mago` y `Enemigo` son hijas de `Personaje`. Lo que comparten (posición, vida, dibujarse)
  va en `Personaje`; lo propio de cada una (el maná del mago, el ataque del enemigo), en la hija.
- **Métodos privados** para lo que solo se usa dentro de la clase (como `procesarEventos()` en `Juego`).
- **`main.cpp` no tiene lógica:** solo crea el `Juego` y llama a `iniciar()`.
- **Diagrama de clases del informe:** todo lo que está en el diagrama tiene que existir en el código (puede
  haber de más, pero no de menos). Antes de borrar o renombrar algo que está en el diagrama, avisen al grupo y
  actualicen la tabla "El diagrama de clases del informe" del README.

## Archivos binarios (`.dat`)

- **Una clase por tipo de registro** (`DatosNivel`, `Partida`, `EnemigoGuardado`...), con su `.h` y su `.cpp`.
- **Registros "planos":** solo `int`, `bool`, `float` y arreglos (`char nombre[30]`, `int muros[12][3]`).
  **Nunca** `std::string`, `std::vector` ni punteros: `fwrite` copia los bytes del objeto tal como están en
  memoria, y esos tipos guardan una dirección a otra parte (que al volver a abrir el juego no apunta a nada).
  Para el texto: `strncpy_s(nombre, sizeof(nombre), texto.c_str(), _TRUNCATE)` al guardar y `std::string(nombre)`
  al leer.
- **Para leer y escribir, `Archivo<T>`** (no se repite el `fopen`/`fread` en cada clase). Usa `fopen_s`, la versión
  de `fopen` que acepta Visual Studio sin `_CRT_SECURE_NO_WARNINGS`.
- **Si se cambia una clase de registro** (se agrega o se saca un atributo), cambia su tamaño y los `.dat` viejos
  ya no se pueden leer: hay que borrarlos y dejar que el juego los vuelva a crear. Si el tamaño del archivo no es
  múltiplo del registro nuevo, `Archivo<T>::leerTodos()` lo ignora y el `Catalogo` lo rehace solo; pero si por
  casualidad sí lo es, se leería basura. Por eso: siempre borrarlos.
- **Los valores del juego van en `Datos/`, no en el código.** Para cambiar uno: editar el `...PorDefecto()` de
  `Catalogo.cpp`, borrar el `.dat` de `Datos/` y abrir el juego desde Visual Studio (lo vuelve a crear). Los `.dat`
  de `Datos/` se suben al repositorio; los de la partida guardada (`partida.dat`, `Mago.dat`, `Enemigos.dat`), no.

## Archivos nuevos

- Crealos **desde Visual Studio** (*Agregar → Nuevo elemento*), cambiando la ubicación a `src\`. Si los
  creás desde el Explorador de Windows, Visual Studio no los ve y no los compila.
- Todo `.h` empieza con `#pragma once`.

## Nombres y estilo

| Qué | Cómo | Ejemplo |
|---|---|---|
| Clases | Mayúscula inicial | `Mapa`, `Camara` |
| Métodos y variables | Primera palabra en minúscula, las demás con mayúscula | `getTamanioMundo`, `areaVisible` |
| Constantes | Todo en mayúscula, con guion bajo | `CANT_CASILLAS`, `MARGEN_CAMARA` |
| Getters / setters | `get` / `set` + nombre | `getTipoTerreno`, `setTipoTerreno` |

- **Nombres en castellano.**
- **Sin `ñ` ni tildes en los nombres** (sí en los comentarios): `tamanio`, no `tamaño`.
- **Indentación con tabs.**
- **Métodos que no modifican el objeto llevan `const`** al final, en el `.h` y en el `.cpp`.
  Ejemplo: `sf::Vector2f getTamanioMundo() const;`.
- **Constantes de clase:** `static const int` para enteros, `static constexpr float` para decimales.
- **Comentarios:** que expliquen el *por qué*, no lo que ya se lee en el código. Los métodos importantes
  se documentan en el `.h` con un comentario normal (`//`) justo arriba de la declaración. No usamos
  formato Doxygen (`/** @brief ... */`).
- **Cada clase lleva un comentario arriba** (en el `.h`, justo antes de `class`) que diga qué es y para qué está.
- **Forma de un `.h`:** `#pragma once`, los `#include` (primero SFML y la biblioteca estándar, después los del
  proyecto), el comentario de la clase y la clase. Adentro, `private:` / `public:` con un tab y los miembros con
  dos. Primero las constantes, después los atributos y al final los métodos.
- **Llaves:** la que abre va en la misma línea (`void Mapa::mostrar() {`, `if (vida < 0) {`), y el `else` va en
  la línea de abajo del `}`. Después del `}` de una función no va `;` (solo después de `class`, `struct` y `enum`).
- **Constructores:** los atributos se inicializan en la lista de inicialización (`: fila(_fila), vida(_vida)`), en el
  mismo orden en que están declarados en el `.h` (C++ los inicializa en ese orden, no en el de la lista).
- **Parámetros con `_`** cuando se llaman igual que un atributo (`setVida(int _vida)`), para no taparlo.

## Posiciones y dibujo

- **La lógica del juego trabaja en filas y columnas, nunca en píxeles.** Personajes, enemigos y objetos
  guardan en qué casilla están, no su posición en pantalla. Los atributos se llaman `fila` y `columna`
  (no `x` e `y`, que suenan a píxeles).
- **Para saber dónde dibujar una casilla, usá siempre `Mapa::casillaAmundo(fila, columna)`.** No hagas la
  cuenta a mano (`columna * ANCHO_CASILLAS`…) en otro lado: cuando pasemos a isométrica, esa cuenta va a
  quedar mal y no va a ser obvio por qué.
- **Orden de parámetros: primero fila, después columna**, igual que `casillas[fila][columna]`.
- **Nada de la ventana dentro del `Mapa`.** El `Mapa` dibuja en coordenadas del mundo y la `Camara` lo
  acomoda en la ventana.
- **Origen de los sprites:** el punto de la imagen que tiene que caer en el centro de la casilla.
  - Baldosas: el centro de la cara de arriba del cubo (ver "Arte").
  - Objetos altos (personajes, enemigos): origen en los pies. Todavía no hay ninguno.

## Orden de los atributos en `Juego.h`

C++ crea los atributos en el orden en que están declarados en el `.h`. La `Camara` le pide el tamaño al mapa
del nivel al crearse, así que **`nivelActual` tiene que estar declarado antes que `camara`**. Si se invierten,
compila igual, pero la cámara le pide el tamaño a un mapa que todavía no existe. Lo mismo con el **`catalogo`:
va antes que `nivelActual`, `menuDificultad` y `ayuda`**, que lo usan al crearse.

## Arte

- **Las imágenes van en `Imagenes/`** y se cargan con ruta relativa: `"Imagenes/mago_64.png"`.
- **PNG con fondo transparente.**
- **Nombres:** las baldosas isométricas terminan en `_iso.png` (`suelo_1_iso.png`). Lo de cada pantalla
  empieza con su nombre (`menu_fondo.png`).
- **Pixel art nítido:** escalar solo por números enteros (×2, ×3), nunca ×1.5, porque los píxeles quedan
  de distinto tamaño.

### Paleta

Todo el juego usa estos colores. Si algo nuevo no combina, probablemente esté usando un color de afuera.

| Nombre | Hex | En SFML | Para qué |
|---|---|---|---|
| Fondo | `#0B0D1A` | `sf::Color(11, 13, 26)` | Fondo de todas las pantallas (`window.clear(...)`) |
| Pizarra | `#3A4258` | `sf::Color(58, 66, 88)` | Paneles, barras vacías, bordes |
| Piedra | `#B8B2A4` | `sf::Color(184, 178, 164)` | Texto normal |
| Tenue | `#5C657C` | `sf::Color(92, 101, 124)` | Texto secundario, opciones deshabilitadas |
| **Gema** | `#5FE0E8` | `sf::Color(95, 224, 232)` | **Solo lo importante:** opción elegida, títulos, maná |
| Gema oscura | `#2E8CA6` | `sf::Color(46, 140, 166)` | Sombras de lo que es Gema |
| Vacío | `#5C4096` | `sf::Color(92, 64, 150)` | El vacío debajo del tablero y la niebla de los abismos |
| Vacío claro | `#9676D6` | `sf::Color(150, 118, 214)` | Brillos y motas que suben del vacío |
| Peligro | `#B04A4A` | `sf::Color(176, 74, 74)` | Vida, daño, avisos |
| Fuego | `#F08C38` | `sf::Color(240, 140, 56)` | Solo el muro de fuego (sus llamas van de `#601A1E` en la punta a `#FFF0C4` en el centro) y el número de turnos que le quedan |

El celeste **Gema** sale de la piedra del báculo del mago. Si se usa para todo, deja de llamar la atención.

### Fuente

**Pixelify Sans** (`Fuentes/PixelifySans-Regular.ttf`), una sola para todo el juego. Licencia libre (OFL):
se puede usar y entregar, siempre que `OFL.txt` viaje con la fuente.

- Tamaños: **22** para opciones y textos, **11** para letra chica. Son los tamaños en los que la fuente
  queda alineada a su grilla de píxeles; otros se ven borrosos.
- Los títulos grandes no se escriben con la fuente: son imágenes (`menu_titulo.png`), porque están
  retocados a mano.
- **Tildes:** SFML no lee bien el texto con tildes si se le pasa directo. Usar
  `sf::String::fromUtf8(...)`:
  ```cpp
  std::string texto = "Cómo jugar";
  sf::Text opcion(sf::String::fromUtf8(texto.begin(), texto.end()), fuente, 22);
  ```

### Menú principal

Pantalla de **1280×720** con su propia vista fija (`sf::View(sf::FloatRect(0, 0, 1280, 720))`), así las medidas
de abajo valen aunque cambie el tamaño de la ventana. `Imagenes/vista_previa_menu.png` muestra cómo
tiene que quedar.

| Qué | Archivo | Dónde |
|---|---|---|
| Fondo | `menu_fondo.png` (1280×720) | En (0, 0). Escena general: la torre de noche, el mago a la izquierda y los tres enemigos a la derecha. Deja libres el lugar del título y el de las opciones. |
| Título | `menu_titulo.png` (524×84) | Centrado en x, arriba en **y = 168** → posición (378, 168). |
| Opciones | texto, fuente 22 | Centradas en x (**x = 640**). La primera con su centro en **y = 520**, las demás cada **40 px**. |
| Selector | `menu_selector.png` (18×18) | A la izquierda de la opción elegida, **14 px** antes del texto, centrado en su altura. |
| Pie | texto, fuente 11, color Tenue | Abajo a la derecha, a 16 px de cada borde. |

- Opción normal: color **Piedra**. Opción elegida (o con el mouse encima): color **Gema** + selector.
- Opción que no se puede usar (por ejemplo "Continuar" sin partida guardada): color **Tenue**, sin selector.
- Opcional: que el selector "respire" cambiando su transparencia (alfa entre 150 y 255) en un ciclo de
  ~1,5 segundos.

### Pantalla de dificultad

Aparece al elegir "Nueva partida" (clase `MenuDificultad`). Es el menú principal con otras opciones: mismo fondo,
mismo título y mismo selector. `Imagenes/vista_previa_dificultad.png` muestra cómo tiene que quedar.

| Qué | Forma | Dónde |
|---|---|---|
| Fondo y título | `menu_fondo.png` y `menu_titulo.png` | Igual que en el menú. |
| "Elegi la dificultad" | texto 22, **Gema**, borde **Fondo** | Centrado en (640, 470). |
| Opciones | texto 22 sin borde: **Gema** la marcada, **Piedra** las demás | Centradas en x = 640, desde y = 520, cada 40 px (como las del menú). |
| Selector | `menu_selector.png` | 14 px a la izquierda de la opción marcada, como en el menú. |
| Descripción | texto 22, **Piedra**, borde **Fondo** | Centrada en (640, 652). Sale de `Dificultades.dat` (hasta 47 letras). |
| Detalle | texto 22, **Tenue**, borde **Fondo** | Centrado en (640, 684): "Codice 100%   Enemigos: vida 100%, danio 100%   Mana +2 por turno". |
| "[Esc] Volver" | texto 22, **Tenue**, borde **Fondo** | Arriba a la izquierda, en (24, 24). |

### Pantalla Como jugar

La abre la opción "Como jugar" del menú (clase `Ayuda`). Pantalla de **1280×720** con su propia vista fija,
igual que el menú. Tiene 6 páginas. `Imagenes/vista_previa_ayuda.png` muestra cómo tiene que quedar (página 3).

| Qué | Archivo / forma | Dónde |
|---|---|---|
| Fondo | `menu_fondo.png` + rectángulo color **Fondo** con alfa 215 | Toda la pantalla: el menú se adivina atrás. |
| Panel | rectángulo **Fondo**, borde de 2 px **Pizarra** | De (120, 50) a (1160, 670). |
| Título | texto 22 con `setScale(2, 2)`, color **Gema** | Centrado en (640, 100). |
| Líneas separadoras | rectángulos de 960×2, **Pizarra** | x = 160, en y = 140 (abajo del título) y en y = 596 (arriba del pie). |
| Dibujo | `ayuda_1.png` … `ayuda_6.png` (unos 380×320 a 380×430) | Su centro en (360, 375). |
| Texto | texto 22; **Gema** los subtítulos, **Piedra** el resto | Desde (600, 168), una línea cada **30 px**. |
| Pie | texto 22, **Tenue** | "[Esc] Volver" en (160, 616); "Flechas: pagina   [-] [=]: musica" alineado a la derecha en x = 1120. |
| Número de página | texto 22, **Piedra** | "1 / 6" centrado en x = 640, y = 616. |
| Flechas | `ayuda_flecha.png` (12×20, apunta a la derecha) | La siguiente en (700, 620); la anterior es la misma espejada (`setScale(-1, 1)`) en (580, 620). Si no hay página para ese lado, alfa 50. |

- **Texto de cada página:** está en el constructor de `Ayuda`, una línea por string. Cada línea entra en
  520 px (de x = 600 a 1120): unos **42 caracteres**. Entran **13 líneas** como mucho. Las que empiezan con
  `#` son subtítulos (se dibujan en Gema y sin el `#`); un string vacío `""` deja un renglón en blanco.
- **Sin tildes,** como el resto de los textos del juego.
- **Los dibujos** son un tablerito de 3×3 armado con las mismas baldosas y personajes del juego, agrandado
  x2 sin suavizar. Si cambia una baldosa o un personaje, hay que volver a armarlos.
- **Página 6 (ENEMIGOS):** los cuatro enemigos en rombo, para que ninguno tape a otro (en el tablerito de 3×3
  no entran cuatro en fila): caballero a la izquierda, espectro atrás, golem a la derecha y el esqueleto
  arquero adelante. El texto ocupa las 13 líneas: si se agrega otro enemigo, no entra en esta página.
- **Los números de las reglas** (vida, daño, maná, alcance, en qué piso se aprende cada habilidad) se arman con
  `std::to_string` a partir del `Catalogo`: si se cambia un `.dat`, la ayuda se actualiza sola. Lo único escrito
  a mano es el movimiento y el maná del mago (página 2). Los números son los de la dificultad **Normal** (la
  página 1 lo aclara). Ojo con el largo: un número
  de dos cifras ocupa una letra más.
- **Los dibujos de las páginas 3 a 5** también tienen marcadores: el rayado de peligro con el daño y el "!"
  (ataques anunciados), la bola y el empuje, y el muro de fuego.

### Baldosas isométricas

Todas las baldosas usan **el mismo lienzo y el cubo en el mismo lugar**. Si una se corre un píxel, el
tablero se ve roto.

```
y = 0    ┌──────────┐
         │          │  espacio libre (30 px) para lo que sobresale hacia arriba
         │          │  (la columna rota, por ejemplo)
y = 30   │    /\    │  ← vértice de arriba del rombo
y = 50   │  < ● >   │  ← centro del rombo = vértices izquierdo y derecho
y = 70   │  |\/ |   │  ← vértice de abajo del rombo
         │  \ | /   │  costados del cubo (14 px)
y = 83   └──────────┘  ← fondo de la imagen
           64 px
```

- **Lienzo de 64×84**, fondo transparente.
- **Cara de arriba:** rombo de **64×40** (`ANCHO_CASILLAS` × `ALTO_CASILLAS` en `Mapa.h`). Vértice de arriba en
  y = 30, centro en **y = 50** (**34 px desde el fondo** de la imagen, que es `CENTRO_ROMBO_DESDE_ABAJO`).
- **Perspectiva:** 40 de alto y no 32 (el isométrico clásico 2:1) para que el tablero se vea un poco más
  desde arriba. Si algún día se cambia, hay que cambiar `ALTO_CASILLAS` **y** volver a generar todo el arte
  con el script (`ALTO_ROMBO`); si no coinciden, las baldosas se pisan o quedan separadas.
- **Costados:** 14 px debajo del rombo, hasta el fondo de la imagen.
- **Lo que sobresale** (columnas, rocas) va hacia arriba, en los 30 px libres. No se puede correr el cubo
  hacia abajo para hacerle lugar.
- **Abismo:** casi transparente, con unas motas de luz violeta. El pozo se ve gracias a las **paredes de
  pozo** de las casillas de atrás (ver abajo), así varias casillas de abismo juntas se leen como un solo agujero.
- **Misma luz en todas:** la luz viene de arriba. En la cara de arriba, el borde de atrás es más claro y el
  de adelante más oscuro; el costado izquierdo es más claro que el derecho.

### Estilo: la cripta flota sobre el vacío

El tablero es una **isla de piedra flotando sobre un vacío violeta**. Abajo del borde cuelga roca rota, y
el mismo vacío se ve por los pozos del abismo, así que se entiende qué pasa si alguien cae.
Vistas previas (1280×720, escala ×2, sin el panel del HUD): [nivel 1](Imagenes/vista_previa_mapa.png) y
[nivel 2](Imagenes/vista_previa_nivel2.png).

| Archivo | Lienzo | Qué es |
|---|---|---|
| `suelo_1_iso.png` | 64×84 | Lisa (4 losas por casilla). La más común (~60 % de las casillas). |
| `suelo_2_iso.png` | 64×84 | Con una grieta (~15 %). |
| `suelo_3_iso.png` | 64×84 | Con musgo en una esquina (~15 %). |
| `suelo_4_iso.png` | 64×84 | Con una runa grabada que apenas brilla (~10 %). Usarla poco: si hay muchas, dejan de llamar la atención. |
| `columna_rota_iso.png` | 64×84 | Columna rota sobre un pedestal cuadrado grande, encima de una baldosa lisa. |
| `abismo_iso.png` | 64×84 | Casilla de abismo (ver arriba). |
| `pared_izquierda_iso.png` | 64×128 | Roca que cuelga debajo del borde de **abajo-izquierda** del rombo (borde de la isla). |
| `pared_derecha_iso.png` | 64×128 | Igual, debajo del borde de **abajo-derecha**. Más oscura (lado sin luz). |
| `pozo_izquierda_iso.png` | 64×160 | Pared de adentro de un pozo: piedra tallada que baja y se pierde en la oscuridad. |
| `pozo_derecha_iso.png` | 64×160 | Igual, del lado derecho. |
| `fondo_juego.png` | 1280×720 | Fondo de la pantalla de juego: oscuro arriba, el vacío violeta abajo. |
| `sombra_personaje.png` | 28×10 | Sombra ovalada que va en los pies de cada personaje. |
| `muro_fuego_iso.png` | 64×84 | Una casilla del muro de fuego (habilidad del nivel 2): piso chamuscado con llamas encima. |
| `muro_fuego_animacion.png` | 192×84 | El mismo muro, animado: **3 cuadros de 64×84** uno al lado del otro. |

- **Mismo centro en todas:** el centro del rombo está siempre a **50 px desde arriba** de la imagen, también
  en las paredes y pozos (aunque sean más altos). Por eso a las paredes no les sirve `CENTRO_ROMBO_DESDE_ABAJO`:
  el origen se pone en `(32, 50)`.
- **Las variantes de suelo se eligen una sola vez** (al crear el mapa) y se guardan; si se eligieran en
  cada `mostrar()` el piso titilaría.
- **Qué pared va:** en una casilla que no es abismo, se mira la casilla de adelante (`fila + 1` para la
  izquierda, `columna + 1` para la derecha):

  | Casilla de adelante | Pared |
  |---|---|
  | Fuera del tablero | `pared_*_iso` (roca que cuelga bajo la isla) |
  | Abismo | `pozo_*_iso` (pared de adentro del pozo) |
  | Suelo o columna | ninguna |
- **Orden:** primero las paredes de la casilla y después su baldosa. La parte de arriba de la pared queda
  tapada por el costado de la baldosa.
- **Muro de fuego:** se dibuja como una baldosa más (mismo lienzo y mismo origen que la columna rota), en vez de
  la baldosa de suelo de esa casilla. El juego usa la tira animada, igual que la bola:
  `setTextureRect(sf::IntRect(cuadro * 64, 0, 64, 84))`, cambiando de cuadro cada 0,15 s (`Mapa::mostrar`).
  `muro_fuego_iso.png` es un cuadro solo, por si alguna vez hace falta la imagen quieta.
  Las llamas sobresalen hasta 44 px arriba del centro. Como el mapa entero se dibuja antes que los personajes,
  el espectro que cruza el fuego se ve delante de las llamas, igual que pasa con la columna. Vista previa: [Imagenes/vista_previa_muro.png](Imagenes/vista_previa_muro.png).
- **El abismo conviene adentro del tablero**, no en el borde: en el borde parece que al tablero le falta un
  pedazo. Rodeado de suelo se lee como un pozo.
- **Fondo:** se dibuja primero y con la vista normal de la ventana (`window.getDefaultView()`), no con la de
  la cámara, así queda fijo y cubre toda la pantalla.
- **Sombra:** origen en su centro `(14, 5)`, puesta en el mismo punto que los pies del personaje y dibujada
  **antes** que él.
- **La grilla:** el borde de cada baldosa es la línea más oscura (`#12141F`), así se distingue cada casilla.
  Adentro, cada casilla tiene **4 losas biseladas** (luz arriba, sombra abajo) separadas por juntas un poco
  más claras que el borde.
- **Pocos detalles:** el piso es el fondo de la acción. Como mucho un reflejo suave por losa (y solo en la
  mitad de ellas); la grieta, el musgo y la runa son pocos y suaves para que no distraigan de los personajes.
- **Cómo se generan:** con un script de Python (`generar_arte_mapa.py`) que **no está en el repositorio**,
  porque el proyecto es solo C++: está en la carpeta `herramientas/` de la PC de Lautaro (ignorada por git).
  Para cambiar algo (un color, una variante nueva), se edita el script y se vuelven a generar los PNG en
  `Imagenes/`. No retoques los PNG a mano: la próxima vez que se corra el script se pierde el retoque.

### Habilidades, ataques y HUD

| Archivo | Lienzo | Qué es | Origen |
|---|---|---|---|
| `marcador_movimiento.png` | 64×40 | Rombo **Piedra** transparente: casilla a la que el mago puede caminar (después de clickearlo). | `(32, 20)` |
| `marcador_alcance.png` | 64×40 | Rombo **Gema** transparente: casilla a la que llega la habilidad. | Centro del rombo, `(32, 20)` |
| `marcador_peligro.png` | 64×40 | Rombo **Peligro** con rayas a 45° (como cinta de peligro) y borde rojo claro opaco: casilla a la que un enemigo **anunció** que va a pegar. Se dibuja arriba de los otros marcadores, para que se vea aunque la casilla también esté marcada para caminar. | `(32, 20)` |
| `marcador_objetivo.png` | 64×40 | Igual que el de alcance, en rojo (**Peligro**): hay un enemigo al alcance. También marca, desvaneciéndose en 0,5 s, las casillas que recibieron un golpe, la explosión del golem o la quemadura del fuego, y con el muro preparado, las 3 casillas donde quedaría. | `(32, 20)` |
| `bola_energia.png` | 32×16 | Bola de energía: **2 cuadros de 16×16** uno al lado del otro, se alternan cada 0,1 s. | Centro del cuadro, `(8, 8)` |
| `bola_espectro.png` | 32×16 | Bola de ectoplasma del espectro: mismo formato que la bola de energía, en los colores del **Vacío**, con dos ojitos oscuros y jirones rojizos para que no se confunda con la del mago. | `(8, 8)` |
| `flecha.png` | 36×14 | Flecha del esqueleto arquero. Apunta a la **derecha** y es un solo dibujo: el código la gira con `setRotation` según hacia dónde vuela (`atan2` de la dirección). | Centro del dibujo, `(18, 7)` |

- **Marcadores:** son 1 px más chicos que el rombo de la baldosa, para que la junta oscura se siga viendo,
  y el relleno es tramado y casi transparente, para que se vea el piso. Se dibujan después del mapa y antes
  que los personajes. En el abismo solo se marca si hay un enemigo (el espectro vuela).
- **Los proyectiles** (la bola y la flecha) salen del pecho y pegan en el pecho: **30 px arriba de los pies**
  (`ALTURA_PECHO` en `Juego.h`). La flecha, además, se dibuja girada: la imagen apunta a la derecha.
- **Barra de vida arriba de la cabeza** (enemigos y mago): 32×4 px (del mundo), 6 px arriba de la imagen
  del personaje. Vacía en **Pizarra**, llena en **Peligro**, con un borde de 1 px color **Fondo**.
- **Panel del HUD a la derecha** (fijo en la pantalla): de x = 960 a 1280, todo el alto. Es la parte de la
  ventana que la cámara deja libre (`PARTE_TABLERO` = 0,75 en `Camara.h`; si se cambia una, cambiar la otra:
  `X_PANEL` en `Hud.cpp`). Relleno **Fondo** casi opaco (alfa 235) y un borde de 2 px **Pizarra** a la izquierda.
  El contenido va de x = 984 a x = 1256 (24 px de margen). Lo que va a la derecha se alinea a x = 1256.
  Las partes se separan con líneas de 2 px **Pizarra**. De arriba hacia abajo:
  - **Nivel y oleada** (y = 24): "Nivel 1/3" en **Gema** a la izquierda y "Oleada 1/2" en **Piedra** a la derecha.
  - **Vida** (y = 84) y **Codice** (y = 144): título en **Piedra** a la izquierda y "80/130" en rojo claro a la
    derecha; 32 px más abajo, la barra de 267×12 (vacía en **Pizarra**, llena en **Peligro**, borde **Fondo** de 2 px).
  - **Mana** (y = 204): igual, con "8/10" en **Gema**, y abajo un rectángulo de 24×12 por cada punto (separados
    3 px: los 10 miden lo mismo que las barras). Llenos en **Gema**, vacíos en **Pizarra**; con una habilidad
    preparada, los que se van a gastar en **Gema oscura**.
  - **Habilidades** (y = 284): encabezado "Habilidades … Mana" en **Tenue**; abajo una por línea (cada 30 px,
    desde y = 318; entran las 3), "[1] Bola de energia" a la izquierda y el costo a la derecha. En **Gema** la
    preparada, en **Tenue** las que no alcanza el maná, en **Piedra** las demás. Se pueden clickear: en el turno del
    jugador, la que está debajo del mouse tiene un fondo **Pizarra** del ancho del panel.
  - **Botón "Terminar turno"** (8 px abajo de la última habilidad): rectángulo del ancho del panel y 32 px de alto,
    relleno **Fondo** con borde de 2 px **Pizarra** y "[Espacio] Terminar turno" centrado en **Tenue**. Con el mouse
    encima, relleno **Pizarra** y texto en **Piedra**.
  - **Qué hacer** (y = 480): en varios renglones (el panel es angosto, se cortan con `\n`), con `setLineSpacing(1.2)`.
    Sin nada elegido, "Click en el mago para moverlo" en **Tenue**; con el mago seleccionado, en **Piedra**; con
    una habilidad preparada, en **Gema** y diciendo con qué tecla se cancela.
  - **"[Esc] Menu"** abajo de todo (y = 676), en **Tenue**, y a la derecha (alineado a x = 1256) el nombre de la
    dificultad, también en **Tenue**.
- **Ataques anunciados:** además del rayado en la casilla, el daño que llega (si le pegan dos, la suma) va
  centrado en la casilla, en rojo claro, tamaño 22 con borde **Fondo**; y un "!" igual arriba de la cabeza de
  cada enemigo que va a atacar (6 px más arriba que los números de daño). Los dibuja `Hud::dibujarAmenazas()`
  con la vista por defecto, para que la letra no se estire con la cámara.
- **Turnos del fuego:** en cada casilla del muro, cuántos turnos de los enemigos le quedan ("3", "2", "1"), en
  **Fuego**, tamaño 22 con borde **Fondo**, 8 px (del mundo) más abajo del centro de la casilla, para no tapar el
  centro de las llamas. Lo dibuja `Hud::dibujarTurnosFuego()`.
- **Aviso del volumen:** al apretar **-** o **=**, "Musica: 7" en **Piedra** con borde **Fondo**, abajo a la
  izquierda (a 16 px de los bordes), en cualquier pantalla. Se ve 1,5 s y se desvanece en el último medio segundo.
  Lo dibuja `Hud::dibujarVolumen()`.
- **Cartel de historia** (antes de cada piso y en "Nivel superado"; lo dibuja `dibujarHistoria()`): mismo velo,
  panel de 760×380 en (260, 170). Título ×2 en **Gema** centrado en (640, 215); un aviso opcional en **Gema**
  centrado en (640, 262) (la habilidad nueva); la historia en **Piedra**, alineada a la izquierda en x = 300, desde
  y = 290, una línea cada 30 px (hasta 6, de hasta 47 letras); y qué tecla apretar en **Tenue**, centrado en
  (640, 515).
- **Carteles** (victoria y derrota; los dibuja `dibujarCartel()`): la pantalla entera se tapa con
  **Fondo** al 75 % de opacidad (alfa 190), y en el medio va un panel de 480×200 en (400, 260), relleno **Fondo**
  con borde **Pizarra** de 2 px. Adentro, centradas en x = 640: el título en y = 310, agrandado ×2 con `setScale`
  (no con tamaño 44, para que siga nítido); el detalle en **Piedra** en y = 370; y qué tecla apretar en **Tenue**
  en y = 420.

  | Cartel | Título | Detalle | Tecla |
  |---|---|---|---|
  | Victoria | "VICTORIA" (**Gema**) | "Sobreviviste a todas las oleadas" | "Enter para volver al menu" |
  | Derrota | "DERROTA" (rojo claro) | Por qué se perdió | "Enter para volver al menu" |
- **Números de daño:** "-10" en rojo claro (`#E8786E`), fuente 22 con borde **Fondo** de 2 px. Aparecen
  arriba de la cabeza, suben 22 px en 0,9 s y en la segunda mitad se desvanecen.
- **Todo el texto del HUD es tamaño 22** y lleva un borde oscuro de 2 px, para leerse sobre cualquier cosa.
- **Cómo se generan:** igual que el arte del mapa, con scripts de Python que no están en el repositorio
  (`generar_arte_habilidad.py` para los marcadores y la bola de energía, `generar_arte_enemigos.py` para la
  bola del espectro, `generar_arte_ayuda.py` para los dibujos de "Como jugar"). El esqueleto arquero y la flecha
  del piso 3 no salieron de esos scripts: se generaron con IA (ver `Imagenes/CREDITOS.txt`).

## Git

- **No subas cambios de `juego_pr2.vcxproj` que hizo Visual Studio solo**, como el "Redestinar proyecto"
  a otra versión. Antes de hacer commit, fijate en los cambios que no los hayas tocado sin querer.
- `build/`, `docs/`, `.vs/` y los `*.user` no se suben (ya están en el `.gitignore`).
