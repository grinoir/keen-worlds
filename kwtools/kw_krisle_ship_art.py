"""The repaired Bean-with-Bacon Megarocket on Krisle (Keen Worlds art).

Atroxian Realm's story: the Megarocket is shot down over Krisle, and the
Krile repair it once Atroxus is defeated. When that world is completed, the
ship stands whole again in its "BWB Crash Site" level.

    python3 tools/kw_krisle_ship_art.py preview OUT.png   # the level, before and after
    python3 tools/kw_krisle_ship_art.py csource OUT.h     # C data for the engine

The ship is Keen 4's own Megarocket from its BWB Megarocket level (level 18
of the freely distributable shareware episode in play/): the foreground
tiles 2708-2801 there. They go into masked-tile slots of Atroxian Realm's
graphics that none of its levels use (found from play/mods/arealm).
"""
import os, struct, sys, tempfile
from PIL import Image

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
from kw_gate_art import EGA, tile_bytes
import keenmap, keenrender

PLAY = os.path.join(os.path.dirname(HERE), 'play')
# The ship in Keen 4's level 18: these columns and rows, these tiles only.
K4_X0, K4_Y0, SHIP_TW, SHIP_TH = 4, 8, 15, 6
K4_TILES = range(2700, 2810)
# Where it stands in Atroxian Realm's level 25: over the wreck in the
# background, so the wreck is gone; its lowest row is in the ground Keen
# walks on (as in Keen 4, where the legs sink into the grass).
LEVEL, ORIGIN = 25, (3, 12)
# Keen 4's ground is drawn in the ship's lowest tile row: those colours go.
GROUND_COLOURS = (2, 6, 10, 14)   # green, brown, light green, yellow
# What is left of the wreck above the hull: background cells that get the
# tile of another cell (x, y, (source x, source y)).
CLEAR = [(x, 12, (x, 11)) for x in range(8, 14)]

FILES = {'EGAGRAPH.CKD': 'EGAGRAPH.CKD', 'EGAHEAD.CKD': 'EGAHEAD.CKD', 'EGADICT.CKD': 'EGADICT.CKD',
         'GFXINFOE.CKD': 'GFXINFOE.CKD', 'GAMEMAPS.CKD': 'maptemp.ckd', 'MAPHEAD.CKD': 'maphead.ckd'}


def arealm_view():
    """A temporary folder with Atroxian Realm's files under the standard names."""
    src = os.path.abspath(os.path.join(PLAY, 'mods', 'arealm'))
    d = tempfile.mkdtemp(prefix='kw_arealm_')
    for std, real in FILES.items():
        os.symlink(os.path.join(src, real), os.path.join(d, std))
    return d


def draw_ship():
    """Pixels of the ship (EGA colour index, None = see-through)."""
    g = keenrender.Graphics(PLAY, '4')
    name, w, h, (bg, fg, info) = keenmap.level(PLAY, '4', 18)
    px = [[None] * (SHIP_TW * 16) for _ in range(SHIP_TH * 16)]
    for ty in range(SHIP_TH):
        for tx in range(SHIP_TW):
            tile = fg[(K4_Y0 + ty) * w + K4_X0 + tx]
            if tile not in K4_TILES:
                continue
            img = g.tile(tile, True).convert('RGBA')
            for y in range(16):
                for x in range(16):
                    r, gg, b, a = img.getpixel((x, y))
                    if a:
                        c = EGA.index((r, gg, b))
                        if ty == SHIP_TH - 1 and c in GROUND_COLOURS:
                            continue
                        px[ty * 16 + y][tx * 16 + x] = c
    return px


def to_image(px):
    img = Image.new('RGBA', (len(px[0]), len(px)), (0, 0, 0, 0))
    for y, row in enumerate(px):
        for x, c in enumerate(row):
            if c is not None:
                img.putpixel((x, y), EGA[c] + (255,))
    return img


def used_cells(px):
    """[(row, col)] of the tile cells that have any pixel."""
    return [(r, c) for r in range(SHIP_TH) for c in range(SHIP_TW)
            if any(px[r * 16 + y][c * 16 + x] is not None for y in range(16) for x in range(16))]


def free_slots(count):
    """Masked-tile slots no Atroxian Realm level uses, that no animation
    leads to and that don't animate, from the top of its tile set."""
    d = arealm_view()
    info = struct.unpack('<23H', open(os.path.join(d, 'GFXINFOE.CKD'), 'rb').read()[:46])
    n16, n16m = info[2], info[3]
    used = set()
    for lv in range(33):
        try:
            used |= set(keenmap.level(d, 'D', lv)[3][1])
        except Exception:
            pass
    ti = open(os.path.join(d, 'MAPHEAD.CKD'), 'rb').read()[402:]   # tile properties follow the map header
    anim = lambda t: struct.unpack_from('b', ti, 2 * n16 + 4 * n16m + t)[0]
    targets = {t + anim(t) for t in range(n16m) if anim(t)}
    slots = [t for t in range(n16m - 1, 0, -1) if t not in used and t not in targets and not anim(t)]
    if len(slots) < count:
        sys.exit('only %d unused tile slots, %d needed' % (len(slots), count))
    return sorted(slots[:count])


def level_image(d, clear, ship):
    name, w, h, (bg, fg, info) = keenmap.level(d, 'D', LEVEL)
    bg = list(bg)
    if clear:
        for (cx, cy, src) in CLEAR:
            bg[cy * w + cx] = bg[src[1] * w + src[0]]
    g = keenrender.Graphics(d, 'D')
    x0, y0, tw, th = 0, 6, 26, 14
    img = Image.new('RGBA', (tw * 16, th * 16), (0, 0, 0, 255))
    for y in range(th):
        for x in range(tw):
            t = g.tile(bg[(y0 + y) * w + x0 + x], False)
            if t:
                img.paste(t, (x * 16, y * 16))
            v = fg[(y0 + y) * w + x0 + x]
            if v:
                t = g.tile(v, True)
                if t:
                    img.alpha_composite(t, (x * 16, y * 16))
    if ship:
        img.alpha_composite(to_image(draw_ship()), ((ORIGIN[0] - x0) * 16, (ORIGIN[1] - y0) * 16))
    return img


def preview(out):
    d = arealm_view()
    a, b = level_image(d, False, False), level_image(d, True, True)
    sheet = Image.new('RGB', (a.width, a.height * 2 + 8), (40, 40, 40))
    sheet.paste(a.convert('RGB'), (0, 0))
    sheet.paste(b.convert('RGB'), (0, a.height + 8))
    sheet.resize((sheet.width * 2, sheet.height * 2), Image.NEAREST).save(out)
    print('wrote', out)


def csource(out):
    px = draw_ship()
    # Every cell gets a slot (the engine composes art over the tile under it;
    # an empty cell just shows that tile).
    cells = [(r, c) for r in range(SHIP_TH) for c in range(SHIP_TW)]
    slots = free_slots(len(cells))
    by_cell = dict(zip(cells, slots))
    lines = ['/* Generated by tools/kw_krisle_ship_art.py -- do not edit by hand. */',
             '// The repaired Megarocket on Krisle: Keen 4\'s ship, in masked-tile slots',
             '// no Atroxian Realm level uses.',
             '#define KW_KRISLE_SHIP_LEVEL %d' % LEVEL,
             '#define KW_KRISLE_SHIP_X %d' % ORIGIN[0], '#define KW_KRISLE_SHIP_Y %d' % ORIGIN[1],
             '#define KW_KRISLE_SHIP_W %d' % SHIP_TW, '#define KW_KRISLE_SHIP_H %d' % SHIP_TH,
             'static const uint16_t kw_krisleShipSlots[1][%d][%d] = {' % (SHIP_TH, SHIP_TW)]
    lines.append('\t{' + ', '.join('{' + ', '.join(str(by_cell.get((r, c), 0)) for c in range(SHIP_TW)) + '}'
                                   for r in range(SHIP_TH)) + '},')
    lines.append('};')
    lines.append('static const uint8_t kw_krisleShipTiles[1][%d][%d][160] = {' % (SHIP_TH, SHIP_TW))
    lines.append('\t{')
    for r in range(SHIP_TH):
        lines.append('\t\t{')
        for c in range(SHIP_TW):
            lines.append('\t\t\t{' + ','.join('0x%02x' % v for v in tile_bytes(px, c, r)) + '},')
        lines.append('\t\t},')
    lines.append('\t},')
    lines.append('};')
    lines.append('// Background cells of the wreck: {x, y, x and y of the cell whose tile replaces it}')
    lines.append('static const uint8_t kw_krisleWreckClear[][4] = {')
    for (cx, cy, src) in CLEAR:
        lines.append('\t{%d, %d, %d, %d},' % (cx, cy, src[0], src[1]))
    lines.append('\t{0, 0, 0, 0},\n};')
    open(out, 'w').write('\n'.join(lines) + '\n')
    print('wrote', out, '(%d cells, slots %d-%d)' % (len(cells), slots[0], slots[-1]))


if __name__ == '__main__':
    {'preview': preview, 'csource': csource}[sys.argv[1]](sys.argv[2])
