"""Border Village's bar (after completion) and the closed stall (before).

    python3 tools/kw_bar_art.py preview OUT.png        # both, on the street
    python3 tools/kw_bar_art.py csource-bar OUT.h      # C data: the bar
    python3 tools/kw_bar_art.py csource-stall OUT.h    # C data: the stall

Both stand on the same spot of Keen 4 level 1's street, map tiles 84-87 x
12-15 (a 64x64 canvas), just inside the village when Keen walks in from the
right. Before the village is freed the stall is boarded up; afterwards the
villagers are back and the bar is open.
"""
import os, sys
from PIL import Image

HERE = os.path.dirname(__file__)
sys.path.insert(0, HERE)
from kw_gate_art import EGA, tile_bytes, free_slots_in

W, H = 64, 64
ORIGIN = (84, 12)
EPISODE, LEVEL = '4', 1
BAR_SLOTS = (300, 599)     # Keen 4 tile slots reserved for the bar
STALL_SLOTS = (600, 899)   # ... and for the closed stall

BLACK, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN, LGREY = range(8)
DGREY, LBLUE, LGREEN, LCYAN, LRED, LMAGENTA, YELLOW, WHITE = range(8, 16)
T = None

# A tiny pixel font: 4x5 letters (# = ink).
FONT = {
    'A': ['.##.', '#..#', '####', '#..#', '#..#'],
    'B': ['###.', '#..#', '###.', '#..#', '###.'],
    'C': ['.###', '#...', '#...', '#...', '.###'],
    'D': ['###.', '#..#', '#..#', '#..#', '###.'],
    'E': ['####', '#...', '###.', '#...', '####'],
    'L': ['#...', '#...', '#...', '#...', '####'],
    'O': ['.##.', '#..#', '#..#', '#..#', '.##.'],
    'R': ['###.', '#..#', '###.', '#.#.', '#..#'],
    'S': ['.###', '#...', '.##.', '...#', '###.'],
}


class Canvas:
    def __init__(self, w=W, h=H):
        self.w, self.h = w, h
        self.px = [[T] * w for _ in range(h)]

    def put(self, x, y, c):
        if 0 <= x < self.w and 0 <= y < self.h:
            self.px[y][x] = c

    def rect(self, x0, y0, x1, y1, c):
        for y in range(y0, y1 + 1):
            for x in range(x0, x1 + 1):
                self.put(x, y, c)

    def box(self, x0, y0, x1, y1, fill, edge=BLACK):
        self.rect(x0, y0, x1, y1, edge)
        self.rect(x0 + 1, y0 + 1, x1 - 1, y1 - 1, fill)

    def text(self, cx, y, s, c, scale=1):
        """Draw text centred on x = cx."""
        width = (len(s) * 5 - 1) * scale
        x = cx - width // 2
        for ch in s:
            for gy, row in enumerate(FONT[ch]):
                for gx, cell in enumerate(row):
                    if cell == '#':
                        self.rect(x + gx * scale, y + gy * scale, x + gx * scale + scale - 1, y + gy * scale + scale - 1, c)
            x += 5 * scale


def draw_bar():
    c = Canvas()
    # Posts holding up the awning.
    c.box(3, 14, 6, 63, BROWN)
    c.box(57, 14, 60, 63, BROWN)
    # Back wall in shadow, with a shelf of bottles.
    c.rect(7, 22, 56, 44, DGREY)
    c.box(8, 33, 55, 35, BROWN)
    for i, x in enumerate(range(11, 54, 6)):
        col = [GREEN, LRED, BROWN, LGREEN, LBLUE, YELLOW, GREEN, LRED][i % 8]
        c.rect(x, 26, x + 2, 32, col)
        c.rect(x + 1, 24, x + 1, 25, col)
        c.put(x, 27, WHITE)            # glint
    # Hanging lantern.
    c.put(31, 22, BLACK); c.put(31, 23, BLACK)
    c.box(29, 24, 33, 28, YELLOW)
    c.put(31, 26, WHITE)
    # Counter: a light top and a planked wooden front.
    c.box(5, 44, 58, 47, LGREY)
    c.rect(6, 45, 57, 45, WHITE)
    c.box(7, 47, 56, 63, BROWN)
    for y in (51, 55, 59):
        c.rect(8, y, 55, y, BLACK)
    for x in (18, 32, 46):
        c.rect(x, 48, x, 62, DGREY)
    # A foaming mug and a bottle on the counter.
    c.box(40, 37, 45, 43, YELLOW)
    c.rect(41, 36, 44, 37, WHITE)
    c.rect(46, 39, 47, 41, YELLOW); c.put(47, 40, T)
    c.box(14, 34, 16, 43, GREEN)
    c.rect(15, 32, 15, 33, GREEN)
    # Striped awning with a scalloped edge.
    for x in range(1, 63):
        col = LRED if (x // 6) % 2 == 0 else WHITE
        c.rect(x, 12, x, 19, col)
        if (x % 6) in (1, 2, 3, 4):
            c.put(x, 20, col)
            if (x % 6) in (2, 3):
                c.put(x, 21, col)
    c.rect(1, 11, 62, 11, BLACK)
    # The sign board on top.
    c.box(12, 0, 51, 10, BROWN)
    c.rect(13, 1, 50, 1, YELLOW)        # top highlight
    c.text(32, 3, 'BAR', YELLOW)
    for x in (13, 50):
        c.put(x, 5, BLACK)               # nail heads
    return c.px


def draw_stall():
    c = Canvas()
    # A plain boarded-up shack.
    c.box(4, 14, 59, 63, BROWN)
    for y in range(18, 63, 5):          # plank seams
        c.rect(5, y, 58, y, DGREY)
    for x in (16, 33, 47):
        c.rect(x, 15, x, 62, BLACK)
    # Two boards nailed across.
    for k in range(48):
        c.rect(8 + k, 22 + k * 30 // 48, 9 + k, 23 + k * 30 // 48, LGREY)
        c.rect(8 + k, 52 - k * 30 // 48, 9 + k, 53 - k * 30 // 48, LGREY)
    # Sagging roof.
    c.box(1, 10, 62, 15, DGREY)
    c.rect(2, 11, 61, 11, LGREY)
    # The CLOSED sign.
    c.box(14, 0, 49, 9, WHITE)
    c.text(32, 3, 'CLOSED', RED)
    c.put(15, 1, LGREY); c.put(48, 1, LGREY)
    return c.px


def to_image(px):
    img = Image.new('RGBA', (W, H), (0, 0, 0, 0))
    for y in range(H):
        for x in range(W):
            if px[y][x] is not None:
                img.putpixel((x, y), EGA[px[y][x]] + (255,))
    return img


def preview(out):
    import keenmap, keenrender
    d = os.path.join(HERE, '..', 'play')
    name, w, h, (bg, fg, info) = keenmap.level(d, EPISODE, LEVEL)
    g = keenrender.Graphics(d, EPISODE)
    x0, y0, x1, y1 = 76, 9, 96, 19
    shots = []
    for art in (draw_stall(), draw_bar()):
        img = Image.new('RGBA', ((x1 - x0) * 16, (y1 - y0) * 16), (0, 0, 0, 255))
        for y in range(y0, y1):
            for x in range(x0, x1):
                t = g.tile(bg[y * w + x], False)
                if t: img.paste(t, ((x - x0) * 16, (y - y0) * 16))
                if fg[y * w + x]:
                    t = g.tile(fg[y * w + x], True)
                    if t: img.alpha_composite(t, ((x - x0) * 16, (y - y0) * 16))
        img.alpha_composite(to_image(art), ((ORIGIN[0] - x0) * 16, (ORIGIN[1] - y0) * 16))
        shots.append(img.convert('RGB'))
    sheet = Image.new('RGB', (shots[0].width * 2 + 8, shots[0].height), (40, 40, 40))
    sheet.paste(shots[0], (0, 0)); sheet.paste(shots[1], (shots[0].width + 8, 0))
    sheet.resize((sheet.width * 3, sheet.height * 3), Image.NEAREST).save(out)
    print('wrote', out, '(left: before, right: after)')


def csource(out, name, px, slot_range):
    tw, th = W // 16, H // 16
    slots = free_slots_in(EPISODE, tw * th, *slot_range)
    up = name.upper()
    lines = ['/* Generated by tools/kw_bar_art.py -- do not edit by hand. */',
             '#define KW_%s_TILES_W %d' % (up, tw),
             '#define KW_%s_TILES_H %d' % (up, th),
             '#define KW_%s_ORIGIN_X %d' % (up, ORIGIN[0]),
             '#define KW_%s_ORIGIN_Y %d' % (up, ORIGIN[1]),
             '// Empty, unused Keen 4 masked-tile slots that hold the art: [frame][row][column].',
             'static const uint16_t kw_%sSlots[1][KW_%s_TILES_H][KW_%s_TILES_W] = {' % (name, up, up),
             '\t{' + ', '.join('{' + ', '.join(str(slots[ty * tw + tx]) for tx in range(tw)) + '}' for ty in range(th)) + '},',
             '};',
             '// [frame][row][column][mask + 4 planes]',
             'static const uint8_t kw_%sTiles[1][KW_%s_TILES_H][KW_%s_TILES_W][160] = {' % (name, up, up),
             '\t{']
    for ty in range(th):
        lines.append('\t\t{')
        for tx in range(tw):
            lines.append('\t\t\t{' + ','.join('0x%02x' % v for v in tile_bytes(px, tx, ty)) + '},')
        lines.append('\t\t},')
    lines += ['\t},', '};']
    open(out, 'w').write('\n'.join(lines) + '\n')
    print('wrote', out)


if __name__ == '__main__':
    cmd, out = sys.argv[1], sys.argv[2]
    if cmd == 'preview':
        preview(out)
    elif cmd == 'csource-bar':
        csource(out, 'bar', draw_bar(), BAR_SLOTS)
    elif cmd == 'csource-stall':
        csource(out, 'stall', draw_stall(), STALL_SLOTS)
