# Genera el arte del tablero ("la cripta flota sobre el vacio"):
# baldosas de suelo, columna rota, abismo, paredes (roca y pozo), fondo, sombra y la vista previa.
#
# Uso (desde la raiz del repo):
#     pip install pillow
#     python herramientas/generar_arte_mapa.py [carpeta_salida]
#
# Deja los PNG en la carpeta que le pases (por defecto salida_arte/, ignorada por git). No pisa
# Imagenes/: primero se revisan y despues se copian a mano. Los colores salen de la Paleta de
# CONTRIBUTING.md.
#
# PERSPECTIVA: la define ALTO_ROMBO. Con 32 el rombo es 2:1 (isometrico clasico); con 40 se ve
# mas desde arriba. Si se cambia, hay que cambiar tambien ALTO_CASILLAS en src/Mapa.h.
import sys, os, random, math
from PIL import Image

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT = sys.argv[1] if len(sys.argv) > 1 else os.path.join(REPO, "salida_arte")
os.makedirs(OUT, exist_ok=True)

# ---------------- geometria ----------------
W = 64                  # ancho del rombo (= ANCHO_CASILLAS)
ALTO_ROMBO = 40         # alto del rombo (= ALTO_CASILLAS)
MEDIO = ALTO_ROMBO // 2
ESPACIO_ARRIBA = 30     # lugar libre arriba del rombo para lo que sobresale (columnas)
T_BLOQUE = 14           # grosor de cada bloque (costados)
CY = ESPACIO_ARRIBA + MEDIO                 # centro del rombo desde arriba (50)
ALTO_LIENZO = CY + MEDIO + T_BLOQUE         # 84 -> el centro queda a 34 px del fondo
T_PARED = 44            # cuanto cuelga, como maximo, la roca bajo el borde de la isla
T_POZO = 86             # profundidad de la pared de pozo (tapa el fondo de un pozo de 2x2)
ALTO_PARED = CY + MEDIO + T_BLOQUE + T_PARED
ALTO_POZO = CY + MEDIO + T_POZO + 4

# ---------------- paleta ----------------
FONDO = (11, 13, 26)
GEMA = (95, 224, 232)
GEMA_OSC = (46, 140, 166)
VIOLETA = (92, 64, 150)
VIOLETA_CLARO = (150, 118, 214)
P = [(20, 22, 34), (34, 38, 54), (44, 50, 70), (54, 61, 84), (63, 71, 96), (76, 86, 112), (96, 106, 134)]
GRILLA = (18, 20, 31)
VACIO_HONDO = (12, 10, 24)
VACIO_LUZ = (40, 28, 74)

BAYER = [[0, 8, 2, 10], [12, 4, 14, 6], [3, 11, 1, 9], [15, 7, 13, 5]]


def mezclar(a, b, t):
    t = max(0.0, min(1.0, t))
    return tuple(int(round(a[i] + (b[i] - a[i]) * t)) for i in range(3))


def media_altura(x):
    """Cuantas filas ocupa el rombo arriba (y abajo) del centro en la columna x."""
    xx = x if x < 32 else 63 - x
    return ((xx + 1) * MEDIO + 31) // 32


def en_rombo(x, y):
    if not 0 <= x < W:
        return False
    h = media_altura(x)
    return CY - h <= y <= CY + h - 1


def borde_inferior(x):
    return CY + media_altura(x) - 1


def borde_superior(x):
    return CY - media_altura(x)


def tono_dither(valor, x, y, paleta):
    n = len(paleta) - 1
    v = valor * n
    i = int(v)
    if i >= n:
        return paleta[n]
    return paleta[i + 1] if (v - i) * 16 > BAYER[y % 4][x % 4] + 0.5 else paleta[i]


class Ruido:
    def __init__(self, seed, celda=8):
        self.r = random.Random(seed)
        self.c = celda
        self.g = {}

    def v(self, i, j):
        if (i, j) not in self.g:
            self.g[(i, j)] = self.r.random()
        return self.g[(i, j)]

    def __call__(self, x, y):
        fx, fy = x / self.c, y / self.c
        i, j = int(math.floor(fx)), int(math.floor(fy))
        tx, ty = fx - i, fy - j
        tx, ty = tx * tx * (3 - 2 * tx), ty * ty * (3 - 2 * ty)
        a = self.v(i, j) + (self.v(i + 1, j) - self.v(i, j)) * tx
        b = self.v(i, j + 1) + (self.v(i + 1, j + 1) - self.v(i, j + 1)) * tx
        return a + (b - a) * ty


# =====================================================================
# SUELO: se dibuja visto desde arriba (32x32, 2x2 losas) y se proyecta al rombo
# =====================================================================
S = 32
L = 16   # lado de cada losa
PIEDRA = dict(
    borde=GRILLA, junta=(32, 36, 52), cruce=(16, 18, 28),
    base=[(52, 59, 81), (58, 66, 89), (64, 72, 96)],
    luz=(82, 92, 120), sombra=(42, 48, 67),
    reflejo=(80, 112, 136),
    musgo=[(40, 62, 56), (50, 78, 64), (64, 96, 72)],
    runa=GEMA_OSC,
)


def textura_suelo(variante, seed):
    """Matriz [u][v] de colores: 2x2 losas biseladas (como la opcion B) pero con poco detalle."""
    rnd = random.Random(seed)
    pal = PIEDRA
    t = [[None] * S for _ in range(S)]
    tono = {(i, j): rnd.choice(pal["base"]) for i in range(2) for j in range(2)}
    for u in range(S):
        for v in range(S):
            lu, lv = u % L, v % L
            if u in (0, S - 1) or v in (0, S - 1):
                c = pal["borde"]
            elif lu == L - 1 or lv == L - 1:
                c = pal["cruce"] if (lu == L - 1 and lv == L - 1) else pal["junta"]
            else:
                c = tono[(u // L, v // L)]
                ini_u = 1 if u < L else 0
                ini_v = 1 if v < L else 0
                if lu == ini_u or lv == ini_v:
                    c = pal["luz"]                    # bisel de cada losa: luz atras
                elif lu == L - 2 or lv == L - 2:
                    c = pal["sombra"]                 # y sombra adelante
                elif rnd.random() < 0.025:
                    c = mezclar(c, pal["sombra"], 0.6)
            t[u][v] = c
    # reflejos: como mucho uno por losa, en la mitad de las losas, y suave
    for i in range(2):
        for j in range(2):
            if rnd.random() < 0.5:
                largo = rnd.choice([3, 4])
                u0, v0 = i * L + rnd.randint(4, 7), j * L + rnd.randint(7, 10)
                for k in range(largo):
                    u, v = u0 + k, v0 - k
                    t[u][v] = mezclar(t[u][v], pal["reflejo"], 0.55)
    if variante == "grieta":
        camino = [(4, 6), (7, 9), (10, 11), (13, 13), (16, 16), (19, 18), (22, 21), (24, 24)]
        for (a0, b0), (a1, b1) in zip(camino, camino[1:]):
            n = max(abs(a1 - a0), abs(b1 - b0))
            for k in range(n + 1):
                u = a0 + (a1 - a0) * k // n
                v = b0 + (b1 - b0) * k // n
                t[u][v] = pal["cruce"]
                if t[u][v + 1] not in (pal["borde"], pal["cruce"], pal["junta"]):
                    t[u][v + 1] = pal["luz"]
    elif variante == "musgo":
        for u in range(S):
            for v in range(S):
                dd = math.hypot(u - 3, v - 27)
                junta = u % L == L - 1 or v % L == L - 1 or u in (0, S - 1) or v in (0, S - 1)
                if dd < 12 and (junta or rnd.random() < 0.05) and rnd.random() < 1 - dd / 13:
                    t[u][v] = rnd.choice(pal["musgo"])
    elif variante == "runa":
        cx = cy = 15.5
        for u in range(S):
            for v in range(S):
                dd = math.hypot(u - cx, v - cy)
                trazo = (8.3 < dd < 9.5) or (abs(u - cx) < 0.8 and abs(v - cy) < 4.5)                     or (abs(v - cy) < 0.8 and abs(u - cx) < 4.5)
                if trazo:
                    t[u][v] = mezclar(t[u][v] or pal["base"][1], pal["runa"], 0.5)
    return t


def costados(img, alto=T_BLOQUE):
    """Costados del bloque: piedra tallada en sillares. El izquierdo recibe mas luz."""
    px = img.load()
    for x in range(W):
        izq = x < 32
        xx = x if izq else 63 - x
        y0 = borde_inferior(x) + 1
        base = P[3] if izq else P[1]
        osc = P[2] if izq else P[0]
        for d in range(alto):
            y = y0 + d
            hilada, dd = d // 7, d % 7
            col = base
            if dd == 6 or (xx + (0 if hilada % 2 == 0 else 11)) % 22 == 21:
                col = osc
            col = mezclar(col, P[0], d / alto * 0.35)
            if d == 0:
                col = P[5] if izq else P[3]
            if x in (0, 63) or d == alto - 1:
                col = GRILLA
            px[x, y] = col + (255,)


def baldosa_suelo(variante, seed):
    img = Image.new("RGBA", (W, ALTO_LIENZO), (0, 0, 0, 0))
    costados(img)
    tex = textura_suelo(variante, seed)
    px = img.load()
    arriba = CY - MEDIO
    for x in range(W):
        for y in range(borde_superior(x), borde_inferior(x) + 1):
            X = x + 0.5 - 32
            Y = (y + 0.5 - arriba) * 64 / ALTO_ROMBO
            u = min(S - 1, max(0, int((X + Y) / 2)))
            v = min(S - 1, max(0, int((Y - X) / 2)))
            px[x, y] = tex[u][v] + (255,)
        # contorno continuo (la proyeccion a veces se saltea un pixel del borde)
        px[x, borde_superior(x)] = GRILLA + (255,)
        px[x, borde_inferior(x)] = GRILLA + (255,)
    for y in range(ALTO_LIENZO):
        for x in (0, 63):
            if en_rombo(x, y):
                px[x, y] = GRILLA + (255,)
    return img


# =====================================================================
# PAREDES
# =====================================================================
def pared_roca(lado, seed):
    """Roca que cuelga bajo el borde de la isla (cuando adelante esta FUERA del tablero)."""
    rnd = random.Random(seed)
    rz = Ruido(seed, 5)
    img = Image.new("RGBA", (W, ALTO_PARED), (0, 0, 0, 0))
    px = img.load()
    bultos = [(4, 16, 7), (15, 34, 6), (24, 22, 5), (30, 12, 4)]
    perfil = []
    for xx in range(32):
        h = 6
        for c, prof, ancho in bultos:
            h = max(h, prof - abs(xx - c) * prof / (ancho * 1.6))
        h += rnd.choice([0, 0, 1, -1])
        perfil.append(int(h * T_PARED / 40) // 2 * 2)
    perfil[0] = perfil[31] = 12
    for x in range(W):
        izq = x < 32
        if (lado == "izq") != izq:
            continue
        xx = x if izq else 63 - x
        y0 = borde_inferior(x) + 1
        largo = min(T_BLOQUE + perfil[xx], ALTO_PARED - y0)
        for d in range(T_BLOQUE, largo):
            y = y0 + d
            n = rz(xx * 2, d * 2)
            col = tono_dither(0.15 + 0.5 * n, x, y, [P[0], P[1], P[2]] if izq else [P[0], P[0], P[1]])
            if rnd.random() < 0.04:
                col = P[0]
            prof = (d - T_BLOQUE) / max(1, largo - T_BLOQUE)
            col = mezclar(col, FONDO, prof * 0.55)
            if d == largo - 1:
                col = mezclar(GRILLA, VIOLETA, 0.35)
            elif d >= largo - 3:
                col = mezclar(col, VIOLETA, 0.55)
            if x in (0, 63):
                col = GRILLA
            px[x, y] = col + (255,)
    return img


def pared_pozo(lado):
    """Pared de adentro de un pozo (cuando adelante hay ABISMO): piedra que baja y se pierde."""
    img = Image.new("RGBA", (W, ALTO_POZO), (0, 0, 0, 0))
    px = img.load()
    for x in range(W):
        izq = x < 32
        if (lado == "izq") != izq:
            continue
        xx = x if izq else 63 - x
        y0 = borde_inferior(x) + 1
        base = P[3] if izq else P[1]
        luz = P[4] if izq else P[2]
        osc = P[2] if izq else P[0]
        for d in range(T_POZO):
            y = y0 + d
            if y >= ALTO_POZO:
                break
            hilada, dd = d // 7, d % 7
            col = base
            if dd == 6 or (xx + (0 if hilada % 2 == 0 else 11)) % 22 == 21:
                col = osc
            elif dd == 0:
                col = luz
            if d == 0:
                col = P[5] if izq else P[3]
            t = (d - 8) / 40
            if t > 0 and t * 16 >= BAYER[y % 4][x % 4]:
                col = mezclar(col, VACIO_HONDO, min(1, t * 1.15))
            if d > 50:
                g = (d - 50) / (T_POZO - 50)
                col = VACIO_HONDO if g * 16 < BAYER[y % 4][x % 4] else mezclar(VACIO_HONDO, VACIO_LUZ, g)
            px[x, y] = col + (255,)
        if x in (31, 32):  # esquina del pozo, donde se juntan las dos paredes
            for d in range(0, 44):
                if y0 + d < ALTO_POZO:
                    px[x, y0 + d] = mezclar(px[x, y0 + d][:3], P[0], 0.5) + (255,)
    return img


# =====================================================================
# ABISMO, COLUMNA, SOMBRA, FONDO
# =====================================================================
def abismo(seed):
    """Casi transparente: lo que se ve son las paredes de pozo de las casillas de atras."""
    rnd = random.Random(seed)
    img = Image.new("RGBA", (W, ALTO_LIENZO), (0, 0, 0, 0))
    px = img.load()
    puestas = 0
    while puestas < 6:
        x, y = rnd.randint(4, 59), rnd.randint(CY - MEDIO + 3, CY + MEDIO - 3)
        if all(en_rombo(x + dx, y + dy) for dx, dy in ((-2, 0), (2, 0), (0, -2), (0, 2))):
            px[x, y] = rnd.choice([VIOLETA_CLARO, VIOLETA_CLARO, (196, 176, 250), GEMA_OSC]) + (255,)
            if rnd.random() < 0.5:
                px[x, y + 1] = VIOLETA + (200,)
            puestas += 1
    return img


def columna_rota(seed):
    """Columna rota sobre un pedestal cuadrado grande (el diseno que mas gusto)."""
    img = baldosa_suelo("lisa", seed + 500)
    px = img.load()

    def fila_rombito(y, cy, hw, hh, cx):
        r = y - (cy - hh)
        if r < 0 or r >= 2 * hh:
            return None
        k = hw / hh
        half = int(k * (r + 1)) if r < hh else int(k * (2 * hh - r))
        return cx - half, cx + half - 1

    # sombra en el piso (hacia adelante-derecha, del lado sin luz)
    for y in range(CY - 8, CY + 16):
        for x in range(26, 62):
            if en_rombo(x, y) and ((x - 42) / 14) ** 2 + ((y - (CY + 6)) / 7) ** 2 < 1:
                r, g, b, _ = px[x, y]
                if (r, g, b) != GRILLA:
                    px[x, y] = (int(r * 0.6), int(g * 0.6), int(b * 0.68), 255)
    # pedestal: cubito de 28x14 y 5 de alto
    cx0, cy0, hw, hh, alto = 32, CY + 2, 14, 7, 5
    for y in range(cy0 - hh, cy0 + hh):
        f = fila_rombito(y, cy0, hw, hh, cx0)
        if f:
            for x in range(f[0], f[1] + 1):
                px[x, y - alto] = (150, 145, 136, 255)
    for x in range(cx0 - hw, cx0 + hw):
        kk = x - (cx0 - hw) if x < cx0 else (cx0 + hw - 1) - x
        yb = cy0 + kk // 2
        for d in range(alto + 1):
            c = (132, 127, 120) if x < cx0 else (92, 90, 92)
            if d == alto:
                c = GRILLA
            px[x, yb - alto + d] = c + (255,)
    # fuste roto
    cx, base, rad = 32, CY + 1, 9
    tonos = [(118, 114, 108), (160, 154, 142), (186, 180, 166), (196, 190, 176), (180, 174, 162),
             (156, 150, 140), (134, 129, 122), (112, 109, 106), (94, 92, 94), (80, 78, 84)]
    alturas = [22, 23, 24, 25, 25, 24, 23, 20, 17, 16, 18, 20, 21, 20, 18, 17, 16, 16, 15]
    for i in range(-rad, rad + 1):
        x = cx + i
        col = tonos[int((i + rad) / (2 * rad + 1) * len(tonos))]
        if (i + rad) % 4 == 1 and abs(i) < rad:
            col = mezclar(col, (54, 54, 66), 0.35)   # estrias
        curva = int(round(math.sqrt(max(0, rad * rad - i * i)) / 2))
        ybase = base - 4 + curva
        ytop = ybase - alturas[i + rad] - curva + 2
        for y in range(ytop, ybase + 1):
            px[x, y] = col + (255,)
        px[x, ytop] = (214, 208, 194, 255)
        px[x, ytop + 1] = mezclar((214, 208, 194), col, 0.5) + (255,)
        px[x, ytop - 1] = GRILLA + (255,)
    for y in range(ALTO_LIENZO):
        for x in (cx - rad - 1, cx + rad + 1):
            v = px[x + (1 if x < cx else -1), y]
            if v[3] and v[:3] != GRILLA and y < base - 4 and v[0] > 100:
                px[x, y] = GRILLA + (255,)
    # cascotes
    for dx, dy, c in [(0, 0, (186, 180, 166)), (1, 0, (160, 154, 142)), (2, 0, (134, 129, 122)),
                      (0, 1, (134, 129, 122)), (1, 1, (110, 106, 102)), (2, 1, (90, 88, 90)), (3, 1, GRILLA),
                      (0, 2, GRILLA), (1, 2, GRILLA), (2, 2, GRILLA)]:
        px[47 + dx, CY + 4 + dy] = c + (255,)
    for dx, dy, c in [(0, 0, (160, 154, 142)), (1, 0, (110, 106, 102)), (0, 1, GRILLA), (1, 1, GRILLA)]:
        px[14 + dx, CY + 2 + dy] = c + (255,)
    return img


def sombra():
    img = Image.new("RGBA", (28, 10), (0, 0, 0, 0))
    px = img.load()
    for y in range(10):
        for x in range(28):
            r = ((x - 13.5) / 13) ** 2 + ((y - 4.5) / 4.5) ** 2
            if r < 1:
                px[x, y] = (6, 6, 14, 110 if r < 0.55 else 60)
    return img


def fondo():
    """Fondo de la pantalla de juego (640x360, se guarda agrandado x2 a 1280x720)."""
    w, h = 640, 360
    img = Image.new("RGBA", (w, h))
    px = img.load()
    rz = Ruido(11, 40)
    rnd = random.Random(12)
    pal = [FONDO, (18, 16, 36), (30, 22, 54), (46, 32, 78)]
    for y in range(h):
        for x in range(w):
            t = (y / h) ** 1.6
            v = t * 0.75 + rz(x, y * 2.5) * 0.5 * t
            px[x, y] = tono_dither(min(0.99, v), x, y, pal) + (255,)
    for y in range(h):
        for x in range(w):
            dx, dy = (x - w / 2) / (w / 2), (y - h / 2) / (h / 2)
            r = dx * dx + dy * dy
            if r > 0.7 and (r - 0.7) * 20 > BAYER[y % 4][x % 4]:
                px[x, y] = mezclar(px[x, y][:3], FONDO, 0.6) + (255,)
    for _ in range(70):
        x, y = rnd.randint(0, w - 1), rnd.randint(int(h * 0.3), h - 1)
        px[x, y] = rnd.choice([VIOLETA, VIOLETA_CLARO, (60, 50, 100)]) + (255,)
    return img


# =====================================================================
# GENERAR
# =====================================================================
piezas = {
    "suelo_1_iso": baldosa_suelo("lisa", 1),
    "suelo_2_iso": baldosa_suelo("grieta", 2),
    "suelo_3_iso": baldosa_suelo("musgo", 3),
    "suelo_4_iso": baldosa_suelo("runa", 4),
    "abismo_iso": abismo(5),
    "columna_rota_iso": columna_rota(6),
    "pared_izquierda_iso": pared_roca("izq", 7),
    "pared_derecha_iso": pared_roca("der", 7),
    "pozo_izquierda_iso": pared_pozo("izq"),
    "pozo_derecha_iso": pared_pozo("der"),
    "sombra_personaje": sombra(),
}
for n, im in piezas.items():
    im.save(os.path.join(OUT, f"{n}.png"))
fondo_img = fondo().resize((1280, 720), Image.NEAREST)
fondo_img.save(os.path.join(OUT, "fondo_juego.png"))


# =====================================================================
# VISTA PREVIA: imita al juego (fondo fijo + camara que encuadra tablero + margen de 64)
# =====================================================================
def cargar(n):
    return Image.open(os.path.join(REPO, "Imagenes", f"{n}.png")).convert("RGBA")


def vista_previa(disenio, nombre, personajes):
    N = 8
    margen = 64
    ancho_mundo, alto_mundo = N * W, N * ALTO_ROMBO
    vis_h = alto_mundo + 2 * margen
    vis_w = max(ancho_mundo + 2 * margen, vis_h * 16 / 9)
    vis_h = vis_w * 9 / 16 if vis_w * 9 / 16 > vis_h else vis_h
    vis_w, vis_h = int(round(vis_w)), int(round(vis_h))
    ox = (vis_w - ancho_mundo) // 2          # corrimiento del mundo dentro de la vista
    oy = (vis_h - alto_mundo) // 2
    capa = Image.new("RGBA", (vis_w, vis_h + 200), (0, 0, 0, 0))

    def c2m(f, c):  # igual que Mapa::casillaAmundo
        return (c - f) * W // 2 + N * W // 2 + ox, (c + f) * MEDIO + MEDIO + oy

    rnd = random.Random(7)
    var = [[rnd.choices([1, 2, 3, 4], [60, 15, 15, 10])[0] for _ in range(N)] for _ in range(N)]
    pj = {(f, c): n for n, f, c in personajes}

    def poner(im, x, y):
        capa.alpha_composite(im, (x - 32, y - CY))

    for f in range(N):
        for c in range(N):
            x, y = c2m(f, c)
            t = disenio[f][c]
            if t != "X":
                if f + 1 >= N:
                    poner(piezas["pared_izquierda_iso"], x, y)
                elif disenio[f + 1][c] == "X":
                    poner(piezas["pozo_izquierda_iso"], x, y)
                if c + 1 >= N:
                    poner(piezas["pared_derecha_iso"], x, y)
                elif disenio[f][c + 1] == "X":
                    poner(piezas["pozo_derecha_iso"], x, y)
            tile = (piezas["abismo_iso"] if t == "X" else
                    piezas["columna_rota_iso"] if t == "C" else
                    piezas[f"suelo_{var[f][c]}_iso"])
            poner(tile, x, y)
        for c in range(N):
            if (f, c) in pj:
                n = pj[(f, c)]
                im = cargar(n)
                x, y = c2m(f, c)
                if n.startswith("pergamino"):
                    capa.alpha_composite(im, (x - im.width // 2, y - (im.height - 34)))
                else:
                    capa.alpha_composite(piezas["sombra_personaje"], (x - 14, y - 5))
                    capa.alpha_composite(im, (x - im.width // 2, y - im.height + 2))
    capa = capa.crop((0, 0, vis_w, vis_h)).resize((1280, 720), Image.NEAREST)
    out = fondo_img.copy()
    out.alpha_composite(capa)
    out.convert("RGB").save(os.path.join(OUT, f"{nombre}.png"))


PJ = [("mago_64", 4, 1), ("pergamino_codice_iso", 5, 0), ("caballero_64", 4, 6),
      ("espectro_64", 1, 3), ("golem_64", 1, 5)]
MAPA = ["SSSSSSSS", "SSSSSSSS", "SSCSXXSS", "SSSSXXSS",
        "SSSSSCSS", "SSXXSSSS", "SSXXSSCS", "SSSSSSSS"]
vista_previa(MAPA, "vista_previa_mapa", PJ)
print("ok")
