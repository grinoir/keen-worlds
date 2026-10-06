"""Read Keen Galaxy GAMEMAPS levels (Carmack + RLEW compressed).

    python3 tools/keenmap.py DIR EPISODE [LEVEL]   # list levels / dump one

Dumps the info plane (plane 2) of a level: sprites/specials as hex values,
and where the map's level entrances (0xC0xx) are.
"""
import struct, sys, os


def carmack(src, explen):
    out = []
    i = 0
    while len(out) < explen // 2:
        w = src[i] | src[i + 1] << 8
        i += 2
        hi, cnt = w >> 8, w & 0xFF
        if hi in (0xA7, 0xA8) and cnt == 0:
            out.append((w & 0xFF00) | src[i]); i += 1
        elif hi == 0xA7:
            back = src[i]; i += 1
            for _ in range(cnt): out.append(out[-back])
        elif hi == 0xA8:
            off = src[i] | src[i + 1] << 8; i += 2
            for k in range(cnt): out.append(out[off + k])
        else:
            out.append(w)
    return out


def rlew(words, tag, n):
    out = []
    i = 0
    while len(out) < n:
        w = words[i]; i += 1
        if w == tag:
            cnt, val = words[i], words[i + 1]; i += 2
            out += [val] * cnt
        else:
            out.append(w)
    return out


def load(d, ep):
    mh = open(os.path.join(d, f'MAPHEAD.CK{ep}'), 'rb').read()
    gm = open(os.path.join(d, f'GAMEMAPS.CK{ep}'), 'rb').read()
    tag = struct.unpack_from('<H', mh)[0]
    return tag, struct.unpack_from('<100I', mh, 2), gm


def level(d, ep, n):
    tag, offs, gm = load(d, ep)
    o = offs[n]
    p = struct.unpack_from('<3I3H2H', gm, o)
    w, h = p[6], p[7]
    name = gm[o + 22:o + 38].split(b'\0')[0].decode(errors='replace')
    planes = []
    for k in range(3):
        raw = gm[p[k]:p[k] + p[3 + k]]
        explen = struct.unpack_from('<H', raw)[0]
        words = carmack(raw[2:], explen)
        planes.append(rlew(words[1:], tag, w * h))
    return name, w, h, planes


if __name__ == '__main__':
    d, ep = sys.argv[1], sys.argv[2]
    name, w, h, (bg, fg, info) = level(d, ep, int(sys.argv[3]))
    print(f'{name}: {w}x{h}')
    for y in range(h):
        row = ''
        for x in range(w):
            v = info[y * w + x]
            row += '  .  ' if v == 0 else f'{v:5x}'[-5:]
        print(f'{y:3} {row}')
