# Genera el arte de las habilidades del mago:
#   marcador_alcance.png   64x40  rombo celeste: casillas a las que llega la habilidad
#   marcador_objetivo.png  64x40  rombo rojo: casilla al alcance donde hay un enemigo
#   marcador_movimiento.png 64x40 rombo claro: casilla a la que el mago puede caminar
#   bola_energia.png       32x16  bola de energia, 2 cuadros de 16x16 (para animarla)
#
# Uso:  python herramientas/generar_arte_habilidad.py [carpeta_salida]
# (por defecto escribe en salida_arte/; despues se copian a mano a Imagenes/)
import os
import sys
from PIL import Image

SALIDA = sys.argv[1] if len(sys.argv) > 1 else "salida_arte"

# Paleta del juego (ver CONTRIBUTING.md)
GEMA = (95, 224, 232)
GEMA_OSCURA = (46, 140, 166)
GEMA_CLARA = (214, 252, 255)
PELIGRO = (176, 74, 74)
PELIGRO_CLARO = (232, 120, 110)
PIEDRA = (184, 178, 164)
VACIO_CLARO = (150, 118, 214)

# Rombo de 64x40 (mismo que las baldosas: ANCHO 64, ALTO 40, centro en (32, 20))
W, H, MEDIO = 64, 40, 20


def media_altura(x):
    xx = x if x < 32 else 63 - x
    return ((xx + 1) * MEDIO + 31) // 32


def dentro(x, y, achicar=0):
    """Si el pixel (x, y) esta dentro del rombo achicado 'achicar' pixeles hacia adentro."""
    if x < achicar * 2 or x > 63 - achicar * 2:
        return False
    h = media_altura(x) - achicar
    return MEDIO - h <= y < MEDIO + h


def marcador(color_borde, color_relleno):
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    px = img.load()
    for y in range(H):
        for x in range(W):
            # Se achica 1 px para que la junta oscura de la baldosa siga viendose
            if not dentro(x, y, 1):
                continue
            borde = not dentro(x, y, 2)
            segundo = not dentro(x, y, 3)
            if borde:
                px[x, y] = color_borde + (230,)
            elif segundo:
                px[x, y] = color_borde + (110,)
            elif (x + y) % 2 == 0:      # relleno tramado y transparente: deja ver el piso
                px[x, y] = color_relleno + (46,)
            else:
                px[x, y] = color_relleno + (22,)
    return img


def bola(cuadro):
    img = Image.new("RGBA", (16, 16), (0, 0, 0, 0))
    px = img.load()
    cx, cy = 7.5, 7.5
    for y in range(16):
        for x in range(16):
            d = ((x - cx) ** 2 + (y - cy) ** 2) ** 0.5
            if d < 2.2:
                px[x, y] = GEMA_CLARA + (255,)
            elif d < 3.6:
                px[x, y] = GEMA + (255,)
            elif d < 5.0:
                px[x, y] = GEMA_OSCURA + (235,)
            elif d < 6.4 and (x + y + cuadro) % 2 == 0:   # halo tramado: cambia entre cuadros
                px[x, y] = GEMA + (120,)
    # Brillo arriba a la izquierda (la luz de toda la escena viene de ahi)
    px[6, 5] = (255, 255, 255, 255)
    # Chispas violetas que giran: en cada cuadro estan en otro lugar
    chispas = [(2, 3), (13, 6), (9, 14), (1, 10)] if cuadro == 0 else [(4, 1), (14, 10), (5, 14), (0, 6)]
    for x, y in chispas:
        px[x, y] = VACIO_CLARO + (230,)
    return img


def main():
    os.makedirs(SALIDA, exist_ok=True)
    marcador(GEMA, GEMA).save(os.path.join(SALIDA, "marcador_alcance.png"))
    marcador(PELIGRO_CLARO, PELIGRO).save(os.path.join(SALIDA, "marcador_objetivo.png"))
    marcador(PIEDRA, PIEDRA).save(os.path.join(SALIDA, "marcador_movimiento.png"))
    hoja = Image.new("RGBA", (32, 16), (0, 0, 0, 0))
    hoja.alpha_composite(bola(0), (0, 0))
    hoja.alpha_composite(bola(1), (16, 0))
    hoja.save(os.path.join(SALIDA, "bola_energia.png"))
    print("Listo:", os.path.abspath(SALIDA))


if __name__ == "__main__":
    main()
