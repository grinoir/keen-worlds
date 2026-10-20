"""The inside of the Bean-with-Bacon Megarocket (Keen Worlds art).

    python3 tools/kw_shiproom_art.py preview OUT.png   # both rooms, 3x
    python3 tools/kw_shiproom_art.py csource OUT.h     # C data for the engine

The flight room and living bay are original Keen Worlds pixel art.  Their
low, white, wedge-shaped pressure hull, blue canopy and exposed aft machinery
are based on the *outside* silhouette of Keen 4's Megarocket, rather than on
another game's cabin.  Keen's pogo, travel bag and bunk live in the aft bay.
Around the ship the picture is see-through: the engine puts the place the ship
stands in behind it.

The result is one picture, 20 x 27 tiles: the flight room on the first
screen (13 rows), a row the camera never shows, and the exit room on
the second screen. It covers map tiles 2-21 x 2-28 of the Megarocket
world's level (tools/kw_shipworld.py). The engine lays it over the level in
three pieces (an art piece holds at most 256 tiles).
"""
import os, sys
from PIL import Image

HERE = os.path.dirname(os.path.abspath(__file__))
PLAY = os.path.abspath(os.path.join(HERE, '..', '..', 'play'))
ASSET_DIR = os.path.abspath(os.path.join(HERE, '..', 'assets'))
sys.path.insert(0, HERE)
import keenmap, keenrender
from kw_gate_art import EGA, tile_bytes, free_slots_in

TW, TH = 20, 27
W, H = TW * 16, TH * 16
ORIGIN = (2, 2)
PIECE_ROWS = 9            # rows in each of the three pieces
SLOT_RANGE = (300, 2915)  # Keen 4 masked-tile slots to take free ones from
EPISODE = '4'
FLIGHT_AT = (48, 55)      # where the rooms go in the picture: placed so that
EXIT_AT = (80, 14 * 16 + 57)  # Keen's feet are halfway into the floor, as in Keen 4


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


def own_hull(kind):
    """An original cutaway based on Keen 4's low wedge Megarocket.

    This deliberately draws the cabin from simple construction marks instead
    of sampling a level: a sloped white nose, blue canopy, belly panels, and
    an unmistakable exposed engine at the back.
    """
    w, h = 224, 96
    r = [[None] * w for _ in range(h)]
    # The low silhouette: pointy nose, long level belly, then engine cowling.
    for x in range(8, 218):
        top = 45 - min(24, max(0, (x - 18) // 4))
        if x > 170:
            top += (x - 170) // 3
        bottom = 82 if x < 186 else 82 - (x - 186) // 5
        for y in range(max(13, top), min(87, bottom)):
            r[y][x] = 8
    # Give the hull some lived-in depth before the hard construction lines:
    # riveted skin, deep under-deck shadow, and the big coloured canopy.
    for y in range(22, 42):
        left = 58 + max(0, (32 - y) * 2)
        right = 158 - max(0, (y - 27) * 2)
        for x in range(left, right):
            r[y][x] = 9 if y < 31 else 3
    for y in range(67, 80):
        for x in range(24, 180):
            if (x + y) % 5:
                r[y][x] = 0 if y < 73 else 8
    for x in range(18, 205, 9):
        y = 40 - min(18, max(0, (x - 28) // 7))
        if 15 < y < 70:
            r[y][x] = 7
    # white skin follows the outside profile; blue keel makes this ours.
    s = Stamp()
    s.line(9, 45, 47, 31, 15); s.line(47, 31, 165, 21, 15)
    s.line(165, 21, 183, 31, 7); s.line(183, 31, 215, 43, 7)
    s.line(8, 46, 8, 62, 7); s.line(9, 63, 31, 72, 7)
    s.line(31, 72, 203, 82, 1); s.line(203, 82, 217, 70, 7)
    # blue canopy, a strong echo of the proper exterior ship rather than a bus.
    s.line(58, 32, 82, 21, 9); s.line(82, 21, 143, 20, 9)
    s.line(143, 20, 158, 27, 9); s.line(58, 32, 75, 42, 3)
    s.line(75, 42, 151, 42, 3); s.line(151, 42, 158, 27, 3)
    # Cockpit with an angled instrument bench and an old-fashioned pilot's
    # chair. The information-plane console remains here, so world-map
    # selection stays exactly where it was.
    s.rect(20, 48, 59, 72, 7); s.rect(23, 50, 57, 70, 8)
    s.rect(27, 52, 50, 65, 0); s.rect(29, 54, 48, 63, 1)
    s.rect(31, 56, 45, 61, 11); s.rect(33, 57, 44, 58, 15)
    s.rect(25, 67, 55, 70, 0); s.rect(28, 67, 52, 68, 7)
    for x in range(31, 52, 5): s.rect(x, 69, x + 2, 71, 4)
    s.rect(55, 57, 62, 70, 0); s.rect(57, 54, 65, 67, 3)
    s.rect(59, 52, 63, 55, 11); s.rect(57, 66, 67, 69, 7)
    s.rect(60, 69, 63, 73, 7); s.rect(55, 72, 68, 74, 7)
    s.rect(63, 50, 103, 60, 7); s.rect(66, 52, 100, 58, 15)
    s.rect(69, 54, 96, 57, 8)
    for x in range(71, 96, 5): s.rect(x, 54, x + 2, 56, 10)
    s.rect(106, 48, 137, 71, 0); s.rect(108, 50, 135, 69, 8)
    s.rect(111, 52, 132, 66, 1)
    for x in range(113, 131, 4): s.rect(x, 54, x + 2, 64, 10)
    s.rect(114, 57, 129, 59, 11); s.rect(118, 63, 131, 66, 2)
    # A proper box of stunners, below the deck rather than a loose screen.
    s.rect(137, 61, 160, 75, 0); s.rect(139, 63, 158, 73, 1)
    s.rect(141, 65, 156, 71, 4); s.rect(143, 66, 154, 69, 12)
    s.rect(147, 70, 151, 73, 6)
    # Pipes and cable looms make the ceiling feel engineered rather than flat.
    s.line(148, 38, 169, 38, 7); s.line(169, 38, 173, 49, 7)
    s.line(151, 40, 168, 40, 15); s.line(168, 40, 171, 50, 15)
    for x in (145, 151, 157, 163): s.rect(x, 43, x + 2, 47, 4)
    # belly ribs give a roomy, walkable cabin without borrowing a room layout.
    for x in range(18, 185, 21):
        s.line(x, 73, x + 10, 79, 7)
    # Aft machinery: layered struts, a hot blue engine core and service panel.
    s.rect(177, 37, 210, 72, 7); s.rect(181, 41, 206, 68, 0)
    s.rect(184, 43, 203, 48, 15); s.rect(184, 52, 203, 56, 15)
    s.rect(187, 58, 201, 67, 1); s.rect(190, 60, 198, 65, 11)
    s.rect(210, 44, 217, 68, 8); s.rect(212, 47, 217, 65, 7)
    s.rect(174, 48, 180, 65, 0); s.rect(175, 50, 179, 54, 4)
    s.rect(175, 57, 179, 62, 10)
    s.onto(r)
    if kind == 'bay':
        # The forward bay gets an asymmetric bulkhead and storage lockers,
        # deliberately unlike the flight room while sharing the same hull.
        s = Stamp()
        s.rect(26, 45, 67, 73, 0); s.rect(29, 48, 64, 70, 8)
        s.rect(33, 52, 60, 56, 7); s.rect(37, 60, 58, 67, 0)
        s.rect(39, 61, 56, 64, 10); s.rect(43, 62, 53, 63, 15)
        s.rect(27, 48, 31, 67, 7); s.rect(33, 48, 36, 67, 0)
        for y in (51, 57, 64): s.rect(34, y, 25 + y // 3, y + 2, 15)
        # A little round porthole and trailing wire give the bay its own
        # personality, not a duplicated cockpit.
        s.rect(58, 46, 72, 59, 0); s.rect(60, 48, 70, 57, 11)
        s.rect(62, 50, 69, 55, 3); s.rect(65, 51, 69, 53, 9)
        s.line(65, 59, 65, 68, 4); s.line(65, 68, 71, 72, 4)
        s.onto(r)
    return r


def furnish_own_exit(r):
    """Keen's portable life, intentionally distinct from the old cabin."""
    s = Stamp()
    # Pogo parked in a magnetic wall clip.
    s.rect(78, 45, 84, 48, 9); s.line(84, 48, 79, 75, 15)
    s.line(82, 48, 77, 75, 7); s.rect(75, 50, 88, 52, 4)
    s.rect(72, 68, 83, 70, 4); s.rect(73, 68, 82, 69, 12)
    s.rect(75, 75, 80, 78, 7); s.rect(76, 78, 79, 80, 4)
    # Small blue travel bag, then the red-and-white bunk under the canopy.
    s.rect(90, 67, 105, 78, 1); s.rect(92, 64, 102, 67, 1)
    s.rect(92, 68, 103, 70, 9); s.rect(96, 70, 99, 74, 14)
    s.rect(112, 57, 115, 80, 6); s.rect(166, 53, 169, 80, 6)
    s.rect(115, 71, 166, 74, 6); s.rect(116, 66, 166, 71, 15)
    s.rect(116, 62, 147, 69, 4); s.rect(116, 62, 147, 64, 12)
    s.rect(151, 61, 164, 66, 7); s.rect(152, 61, 164, 63, 15)
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


def model_room(filename):
    """Read a prepared model concept panel as masked EGA pixels.

    The source sheets deliberately use a purple surrounding field.  It is
    discarded here, leaving the two separate cutaways transparent just like
    the old tile art; all other colours are reduced to the game EGA palette.
    """
    img = Image.open(os.path.join(ASSET_DIR, filename)).convert('RGB')
    room = []
    for y in range(img.height):
        row = []
        for x in range(img.width):
            rgb = img.getpixel((x, y))
            r, g, b = rgb
            if 65 <= r <= 125 and g < 35 and 65 <= b <= 125:
                row.append(None)
            else:
                row.append(min(range(len(EGA)), key=lambda i: sum((rgb[j] - EGA[i][j]) ** 2 for j in range(3))))
        room.append(row)
    return room


def draw():
    px = [[None] * W for _ in range(H)]
    flight = model_room('kw_shiproom_front.png')
    exit_room = model_room('kw_shiproom_aft.png')
    for room, (ox, oy) in ((flight, FLIGHT_AT), (exit_room, EXIT_AT)):
        for y, row in enumerate(room):
            px[oy + y][ox:ox + len(row)] = row
    return px


def draw_with_hint():
    px = draw()
    for y, row in enumerate(hint_frames()[0]):
        for x, c in enumerate(row):
            if c is not None:
                px[FLIGHT_AT[1] + HINT_AT[1] + y][FLIGHT_AT[0] + HINT_AT[0] + x] = c
    return px


HINT_AT = (96, 57)        # in the flight room (tile-aligned in the level): by the navigation screen
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
    slots_all = free_slots_in(EPISODE, TW * TH + 2, *SLOT_RANGE, d=PLAY)
    slots = slots_all[:TW * TH]
    pieces = TH // PIECE_ROWS
    lines = ['/* Generated by tools/kw_shiproom_art.py -- do not edit by hand. */',
             '// The inside of the Megarocket: original Keen Worlds rooms, in one picture',
             '// of %d pieces of %d rows.' % (pieces, PIECE_ROWS),
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
