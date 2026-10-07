"""Border Village's tavern: a bigger stone house on the street, boarded up
until the village is freed, with a bar room inside (Keen Worlds).

    python3 tools/kw_barhouse.py preview OUTDIR     # street before/after, room
    python3 tools/kw_barhouse.py csource SRCDIR     # C headers for the engine

Everything is built from Border Village's own tiles (Keen 4 level 1) plus
Keen Worlds art:

- The inn replaces the fourth house (x 75-82) with one long stone house,
  x 75-91, rows 9-15: the first house's picture stretched at the pixel level
  (a strip of plain wall repeated, so the roof stays seamless) with a second
  door. The left door is the fourth house's original door and still leads to
  its original room; the right one is the tavern's.
- The room is the first house's room (x 2-12, rows 43-49), widened, carved
  into the solid rock band at x 58-81, rows 26-32, clear of the existing room
  below (rows 43-48) so the camera never shows both.
- Art: boards over the door (before), and inside (after) the bar with an
  animated barmaid, tables with stools, and the creator's portrait
  (tools/kw_portrait_art.py).

The engine applies the tile edits at load time (kw_habitat.c), so the
player's GAMEMAPS.CK4 stays untouched.
"""
import os, sys
from PIL import Image

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import keenmap, keenrender
from kw_gate_art import EGA, tile_bytes, free_slots_in
from kw_bar_art import Canvas, FONT

DATA = os.path.join(os.path.dirname(HERE), 'play')
EPISODE, LEVEL = '4', 1

BLACK, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN, LGREY = range(8)
DGREY, LBLUE, LGREEN, LCYAN, LRED, LMAGENTA, YELLOW, WHITE = range(8, 16)
T = None

# ---------------------------------------------------------------- the house

INN_X, INN_TOP, INN_TILES_W, INN_TILES_H = 75, 9, 17, 7
TEMPLATE_X, TEMPLATE_TOP = 9, 9          # the first house, 10 x 7 tiles
# Pixel columns of the template picture: the part kept on the left (door and
# window), a plain-wall strip under a level stretch of roof, and the right end.
LEFT_END, STRIP, RIGHT_START = 126, (126, 134), 126
DOOR_PIXELS = (30, 64)                   # the door, with a little wall around
# Below this pixel row the stretched part takes plain wall from these columns
# (between the door and the window) instead of the strip, whose wall holds
# the house's right-edge stones.
UNDERSIDE_FROM_ROW, UNDERSIDE_COLS = 36, (128, 129)
WALL_FROM_ROW = 56
PLAIN_WALL = [64, 65, 66, 67]
DOOR_ROWS = (14, 15)
OLD_DOOR_X = INN_X + 2                   # the fourth house's door (x 77-78)
DOOR_X = INN_X + 10                      # the tavern door (x 85-86)
TEMPLATE_DOOR_TILES = {(0, 14): 2026, (1, 14): 2027, (0, 15): 2044, (1, 15): 2045}

# ---------------------------------------------------------------- the room

ROOM_X, ROOM_TOP = 58, 26
# Template columns of the first house's room (x 2-12): rock and wall (2-3),
# window (4-5), wall (6), door (7-8), then plain wall repeated, then the right
# wall (11-12). The pole columns (9-10) are left out.
ROOM_COLS = [2, 3, 4, 5, 6, 7, 8] + [6] * 15 + [11, 12]
ROOM_ROWS = list(range(43, 50))
ROOM_DOOR_X = ROOM_X + ROOM_COLS.index(7)
ROOM_FLOOR = ROOM_TOP + ROOM_ROWS.index(49)        # solid floor row
ROOM_STAND = ROOM_FLOOR - 1                        # Keen's feet row

# Door destinations (info plane: x << 8 | y, the tile Keen arrives on).
TO_ROOM = (ROOM_DOOR_X << 8) | ROOM_STAND
TO_STREET = (DOOR_X << 8) | (DOOR_ROWS[1] + 1)   # one row below, as the original doors do


def level():
    return keenmap.level(DATA, EPISODE, LEVEL)


def edits():
    """(when, plane, x, y, value) tile edits. when: 'always' or 'after'."""
    name, w, h, (bg, fg, info) = level()
    out = []
    # The inn's area is cleared (the fourth house goes); its picture is art.
    # Under both doors go door tiles, so the art keeps their door flags.
    for y in range(INN_TOP, INN_TOP + INN_TILES_H):
        for x in range(INN_X, INN_X + INN_TILES_W):
            v = 0
            for dx0 in (OLD_DOOR_X, DOOR_X):
                if (x - dx0, y) in TEMPLATE_DOOR_TILES:
                    v = TEMPLATE_DOOR_TILES[(x - dx0, y)]
            out.append(('always', 1, x, y, v))
    # The room: background and foreground, info cleared (no template items).
    for j, ty in enumerate(ROOM_ROWS):
        for i, tx in enumerate(ROOM_COLS):
            x, y = ROOM_X + i, ROOM_TOP + j
            if ty == 48 and tx in (7, 8):
                tx = 6          # plain floor: the template has the pole's hole here
            out.append(('always', 0, x, y, bg[ty * w + tx]))
            out.append(('always', 1, x, y, fg[ty * w + tx]))
            out.append(('always', 2, x, y, 0))
    # The doors only lead anywhere once the village is freed.
    for y in DOOR_ROWS:
        for x in (DOOR_X, DOOR_X + 1):
            out.append(('after', 2, x, y, TO_ROOM))
    for y in (ROOM_STAND - 1, ROOM_STAND):
        for x in (ROOM_DOOR_X, ROOM_DOOR_X + 1):
            out.append(('after', 2, x, y, TO_STREET))
    return out


def edited_level(when_done):
    name, w, h, (bg, fg, info) = level()
    planes = [list(bg), list(fg), list(info)]
    for when, plane, x, y, v in edits():
        if when == 'always' or when_done:
            planes[plane][y * w + x] = v
    return w, h, planes

# ---------------------------------------------------------------- the art


def template_house():
    """The first house's foreground as 160x112 pixels (None = transparent)."""
    name, w, h, (bg, fg, info) = level()
    g = keenrender.Graphics(DATA, EPISODE)
    img = Image.new('RGBA', (160, 112), (0, 0, 0, 0))
    for j in range(7):
        for i in range(10):
            t = fg[(TEMPLATE_TOP + j) * w + TEMPLATE_X + i]
            if t:
                tile = g.tile(t, True)
                if tile:
                    img.alpha_composite(tile, (i * 16, j * 16))
    lut = {c: k for k, c in enumerate(EGA)}
    return [[lut[img.getpixel((x, y))[:3]] if img.getpixel((x, y))[3] else T for x in range(160)] for y in range(112)]


def draw_inn():
    src = template_house()
    W = INN_TILES_W * 16
    out = [[T] * W for _ in range(112)]
    right_w = 160 - RIGHT_START
    strip_w = STRIP[1] - STRIP[0]
    for y in range(112):
        row = src[y]
        out[y][:LEFT_END] = row[:LEFT_END]
        x = LEFT_END
        while x < W - right_w:
            for k in range(strip_w):
                if x + k < W - right_w:
                    if y < UNDERSIDE_FROM_ROW:
                        out[y][x + k] = row[STRIP[0] + k]      # the level roof
                    elif y < WALL_FROM_ROW:
                        # The roof's shadowed underside as one straight band
                        # (repeating the strip here gives a row of teeth).
                        out[y][x + k] = row[UNDERSIDE_COLS[(x + k) % 2]]
                    else:
                        # Plain wall below the roof, sampled between the door
                        # and the window with the same dither phase.
                        col = PLAIN_WALL[(x + k) % 2::2][(x + k) // 2 % (len(PLAIN_WALL) // 2)]
                        out[y][x + k] = row[col]
            x += strip_w
        out[y][W - right_w:] = row[RIGHT_START:]
    # The tavern door: the template door pasted onto the long wall, aligned
    # with its door tiles. Only the wall part (below the roof) is copied.
    shift = (DOOR_X - INN_X) * 16 - (OLD_DOOR_X - INN_X) * 16
    for y in range(62, 112):
        for x in range(*DOOR_PIXELS):
            if src[y][x] is not None:
                out[y][x + shift] = src[y][x]
    return out


def draw_boards():
    """Boards nailed across the door (2x2 tiles over the door)."""
    c = Canvas(32, 32)
    for y0 in (4, 13, 22):
        c.box(1, y0, 30, y0 + 5, BROWN)
        c.rect(2, y0 + 1, 29, y0 + 1, YELLOW if y0 == 4 else LGREY)
        c.put(4, y0 + 3, BLACK); c.put(27, y0 + 3, BLACK)   # nails
    # A crooked board across.
    for k in range(26):
        c.rect(3 + k, 27 - k, 4 + k, 29 - k, BROWN)
        c.put(3 + k, 27 - k, BLACK)
    return c.px


BAR_W, BAR_H = 7 * 16, 5 * 16          # canvas for the bar end of the room
BAR_X = ROOM_X + 16                    # tiles 74-80
BAR_Y = ROOM_TOP + 1                   # rows 34-38
BAR_FRAMES = 4


def draw_barmaid(c, x0, y0, frame):
    """The barmaid in the style of Keen's sprites (black outlines, two-tone
    shading, about Princess Lindsey's size): only her head and upper body show
    above the counter. x0, y0: top-left of a 22x26 box; the counter top is at
    y0 + 26. She dries a glass (frames 0-3) and blinks in frame 3."""
    OUT, SKIN, SKIN2, HAIR, HAIR2 = BLACK, WHITE, LGREY, YELLOW, BROWN
    DRESS, DRESS2, BLOUSE = RED, LRED, WHITE

    def outlined(pts, fill):
        for (x, y) in pts:
            c.put(x0 + x, y0 + y, fill)

    # Hair: a bun on top, then the head of hair; outlined.
    for (x, y, w) in ((9, 0, 4), (8, 1, 6)):
        c.rect(x0 + x, y0 + y, x0 + x + w - 1, y0 + y, HAIR)
    c.rect(x0 + 6, y0 + 2, x0 + 15, y0 + 4, HAIR)
    c.rect(x0 + 5, y0 + 5, x0 + 16, y0 + 12, HAIR)
    c.rect(x0 + 6, y0 + 4, x0 + 15, y0 + 4, HAIR2)       # band
    # Face.
    c.rect(x0 + 7, y0 + 5, x0 + 14, y0 + 12, SKIN)
    c.rect(x0 + 7, y0 + 5, x0 + 14, y0 + 5, HAIR)        # fringe
    c.put(x0 + 7, y0 + 12, SKIN2); c.put(x0 + 14, y0 + 12, SKIN2)
    if frame == 3:
        c.rect(x0 + 8, y0 + 8, x0 + 9, y0 + 8, OUT)      # blink
        c.rect(x0 + 12, y0 + 8, x0 + 13, y0 + 8, OUT)
    else:
        c.put(x0 + 9, y0 + 7, OUT); c.put(x0 + 9, y0 + 8, LBLUE)
        c.put(x0 + 12, y0 + 7, OUT); c.put(x0 + 12, y0 + 8, LBLUE)
    c.put(x0 + 10, y0 + 9, SKIN2)                         # nose
    c.rect(x0 + 10, y0 + 11, x0 + 11, y0 + 11, RED)       # smile
    # Outline round the head.
    for y in range(1, 13):
        row = [x for x in range(22) if c.px[y0 + y][x0 + x] not in (None,)]
    for y in range(0, 14):
        for x in range(0, 22):
            cur = c.px[y0 + y][x0 + x] if 0 <= y0 + y < c.h and 0 <= x0 + x < c.w else None
            if cur is None:
                near = any(0 <= y0 + y + dy < c.h and 0 <= x0 + x + dx < c.w and
                           c.px[y0 + y + dy][x0 + x + dx] in (HAIR, HAIR2, SKIN, SKIN2)
                           for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1)))
                if near and y < 13:
                    c.put(x0 + x, y0 + y, OUT)
    # Neck, shoulders and a white blouse.
    c.rect(x0 + 9, y0 + 13, x0 + 12, y0 + 13, SKIN)
    c.rect(x0 + 4, y0 + 14, x0 + 17, y0 + 15, BLOUSE)
    c.put(x0 + 3, y0 + 15, OUT); c.put(x0 + 18, y0 + 15, OUT)
    c.rect(x0 + 4, y0 + 13, x0 + 8, y0 + 13, OUT); c.rect(x0 + 13, y0 + 13, x0 + 17, y0 + 13, OUT)
    # Red bodice, a curvy figure, with a laced front.
    c.rect(x0 + 3, y0 + 16, x0 + 18, y0 + 25, DRESS)
    c.rect(x0 + 2, y0 + 16, x0 + 2, y0 + 25, OUT); c.rect(x0 + 19, y0 + 16, x0 + 19, y0 + 25, OUT)
    for (cx, cy) in ((7, 18), (14, 18)):               # rounded highlights
        c.rect(x0 + cx - 2, y0 + cy - 1, x0 + cx + 1, y0 + cy + 1, DRESS2)
        c.put(x0 + cx - 3, y0 + cy, DRESS2); c.put(x0 + cx + 2, y0 + cy, DRESS2)
    c.rect(x0 + 10, y0 + 16, x0 + 11, y0 + 20, OUT)     # cleavage line / lacing
    c.put(x0 + 10, y0 + 22, OUT); c.put(x0 + 11, y0 + 24, OUT)
    c.rect(x0 + 4, y0 + 23, x0 + 17, y0 + 23, BROWN)    # waist ribbon
    # Arms: one holds the glass at her side, the other rubs it with a cloth.
    rub = [0, 1, 2, 1][frame]
    c.rect(x0 + 0, y0 + 16, x0 + 1, y0 + 22, SKIN)
    c.rect(x0 - 1, y0 + 16, x0 - 1, y0 + 22, OUT)
    gx, gy = x0 + 0, y0 + 18                             # the glass
    c.rect(gx - 1, gy, gx + 3, gy + 5, OUT)
    c.rect(gx, gy + 1, gx + 2, gy + 4, LCYAN)
    c.put(gx, gy + 1, WHITE)
    cx, cy = x0 + 3 + rub, y0 + 17 + rub                 # the cloth hand
    c.rect(x0 + 18, y0 + 17, x0 + 19, y0 + 20, SKIN)
    c.rect(cx + 2, cy + 1, x0 + 17, cy + 2, SKIN)
    c.rect(cx - 1, cy - 1, cx + 2, cy + 3, OUT)
    c.rect(cx, cy, cx + 1, cy + 2, WHITE)


SIGN_X, SIGN_Y = DOOR_X + 2, DOOR_ROWS[0] - 2   # on the wall right of the tavern door


def draw_sign():
    """The inn's BAR sign: hanging by short chains from an iron bracket fixed
    to the wall beside the tavern door (below the roof's shadow line)."""
    c = Canvas(48, 48)
    c.box(1, 9, 4, 16, DGREY)                     # wall plate with bolts
    c.put(2, 10, LGREY); c.put(2, 14, LGREY)
    c.rect(4, 11, 44, 12, BLACK)                  # the iron arm
    c.rect(5, 13, 8, 13, BLACK); c.rect(5, 14, 6, 14, BLACK)   # brace
    for x in (10, 38):                            # chains
        for y in range(13, 17):
            c.put(x, y, BLACK if y % 2 else DGREY)
    c.box(4, 17, 44, 39, BROWN)                   # the board
    c.rect(5, 18, 43, 18, YELLOW)                 # lit top edge
    c.rect(5, 38, 43, 38, BLACK)
    c.box(6, 20, 42, 36, BROWN)                   # carved inner frame
    c.text(25, 23, 'BAR', BROWN, scale=2)         # drop shadow
    c.text(24, 22, 'BAR', YELLOW, scale=2)
    return c.px


def draw_bar(frame):
    c = Canvas(BAR_W, BAR_H)
    FLOOR = BAR_H                     # the bottom of the canvas is the floor
    TOP = FLOOR - 20                  # counter top: about Keen's chest
    # Back shelves with small bottles.
    for sy in (26, 42):
        c.box(6, sy, 89, sy + 2, BROWN)              # clear of the window
        for i, x in enumerate(range(9, 87, 6)):
            if sy == 42 and 34 <= x <= 62:
                continue                               # the barmaid stands here
            col = [GREEN, LRED, BROWN, LGREEN, LBLUE, YELLOW, MAGENTA][(i + sy) % 7]
            c.rect(x, sy - 6, x + 2, sy - 1, col)
            c.put(x + 1, sy - 8, col); c.put(x + 1, sy - 7, col)
            c.put(x, sy - 5, WHITE)
    c.box(38, 2, 73, 12, BROWN)
    c.rect(39, 3, 72, 3, YELLOW)
    c.text(56, 5, 'BAR', YELLOW)
    # The barmaid behind the counter.
    draw_barmaid(c, 37, TOP - 26, frame)
    # The counter: polished top, wooden front with panels.
    c.box(0, TOP, 78, TOP + 3, LGREY)
    c.rect(1, TOP + 1, 77, TOP + 1, WHITE)
    c.box(2, TOP + 3, 76, FLOOR - 1, BROWN)
    for x in range(14, 76, 12):
        c.rect(x, TOP + 5, x, FLOOR - 2, BLACK)
        c.rect(x - 9, TOP + 6, x - 3, TOP + 6, YELLOW)
    # On the counter: a mug of ale and a bottle.
    c.box(14, TOP - 6, 18, TOP - 1, YELLOW); c.rect(15, TOP - 7, 17, TOP - 6, WHITE)
    c.put(19, TOP - 4, YELLOW); c.put(19, TOP - 3, YELLOW)
    c.box(68, TOP - 8, 70, TOP - 1, GREEN); c.put(69, TOP - 10, GREEN); c.put(69, TOP - 9, GREEN)
    # Two barrels standing on the floor at the end.
    for bx in (82, 95):
        c.box(bx, FLOOR - 16, bx + 10, FLOOR - 1, BROWN)
        c.rect(bx + 1, FLOOR - 13, bx + 9, FLOOR - 13, DGREY)
        c.rect(bx + 1, FLOOR - 5, bx + 9, FLOOR - 5, DGREY)
        c.put(bx + 5, FLOOR - 9, BLACK)                  # tap hole
    return c.px


FURN_W, FURN_H = 6 * 16, 2 * 16       # tables and stools on the floor
FURN_X = ROOM_X + 10                  # tiles 68-73
FURN_Y = ROOM_STAND - 1               # rows 37-38


def draw_furniture():
    c = Canvas(FURN_W, FURN_H)
    for tx in (6, 54):
        # Round table: top, a single leg, a foot.
        c.box(tx + 4, 12, tx + 31, 16, BROWN)
        c.rect(tx + 5, 13, tx + 30, 13, YELLOW)
        c.box(tx + 15, 16, tx + 20, 29, BROWN)
        c.box(tx + 10, 29, tx + 25, 31, BROWN)
        # A mug and a candle on it.
        c.box(tx + 8, 6, tx + 13, 12, YELLOW); c.rect(tx + 9, 5, tx + 12, 6, WHITE)
        c.rect(tx + 22, 7, tx + 24, 11, WHITE); c.put(tx + 23, 5, YELLOW); c.put(tx + 23, 6, LRED)
        # Stools either side.
        for sx in (tx - 4, tx + 32):
            c.box(sx, 20, sx + 8, 22, BROWN)
            c.rect(sx + 1, 23, sx + 1, 31, BROWN); c.rect(sx + 7, 23, sx + 7, 31, BROWN)
    return c.px


def to_image(px):
    h, w = len(px), len(px[0])
    img = Image.new('RGBA', (w, h), (0, 0, 0, 0))
    for y in range(h):
        for x in range(w):
            if px[y][x] is not None:
                img.putpixel((x, y), EGA[px[y][x]] + (255,))
    return img


def art_pieces(done):
    """(name, x, y, frames as pixel lists, when)."""
    import kw_portrait_art as P
    pieces = [('inn', INN_X, INN_TOP, [draw_inn()], 'always'),
              ('boards', DOOR_X, DOOR_ROWS[0], [draw_boards()], 'before'),
              ('sign', SIGN_X, SIGN_Y, [draw_sign()], 'always'),
              ('tavern', BAR_X, BAR_Y, [draw_bar(f) for f in range(BAR_FRAMES)], 'after'),
              ('furniture', FURN_X, FURN_Y, [draw_furniture()], 'after'),
              ('portrait', P.ORIGIN[0], P.ORIGIN[1], [P.draw()], 'after')]
    return [p for p in pieces if p[4] == 'always' or (p[4] == 'after') == done]


ART_ORDER = ['inn', 'boards', 'tavern', 'furniture', 'portrait']

# ---------------------------------------------------------------- output


def render(x0, y0, x1, y1, done, frame=0):
    w, h, (bg, fg, info) = edited_level(done)
    g = keenrender.Graphics(DATA, EPISODE)
    img = Image.new('RGBA', ((x1 - x0) * 16, (y1 - y0) * 16), (0, 0, 0, 255))
    for y in range(y0, y1):
        for x in range(x0, x1):
            t = g.tile(bg[y * w + x], False)
            if t: img.paste(t, ((x - x0) * 16, (y - y0) * 16))
            if fg[y * w + x]:
                t = g.tile(fg[y * w + x], True)
                if t: img.alpha_composite(t, ((x - x0) * 16, (y - y0) * 16))
    for name, ax, ay, frames, when in art_pieces(done):
        img.alpha_composite(to_image(frames[frame % len(frames)]), ((ax - x0) * 16, (ay - y0) * 16))
    return img.convert('RGB')


def preview(outdir):
    os.makedirs(outdir, exist_ok=True)
    before, after = render(62, 4, 96, 18, False), render(62, 4, 96, 18, True)
    sheet = Image.new('RGB', (before.width * 2 + 8, before.height), (40, 40, 40))
    sheet.paste(before, (0, 0)); sheet.paste(after, (before.width + 8, 0))
    sheet.resize((sheet.width * 2, sheet.height * 2), Image.NEAREST).save(os.path.join(outdir, 'tavern_street.png'))
    frames = [render(ROOM_X - 1, ROOM_TOP - 1, ROOM_X + len(ROOM_COLS) + 1, ROOM_TOP + len(ROOM_ROWS) + 1, True, f)
              for f in range(BAR_FRAMES)]
    frames[0].resize((frames[0].width * 2, frames[0].height * 2), Image.NEAREST).save(os.path.join(outdir, 'tavern_room.png'))
    big = [f.resize((f.width * 3, f.height * 3), Image.NEAREST) for f in frames]
    big[0].save(os.path.join(outdir, 'tavern_room.gif'), save_all=True, append_images=big[1:], duration=170, loop=0)
    print('wrote', outdir, 'TO_ROOM=%04x TO_STREET=%04x' % (TO_ROOM, TO_STREET))


def cell_major_slots(count_cells, frames, lo, hi):
    """Slots per cell, each cell's frames close together (animation links are
    int8 offsets between consecutive frames, including the wrap-around)."""
    free = free_slots_in(EPISODE, count_cells * frames, lo, hi)
    slots = [[0] * count_cells for _ in range(frames)]
    for cell in range(count_cells):
        for f in range(frames):
            slots[f][cell] = free[cell * frames + f]
    for cell in range(count_cells):
        assert abs(slots[frames - 1][cell] - slots[0][cell]) < 127
    return slots


SLOT_RANGES = {'boards': (600, 699), 'sign': (700, 899),
               'tavern': (1200, 2099), 'furniture': (900, 1199), 'inn': (2100, 2915)}


def header_for(name, x, y, frames, slots):
    h, w = len(frames[0]) // 16, len(frames[0][0]) // 16
    up = name.upper()
    lines = ['#define KW_%s_TILES_W %d' % (up, w), '#define KW_%s_TILES_H %d' % (up, h),
             '#define KW_%s_FRAMES %d' % (up, len(frames)),
             '#define KW_%s_ORIGIN_X %d' % (up, x), '#define KW_%s_ORIGIN_Y %d' % (up, y),
             'static const uint16_t kw_%sSlots[%d][%d][%d] = {' % (name, len(frames), h, w)]
    for f in range(len(frames)):
        lines.append('\t{' + ', '.join('{' + ', '.join(str(slots[f][r * w + c]) for c in range(w)) + '}' for r in range(h)) + '},')
    lines += ['};', 'static const uint8_t kw_%sTiles[%d][%d][%d][160] = {' % (name, len(frames), h, w)]
    for px in frames:
        lines.append('\t{')
        for r in range(h):
            lines.append('\t\t{')
            for c in range(w):
                lines.append('\t\t\t{' + ','.join('0x%02x' % v for v in tile_bytes(px, c, r)) + '},')
            lines.append('\t\t},')
        lines.append('\t},')
    lines.append('};')
    return lines


def csource(srcdir):
    # Map edits.
    lines = ['/* Generated by tools/kw_barhouse.py -- do not edit by hand. */',
             '// Border Village tavern: tile edits (when: 0 always, 1 after completion).',
             'static const KW_MapEdit kw_tavernEdits[] = {']
    for when, plane, x, y, v in edits():
        lines.append('\t{%d, %d, %d, %d, %d},' % (1 if when == 'after' else 0, plane, x, y, v))
    lines += ['};',
              '#define KW_TAVERN_DOOR_X %d' % DOOR_X, '#define KW_TAVERN_DOOR_Y %d' % DOOR_ROWS[0],
              '#define KW_TAVERN_ROOM_DOOR_X %d' % ROOM_DOOR_X, '#define KW_TAVERN_ROOM_STAND %d' % ROOM_STAND,
              '#define KW_TAVERN_BARMAID_X %d' % (BAR_X + 2), '#define KW_TAVERN_BARMAID_Y %d' % ROOM_STAND,
              '']
    seen = set()
    for name, x, y, frames, when in art_pieces(True) + art_pieces(False):
        if name == 'portrait' or name in seen:
            continue                                    # portrait: its own generator
        seen.add(name)
        cells = (len(frames[0]) // 16) * (len(frames[0][0]) // 16)
        slots = cell_major_slots(cells, len(frames), *SLOT_RANGES[name])
        lines += header_for(name, x, y, frames, slots) + ['']
    out = os.path.join(srcdir, 'kw_tavern_art.h')
    open(out, 'w').write('\n'.join(lines) + '\n')
    print('wrote', out)


if __name__ == '__main__':
    {'preview': preview, 'csource': csource}[sys.argv[1]](sys.argv[2])
