"""The Bean-with-Bacon Megarocket parked on a fan game's world map (Keen
Worlds art), for worlds without a ship of their own: Keen 8 (Calidune).

    python3 tools/kw_mapship_art.py preview OUT.png   # both maps, with the art
    python3 tools/kw_mapship_art.py csource OUT.h     # C data for the engine

The ship is Keen 4's own world-map Megarocket (2 x 1 tiles), taken from
the Keen 4 shareware data in play/. The tiles go into masked-tile
slots of each mod's own graphics that no level uses, found from the
player's files in play/mods/keen8.
"""
import os, sys, tempfile
from PIL import Image

HERE = os.path.dirname(__file__)
sys.path.insert(0, HERE)
from kw_gate_art import EGA, tile_bytes

BLACK, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN, LGREY = range(8)
DGREY, LBLUE, LGREEN, LCYAN, LRED, LMAGENTA, YELLOW, WHITE = range(8, 16)
T = None

SHIP_W, SHIP_H = 32, 16
KEEN4_SHIP_TILES = (1012, 1013)

# World: (mod folder, engine episode, ship origin)
WORLDS = {
    'keen8': ('keen8', '6', (18, 38)),
}
# Background cells to clear under the art, per world (none at present).
CLEAR = {}
# The mods' file names, for the slot search (it reads standard names).
MOD_FILES = {
    '6': {'EGAGRAPH.CK6': 'EGAGRAPH.CK6', 'EGAHEAD.CK6': 'EGAHEAD.CK6', 'EGADICT.CK6': 'EGADICT.CK6',
          'GFXINFOE.CK6': 'GFXINFOE.CK6', 'GAMEMAPS.CK6': 'editmaps.ck6', 'MAPHEAD.CK6': 'maphead.ck6',
          'TILEINFO.CK6': 'Keen6.tli'},
    '5': {'EGAGRAPH.CK5': 'EGAGRAPH.CK5', 'EGAHEAD.CK5': 'EGAHEAD.CK5', 'EGADICT.CK5': 'EGADICT.CK5',
          'GFXINFOE.CK5': 'GFXINFOE.CK5', 'GAMEMAPS.CK5': 'editmaps.ck5', 'MAPHEAD.CK5': 'maphead.ck5',
          'TILEINFO.CK5': 'Keen9.tli'},
}


def canvas(w, h):
    return [[T] * w for _ in range(h)]


def outline(px):
    h, w = len(px), len(px[0])
    filled = [[px[y][x] is not None for x in range(w)] for y in range(h)]
    for y in range(h):
        for x in range(w):
            if not filled[y][x] and any(0 <= x + dx < w and 0 <= y + dy < h and filled[y + dy][x + dx]
                                        for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1))):
                px[y][x] = BLACK


def draw_ship():
    """The real thing: Keen 4's Megarocket from its world map (masked tiles
    1012-1013 beside Keen's start). Keen 4 v1.4 is the freely distributable
    shareware episode."""
    import keenrender
    g = keenrender.Graphics(os.path.join(HERE, '..', 'play'), '4')
    px = canvas(SHIP_W, SHIP_H)
    for i, tile in enumerate(KEEN4_SHIP_TILES):
        img = g.tile(tile, True).convert('RGBA')
        for y in range(16):
            for x in range(16):
                r, gg, b, a = img.getpixel((x, y))
                if a:
                    px[y][i * 16 + x] = EGA.index((r, gg, b))
    return px


def to_image(px):
    h, w = len(px), len(px[0])
    img = Image.new('RGBA', (w, h), (0, 0, 0, 0))
    for y in range(h):
        for x in range(w):
            if px[y][x] is not None:
                img.putpixel((x, y), EGA[px[y][x]] + (255,))
    return img


def mod_view(world):
    """A temporary folder with the mod's files under the standard names."""
    folder, ep = WORLDS[world][0], WORLDS[world][1]
    src = os.path.abspath(os.path.join(HERE, '..', 'play', 'mods', folder))
    d = tempfile.mkdtemp(prefix='kw_' + world + '_')
    for std, real in MOD_FILES[ep].items():
        os.symlink(os.path.join(src, real), os.path.join(d, std))
    return d, ep


def preview(out):
    import keenmap, keenrender
    sheets = []
    for world, (folder, ep, ship) in WORLDS.items():
        d, _ = mod_view(world)
        name, w, h, (bg, fg, info) = keenmap.level(d, ep, 0)
        bg = list(bg)
        for (cx, cy) in CLEAR.get(world, ()):
            bg[cy * w + cx] = 50   # as kw_mapShips[] in kw_world.c
        g = keenrender.Graphics(d, ep)
        x0, y0 = ship[0] - 8, ship[1] - 8
        img = Image.new('RGBA', (20 * 16, 14 * 16), (0, 0, 0, 255))
        for y in range(14):
            for x in range(20):
                t = g.tile(bg[(y0 + y) * w + x0 + x], False)
                if t: img.paste(t, (x * 16, y * 16))
                v = fg[(y0 + y) * w + x0 + x]
                if v:
                    t = g.tile(v, True)
                    if t: img.alpha_composite(t, (x * 16, y * 16))
        img.alpha_composite(to_image(draw_ship()), ((ship[0] - x0) * 16, (ship[1] - y0) * 16))
        sheets.append(img)
    out_img = Image.new('RGB', (sum(s.width for s in sheets) + 8, sheets[0].height), (40, 40, 40))
    x = 0
    for s in sheets:
        out_img.paste(s.convert('RGB'), (x, 0))
        x += s.width + 8
    out_img.resize((out_img.width * 3, out_img.height * 3), Image.NEAREST).save(out)
    print('wrote', out)


def c_art(lines, name, px, slots):
    tw, th = len(px[0]) // 16, len(px) // 16
    lines.append('static const uint16_t %sSlots[1][%d][%d] = {' % (name, th, tw))
    lines.append('\t{' + ', '.join('{' + ', '.join(str(slots[r * tw + c]) for c in range(tw)) + '}' for r in range(th)) + '},')
    lines.append('};')
    lines.append('static const uint8_t %sTiles[1][%d][%d][160] = {' % (name, th, tw))
    lines.append('\t{')
    for r in range(th):
        lines.append('\t\t{')
        for c in range(tw):
            lines.append('\t\t\t{' + ','.join('0x%02x' % v for v in tile_bytes(px, c, r)) + '},')
        lines.append('\t\t},')
    lines.append('\t},')
    lines.append('};')


def free_slots_for(world, count):
    """Masked-tile slots no level of the mod uses (nor any animation), taken
    from the top of the tile set. The mods filled every empty slot with
    graphics of their own, so these hold unused graphics; the engine builds
    the art tiles in memory over them (and locks them there)."""
    import struct, keenmap
    d, ep = mod_view(world)
    info = struct.unpack('<23H', open(os.path.join(d, 'GFXINFOE.CK' + ep), 'rb').read()[:46])
    n16, n16m = info[2], info[3]
    used = set()
    for lv in range(30):
        try:
            used |= set(keenmap.level(d, ep, lv)[3][1])
        except Exception:
            pass
    ti = open(os.path.join(d, 'TILEINFO.CK' + ep), 'rb').read()
    anim = lambda t: struct.unpack('b', ti[2 * n16 + 4 * n16m + t:2 * n16 + 4 * n16m + t + 1])[0]
    targets = {t + anim(t) for t in range(n16m) if anim(t)}
    slots = [t for t in range(n16m - 1, 0, -1) if t not in used and t not in targets and not anim(t)]
    assert len(slots) >= count, 'not enough unused tile slots'
    return sorted(slots[:count])


def csource(out):
    lines = ['/* Generated by tools/kw_mapship_art.py -- do not edit by hand. */']
    lines.append('#define KW_MAPSHIP_TILES_W %d' % (SHIP_W // 16))
    lines.append('#define KW_MAPSHIP_TILES_H %d' % (SHIP_H // 16))
    for world, (folder, ep, ship) in WORLDS.items():
        n = (SHIP_W // 16) * (SHIP_H // 16)
        slots = free_slots_for(world, n)
        tag = 'kw_' + world
        lines.append('// %s: masked-tile slots no level of the mod uses.' % world)
        lines.append('#define KW_%s_SHIP_X %d' % (world.upper(), ship[0]))
        lines.append('#define KW_%s_SHIP_Y %d' % (world.upper(), ship[1]))
        c_art(lines, tag + 'Ship', draw_ship(), slots)
    open(out, 'w').write('\n'.join(lines) + '\n')
    print('wrote', out)


if __name__ == '__main__':
    {'preview': preview, 'csource': csource}[sys.argv[1]](sys.argv[2])
