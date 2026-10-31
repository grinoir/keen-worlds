"""Keen's outpost in Border Village: a hut at the end of the street, boarded
up until the outpost opens, with a room carved into the rock below
(Keen Worlds, KEENS_OUTPOST.md in the workspace root).

    python3 kwtools/kw_outpost.py preview OUTDIR [GAMEDIR]   # street, room
    python3 kwtools/kw_outpost.py csource SRCDIR [GAMEDIR]   # C header

GAMEDIR is a folder with the Keen 4 files (default: the workspace's play/).
Everything is built from Border Village's own tiles (Keen 4 level 1) plus
Keen Worlds art:

- The hut is the second house's picture (x 37-44), copied to the flat end of
  the street (x 102-109, where the hill comes down), with a radio mast on
  its roof whose light blinks. Under its door go the second house's door
  tiles, so the art keeps their door flags.
- The room is the first house's room (x 2-12, rows 43-49), widened, carved
  into the solid black fill at x 93-108, rows 43-49 (nothing else is there).
- Inside: shelves for one keepsake per saved world (each a piece of its
  own, shown once that world is saved), a bunk, a star chart and Spot.

The engine applies the tile edits at load time (kw_outpost.c), so the
player's GAMEMAPS.CK4 stays untouched.
"""
import glob, os, re, sys
from PIL import Image

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import keenmap, keenrender
from kw_gate_art import EGA, tile_bytes, free_slots_in
from kw_bar_art import Canvas
import kw_barhouse

DATA = os.path.join(os.path.dirname(os.path.dirname(HERE)), 'play')
EPISODE, LEVEL = '4', 1

BLACK, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN, LGREY = range(8)
DGREY, LBLUE, LGREEN, LCYAN, LRED, LMAGENTA, YELLOW, WHITE = range(8, 16)
T = None

# ---------------------------------------------------------------- the hut

HUT_X, HUT_TOP, HUT_W, HUT_H = 102, 9, 8, 7
TEMPLATE_X, TEMPLATE_TOP = 37, 9               # the second house
TEMPLATE_DOOR_X = 39                           # its door: x 39-40, rows 14-15
DOOR_X = HUT_X + (TEMPLATE_DOOR_X - TEMPLATE_X)
DOOR_ROWS = (14, 15)
MAST_X, MAST_TOP = HUT_X + 5, HUT_TOP - 2      # on the roof's ridge, 1x3 tiles

# ---------------------------------------------------------------- the room

ROOM_X, ROOM_TOP = 93, 43
ROOM_COLS = [2, 3, 4, 5, 6, 7, 8] + [6] * 7 + [11, 12]
ROOM_ROWS = list(range(43, 50))
ROOM_DOOR_X = ROOM_X + ROOM_COLS.index(7)
ROOM_FLOOR = ROOM_TOP + ROOM_ROWS.index(49)
ROOM_STAND = ROOM_FLOOR - 1

TO_ROOM = (ROOM_DOOR_X << 8) | ROOM_STAND
TO_STREET = (DOOR_X << 8) | (DOOR_ROWS[1] + 1)

SHELF_X, SHELF_TOP, SHELF_W, SHELF_H = ROOM_DOOR_X + 2, ROOM_TOP + 1, 6, 3
BUNK_X, BUNK_Y = SHELF_X + 4, ROOM_STAND - 1   # 3x2, on the floor
SPOT_X, SPOT_Y = SHELF_X + 1, ROOM_STAND - 1   # 2x2
CHART_X, CHART_Y = ROOM_DOOR_X - 1, ROOM_TOP + 2  # 1x2, between window and door

# One keepsake per world, in the order of the Chronicle, left to right and
# top to bottom on the shelves. The names are KW_World's.
KEEPSAKES = ['KEEN4', 'KEEN5', 'KEEN6', 'KEEN7', 'KEEN8', 'KEEN9',
             'KEEN95', 'AREALM', 'FITF', 'ALPHAMATIC', 'RTTS', 'OSI',
             'CK55', 'ASYLUM', 'BOUNTY', 'SUBURB']


def level(d=None):
    return keenmap.level(d or DATA, EPISODE, LEVEL)


def edits(d=None):
    """(when, plane, x, y, value). when: 'always' or 'open' (the outpost)."""
    name, w, h, (bg, fg, info) = level(d)
    out = []
    # The hut's door tiles, for their door flags (the art covers them).
    for dy in range(2):
        for dx in range(2):
            v = fg[(DOOR_ROWS[0] + dy) * w + TEMPLATE_DOOR_X + dx]
            out.append(('always', 1, DOOR_X + dx, DOOR_ROWS[0] + dy, v))
    # The room.
    for j, ty in enumerate(ROOM_ROWS):
        for i, tx in enumerate(ROOM_COLS):
            x, y = ROOM_X + i, ROOM_TOP + j
            if ty == 48 and tx in (7, 8):
                tx = 6          # plain floor: the template has the pole's hole here
            out.append(('always', 0, x, y, bg[ty * w + tx]))
            out.append(('always', 1, x, y, fg[ty * w + tx]))
            out.append(('always', 2, x, y, 0))
    for y in DOOR_ROWS:
        for x in (DOOR_X, DOOR_X + 1):
            out.append(('open', 2, x, y, TO_ROOM))
    for y in (ROOM_STAND - 1, ROOM_STAND):
        for x in (ROOM_DOOR_X, ROOM_DOOR_X + 1):
            out.append(('open', 2, x, y, TO_STREET))
    return out

# ---------------------------------------------------------------- the art


def draw_hut(d=None):
    """The second house's picture, 8x7 tiles."""
    name, w, h, (bg, fg, info) = level(d)
    g = keenrender.Graphics(d or DATA, EPISODE)
    img = Image.new('RGBA', (HUT_W * 16, HUT_H * 16), (0, 0, 0, 0))
    for j in range(HUT_H):
        for i in range(HUT_W):
            t = fg[(TEMPLATE_TOP + j) * w + TEMPLATE_X + i]
            if t:
                tile = g.tile(t, True)
                if tile:
                    img.alpha_composite(tile, (i * 16, j * 16))
    lut = {c: k for k, c in enumerate(EGA)}
    return [[lut[img.getpixel((x, y))[:3]] if img.getpixel((x, y))[3] else T
             for x in range(img.width)] for y in range(img.height)]


def draw_mast(frame):
    """A radio mast on the ridge: a pole, two cross-bars, a light on top
    (blinking red, frame 1 off). 1x3 tiles; its foot stands on the roof."""
    c = Canvas(16, 48)
    c.rect(7, 6, 8, 47, DGREY)
    c.rect(7, 6, 7, 47, LGREY)
    for y, half in ((16, 5), (26, 4)):
        c.rect(8 - half, y, 7 + half, y, BLACK)
        c.put(8 - half, y + 1, DGREY); c.put(7 + half, y + 1, DGREY)
    c.rect(4, 36, 11, 36, BLACK)                    # guy-wire brackets
    for k in range(10):
        c.put(5 - k // 3, 37 + k, DGREY); c.put(10 + k // 3, 37 + k, DGREY)
    c.box(6, 2, 9, 5, LRED if frame == 0 else RED)
    if frame == 0:
        c.put(7, 3, WHITE)
        c.put(5, 1, YELLOW); c.put(10, 1, YELLOW)
    return c.px


# The hut's two windows (pixels of the hut's picture) light up at the end of
# the opening scene ("It was a home by Tuesday") and stay lit: the dark of
# the window holes in yellow, as the scene draws it. 3x4 tiles over the hut.
LIGHT_X, LIGHT_TOP = HUT_X + 4, HUT_TOP + 3
WINDOWS = [(66, 48, 98, 74), (66, 78, 98, 100)]     # in the hut's picture


def draw_light(d=None):
    hut = draw_hut(d)
    c = Canvas(48, 64)
    ox, oy = (LIGHT_X - HUT_X) * 16, (LIGHT_TOP - HUT_TOP) * 16
    for x0, y0, x1, y1 in WINDOWS:
        for y in range(y0, y1):
            for x in range(x0, x1):
                if hut[y][x] == BLACK:
                    c.put(x - ox, y - oy, YELLOW)
    return c.px


def draw_shelves():
    """Three wooden shelf boards on the wall, 6x3 tiles: each tile's bottom
    rows are the board the keepsake stands on."""
    c = Canvas(SHELF_W * 16, SHELF_H * 16)
    for r in range(SHELF_H):
        y = r * 16 + 13
        c.rect(0, y, SHELF_W * 16 - 1, y + 1, BROWN)
        c.rect(0, y, SHELF_W * 16 - 1, y, YELLOW)
        c.rect(1, y + 2, SHELF_W * 16 - 2, y + 2, BLACK)
        for bx in (6, SHELF_W * 16 - 8):            # brackets
            c.rect(bx, y + 2, bx + 1, y + 2, BLACK)
            c.put(bx, y + 3, BLACK)
    return c.px


def draw_bunk():
    """A low wooden bunk with a blue blanket and a pillow, 3x2 tiles."""
    c = Canvas(48, 32)
    c.box(1, 6, 4, 31, BROWN)                       # head post
    c.box(43, 14, 46, 31, BROWN)                    # foot post
    c.box(4, 20, 43, 27, BROWN)                     # frame
    c.rect(5, 21, 42, 21, YELLOW)
    c.box(5, 15, 42, 20, BLUE)                      # blanket
    c.rect(6, 16, 41, 16, LBLUE)
    for x in range(9, 41, 6):
        c.put(x, 18, LBLUE)
    c.box(6, 11, 17, 16, WHITE)                     # pillow
    c.rect(7, 12, 16, 12, WHITE); c.rect(7, 15, 16, 15, LGREY)
    c.rect(6, 28, 7, 31, BROWN); c.rect(40, 28, 41, 31, BROWN)
    return c.px


def draw_spot():
    """Spot, Keen's Yorp, about Keen's knee height on his own: a green body,
    one eye on a stalk, a gold collar. 2x2 tiles, standing on the floor."""
    c = Canvas(32, 32)
    ox, oy = 8, 6
    G, G2 = LGREEN, GREEN
    c.box(ox + 4, oy + 0, ox + 11, oy + 7, WHITE)    # the eye
    c.rect(ox + 6, oy + 2, ox + 8, oy + 5, BLACK)
    c.put(ox + 6, oy + 2, LGREY)
    c.box(ox + 6, oy + 7, ox + 10, oy + 12, G)       # the stalk
    c.rect(ox + 9, oy + 8, ox + 9, oy + 11, G2)
    c.rect(ox + 4, oy + 12, ox + 12, oy + 13, YELLOW) # the collar
    c.put(ox + 8, oy + 13, BROWN)
    c.box(ox + 1, oy + 13, ox + 15, oy + 22, G)      # the body
    c.rect(ox + 12, oy + 14, ox + 14, oy + 21, G2)
    c.box(ox + 3, oy + 16, ox + 13, oy + 20, BLACK)  # the mouth, open
    c.rect(ox + 9, oy + 18, ox + 12, oy + 19, LRED)
    c.put(ox + 5, oy + 18, WHITE); c.put(ox + 6, oy + 18, WHITE)
    c.box(ox + 2, oy + 22, ox + 6, oy + 25, G)       # feet
    c.box(ox + 10, oy + 22, ox + 14, oy + 25, G)
    return c.px


def draw_chart():
    """A star chart pinned to the wall: dots for worlds, a dashed course."""
    c = Canvas(16, 32)
    c.box(1, 2, 14, 29, BLUE, BLACK)
    c.rect(1, 2, 14, 3, WHITE); c.rect(1, 28, 14, 29, WHITE)    # rolled ends
    c.put(7, 1, LRED)                                           # the pin
    stars = [(4, 7, YELLOW), (10, 10, LGREEN), (5, 15, LRED), (11, 19, LCYAN), (6, 24, WHITE)]
    for a, b in zip(stars, stars[1:]):
        steps = max(abs(b[0] - a[0]), abs(b[1] - a[1]))
        for s in range(0, steps, 2):
            c.put(a[0] + (b[0] - a[0]) * s // steps, a[1] + (b[1] - a[1]) * s // steps, LBLUE)
    for x, y, col in stars:
        c.rect(x, y, x + 1, y + 1, col)
    return c.px


def keepsake(name):
    """A 16x16 keepsake standing on a shelf board (whose top is row 13)."""
    c = Canvas(16, 16)
    B = 12                                          # the row it stands on
    if name == 'KEEN4':        # a red gem from the Shadowlands
        for k in range(4):
            c.rect(8 - k - 1, B - 6 + k, 8 + k, B - 6 + k, LRED)
        for k in range(4):
            c.rect(5 + k, B - 2 + k // 2, 10 - k, B - 2 + k // 2, RED)
        c.put(6, B - 5, WHITE)
    elif name == 'KEEN5':      # a burnt-out Omegamatic fuse
        c.box(5, B - 9, 10, B, DGREY)
        c.rect(6, B - 8, 9, B - 1, LGREY)
        c.rect(7, B - 6, 8, B - 3, BLACK)
        c.rect(7, B - 11, 8, B - 10, BLACK)
    elif name == 'KEEN6':      # a can of Bloog soda
        c.box(5, B - 9, 10, B, LGREEN)
        c.rect(6, B - 6, 9, B - 4, WHITE)
        c.put(7, B - 5, GREEN); c.put(8, B - 5, GREEN)
        c.rect(6, B - 9, 9, B - 9, LGREY)
    elif name == 'KEEN7':      # one of Krodacia's keys
        c.box(3, B - 5, 7, B - 1, YELLOW)
        c.put(5, B - 3, BLACK)
        c.rect(8, B - 3, 13, B - 3, YELLOW)
        c.put(11, B - 2, YELLOW); c.put(13, B - 2, YELLOW)
    elif name == 'KEEN8':      # a jar of Calidune sand
        c.box(4, B - 9, 11, B, LCYAN)
        c.rect(5, B - 5, 10, B - 1, YELLOW)
        c.rect(5, B - 6, 10, B - 6, BROWN)
        c.rect(5, B - 10, 10, B - 10, BROWN)
    elif name == 'KEEN9':      # a brain in a jar (the Battle of the Brains)
        c.box(4, B - 10, 11, B, LCYAN)
        c.box(5, B - 7, 10, B - 2, LMAGENTA, MAGENTA)
        c.put(7, B - 5, MAGENTA); c.put(8, B - 4, MAGENTA)
        c.rect(4, B - 11, 11, B - 11, DGREY)
    elif name == 'KEEN95':     # an accumulator, still glowing a little
        c.box(4, B - 8, 11, B, DGREY)
        c.rect(5, B - 7, 10, B - 1, YELLOW)
        c.rect(7, B - 10, 8, B - 9, LGREY)
        c.rect(6, B - 5, 9, B - 4, WHITE)
    elif name == 'AREALM':     # a Krile crystal
        for k in range(5):
            c.rect(7 - k // 2, B - 9 + k, 8 + k // 2, B - 9 + k, LMAGENTA)
        for k in range(5):
            c.rect(5 + k // 2, B - 4 + k, 10 - k // 2, B - 4 + k, MAGENTA)
        c.put(7, B - 7, WHITE)
    elif name == 'FITF':       # the cryptic note from Gnosticus Pi
        c.box(3, B - 7, 12, B, WHITE, LGREY)
        c.rect(5, B - 5, 10, B - 5, BLUE); c.rect(5, B - 3, 9, B - 3, BLUE)
        c.put(11, B - 1, LGREY)
    elif name == 'ALPHAMATIC': # the decryption key, on a floppy disk
        c.box(3, B - 9, 12, B, BLUE)
        c.rect(5, B - 8, 10, B - 6, LGREY)
        c.rect(5, B - 3, 10, B - 1, WHITE)
        c.put(8, B - 7, BLUE)
    elif name == 'RTTS':       # a lantern from the Shadowlands
        c.rect(7, B - 11, 8, B - 10, BLACK)
        c.box(5, B - 9, 10, B, BLACK)
        c.rect(6, B - 8, 9, B - 1, YELLOW)
        c.rect(7, B - 6, 8, B - 3, WHITE)
    elif name == 'OSI':        # a Shikadi station pass
        c.box(3, B - 7, 12, B, LGREY)
        c.rect(4, B - 6, 6, B - 3, LBLUE)
        c.rect(8, B - 5, 11, B - 5, BLACK); c.rect(8, B - 3, 10, B - 3, BLACK)
    elif name == 'CK55':       # a gear from the Omegamatic II
        for y in range(-4, 5):
            for x in range(-4, 5):
                if x * x + y * y <= 18:
                    c.put(8 + x, B - 5 + y, DGREY if x * x + y * y > 9 else LGREY)
        for x, y in ((0, -5), (0, 5), (-5, 0), (5, 0)):
            c.put(8 + x, B - 5 + y, DGREY)
        c.rect(7, B - 6, 8, B - 5, BLACK)
    elif name == 'ASYLUM':     # a letter with no address
        c.box(2, B - 6, 13, B, WHITE, LGREY)
        for k in range(5):
            c.put(3 + k, B - 5 + k // 2, LGREY); c.put(12 - k, B - 5 + k // 2, LGREY)
        c.put(10, B - 2, LRED)
    elif name == 'BOUNTY':     # Rick Uller's wanted poster
        c.box(3, B - 10, 12, B, YELLOW, BROWN)
        c.rect(5, B - 8, 10, B - 8, RED)
        c.box(6, B - 6, 9, B - 3, DGREY, BLACK)
        c.rect(5, B - 1, 10, B - 1, BROWN)
    elif name == 'SUBURB':     # Mortimer's dummy head
        c.box(4, B - 9, 11, B - 2, YELLOW)
        c.rect(5, B - 10, 10, B - 10, BLACK)            # hair
        c.rect(5, B - 6, 6, B - 6, BLACK); c.rect(9, B - 6, 10, B - 6, BLACK)  # glasses
        c.rect(7, B - 6, 8, B - 6, BLACK)
        c.rect(6, B - 4, 9, B - 4, RED)
        c.rect(6, B - 1, 9, B, DGREY)                   # its stump of a neck
        c.put(7, B, LRED)                               # a spark
    else:
        raise ValueError(name)
    return c.px


def keepsake_place(i):
    return SHELF_X + i % SHELF_W, SHELF_TOP + i // SHELF_W


def to_image(px):
    h, w = len(px), len(px[0])
    img = Image.new('RGBA', (w, h), (0, 0, 0, 0))
    for y in range(h):
        for x in range(w):
            if px[y][x] is not None:
                img.putpixel((x, y), EGA[px[y][x]] + (255,))
    return img


def art_pieces(d=None):
    """(name, x, y, frames as pixel lists, when). when: 'always', 'closed'
    (until the outpost opens), 'open', or a world's name for a keepsake."""
    pieces = [('hut', HUT_X, HUT_TOP, [draw_hut(d)], 'always'),
              ('mast', MAST_X, MAST_TOP, [draw_mast(0), draw_mast(1)], 'always'),
              ('hutboards', DOOR_X, DOOR_ROWS[0], [kw_barhouse.draw_boards()], 'closed'),
              ('hutlight', LIGHT_X, LIGHT_TOP, [draw_light(d)], 'lit'),
              ('shelves', SHELF_X, SHELF_TOP, [draw_shelves()], 'open'),
              ('bunk', BUNK_X, BUNK_Y, [draw_bunk()], 'open'),
              ('chart', CHART_X, CHART_Y, [draw_chart()], 'open'),
              ('spot', SPOT_X, SPOT_Y, [draw_spot()], 'spot')]
    for i, world in enumerate(KEEPSAKES):
        x, y = keepsake_place(i)
        pieces.append(('keep' + world.lower(), x, y, [keepsake(world)], world))
    return pieces

# ---------------------------------------------------------------- output


def render(x0, y0, x1, y1, shown, d=None, frame=0):
    """shown(when) -> bool: which pieces and edits to show."""
    name, w, h, (bg, fg, info) = level(d)
    planes = [list(bg), list(fg), list(info)]
    for when, plane, x, y, v in edits(d):
        if when == 'always' or shown(when):
            planes[plane][y * w + x] = v
    bg, fg = planes[0], planes[1]
    g = keenrender.Graphics(d or DATA, EPISODE)
    img = Image.new('RGBA', ((x1 - x0) * 16, (y1 - y0) * 16), (0, 0, 0, 255))
    for y in range(y0, y1):
        for x in range(x0, x1):
            t = g.tile(bg[y * w + x], False)
            if t: img.paste(t, ((x - x0) * 16, (y - y0) * 16))
            if fg[y * w + x]:
                t = g.tile(fg[y * w + x], True)
                if t: img.alpha_composite(t, ((x - x0) * 16, (y - y0) * 16))
    for name, ax, ay, frames, when in art_pieces(d):
        if when == 'always' or shown(when):
            img.alpha_composite(to_image(frames[frame % len(frames)]), ((ax - x0) * 16, (ay - y0) * 16))
    return img.convert('RGB')


def preview(outdir, d=None):
    os.makedirs(outdir, exist_ok=True)
    closed = render(92, 5, 110, 18, lambda when: when == 'closed', d)
    opened = render(92, 5, 110, 18, lambda when: when != 'closed', d, 1)
    sheet = Image.new('RGB', (closed.width * 2 + 8, closed.height), (40, 40, 40))
    sheet.paste(closed, (0, 0)); sheet.paste(opened, (closed.width + 8, 0))
    sheet.resize((sheet.width * 2, sheet.height * 2), Image.NEAREST).save(os.path.join(outdir, 'outpost_street.png'))
    box = (ROOM_X - 1, ROOM_TOP - 1, ROOM_X + len(ROOM_COLS) + 1, ROOM_TOP + len(ROOM_ROWS) + 1)
    empty = render(*box, lambda when: when in ('open', 'spot'), d)
    full = render(*box, lambda when: when != 'closed', d)
    sheet = Image.new('RGB', (empty.width, empty.height * 2 + 8), (40, 40, 40))
    sheet.paste(empty, (0, 0)); sheet.paste(full, (0, empty.height + 8))
    sheet.resize((sheet.width * 3, sheet.height * 3), Image.NEAREST).save(os.path.join(outdir, 'outpost_room.png'))
    px = scene_picture(d)
    img = Image.new('RGB', (320, 144))
    img.putdata([EGA[c] for row in px for c in row])
    img.resize((640, 288), Image.NEAREST).save(os.path.join(outdir, 'outpost_scene.png'))
    print('wrote', outdir, 'TO_ROOM=%04x TO_STREET=%04x' % (TO_ROOM, TO_STREET))


# The opening scene's background (src/kw_story_scenes.c): the end of the
# street with the hut, at the map's own size (320x144: x 90-109 from half
# way down row 7). The scene's Keen and Spot stand in front of it.
SCENE_X0, SCENE_Y0 = 90 * 16, 7 * 16 + 8


def scene_picture(d=None):
    street = render(SCENE_X0 // 16, SCENE_Y0 // 16, SCENE_X0 // 16 + 20, SCENE_Y0 // 16 + 10,
                    lambda when: when == 'open', d, 0)     # (not 'lit': the scene lights them)
    oy = SCENE_Y0 % 16
    crop = street.crop((0, oy, 320, oy + 144))
    lut = {c: k for k, c in enumerate(EGA)}
    return [[lut[crop.getpixel((x, y))] for x in range(320)] for y in range(144)]


def scene_source(srcdir, d=None):
    px = scene_picture(d)
    packed = [px[y][x] << 4 | px[y][x + 1] for y in range(144) for x in range(0, 320, 2)]
    lines = ['/* Generated by kwtools/kw_outpost.py -- do not edit by hand. */',
             '// The outpost scene\'s background: the end of Border Village\'s street with',
             '// the hut (320x144, two pixels to a byte, high nibble first).',
             'static const uint8_t kw_outpostScenePicture[%d] = {' % len(packed)]
    for i in range(0, len(packed), 40):
        lines.append('\t' + ','.join(str(b) for b in packed[i:i + 40]) + ',')
    lines.append('};')
    # Spot as he is in the hut (draw_spot), for the scene at the level's size
    # (a pixel of 16: see-through)
    spot = draw_spot()
    ys = [y for y in range(32) if any(c is not None for c in spot[y])]
    xs = [x for x in range(32) if any(spot[y][x] is not None for y in range(32))]
    w, h = xs[-1] - xs[0] + 1, ys[-1] - ys[0] + 1
    lines += ['#define KW_OUTPOST_SPOT_W %d' % w, '#define KW_OUTPOST_SPOT_H %d' % h,
              'static const uint8_t kw_outpostSpotPixels[%d] = {' % (w * h),
              '\t' + ','.join(str(16 if spot[y][x] is None else spot[y][x])
                              for y in range(ys[0], ys[-1] + 1) for x in range(xs[0], xs[-1] + 1)) + ',', '};']
    out = os.path.join(srcdir, 'kw_outpost_scene.h')
    open(out, 'w').write('\n'.join(lines) + '\n')
    print('wrote', out)


def taken_slots(srcdir):
    """Every masked-tile slot another generated header already uses (all
    worlds: a slot is free only if nobody claims it)."""
    taken = set()
    for f in glob.glob(os.path.join(srcdir, 'kw_*.h')):
        if os.path.basename(f) == 'kw_outpost_art.h':
            continue
        s = open(f).read()
        for m in re.finditer(r'\w+Slots\[[^=]*=\s*\{(.*?)\};', s, re.S):
            taken |= {int(x) for x in re.findall(r'\d+', m.group(1))}
    return taken


SLOT_RANGE = (1, 2915)


def csource(srcdir, d=None):
    d = d or DATA
    pieces = art_pieces(d)
    need = sum(len(f) * (len(f[0]) // 16) * (len(f[0][0]) // 16) for _, _, _, f, _ in pieces)
    taken = taken_slots(srcdir)
    free = [s for s in free_slots_in(EPISODE, None, *SLOT_RANGE, d=d) if s not in taken]
    assert len(free) >= need, 'not enough free tile slots: %d < %d' % (len(free), need)
    lines = ['/* Generated by kwtools/kw_outpost.py -- do not edit by hand. */',
             '// Keen\'s outpost in Border Village: tile edits (when: 0 always, 1 once open).',
             'static const KW_MapEdit kw_outpostEdits[] = {']
    for when, plane, x, y, v in edits(d):
        lines.append('\t{%d, %d, %d, %d, %d},' % (1 if when == 'open' else 0, plane, x, y, v))
    lines += ['};',
              '#define KW_OUTPOST_DOOR_X %d' % DOOR_X, '#define KW_OUTPOST_DOOR_Y %d' % DOOR_ROWS[0],
              '#define KW_OUTPOST_ROOM_DOOR_X %d' % ROOM_DOOR_X, '#define KW_OUTPOST_ROOM_STAND %d' % ROOM_STAND,
              # where Keen talks to Spot, looks at the shelves and lies down
              # on the bunk (apart; the chart beside the door is only a picture)
              '#define KW_OUTPOST_SPOT_X %d' % SPOT_X, '#define KW_OUTPOST_SHELF_X %d' % (SHELF_X + 3),
              '#define KW_OUTPOST_BUNK_X %d' % (BUNK_X + 2),
              '#define KW_OUTPOST_KEEPSAKES %d' % len(KEEPSAKES), '']
    k = 0
    for name, x, y, frames, when in pieces:
        h, w = len(frames[0]) // 16, len(frames[0][0]) // 16
        # Frames of a cell sit next to each other (animation links are int8).
        slots = [[0] * (w * h) for _ in frames]
        for cell in range(w * h):
            for f in range(len(frames)):
                slots[f][cell] = free[k]; k += 1
        lines += kw_barhouse.header_for(name, x, y, frames, slots) + ['']
    lines.append('// Which world each keepsake belongs to, in shelf order.')
    lines.append('#define KW_OUTPOST_KEEPSAKE_WORLDS ' + ', '.join('KW_' + wd for wd in KEEPSAKES))
    out = os.path.join(srcdir, 'kw_outpost_art.h')
    open(out, 'w').write('\n'.join(lines) + '\n')
    print('wrote', out, '(%d tile slots)' % need)
    scene_source(srcdir, d)


if __name__ == '__main__':
    d = sys.argv[3] if len(sys.argv) > 3 else None
    {'preview': preview, 'csource': csource}[sys.argv[1]](sys.argv[2], d)
