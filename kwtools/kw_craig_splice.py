"""Give a Craig world the Craig player sprite from another Craig build.

    python3 kwtools/kw_craig_splice.py AURORADIR CRAIGDIR

Aurora Bliss (Craig's story, part one) ships no distinct Craig player sprite --
you walk as Keen. Revival of Bliss's CRAIG build does draw Craig (same Keen 4
engine, same author). This copies Craig's character sprites -- their pixel data
(the Keen sprite chunks) and their dimensions (the matching entries of the sprite
header chunk) -- from the Craig build into Aurora's graphics, so Craig's story
begins as Craig. Only the player's own chunks move; every creature, tile and
level of Aurora is left exactly as it was. The two builds share one graphics
dictionary, so the sprite pixel data copies as-is; the header chunk is unpacked,
its player entries swapped, and packed again with that dictionary.
"""
import struct, sys, os
OFFSPRITES = 124  # Keen 4: sprites start at chunk 124
HDR_SPRITES = 2   # the sprite-dimension table's chunk
ENTRY = 18        # bytes per sprite (9 words)
# Keen's own character sprites (stand/run/jump/shoot/pole/pogo/enter/climb/swim)
KEEN = list(range(130, 164)) + list(range(167, 174)) + list(range(180, 215)) + list(range(309, 315))
# Keen on the world map: 8 walk directions x 3 frames (SPR_MAPKEEN_WALK1_* ..+2),
# chunks 254-277 in Keen 4 (same layout in the Craig build). Spliced into the
# map too so Craig's story is Craig on the map as well, not just in levels. The
# ship never uses these, so kw_craig_ship.py (which imports KEEN) leaves them out.
MAP_KEEN = list(range(254, 278))

def read_dict(path):
    d = open(path, 'rb').read()
    return [(d[i] | d[i+1] << 8, d[i+2] | d[i+3] << 8) for i in range(0, 255*4, 4)]

def huff_expand(data, explen, table):
    out = bytearray(); node = 254; i = 0; ch = data[0]; bit = 1; i = 1
    while len(out) < explen:
        node = table[node][1] if (ch & bit) else table[node][0]
        if node > 255:
            node -= 256
        else:
            out.append(node & 0xFF); node = 254
            if len(out) == explen: break
        bit <<= 1
        if bit == 256:
            if i >= len(data): break
            ch = data[i]; i += 1; bit = 1
    return bytes(out)

def huff_compress(data, table):
    # byte -> list of bits, by DFS from the root (node 254)
    paths = {}
    def dfs(node, bits):
        for b, val in ((0, table[node][0]), (1, table[node][1])):
            if val <= 255: paths[val] = bits + [b]
            else: dfs(val - 256, bits + [b])
    dfs(254, [])
    out = bytearray(); cur = 0; nb = 0
    for byte in data:
        for b in paths[byte]:
            if b: cur |= (1 << nb)
            nb += 1
            if nb == 8: out.append(cur); cur = 0; nb = 0
    if nb: out.append(cur)
    return bytes(out)

def chunks(dir):
    d = open(dir + '/EGAHEAD.CK4', 'rb').read()
    head = [d[i] | d[i+1] << 8 | d[i+2] << 16 for i in range(0, len(d), 3)]
    gr = open(dir + '/EGAGRAPH.CK4', 'rb').read(); N = len(head); out = []
    for i in range(N):
        o = head[i]
        if o == 0xFFFFFF: out.append(None); continue
        nxt = len(gr)
        for j in range(i+1, N):
            if head[j] != 0xFFFFFF: nxt = head[j]; break
        out.append(gr[o:nxt])
    return out

def main(aur, rev):
    table = read_dict(aur + '/EGADICT.CK4')
    assert open(aur+'/EGADICT.CK4','rb').read() == open(rev+'/EGADICT.CK4','rb').read(), 'dictionaries differ'
    ac, rc = chunks(aur), chunks(rev)
    assert len(ac) == len(rc), 'chunk count mismatch'
    # 1) the sprite header: unpack both, swap only Keen's entries, repack
    def unpack(blob):
        explen = struct.unpack_from('<I', blob, 0)[0]
        return explen, bytearray(huff_expand(blob[4:], explen, table))
    ae, atab = unpack(ac[HDR_SPRITES]); re_, rtab = unpack(rc[HDR_SPRITES])
    player = KEEN + MAP_KEEN  # in levels and on the world map
    for c in player:
        s = c - OFFSPRITES
        atab[s*ENTRY:(s+1)*ENTRY] = rtab[s*ENTRY:(s+1)*ENTRY]
    ac[HDR_SPRITES] = struct.pack('<I', ae) + huff_compress(atab, table)
    # 2) the player pixel data
    for c in player:
        if rc[c] is not None: ac[c] = rc[c]
    # 3) rebuild EGAGRAPH + EGAHEAD
    gr = bytearray(); head = bytearray()
    for ch in ac:
        if ch is None: head += b'\xff\xff\xff'
        else:
            o = len(gr); head += bytes([o & 0xFF, (o>>8) & 0xFF, (o>>16) & 0xFF]); gr += ch
    open(aur + '/EGAGRAPH.CK4', 'wb').write(gr); open(aur + '/EGAHEAD.CK4', 'wb').write(head)
    print('Craig spliced into %s: %d player chunks (%d map) + header entries (EGAGRAPH %d bytes)' % (
        aur, len(player), len(MAP_KEEN), len(gr)))

if __name__ == '__main__':
    if len(sys.argv) != 3: sys.exit(__doc__)
    main(sys.argv[1], sys.argv[2])
