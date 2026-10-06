"""The inside of the Bean-with-Bacon Megarocket (Keen Worlds art).

    python3 tools/kw_shiproom_art.py preview OUT.png   # both rooms, 3x
    python3 tools/kw_shiproom_art.py csource OUT.h     # C data for the engine

The two rooms of Keen's ship as they are in Foray in the Forest (by Nisaba
and K1n9_Duk3, GPL v2 or later): the flight room and the exit room. The
pictures are taken from that game's own files (its level 1, "BwB", read from
play/mods/fitf) and then cleaned here: the things lying about in them (the
bottles, papers, radio, the creature, the toolbox...) are painted out,
so what is left is the ship itself. Keen Worlds adds three things of
Keen's to the exit room: his pogo stick, his bag and a bed. Around the ship
the picture is see-through: the engine puts the place the ship stands in
behind it.

The result is one picture, 20 x 27 tiles: the flight room on the first
screen (13 rows), a row the camera never shows, and the exit room on
the second screen. It covers map tiles 2-21 x 2-28 of the Megarocket
world's level (tools/kw_shipworld.py). The engine lays it over the level in
three pieces (an art piece holds at most 256 tiles).
"""
import os, struct, sys
from PIL import Image

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import keenmap, keenrender
from kw_gate_art import EGA, tile_bytes, free_slots_in

FITF = os.path.join(HERE, '..', 'play', 'mods', 'fitf')
FITF_LEVEL = 1
FLIGHT_BOX = (5, 24, 19, 30)   # the rooms in that level, in tiles (x0, y0, x1, y1)
EXIT_BOX = (7, 38, 17, 44)

TW, TH = 20, 27
W, H = TW * 16, TH * 16
ORIGIN = (2, 2)
PIECE_ROWS = 9            # rows in each of the three pieces
SLOT_RANGE = (300, 2915)  # Keen 4 masked-tile slots to take free ones from
EPISODE = '4'
FLIGHT_AT = (48, 64)      # where the rooms go in the picture
EXIT_AT = (80, 14 * 16 + 64)


def foray_rooms():
    """Both rooms as rows of EGA colour numbers, as Foray in the Forest draws them."""
    # the game's map file is called MAPTEMP there; the reader wants GAMEMAPS
    load = keenmap.load

    def load_maptemp(folder, ep):
        mh = open(os.path.join(folder, 'MAPHEAD.CK' + ep), 'rb').read()
        gm = open(os.path.join(folder, 'MAPTEMP.CK' + ep), 'rb').read()
        return struct.unpack_from('<H', mh)[0], struct.unpack_from('<100I', mh, 2), gm
    keenmap.load = load_maptemp
    try:
        _, w, h, (bg, fg, _) = keenmap.level(FITF, 'S', FITF_LEVEL)
    finally:
        keenmap.load = load
    g = keenrender.Graphics(FITF, 'S')
    rooms = []
    for x0, y0, x1, y1 in (FLIGHT_BOX, EXIT_BOX):
        img = Image.new('RGBA', ((x1 - x0) * 16, (y1 - y0) * 16), (0, 0, 0, 255))
        for y in range(y0, y1):
            for x in range(x0, x1):
                t = g.tile(bg[y * w + x], False)
                if t:
                    img.paste(t, ((x - x0) * 16, (y - y0) * 16))
                if fg[y * w + x]:
                    t = g.tile(fg[y * w + x], True)
                    if t:
                        img.alpha_composite(t, ((x - x0) * 16, (y - y0) * 16))
        img = img.convert('RGB')
        rooms.append([[EGA.index(img.getpixel((x, y))) for x in range(img.width)] for y in range(img.height)])
    return rooms


def clean_flight(r):
    """The hold under the flight deck: a plain wall, and the floor's pattern
    carried on where the bottles, the papers and the radio stood. The box
    of stunners stays: it is where Keen tops his shots up."""
    for y in range(53, 76):
        for x in range(86, 188):
            if 150 <= x <= 167 and 53 <= y <= 70:         # the panel on the wall stays
                continue
            if 119 <= x <= 149:                           # and so does the box of stunners
                continue
            r[y][x] = 8
    for y in range(61, 76):                               # the box's left edge, where a magazine leaned on it
        if r[y][119]:
            r[y][118] = 0
    for y in range(64, 76):
        for x in range(77, 86):
            r[y][x] = 8
    for y in range(76, 82):
        for x in range(112, 96, -1):
            r[y][x] = r[y][x + 16]
    # the wall panel the radio stood in front of: its lower right corner
    for y in range(60, 68):
        for x in range(161, 166):
            r[y][x] = 0
        r[y][166], r[y][167] = 8, 0
    for x in range(158, 167):
        r[68][x] = 8
    for x in range(152, 168):
        r[69][x] = 0
    for y in range(70, 76):                               # (a last blue corner of the radio)
        for x in range(150, 170):
            r[y][x] = 8
    for x, y, c in ((162, 62, 11), (164, 62, 3), (162, 65, 9), (164, 66, 11), (163, 64, 3)):
        r[y][x] = c
    return r


def inside_end(row):
    """The last column of the room's inside in this row (the hull's dotted
    edge starts two further right)."""
    x = max(i for i, v in enumerate(row) if v)
    while x >= 2 and row[x - 2] == 8 and row[x - 1] == 0:
        x -= 2
    return x - 3


def clean_exit(r):
    """The exit room without the pink thing by the door, the toy on the
    shelf, the toolbox and spanner, and the creature in the corner. What was
    behind them is made from what is beside them: the wall's columns, the
    shelf's slats and posts, the floor's slabs."""
    o = [row[:] for row in r]
    end = [inside_end(row) if any(row) else 0 for row in o]
    left = [min(i for i, v in enumerate(row) if v) if any(row) else 0 for row in o]
    like = lambda y: 61 if y % 2 else 62          # a clean row of the same dither phase
    # left of the door: the wall's columns carry on down, then the floor
    for y in range(63, 72):
        for x in range(left[y] + 4, 37):
            if y in (63, 64) and x < 21:
                r[y][x] = 7 if x < 17 else 0
            else:
                r[y][x] = o[like(y)][x] if y < 71 else 0
    for y in range(72, 76):
        for x in range(left[y] + 4, 35):
            r[y][x] = o[y][x + 16] if x + 16 >= 36 else o[y][x + 32]
    # the shelf: its slats, and the block under it
    for y in range(39, 67):
        for x in range(82, 110):
            r[y][x] = o[y][93]
    for y in range(51, 54):
        r[y][80] = r[y][81] = 0
    for y in range(33, 39):
        for x in range(102, 110):
            r[y][x] = 8
    for y in range(67, 72):
        for x in range(77, 110):
            r[y][x] = 7 if y == 67 else 8 if y < 71 else 0
    for y in (20, 21):                            # a red end on a cable, which read as a stray dot
        for x in range(84, 88):
            r[y][x] = {4: 1, 12: 9}.get(r[y][x], r[y][x])
    # the corner: the shelf's posts, then wall columns like those left of the door
    for y in range(20, 72):
        for x in range(104, end[y] + 3):
            if x > end[y]:
                r[y][x] = 0
            elif y < 30:
                if x >= 119:
                    r[y][x] = o[y][60]
            elif 110 <= x <= 120:
                r[y][x] = (0, 7, 8, 8, 8, 0, 7, 8, 8, 8, 0)[x - 110] if y < 71 else 0
            elif x > 120:
                src = 19 + x - 121
                r[y][x] = (o[y if y < 63 else like(y)][src] if src <= 36 else 8) if y < 71 else 0
    # the floor, carried on to the right
    for y in range(72, 86):
        for x in range(77, end[y] + 3):
            r[y][x] = r[y][x - 16] if x <= end[y] else 0
    return r


class Stamp:
    """Something drawn into a room: its pixels, and a black line around them."""
    def __init__(self):
        self.px = {}

    def rect(self, x0, y0, x1, y1, c):
        for y in range(y0, y1):
            for x in range(x0, x1):
                self.px[(x, y)] = c

    def line(self, x0, y0, x1, y1, c):
        n = max(abs(x1 - x0), abs(y1 - y0), 1)
        for t in range(n + 1):
            self.px[(round(x0 + (x1 - x0) * t / n), round(y0 + (y1 - y0) * t / n))] = c

    def onto(self, r):
        for (x, y) in self.px:
            for dx, dy in ((-1, 0), (1, 0), (0, -1), (0, 1)):
                if (x + dx, y + dy) not in self.px:
                    r[y + dy][x + dx] = 0
        for (x, y), c in self.px.items():
            r[y][x] = c


def furnish_exit(r):
    """Keen's own things in the exit room (Keen Worlds' drawing): his pogo
    stick against the wall, his bag, and a bed."""
    # the pogo stick, leaning by the door
    s = Stamp()
    s.line(80, 50, 76, 76, 7), s.line(81, 50, 77, 76, 15)        # the shaft
    s.rect(76, 49, 86, 51, 4), s.rect(76, 49, 86, 50, 12)        # handlebar
    s.rect(72, 68, 83, 70, 4), s.rect(72, 68, 83, 69, 12)        # foot bar
    for y in range(71, 77):
        s.rect(75 + (y % 2), y, 79 + (y % 2), y + 1, 8 if y % 2 else 7)   # the spring
    s.rect(75, 77, 79, 78, 4)
    s.onto(r)
    # the bag
    s = Stamp()
    s.rect(85, 66, 95, 78, 1)
    s.rect(86, 64, 94, 66, 1)
    s.rect(85, 66, 87, 78, 9), s.rect(86, 64, 94, 65, 9)         # lit from the left
    s.rect(85, 69, 95, 70, 9)                                    # the flap's edge
    s.rect(89, 69, 91, 73, 6), s.rect(89, 72, 91, 73, 14)        # strap and buckle
    s.rect(88, 61, 92, 62, 6), s.rect(87, 62, 88, 64, 6), s.rect(92, 62, 93, 64, 6)   # the handle
    s.onto(r)
    # the bed, along the wall on the right
    s = Stamp()
    s.rect(98, 58, 100, 78, 6), s.rect(98, 58, 99, 78, 14)       # foot end
    s.rect(138, 52, 141, 78, 6), s.rect(138, 52, 139, 78, 14)    # head end
    s.rect(100, 69, 138, 72, 6), s.rect(100, 69, 138, 70, 14)    # the side rail
    s.rect(100, 65, 138, 69, 7), s.rect(100, 65, 138, 66, 15)    # mattress
    s.rect(100, 61, 126, 68, 4), s.rect(100, 61, 126, 62, 12)    # blanket
    s.rect(122, 62, 126, 68, 12), s.rect(100, 67, 126, 68, 6)
    s.rect(128, 60, 137, 65, 15), s.rect(129, 64, 137, 65, 7)    # pillow
    s.onto(r)
    return r


def cut_out(room):
    """A room with everything outside the ship see-through (None), so the
    place the ship stands in shows around it."""
    h, w = len(room), len(room[0])
    rows = [[i for i, v in enumerate(row) if v] for row in room]
    cols = [[y for y in range(h) if room[y][x]] for x in range(w)]
    return [[room[y][x] if rows[y] and cols[x] and rows[y][0] <= x <= rows[y][-1] and cols[x][0] <= y <= cols[x][-1]
             else None for x in range(w)] for y in range(h)]


def draw():
    flight, exit_room = foray_rooms()
    px = [[None] * W for _ in range(H)]
    for room, (ox, oy) in ((clean_flight(flight), FLIGHT_AT), (furnish_exit(clean_exit(exit_room)), EXIT_AT)):
        for y, row in enumerate(cut_out(room)):
            px[oy + y][ox:ox + len(row)] = row
    return px


def draw_with_hint():
    px = draw()
    for y, row in enumerate(hint_frames()[0]):
        for x, c in enumerate(row):
            if c is not None:
                px[FLIGHT_AT[1] + HINT_AT[1] + y][FLIGHT_AT[0] + HINT_AT[0] + x] = c
    return px


HINT_AT = (96, 48)        # in the flight room: the tile right of the navigation screen
ARROW = ['...#...', '..###..', '.#####.', '#######', '..###..', '..###..']


def hint_frames():
    """A small lit sign with an arrow pointing up, bright then dim: "press
    up here". Two frames of one tile, see-through around the sign."""
    frames = []
    for colour in (14, 6):
        t = [[None] * 16 for _ in range(16)]
        for y in range(6, 16):
            for x in range(2, 13):
                t[y][x] = 7 if y in (6, 15) or x in (2, 12) else 0
        for dy, row in enumerate(ARROW):
            for dx, ch in enumerate(row):
                if ch == '#':
                    t[8 + dy][4 + dx] = colour
        frames.append(t)
    return frames


def to_image(px):
    img = Image.new('RGB', (W, H))
    for y in range(H):
        for x in range(W):
            img.putpixel((x, y), EGA[px[y][x]] if px[y][x] is not None else (96, 0, 96))
    return img


def preview(out):
    to_image(draw_with_hint()).resize((W * 3, H * 3), Image.NEAREST).save(out)
    print('wrote', out)


def csource(out):
    px = draw()
    slots_all = free_slots_in(EPISODE, TW * TH + 2, *SLOT_RANGE)
    slots = slots_all[:TW * TH]
    pieces = TH // PIECE_ROWS
    lines = ['/* Generated by tools/kw_shiproom_art.py -- do not edit by hand. */',
             '// The inside of the Megarocket: the two rooms of Keen\'s ship from Foray in the',
             '// Forest (Nisaba and K1n9_Duk3, GPL v2 or later), cleaned of the things lying',
             '// about in them. One picture in %d pieces of %d rows.' % (pieces, PIECE_ROWS),
             '#define KW_SHIPROOM_ORIGIN_X %d' % ORIGIN[0], '#define KW_SHIPROOM_ORIGIN_Y %d' % ORIGIN[1],
             '#define KW_SHIPROOM_TILES_W %d' % TW, '#define KW_SHIPROOM_PIECE_H %d' % PIECE_ROWS,
             '#define KW_SHIPROOM_PIECES %d' % pieces,
             '// Empty, unused Keen 4 masked-tile slots that hold the art: [piece][row][column].',
             'static const uint16_t kw_shipRoomSlots[KW_SHIPROOM_PIECES][KW_SHIPROOM_PIECE_H][KW_SHIPROOM_TILES_W] = {']
    for p in range(pieces):
        lines.append('\t{' + ', '.join('{' + ', '.join(str(slots[ty * TW + tx]) for tx in range(TW)) + '}'
                                       for ty in range(p * PIECE_ROWS, (p + 1) * PIECE_ROWS)) + '},')
    lines += ['};',
              'static const uint8_t kw_shipRoomTiles[KW_SHIPROOM_PIECES][KW_SHIPROOM_PIECE_H][KW_SHIPROOM_TILES_W][160] = {']
    for p in range(pieces):
        lines.append('\t{')
        for ty in range(p * PIECE_ROWS, (p + 1) * PIECE_ROWS):
            lines.append('\t\t{')
            for tx in range(TW):
                lines.append('\t\t\t{' + ','.join('0x%02x' % v for v in tile_bytes(px, tx, ty)) + '},')
            lines.append('\t\t},')
        lines.append('\t},')
    lines.append('};')
    # the blinking sign: two slots close together (the animation link is a signed byte)
    hint = slots_all[TW * TH:TW * TH + 2]
    assert hint[1] - hint[0] < 127
    lines += ['// The "up" sign by the navigation screen: [frame][row][column].',
              '#define KW_SHIPHINT_X %d' % (ORIGIN[0] + (FLIGHT_AT[0] + HINT_AT[0]) // 16),
              '#define KW_SHIPHINT_Y %d' % (ORIGIN[1] + (FLIGHT_AT[1] + HINT_AT[1]) // 16),
              'static const uint16_t kw_shipHintSlots[2][1][1] = {{{%d}}, {{%d}}};' % tuple(hint),
              'static const uint8_t kw_shipHintTiles[2][1][1][160] = {']
    for t in hint_frames():
        lines.append('\t{{{' + ','.join('0x%02x' % v for v in tile_bytes(t, 0, 0)) + '}}},')
    lines.append('};')
    open(out, 'w').write('\n'.join(lines) + '\n')
    print('wrote', out, '(%d tiles, slots %d-%d)' % (TW * TH, slots[0], slots[TW * TH - 1]))


if __name__ == '__main__':
    {'preview': preview, 'csource': csource}[sys.argv[1]](sys.argv[2])
