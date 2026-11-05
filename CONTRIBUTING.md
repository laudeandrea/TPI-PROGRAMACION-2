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
  usa (como `TipoTerreno` en `Casilla.h`), declarados antes de la clase.
- **Atributos privados.** Si otra clase necesita un dato, se le da un método público (`get...`).
- **Excepción, clases base:** en las clases de las que heredan otras (como `Personaje`), los atributos
  que necesitan las hijas van en `protected`, no en `private`. Con `private`, `Mago` no podría usar la
  `vida` ni la posición aunque herede de `Personaje`. Para las demás clases, `protected` sigue siendo
  como `private`: nadie de afuera los ve.
- **Herencia:** `Mago` y `Enemigo` son hijas de `Personaje`. Lo que comparten (posición, vida, dibujarse)
  va en `Personaje`; lo propio de cada una (el maná del mago, el ataque del enemigo), en la hija.
- **Métodos privados** para lo que solo se usa dentro de la clase (como `procesarEventos()` en `Juego`).
- **`main.cpp` no tiene lógica:** solo crea el `Juego` y lo ejecuta.

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
- **Comentarios:** que expliquen el *por qué*, no lo que ya se lee en el código. Los métodos públicos
  importantes se documentan en el `.h` con formato Doxygen (`/** @brief ... */`).

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

C++ crea los atributos en el orden en que están declarados en el `.h`. La `Camara` usa el `Mapa` al
crearse, así que **`mapa` tiene que estar declarado antes que `camara`**. Si se invierten, compila igual,
pero la cámara le pide el tamaño a un mapa que todavía no existe.

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
Vista previa: [Imagenes/vista_previa_mapa.png](Imagenes/vista_previa_mapa.png) (1280×720, escala ×2).

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
- **Cómo se generan:** todas estas imágenes salen de [herramientas/generar_arte_mapa.py](herramientas/generar_arte_mapa.py).
  Para cambiar algo (un color, una variante nueva), se edita el script y se corre desde la raíz del repo:
  `python herramientas/generar_arte_mapa.py` (hace falta `pip install pillow`). Deja los PNG en `salida_arte/`
  (ignorada por git); se revisan y después se copian a `Imagenes/`. No retoques los PNG a mano: la próxima
  vez que alguien corra el script se pierde el retoque.

### Habilidades y HUD

| Archivo | Lienzo | Qué es | Origen |
|---|---|---|---|
| `marcador_movimiento.png` | 64×40 | Rombo **Piedra** transparente: casilla a la que el mago puede caminar (después de clickearlo). | `(32, 20)` |
| `marcador_alcance.png` | 64×40 | Rombo **Gema** transparente: casilla a la que llega la habilidad. | Centro del rombo, `(32, 20)` |
| `marcador_objetivo.png` | 64×40 | Igual, en rojo (**Peligro**): hay un enemigo al alcance. | `(32, 20)` |
| `bola_energia.png` | 32×16 | Bola de energía: **2 cuadros de 16×16** uno al lado del otro, se alternan cada 0,1 s. | Centro del cuadro, `(8, 8)` |

- **Marcadores:** son 1 px más chicos que el rombo de la baldosa, para que la junta oscura se siga viendo,
  y el relleno es tramado y casi transparente, para que se vea el piso. Se dibujan después del mapa y antes
  que los personajes. En el abismo solo se marca si hay un enemigo (el espectro vuela).
- **La bola** sale del pecho del mago y pega en el pecho del enemigo: **30 px arriba de los pies**
  (`ALTURA_PECHO` en `Juego.h`).
- **Barra de vida de los enemigos:** 32×4 px (del mundo), 6 px arriba de la imagen del personaje.
  Vacía en **Pizarra**, llena en **Peligro**, con un borde de 1 px color **Fondo**.
- **HUD de maná** (fijo en la pantalla, abajo a la izquierda, en (32, 612)): título "Mana" en **Piedra**,
  un rectángulo de 16×12 por cada punto (lleno en **Gema**, vacío en **Pizarra**; con la Q apretada, los
  puntos que se van a gastar en **Gema oscura**) y el número "8/10" en **Gema**. Abajo, la ayuda de la
  habilidad: **Piedra** normal, **Gema** con la Q apretada, **Tenue** si no alcanza el maná.
- **Números de daño:** "-10" en rojo claro (`#E8786E`), fuente 22 con borde **Fondo** de 2 px. Aparecen
  arriba de la cabeza, suben 22 px en 0,9 s y en la segunda mitad se desvanecen.
- **Todo el texto del HUD es tamaño 22** y lleva un borde oscuro de 2 px, para leerse sobre cualquier cosa.
- **Cómo se generan:** [herramientas/generar_arte_habilidad.py](herramientas/generar_arte_habilidad.py),
  igual que el arte del mapa: `python herramientas/generar_arte_habilidad.py`.

## Git

- **No subas cambios de `juego_pr2.vcxproj` que hizo Visual Studio solo**, como el "Redestinar proyecto"
  a otra versión. Antes de hacer commit, fijate en los cambios que no los hayas tocado sin querer.
- `build/`, `docs/`, `.vs/` y los `*.user` no se suben (ya están en el `.gitignore`).
