# Pautas del proyecto

Reglas para que el código y el arte encajen entre todos. Si algo no está claro o no estás de acuerdo con
alguna, hablémoslo antes de cambiarla.

Para entender cómo está armado el código, primero leé la sección "Cómo funciona el código" del
[README](README.md).

## Programación orientada a objetos

- **Nada de funciones globales.** Todo va en una clase. Si una función no encaja en ninguna clase
  existente, probablemente haga falta una clase nueva.
- **Cada clase tiene su `.h` y su `.cpp`** en `src/`. El `.h` tiene la declaración y el `.cpp` la
  implementación. Excepción: los `enum` (como `TipoTerreno`) solo necesitan `.h`.
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

- **Las imágenes van en `Imagenes/`** y se cargan con ruta relativa: `"Imagenes/suelo.png"`.
- **PNG con fondo transparente.**
- **Nombres:** las baldosas isométricas terminan en `_iso.png` (`suelo_iso.png`). Las sin `_iso` son las
  viejas de vista de arriba: no las pisen.

### Baldosas isométricas

Todas las baldosas usan **el mismo lienzo y el cubo en el mismo lugar**. Si una se corre un píxel, el
tablero se ve roto.

```
y = 0    ┌──────────┐
         │          │  espacio libre (30 px) para lo que sobresale hacia arriba
         │          │  (la columna rota, por ejemplo)
y = 30   │    /\    │  ← vértice de arriba del rombo
y = 46   │  < ● >   │  ← centro del rombo = vértices izquierdo y derecho
y = 62   │  |\/ |   │  ← vértice de abajo del rombo
         │  \ | /   │  costados del cubo
y = 79   └──────────┘  ← fondo de la imagen
           64 px
```

- **Lienzo de 64×80**, fondo transparente.
- **Cara de arriba:** rombo de 64×32, con bordes en escalera de **2 píxeles al costado por 1 hacia abajo**.
  Vértice de arriba en y = 30, centro en **y = 46** (34 px desde el fondo de la imagen).
- **Costados:** debajo del rombo, hasta el fondo de la imagen.
- **Lo que sobresale** (columnas, rocas) va hacia arriba, en los 30 px libres. No se puede correr el cubo
  hacia abajo para hacerle lugar.
- **Abismo:** imagen casi transparente, así se ve el fondo y los costados de los cubos vecinos.
- **Misma luz en todas:** la cara de arriba más clara y los dos costados más oscuros, siempre del mismo lado.

## Git

- **No subas cambios de `juego_pr2.vcxproj` que hizo Visual Studio solo**, como el "Redestinar proyecto"
  a otra versión. Antes de hacer commit, fijate en los cambios que no los hayas tocado sin querer.
- `build/`, `docs/`, `.vs/` y los `*.user` no se suben (ya están en el `.gitignore`).
