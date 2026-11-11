# Genera el arte de los ataques de los enemigos:
#   bola_espectro.png   32x16  bola de ectoplasma que tira el espectro, 2 cuadros de 16x16 (para animarla)
#
# Mismo formato que bola_energia.png, asi la clase Proyectil la dibuja igual.
# Es violeta (los colores del Vacio) para que no se confunda con la bola celeste del mago.
#
# Uso:  python herramientas/generar_arte_enemigos.py [carpeta_salida]
# (por defecto escribe en salida_arte/; despues se copian a mano a Imagenes/)
import os
import sys
from PIL import Image

SALIDA = sys.argv[1] if len(sys.argv) > 1 else "salida_arte"

# Paleta del juego (ver CONTRIBUTING.md)
FONDO = (11, 13, 26)
VACIO = (92, 64, 150)
VACIO_CLARO = (150, 118, 214)
VACIO_BRILLO = (222, 208, 255)   # el centro: casi blanco, apenas violeta
PELIGRO_CLARO = (232, 120, 110)


def bola(cuadro):
    img = Image.new("RGBA", (16, 16), (0, 0, 0, 0))
    px = img.load()
    cx, cy = 7.5, 7.5
    for y in range(16):
        for x in range(16):
            d = ((x - cx) ** 2 + (y - cy) ** 2) ** 0.5
            if d < 2.0:
                px[x, y] = VACIO_BRILLO + (255,)
            elif d < 3.4:
                px[x, y] = VACIO_CLARO + (255,)
            elif d < 4.8:
                px[x, y] = VACIO + (240,)
            elif d < 6.2 and (x + y + cuadro) % 2 == 0:   # halo tramado: cambia entre cuadros
                px[x, y] = VACIO_CLARO + (110,)
    # Brillo arriba a la izquierda (la luz de toda la escena viene de ahi)
    px[6, 5] = (255, 255, 255, 255)
    # Ojitos del fantasma: dos puntos oscuros que lo distinguen de la bola del mago
    px[6, 8] = FONDO + (255,)
    px[9, 8] = FONDO + (255,)
    # Jirones rojizos que se desprenden: en cada cuadro estan en otro lugar
    jirones = [(1, 4), (14, 9), (4, 14)] if cuadro == 0 else [(3, 1), (13, 13), (0, 9)]
    for x, y in jirones:
        px[x, y] = PELIGRO_CLARO + (200,)
    return img


def main():
    os.makedirs(SALIDA, exist_ok=True)
    hoja = Image.new("RGBA", (32, 16), (0, 0, 0, 0))
    hoja.alpha_composite(bola(0), (0, 0))
    hoja.alpha_composite(bola(1), (16, 0))
    hoja.save(os.path.join(SALIDA, "bola_espectro.png"))
    print("Listo:", os.path.abspath(SALIDA))


if __name__ == "__main__":
    main()
