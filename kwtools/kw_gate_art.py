"""Draw the Keen 5 boarding gate (Keen Worlds art) and convert it to tiles.

    python3 tools/kw_gate_art.py preview OUT.png      # gate over the real dock
    python3 tools/kw_gate_art.py csource OUT.h        # C data for the engine

The gate covers map tiles x 29-30, y 94-96 on Keen 5's world map (a 32x48
pixel canvas). It is an overlay: transparent pixels keep the original station
rim tiles, which the engine combines with this art at load time. There are
FRAMES animation frames (running lights).
"""
import os, sys
from PIL import Image

sys.path.insert(0, os.path.dirname(__file__))

W, H = 48, 48            # 3 x 3 tiles
ORIGIN = (29, 94)        # top-left map tile
FRAMES = 3

# EGA colour indices.
BLACK, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN, LGREY = range(8)
DGREY, LBLUE, LGREEN, LCYAN, LRED, LMAGENTA, YELLOW, WHITE = range(8, 16)
T = None  # transparent


def draw(frame):
    px = [[T] * W for _ in range(H)]

    def put(x, y, c):
        if 0 <= x < W and 0 <= y < H:
            px[y][x] = c

    def rect(x0, y0, x1, y1, c):
        for y in range(y0, y1 + 1):
            for x in range(x0, x1 + 1):
                put(x, y, c)

    def box(x0, y0, x1, y1, fill, edge=BLACK):
        rect(x0, y0, x1, y1, edge)
        rect(x0 + 1, y0 + 1, x1 - 1, y1 - 1, fill)

    # --- Support truss under the tube (also hides the old gangway knob) ---
    box(6, 33, 30, 40, DGREY)
    for x in range(8, 29, 4):            # diagonal bracing
        for k in range(5):
            put(x + k, 34 + k, BLACK)
            put(x + 4 - k, 34 + k, LGREY)
    rect(7, 34, 29, 34, LGREY)

    # --- Tube: from the station gate to the ship ---
    top, bot = 13, 33
    box(8, top, 28, bot, LGREY)
    rect(9, top + 1, 27, top + 1, WHITE)          # top highlight
    rect(9, top + 12, 27, bot - 1, DGREY)         # lower shading
    rect(9, top + 11, 27, top + 11, LGREY)
    for x in range(14, 27, 6):                    # ribs
        rect(x, top + 2, x, bot - 1, BLACK)
        rect(x + 1, top + 2, x + 1, bot - 1, WHITE)
    # Walkway grating along the middle, where Keen walks.
    for x in range(9, 28):
        put(x, top + 9, BLACK if x % 2 else DGREY)
    # Running lights along the top and bottom: one lamp in three lit,
    # moving towards the ship each frame.
    for i, x in enumerate(range(9, 27, 2)):
        lit = (i % FRAMES) == frame
        for y in (top + 4, bot - 3):
            put(x, y, WHITE if lit else BLUE)
            put(x + 1, y, LCYAN if lit else BLUE)
            put(x, y + 1, LCYAN if lit else BLUE)

    # --- Gate portal on the station side ---
    box(1, 3, 11, 45, LGREY)
    rect(2, 4, 2, 44, WHITE)
    rect(10, 4, 10, 44, DGREY)
    box(3, 1, 9, 4, LGREY)                         # top cap
    box(3, 43, 9, 46, LGREY)                       # bottom cap
    # Energy field in the doorway, pulsing.
    field = [[BLUE, LBLUE, LCYAN], [LBLUE, LCYAN, WHITE], [LCYAN, WHITE, LBLUE]][frame]
    rect(4, 7, 8, 40, BLACK)
    for y in range(8, 40):
        rect(5, y, 7, y, field[(y // 3) % 3])
    # Status lamps.
    put(5, 2, LRED if frame != 1 else RED)
    put(7, 2, LRED if frame == 1 else RED)
    put(5, 45, LGREEN if frame != 2 else GREEN)
    put(7, 45, LGREEN if frame == 2 else GREEN)
    # Yellow-black hazard band on the gate.
    for y in range(5, 43):
        put(9, y, YELLOW if (y // 2) % 2 else BLACK)

    # --- Docking collar: a ring ending at the ship's nose (canvas x 32) ---
    box(27, 10, 32, 36, WHITE)
    rect(29, 12, 29, 34, LCYAN if frame == 0 else CYAN)
    rect(31, 12, 31, 34, LCYAN if frame == 1 else CYAN)
    rect(28, 11, 28, 35, LGREY)
    # Clamp jaws gripping the hull above and below the nose.
    box(26, 8, 37, 11, LGREY)
    box(26, 35, 37, 38, LGREY)
    rect(36, 12, 37, 19, LGREY)
    rect(36, 29, 37, 34, LGREY)
    rect(37, 12, 37, 19, BLACK)
    rect(37, 29, 37, 34, BLACK)
    put(31, 9, LRED if frame == 2 else RED)
    put(31, 36, LRED if frame == 2 else RED)
    return px


EGA = [(0, 0, 0), (0, 0, 170), (0, 170, 0), (0, 170, 170), (170, 0, 0), (170, 0, 170), (170, 85, 0), (170, 170, 170),
       (85, 85, 85), (85, 85, 255), (85, 255, 85), (85, 255, 255), (255, 85, 85), (255, 85, 255), (255, 255, 85), (255, 255, 255)]


def to_image(px):
    img = Image.new('RGBA', (W, H), (0, 0, 0, 0))
    for y in range(H):
        for x in range(W):
            if px[y][x] is not None:
                img.putpixel((x, y), EGA[px[y][x]] + (255,))
    return img


def tile_bytes(px, tx, ty):
    """One masked 16x16 tile: mask plane (1 = transparent) then 4 colour planes."""
    out = bytearray(160)
    for y in range(16):
        for x in range(16):
            c = px[ty * 16 + y][tx * 16 + x]
            byte, bit = y * 2 + x // 8, 7 - x % 8
            if c is None:
                out[byte] |= 1 << bit
                continue
            for plane in range(4):
                if (c >> plane) & 1:
                    out[32 + plane * 32 + byte] |= 1 << bit
    return bytes(out)


def preview(out):
    import keenmap, keenrender
    d = os.path.join(os.path.dirname(__file__), '..', 'play')
    name, w, h, (bg, fg, info) = keenmap.level(d, '5', 0)
    g = keenrender.Graphics(d, '5')
    x0, y0, x1, y1 = 24, 91, 35, 99
    sheet = Image.new('RGB', ((x1 - x0) * 16 * FRAMES + 8 * (FRAMES - 1), (y1 - y0) * 16), (40, 40, 40))
    for f in range(FRAMES):
        img = Image.new('RGBA', ((x1 - x0) * 16, (y1 - y0) * 16), (0, 0, 0, 255))
        for y in range(y0, y1):
            for x in range(x0, x1):
                t = g.tile(bg[y * w + x], False)
                if t: img.paste(t, ((x - x0) * 16, (y - y0) * 16))
                v = fg[y * w + x]
                if v and not (y == 95 and x in (29, 30)):  # the old solid knob is replaced
                    t = g.tile(v, True)
                    if t: img.alpha_composite(t, ((x - x0) * 16, (y - y0) * 16))
        img.alpha_composite(to_image(draw(f)), ((ORIGIN[0] - x0) * 16, (ORIGIN[1] - y0) * 16))
        sheet.paste(img.convert('RGB'), (f * (img.width + 8), 0))
    sheet.resize((sheet.width * 4, sheet.height * 4), Image.NEAREST).save(out)
    print('wrote', out)


def free_slots_in(episode, count, lo, hi, d=None):
    """Masked-tile slots lo..hi with no graphics in EGAGRAPH and unused by any
    of the episode's levels. Keep a range narrow when the slots are animation
    frames, so the offsets between frames fit in an int8. d: the game folder
    (default play/), with standard file names."""
    import struct, keenmap
    d = d or os.path.join(os.path.dirname(__file__), '..', 'play')
    head = open(os.path.join(d, 'EGAHEAD.CK%s' % episode), 'rb').read()
    starts = [int.from_bytes(head[i:i + 3], 'little') for i in range(0, len(head), 3)]
    info = struct.unpack('<23H', open(os.path.join(d, 'GFXINFOE.CK%s' % episode), 'rb').read()[:46])
    n16m, off16m = info[3], info[9]
    used = set()
    for lv in range(30):
        try:
            used |= set(keenmap.level(d, episode, lv)[3][1])
        except Exception:
            pass
    # Also skip tiles that an original foreground animation points at: some
    # chains end on an empty tile (e.g. Keen 4's 1275 -> 1276), and giving
    # such a tile an animation of its own would break the original chain.
    ti = open(os.path.join(d, 'TILEINFO.CK%s' % episode), 'rb').read()
    n16 = info[2]
    anim = lambda t: struct.unpack('b', ti[2 * n16 + 4 * n16m + t:2 * n16 + 4 * n16m + t + 1])[0]
    targets = {t + anim(t) for t in range(n16m) if anim(t)}
    slots = [t for t in range(lo, min(hi, n16m - 1) + 1)
             if starts[off16m + t] == 0xFFFFFF and t not in used and t not in targets]
    assert len(slots) >= count, 'not enough free tile slots'
    return slots[:count]


def free_slots(count):
    """Keen 5 slots for the gate, from 990-1090 so animation offsets fit."""
    return free_slots_in('5', count, 990, 1090)


def csource(out):
    lines = ['/* Generated by tools/kw_gate_art.py -- do not edit by hand. */',
             '#define KW_GATE_FRAMES %d' % FRAMES,
             '#define KW_GATE_TILES_W %d' % (W // 16),
             '#define KW_GATE_TILES_H %d' % (H // 16),
             '#define KW_GATE_ORIGIN_X %d' % ORIGIN[0],
             '#define KW_GATE_ORIGIN_Y %d' % ORIGIN[1],
             ]
    cells = (W // 16) * (H // 16)
    slots = free_slots(FRAMES * cells)
    lines.append('// Empty, unused Keen 5 masked-tile slots that hold the art: [frame][row][column].')
    lines.append('static const uint16_t kw_gateSlots[KW_GATE_FRAMES][KW_GATE_TILES_H][KW_GATE_TILES_W] = {')
    for f in range(FRAMES):
        rows = []
        for ty in range(H // 16):
            rows.append('{' + ', '.join(str(slots[f * cells + ty * (W // 16) + tx]) for tx in range(W // 16)) + '}')
        lines.append('\t{' + ', '.join(rows) + '},')
    lines.append('};')
    lines.append('// [frame][row][column][mask + 4 planes]')
    lines.append('static const uint8_t kw_gateTiles[KW_GATE_FRAMES][KW_GATE_TILES_H][KW_GATE_TILES_W][160] = {')
    for f in range(FRAMES):
        px = draw(f)
        lines.append('\t{')
        for ty in range(H // 16):
            lines.append('\t\t{')
            for tx in range(W // 16):
                b = tile_bytes(px, tx, ty)
                lines.append('\t\t\t{' + ','.join('0x%02x' % v for v in b) + '},')
            lines.append('\t\t},')
        lines.append('\t},')
    lines.append('};')
    open(out, 'w').write('\n'.join(lines) + '\n')
    print('wrote', out)


if __name__ == '__main__':
    {'preview': preview, 'csource': csource}[sys.argv[1]](sys.argv[2])
