"""Build Keen Worlds' own world, "The Megarocket": the inside of Keen's ship.

Every ship in the galaxy is boarded into this room, and its flight console is
where Keen picks a destination. It is Keen Worlds' own world: its map is
written here, on Keen 4's tiles (the one episode Keen Worlds can rely on),
with Keen Worlds' own art added by the engine when the level loads.

    python3 tools/kw_shipworld.py omnispeak/data/megarocket

writes SHIPMAPS.CK4, SHIPHEAD.CK4 (the map, in the game's own map format)
and SHIP.CK4 (the world's variable file). play/run.sh copies the folder to
play/megarocket.

The room is drawn below as text, one character a tile.
"""
import os, struct, sys

RLEW_TAG = 0xABCD

# Keen 4 tiles the first, plain version of the room is built from
BACK = 170       # a black background tile (the engine puts the place the ship stands in here)
WALL = 1642      # a solid block (pyramid stone)
LEDGE = 216      # a tile Keen can stand on and jump up through
KEEN = 1         # info value: Keen starts here, facing right
KEEN_LEFT = 2    # ... facing left
CONSOLE = 0x7701 # info value: the flight console ("up" opens the list of worlds)
HATCH = 0x7702   # info value: the hatch ("up" leaves the ship)
WAY_AFT = 0x7703 # info value: walking here takes Keen to the exit room
WAY_FORE = 0x7704 # ... and here, back to the flight room
AMMO = 0x7705    # info value: the box of stunners ("up" tops Keen's shots up)
BED = 0x7706     # info value: the bed ("up": Keen falls asleep, into Keen Dreams)
SCROLL_STOP = 25 # Keen 4's own: the camera does not scroll past this row

# The level: two rooms, each a screen (20 x 13 tiles), one above the other,
# with a row between them that the camera is kept from showing ('B').
# '#' wall, '.' open, 'P' the flight deck (Keen jumps up onto it), 'K'/'k' Keen (facing right/left), 'C' the navigation
# screen (also the flight deck's controls), 'A' the box of stunners, 'Z' the bed, 'H' the hatch, 'W' the way from the flight room to the exit room
# and 'V' the way back (all four are places in the row Keen walks in). The
# game needs a border of two tiles around what the camera may show.
ROOM = [
    "########################",
    "########################",
    "########################",
    "########################",
    "########################",
    "########################",
    "########################",
    "#############....#######",
    "#############CCC.#######",
    "##########...PPP...#####",
    "##########CCAAA...W#####",
    "########################",
    "########################",
    "########################",
    "########################",
    "##B#####################",
    "########################",
    "########################",
    "########################",
    "########################",
    "########################",
    "########################",
    "#######.........########",
    "#######.........########",
    "#######V.HHK.ZZZ########",
    "########################",
    "########################",
    "########################",
    "########################",
    "########################",
    "########################",
]
LEVELS = [(1, "The Megarocket", ROOM)]   # (number, name, rows)


def rlew(words):
    out, i = [], 0
    while i < len(words):
        j = i
        while j < len(words) and words[j] == words[i] and j - i < 0xFFFF:
            j += 1
        if j - i > 3 or words[i] == RLEW_TAG:
            out += [RLEW_TAG, j - i, words[i]]
        else:
            out += words[i:j]
        i = j
    return out


def carmack(words):
    """Carmack-format data without compression: words as they are, with the
    two marker bytes (0xA7, 0xA8) escaped."""
    out = bytearray()
    for w in words:
        if (w >> 8) in (0xA7, 0xA8):
            out += bytes([0, w >> 8, w & 0xFF])
        else:
            out += struct.pack('<H', w)
    return bytes(out)


def plane_data(words):
    r = rlew(words)
    body = [len(words) * 2] + r                 # RLEW-expanded size, then the data
    return struct.pack('<H', len(body) * 2) + carmack(body)   # Carmack-expanded size, then the data


def build(out):
    data = bytearray(b'TED5v1.0')
    offsets = [0] * 100
    for number, name, rows in LEVELS:
        h, w = len(rows), len(rows[0])
        assert all(len(r) == w for r in rows), 'rows of different length in ' + name
        back, fore, info = [], [], []
        for row in rows:
            for ch in row:
                back.append(BACK)
                fore.append(WALL if ch in '#B' else LEDGE if ch == 'P' else 0)
                info.append({'K': KEEN, 'k': KEEN_LEFT, 'C': CONSOLE, 'H': HATCH, 'W': WAY_AFT, 'V': WAY_FORE, 'B': SCROLL_STOP, 'A': AMMO, 'Z': BED}.get(ch, 0))
        starts, lengths = [], []
        for plane in (back, fore, info):
            p = plane_data(plane)
            starts.append(len(data))
            lengths.append(len(p))
            data += p
        offsets[number] = len(data)
        data += struct.pack('<3I3H2H16s', *starts, *lengths, w, h, name.encode()[:15]) + b'!ID!'
    open(os.path.join(out, 'SHIPMAPS.CK4'), 'wb').write(data)
    open(os.path.join(out, 'SHIPHEAD.CK4'), 'wb').write(struct.pack('<H100i', RLEW_TAG, *offsets))
    names = ['' for _ in range(max(n for n, _, _ in LEVELS) + 1)]
    for number, name, _ in LEVELS:
        names[number] = name
    with open(os.path.join(out, 'SHIP.CK4'), 'w') as f:
        f.write('# The Megarocket: Keen Worlds\' own world, the inside of Keen\'s ship.\n'
                '# Written by tools/kw_shipworld.py. It runs on Keen 4 (tiles, sprites,\n'
                '# sounds); only the map is its own.\n\n'
                '%string ca_mapFile "SHIPMAPS.CK4"\n%string ca_mapHead "SHIPHEAD.CK4"\n\n')
        f.write('%stringarray ck_str_levelNames\n' + ',\n'.join('\t"%s"' % n for n in names) + '\n\n')
        f.write('%stringarray ck_str_levelEntryText\n' +
                ',\n'.join('\t"Keen boards the\\nBean-with-Bacon\\nMegarocket"' for _ in names) + '\n\n')
        f.write('%override "EPISODE.CK4"\n')
    print('wrote the Megarocket world to %s: %d level(s)' % (out, len(LEVELS)))


if __name__ == '__main__':
    if len(sys.argv) != 2:
        sys.exit(__doc__)
    build(sys.argv[1])
