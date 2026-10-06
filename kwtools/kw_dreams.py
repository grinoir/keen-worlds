"""Make the player's Keen Dreams readable by the Keen Worlds engine.

    python3 tools/kw_dreams.py play/dreams play/mods/dreams

Keen Dreams (id Software, 1991-92; shareware version 1.13) is the game
before Keen 4: the same kind of files, a little older. The engine reads
Keen 4-6's kind, so this tool rewrites the player's copy into that:

- the graphics' and sounds' directories and dictionaries, which are inside
  the program (KDREAMS.EXE) and not files of their own, are taken out of it
  (the places are those ReflectionHLE gives for this version);
- the graphics directory's entries are four bytes each there, three here;
- the levels are packed differently (Huffman and RLEW, where Keen 4-6 use
  Carmack and RLEW), and are unpacked and written again in the newer form;
- the tile properties, which follow the map directory in the program, go
  after the map header as the engine takes them;
- a GFXINFOE and an AUDINFOE file with the counts the program has built in.

Nothing of the game is changed; the files written are the player's copy in
another form. The game's code is ported separately (tools/kw_dreams_port.py).
"""
import os, shutil, struct, sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from unlzexe import unlzexe

EXT = 'KDR'
# Version 1.13 (shareware): where the built-in files are in the unpacked program
EMBEDDED = {'EGAHEAD': (0x1cb20, 12068), 'EGADICT': (0x29846, 1024), 'MAPHEAD': (0x1fa50, 11824),
            'MAPDICT': (0x29c46, 1020), 'AUDIOHHD': (0x22880, 340), 'AUDIODCT': (0x2a042, 1024)}
EXE_SIZE = 81619
# The counts of its graphics (GRAPHKDR.H)
GFX = dict(numTiles8=72, numTiles8m=36, numTiles16=1440, numTiles16m=1206, offTiles8=368, offTiles8m=369,
           offTiles16=370, offTiles16m=1810, numBitmaps=65, numMasked=2, numSprites=297,
           offBitmaps=4, offMasked=69, offSprites=71, numChunks=3016)
RLEW_TAG = 0xABCD


def huff_expand(src, nodes, length):
    out, node, p, bit = bytearray(), 254, 0, 0
    while len(out) < length:
        v = nodes[node * 2 + ((src[p] >> bit) & 1)]
        bit += 1
        if bit == 8:
            bit, p = 0, p + 1
        if v < 256:
            out.append(v)
            node = 254
        else:
            node = v - 256
    return bytes(out)


def rlew_expand(words, tag, count):
    out, i = [], 0
    while len(out) < count:
        w = words[i]
        i += 1
        if w == tag:
            out += [words[i + 1]] * words[i]
            i += 2
        else:
            out.append(w)
    return out[:count]


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


def carmack_plain(words):
    """Carmack-format data without compression (its two marker bytes escaped)."""
    out = bytearray()
    for w in words:
        if (w >> 8) in (0xA7, 0xA8):
            out += bytes([0, w >> 8, w & 0xFF])
        else:
            out += struct.pack('<H', w)
    return bytes(out)


def plane_data(words):
    body = [len(words) * 2] + rlew(words)
    return struct.pack('<H', len(body) * 2) + carmack_plain(body)


def read_levels(game, image):
    """[(number, name, width, height, [three planes of tile numbers])]"""
    head = image[EMBEDDED['MAPHEAD'][0]:][:EMBEDDED['MAPHEAD'][1]]
    tag = struct.unpack_from('<H', head)[0]
    offsets = struct.unpack_from('<100i', head, 2)
    sizes = head[402:502]
    nodes = struct.unpack('<510H', image[EMBEDDED['MAPDICT'][0]:][:1020])
    maps = open(os.path.join(game, 'KDREAMS.MAP'), 'rb').read()
    levels = []
    for n in range(100):
        if offsets[n] <= 0:
            continue
        hdr = huff_expand(maps[offsets[n]:offsets[n] + sizes[n]], nodes, 38)
        starts = struct.unpack_from('<3i', hdr, 0)
        lengths = struct.unpack_from('<3H', hdr, 12)
        w, h = struct.unpack_from('<2H', hdr, 18)
        name = hdr[22:38].split(b'\0')[0].decode('latin1')
        planes = []
        for k in range(3):
            raw = maps[starts[k]:starts[k] + lengths[k]]
            expanded = struct.unpack_from('<H', raw)[0]            # the size of the RLEW data
            data = huff_expand(raw[2:], nodes, expanded)
            words = struct.unpack('<%dH' % (len(data) // 2), data[:len(data) // 2 * 2])
            planes.append(rlew_expand(words[1:], tag, w * h))      # (its first word: the size unpacked)
        levels.append((n, name, w, h, planes))
    return levels, head[502:]


def main(game, out):
    exe = os.path.join(game, 'KDREAMS.EXE')
    if os.path.getsize(exe) != EXE_SIZE:
        sys.exit('This is not Keen Dreams 1.13 (shareware): KDREAMS.EXE is %d bytes, not %d.' % (os.path.getsize(exe), EXE_SIZE))
    image = unlzexe(exe)
    os.makedirs(out, exist_ok=True)
    put = lambda name, data: open(os.path.join(out, name + '.' + EXT), 'wb').write(data)

    # graphics
    shutil.copyfile(os.path.join(game, 'KDREAMS.EGA'), os.path.join(out, 'EGAGRAPH.' + EXT))
    head = image[EMBEDDED['EGAHEAD'][0]:][:EMBEDDED['EGAHEAD'][1]]
    entries = struct.unpack('<%di' % (len(head) // 4), head)
    assert len(entries) == GFX['numChunks'] + 1 and entries[-1] == os.path.getsize(os.path.join(game, 'KDREAMS.EGA'))
    put('EGAHEAD', b''.join((0xFFFFFF if e < 0 else e).to_bytes(3, 'little') for e in entries))
    put('EGADICT', image[EMBEDDED['EGADICT'][0]:][:1024])
    g = GFX
    put('GFXINFOE', struct.pack('<23H', g['numTiles8'], g['numTiles8m'], g['numTiles16'], g['numTiles16m'], 0, 0,
                                g['offTiles8'], g['offTiles8m'], g['offTiles16'], g['offTiles16m'], g['numChunks'], g['numChunks'],
                                g['numBitmaps'], g['numMasked'], g['numSprites'], g['offBitmaps'], g['offMasked'], g['offSprites'],
                                0, 1, 2, 0, g['numChunks']))

    # levels, and the tile properties after the map header
    levels, tileinfo = read_levels(game, image)
    data = bytearray(b'TED5v1.0')
    offsets = [0] * 100
    for number, name, w, h, planes in levels:
        starts, lengths = [], []
        for plane in planes:
            p = plane_data(list(plane))
            starts.append(len(data))
            lengths.append(len(p))
            data += p
        offsets[number] = len(data)
        data += struct.pack('<3I3H2H16s', *starts, *lengths, w, h, name.encode('latin1')[:15]) + b'!ID!'
    put('GAMEMAPS', bytes(data))
    put('MAPHEAD', struct.pack('<H100i', RLEW_TAG, *offsets) + tileinfo)

    # sounds: PC speaker, AdLib and (unused) digitized, a third of the chunks each; no music
    shutil.copyfile(os.path.join(game, 'KDREAMS.AUD'), os.path.join(out, 'AUDIO.' + EXT))
    # The header's entries for the digitized sounds the game doesn't have
    # point past the end of the file: an empty chunk starts where it ends.
    audsize = os.path.getsize(os.path.join(game, 'KDREAMS.AUD'))
    starts = list(struct.unpack_from('<%dI' % (EMBEDDED['AUDIOHHD'][1] // 4), image, EMBEDDED['AUDIOHHD'][0]))
    put('AUDIOHHD', struct.pack('<%dI' % len(starts), *[min(s, audsize) for s in starts]))
    put('AUDIODCT', image[EMBEDDED['AUDIODCT'][0]:][:1024])
    chunks = EMBEDDED['AUDIOHHD'][1] // 4 - 1
    sounds = chunks // 3
    put('AUDINFOE', struct.pack('<7H', 0, sounds, chunks, 0, sounds, sounds * 2, sounds * 3))
    print('wrote %s: %d levels (%s), %d graphics chunks, %d sounds' %
          (out, len(levels), ', '.join('%d %s' % (n, name) for n, name, _, _, _ in levels[:4]) + '...', g['numChunks'], sounds))


if __name__ == '__main__':
    if len(sys.argv) != 3:
        sys.exit(__doc__)
    main(*sys.argv[1:])
