"""The barmaid's portrait for her dialogue card (Keen Worlds art).

    python3 tools/kw_barmaid_portrait.py preview OUT.png   # beside the elder
    python3 tools/kw_barmaid_portrait.py csource OUT.h     # C data

A 48x64 head-and-shoulders picture in the style of the game's own dialogue
pictures (PIC_ORACLE, PIC_KEENTALK1): black outlines, dithered shading. The
background is transparent so the card's white shows through. Stored as 3x4
masked tiles in Keen 4 tile slots; the engine draws it on the card.
"""
import os, sys
from PIL import Image

HERE = os.path.dirname(__file__)
sys.path.insert(0, HERE)
from kw_gate_art import EGA, tile_bytes, free_slots_in

W, H = 48, 64
EPISODE = '4'
SLOT_RANGE = (300, 599)

BLACK, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN, LGREY = range(8)
DGREY, LBLUE, LGREEN, LCYAN, LRED, LMAGENTA, YELLOW, WHITE = range(8, 16)
T = None


def draw():
    px = [[T] * W for _ in range(H)]

    def put(x, y, c):
        if 0 <= x < W and 0 <= y < H:
            px[y][x] = c

    def ellipse(cx, cy, rx, ry, c, shade=None, dither=False):
        for y in range(cy - ry, cy + ry + 1):
            for x in range(cx - rx, cx + rx + 1):
                if ((x - cx) / (rx + 0.5)) ** 2 + ((y - cy) / (ry + 0.5)) ** 2 <= 1:
                    col = c
                    if shade is not None and x > cx + rx * 0.35 and (not dither or (x + y) % 2):
                        col = shade
                    put(x, y, col)

    def outline():
        filled = [[px[y][x] is not None for x in range(W)] for y in range(H)]
        for y in range(H):
            for x in range(W):
                if filled[y][x]:
                    continue
                if any(0 <= x + dx < W and 0 <= y + dy < H and filled[y + dy][x + dx]
                       for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1))):
                    px[y][x] = BLACK

    # Shoulders and body (drawn first, the head overlaps them).
    for y in range(46, 64):                      # off-shoulder white blouse
        half = min(22, 12 + (y - 46) * 2)
        for x in range(24 - half, 24 + half):
            edge = x < 24 - half + 3 or x >= 24 + half - 3
            put(x, y, LGREY if edge and (x + y) % 2 else WHITE)
    for y in range(51, 64):                      # red bodice
        half = min(18, 9 + (y - 51))
        for x in range(24 - half, 24 + half):
            put(x, y, RED)
    ellipse(17, 57, 7, 5, LRED, RED, dither=True)    # curves, softly lit
    ellipse(30, 57, 7, 5, LRED, RED, dither=True)
    for y in range(52, 64, 3):                   # lacing
        put(23, y, BLACK); put(24, y + 1, BLACK)
    for x in range(10, 38):                      # blouse trim
        put(x, 51 if 14 <= x <= 33 else 50, WHITE)
    # Neck and the skin above the bodice.
    for y in range(38, 51):
        for x in range(19, 29):
            put(x, y, WHITE if x < 26 or (x + y) % 2 else LGREY)
    for y in range(44, 51):
        for x in range(14, 34):
            if abs(x - 23.5) < 10 - (y - 44):
                put(x, y, WHITE)
    # Face.
    ellipse(24, 30, 10, 12, WHITE, LGREY, dither=True)
    # Hair: a big blonde updo with a bun, and curls framing the face.
    ellipse(24, 7, 6, 5, YELLOW, BROWN, dither=True)        # bun
    ellipse(24, 15, 14, 7, YELLOW, BROWN, dither=True)
    for y in range(15, 21):                      # fringe over the forehead
        for x in range(13, 36):
            if y < 16 + (abs(x - 24) // 3):
                put(x, y, YELLOW if (x + y) % 3 else BROWN)
    ellipse(12, 33, 3, 9, YELLOW, BROWN, dither=True)      # side curls
    ellipse(36, 33, 3, 9, YELLOW, BROWN, dither=True)
    put(24, 10, WHITE); put(23, 11, WHITE)                  # shine in the bun
    outline()
    # Eyes: lashes, white, blue iris.
    for ex in (19, 29):
        for x in range(ex - 2, ex + 3):
            put(x, 27, BLACK)
        put(ex - 2, 26, BLACK); put(ex + 2, 26, BLACK)
        put(ex - 1, 28, WHITE); put(ex, 28, LBLUE); put(ex + 1, 28, LBLUE)
        put(ex, 29, BLUE); put(ex + 1, 29, LBLUE)
        put(ex - 1, 29, WHITE)
        for x in range(ex - 2, ex + 2):                     # brows
            put(x, 24, BROWN)
    # Nose, cheeks and a smile.
    put(24, 31, LGREY); put(24, 32, LGREY); put(25, 33, LGREY)
    for (x, y) in ((16, 33), (17, 34), (32, 33), (31, 34)):
        put(x, y, LMAGENTA)
    for x in range(21, 28):
        put(x, 37, RED)
    put(20, 36, RED); put(28, 36, RED)
    for x in range(22, 27):
        put(x, 38, LRED)
    # A beauty mark.
    put(30, 36, BROWN)
    return px


def to_image(px):
    img = Image.new('RGBA', (W, H), (0, 0, 0, 0))
    for y in range(H):
        for x in range(W):
            if px[y][x] is not None:
                img.putpixel((x, y), EGA[px[y][x]] + (255,))
    return img


def preview(out):
    import keenrender, struct
    d = os.path.join(HERE, '..', 'play')
    sp = keenrender.Sprites(d, EPISODE)
    info = struct.unpack('<23H', open(os.path.join(d, 'GFXINFOE.CK4'), 'rb').read()[:46])
    raw = sp._chunk(info[18])
    w, h = struct.unpack_from('<HH', raw, (110 - info[15]) * 4)
    data = sp._chunk(110)
    oracle = Image.new('RGB', (w * 8, h))
    for y in range(h):
        for x in range(w * 8):
            b, bit = y * w + x // 8, 7 - x % 8
            oracle.putpixel((x, y), EGA[sum(((data[w * h * q + b] >> bit) & 1) << q for q in range(4))])
    card = Image.new('RGBA', (W * 2 + 16, H), (255, 255, 255, 255))
    card.paste(oracle, (0, 0))
    card.alpha_composite(to_image(draw()), (W + 16, 0))
    card.convert('RGB').resize((card.width * 4, card.height * 4), Image.NEAREST).save(out)
    print('wrote', out)


def csource(out):
    tw, th = W // 16, H // 16
    slots = free_slots_in(EPISODE, tw * th, *SLOT_RANGE)
    px = draw()
    lines = ['/* Generated by tools/kw_barmaid_portrait.py -- do not edit by hand. */',
             '#define KW_BARMAIDFACE_TILES_W %d' % tw, '#define KW_BARMAIDFACE_TILES_H %d' % th,
             'static const uint16_t kw_barmaidfaceSlots[1][%d][%d] = {' % (th, tw),
             '\t{' + ', '.join('{' + ', '.join(str(slots[r * tw + c]) for c in range(tw)) + '}' for r in range(th)) + '},',
             '};', 'static const uint8_t kw_barmaidfaceTiles[1][%d][%d][160] = {' % (th, tw), '\t{']
    for r in range(th):
        lines.append('\t\t{')
        for c in range(tw):
            lines.append('\t\t\t{' + ','.join('0x%02x' % v for v in tile_bytes(px, c, r)) + '},')
        lines.append('\t\t},')
    lines += ['\t},', '};']
    open(out, 'w').write('\n'.join(lines) + '\n')
    print('wrote', out)


if __name__ == '__main__':
    {'preview': preview, 'csource': csource}[sys.argv[1]](sys.argv[2])
