"""Read a Galaxy game's ending (or any text chunk) from its graphics files,
page by page, the way the engine lays it out (ck_text.c: every ^P starts a
page, ^E ends the text).

    from kw_end_text import pages
    pages(folder, chunk, egadict) -> [[(colour, text), ...], ...]

Each page is a list of runs of text, each with the colour code (^c) it is
printed in (None before the first): the colour says who speaks in Dave216's
games. Picture, timing and layout commands are dropped, lines joined.
"""
import os, re, struct


def _find(folder, name):
    for f in os.listdir(folder):
        if f.lower() == name.lower():
            return os.path.join(folder, f)
    raise FileNotFoundError(os.path.join(folder, name))


def chunk(folder, ext, number, egadict):
    head = open(_find(folder, 'EGAHEAD.' + ext), 'rb').read()
    gfx = open(_find(folder, 'EGAGRAPH.' + ext), 'rb').read()
    d = open(egadict, 'rb').read()
    nodes = [struct.unpack_from('<HH', d, i * 4) for i in range(255)]

    def off(i):
        v = head[i * 3] | head[i * 3 + 1] << 8 | head[i * 3 + 2] << 16
        return None if v == 0xFFFFFF else v
    a, j = off(number), number + 1
    while off(j) is None:
        j += 1
    raw = gfx[a:off(j)]
    n = struct.unpack_from('<I', raw)[0]
    out, node, pos, bit = bytearray(), 254, 4, 0
    while len(out) < n:
        v = nodes[node][(raw[pos] >> bit) & 1]
        bit += 1
        if bit == 8:
            bit, pos = 0, pos + 1
        if v < 256:
            out.append(v)
            node = 254
        else:
            node = v - 256
    return out.decode('latin1')


def pages(folder, number, egadict, ext='CK4'):
    text = chunk(folder, ext, number, egadict).split('^E')[0]
    result = []
    for page in text.split('^P')[1:]:
        lines = [l for l in page.replace('\r', '').split('\n') if not re.match(r'\s*\^[GTBLA]', l)]
        body = ' '.join(l.strip() for l in lines)
        runs, colour = [], None
        for part in re.split(r'(\^[cC][0-9A-Fa-f])', body):
            if re.fullmatch(r'\^[cC][0-9A-Fa-f]', part):
                colour = part[1:]
                continue
            part = re.sub(r'\s+', ' ', part).strip()
            if part:
                runs.append((colour, part))
        result.append(runs)
    return result


if __name__ == '__main__':
    import sys
    for i, p in enumerate(pages(sys.argv[1], int(sys.argv[2]), sys.argv[3], *sys.argv[4:])):
        print('-- page %d' % i)
        for colour, t in p:
            print('  [%s] %s' % (colour, t))
