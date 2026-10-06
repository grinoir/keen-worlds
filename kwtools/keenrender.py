"""Render a Keen Galaxy level (background + foreground tiles) to a PNG.

    python3 tools/keenrender.py DATADIR EPISODE LEVEL OUT.png [--grid]

DATADIR must hold EGAGRAPH/EGAHEAD/EGADICT/GFXINFOE/MAPHEAD/GAMEMAPS for the
episode (play/ does). --grid draws tile coordinates every 4 tiles and marks
info-plane values in red.
"""
import os, struct, sys
from PIL import Image, ImageDraw

sys.path.insert(0, os.path.dirname(__file__))
import keenmap

EGA = [(0, 0, 0), (0, 0, 170), (0, 170, 0), (0, 170, 170), (170, 0, 0), (170, 0, 170), (170, 85, 0), (170, 170, 170),
       (85, 85, 85), (85, 85, 255), (85, 255, 85), (85, 255, 255), (255, 85, 85), (255, 85, 255), (255, 255, 85), (255, 255, 255)]


class Graphics:
    def __init__(self, d, ep):
        f = lambda n: open(os.path.join(d, f'{n}.CK{ep}'), 'rb').read()
        self.graph = f('EGAGRAPH')
        head = f('EGAHEAD')
        self.starts = [int.from_bytes(head[i:i + 3], 'little') for i in range(0, len(head), 3)]
        self.nodes = struct.unpack('<510H', f('EGADICT')[:1020])
        info = struct.unpack('<23H', f('GFXINFOE')[:46])
        self.off16, self.off16m = info[8], info[9]
        self.cache = {}

    def chunk(self, n, length):
        a = self.starts[n]
        if a == 0xFFFFFF:
            return None
        j = n + 1
        while self.starts[j] == 0xFFFFFF:
            j += 1
        src = self.graph[a:self.starts[j]]
        out = bytearray(); node = 254; p = bit = 0
        while len(out) < length:
            v = self.nodes[node * 2 + ((src[p] >> bit) & 1)]
            bit += 1
            if bit == 8: bit = 0; p += 1
            if v < 256: out.append(v); node = 254
            else: node = v - 256
        return bytes(out)

    def tile(self, n, masked):
        key = (n, masked)
        if key in self.cache:
            return self.cache[key]
        size = 160 if masked else 128
        data = self.chunk((self.off16m if masked else self.off16) + n, size)
        img = None
        if data:
            img = Image.new('RGBA', (16, 16))
            px = img.load()
            base = 32 if masked else 0
            for y in range(16):
                for x in range(16):
                    byte, b = y * 2 + x // 8, 7 - x % 8
                    if masked and (data[byte] >> b) & 1:
                        px[x, y] = (0, 0, 0, 0); continue
                    c = sum(((data[base + pl * 32 + byte] >> b) & 1) << pl for pl in range(4))
                    px[x, y] = EGA[c] + (255,)
        self.cache[key] = img
        return img


def render(d, ep, lvl, out, grid=False):
    name, w, h, (bg, fg, info) = keenmap.level(d, ep, lvl)
    g = Graphics(d, ep)
    img = Image.new('RGBA', (w * 16, h * 16), (0, 0, 0, 255))
    for y in range(h):
        for x in range(w):
            t = g.tile(bg[y * w + x], False)
            if t: img.paste(t, (x * 16, y * 16))
            if fg[y * w + x]:
                t = g.tile(fg[y * w + x], True)
                if t: img.alpha_composite(t, (x * 16, y * 16))
    if grid:
        dr = ImageDraw.Draw(img)
        for y in range(h):
            for x in range(w):
                v = info[y * w + x]
                if v:
                    dr.rectangle([x * 16, y * 16, x * 16 + 15, y * 16 + 15], outline=(255, 0, 0))
                    dr.text((x * 16 + 1, y * 16 + 3), f'{v:x}', fill=(255, 255, 0))
        for x in range(0, w, 4):
            dr.text((x * 16 + 1, 1), str(x), fill=(255, 255, 255))
        for y in range(0, h, 4):
            dr.text((1, y * 16 + 1), str(y), fill=(255, 255, 255))
    img.convert('RGB').resize((w * 32, h * 32), Image.NEAREST).save(out)
    print(f'{name}: {w}x{h} -> {out}')


if __name__ == '__main__':
    render(sys.argv[1], sys.argv[2], int(sys.argv[3]), sys.argv[4], '--grid' in sys.argv)


class Sprites:
    """Decode sprites (masked, planar) from EGAGRAPH, for previews."""

    def __init__(self, d, ep):
        import struct
        self.g = Graphics(d, ep)
        info = struct.unpack('<23H', open(os.path.join(d, f'GFXINFOE.CK{ep}'), 'rb').read()[:46])
        self.num, self.off, hdr = info[14], info[17], info[20]
        raw = self._chunk(hdr)
        self.table = [struct.unpack_from('<HHhhhhhhH', raw, i * 18) for i in range(self.num)]

    def _chunk(self, n):
        import struct
        g = self.g
        a = g.starts[n]
        j = n + 1
        while g.starts[j] == 0xFFFFFF:
            j += 1
        length = struct.unpack_from('<I', g.graph, a)[0]
        src = g.graph[a + 4:g.starts[j]]
        out = bytearray(); node = 254; p = bit = 0
        while len(out) < length:
            v = g.nodes[node * 2 + ((src[p] >> bit) & 1)]
            bit += 1
            if bit == 8: bit = 0; p += 1
            if v < 256: out.append(v); node = 254
            else: node = v - 256
        return out

    def image(self, chunk):
        w, h = self.table[chunk - self.off][0], self.table[chunk - self.off][1]
        data = self._chunk(chunk)
        plane = w * h
        im = Image.new('RGBA', (w * 8, h), (0, 0, 0, 0))
        for y in range(h):
            for x in range(w * 8):
                byte, b = y * w + x // 8, 7 - x % 8
                if (data[byte] >> b) & 1:
                    continue
                c = sum(((data[plane * (1 + q) + byte] >> b) & 1) << q for q in range(4))
                im.putpixel((x, y), EGA[c] + (255,))
        return im
